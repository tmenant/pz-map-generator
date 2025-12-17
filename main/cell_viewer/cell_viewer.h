#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Window.hpp>
#include <SFML/Window/WindowEnums.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <algorithm>
#include <cassert>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "constants.h"
#include "core/cell_coord.h"
#include "files/lotheader.h"
#include "files/lotpack.h"
#include "files/texturepack.h"
#include "services/map_files_service.h"
#include "services/tilesheet_service.h"
#include <fmt/base.h>
#include <fmt/format.h>

struct ViewState
{
    sf::Vector2i lastMousePos;
    sf::Vector2f center = { .0f, .0f };
    float zoomLevel = 1.0f;
    bool isDragging = false;

    void applyTo(sf::View &view, sf::RenderWindow &window)
    {
        zoomLevel = fastClamp(zoomLevel, 0.2f, 4.f);

        view.setSize({ window.getSize().x * zoomLevel, window.getSize().y * zoomLevel });
        view.setCenter(center);
    }

    float fastClamp(float value, float min, float max)
    {
        if (value <= min) return min;
        if (value >= max) return max;

        return value;
    }
};

class CellViewer
{
public:
    LotHeader lotheader;
    Lotpack lotpack;

    TilesheetService *tilesheetService;

    std::unordered_map<std::string, std::optional<sf::Texture>> texturesByName;
    std::unordered_set<std::string> spritesNotFound;

    ViewState viewState;
    sf::View *view;

    bool firstFrame = true;

    CellViewer(sf::View *view, MapFilesService *mapFileService, TilesheetService *tilesheetService, int x, int y)
    {
        this->lotheader = mapFileService->LoadLotheaderByPosition(x, y);
        this->lotpack = mapFileService->LoadLotpackByPosition(x, y, &lotheader);
        this->tilesheetService = tilesheetService;

        this->viewState = ViewState();
        this->view = view;

        this->texturesByName = std::unordered_map<std::string, std::optional<sf::Texture>>{};
        this->spritesNotFound = std::unordered_set<std::string>{};

        std::sort(lotpack.squareMap.begin(), lotpack.squareMap.end(), [](const SquareData &a, const SquareData &b)
        {
            if (a.coord.chunk_idx() != b.coord.chunk_idx())
                return a.coord.chunk_idx() < b.coord.chunk_idx();

            if (a.coord.z() != b.coord.z())
                return a.coord.z() < b.coord.z();

            if (a.coord.y() != b.coord.y())
                return a.coord.y() < b.coord.y();

            if (a.coord.x() != b.coord.x())
                return a.coord.x() < b.coord.x();

            return false;
        });
    }

    sf::Texture *getOrCreateTexture(const std::string &textureName)
    {
        TexturePack::Page *page = tilesheetService->getPageByTextureName(textureName);
        if (!page)
        {
            texturesByName[page->name] = std::nullopt;
            return nullptr;
        }

        auto it = texturesByName.find(page->name);
        if (it != texturesByName.end())
        {
            return it->second.has_value() ? &it->second.value() : nullptr;
        }

        sf::Texture texture;
        if (!texture.loadFromMemory(page->png.data(), page->png.size()))
        {
            texturesByName[page->name] = std::nullopt;
            return nullptr;
        }

        auto _ = texture.copyToImage().saveToFile("ignore/sprites/" + page->name + ".png");

        texturesByName[page->name] = texture;
        return &texturesByName[page->name].value();
    }

    std::optional<sf::Sprite> createSprite(TexturePack::Texture *textureData)
    {
        assert(textureData != nullptr);

        sf::Texture *texture = getOrCreateTexture(textureData->name);

        if (texture == nullptr)
        {
            if (!spritesNotFound.contains(textureData->name))
            {
                spritesNotFound.insert(textureData->name);
                fmt::println("sprite not found: '{}'", textureData->name);
            }

            throw std::runtime_error("sprite not found");
            return std::nullopt;
        }

        sf::Sprite sprite(*texture);

        return sprite;
    }

    void handleEvents(const sf::Event &event, sf::RenderWindow &window)
    {
        if (const auto *mouseWheel = event.getIf<sf::Event::MouseWheelScrolled>())
        {
            if (mouseWheel->wheel == sf::Mouse::Wheel::Vertical)
            {
                float factor = (mouseWheel->delta > 0) ? 0.9f : 1.1f;

                viewState.zoomLevel *= factor;
                viewState.lastMousePos = sf::Vector2i(window.mapPixelToCoords(mouseWheel->position, *view));
            }
        }

        if (const auto *mouseButton = event.getIf<sf::Event::MouseButtonPressed>())
        {
            if (mouseButton->button == sf::Mouse::Button::Left)
            {
                viewState.isDragging = true;
                viewState.lastMousePos = mouseButton->position;
            }
        }

        if (const auto *mouseButton = event.getIf<sf::Event::MouseButtonReleased>())
        {
            if (mouseButton->button == sf::Mouse::Button::Left)
            {
                viewState.isDragging = false;
            }
        }

        if (const auto *mouseMove = event.getIf<sf::Event::MouseMoved>())
        {
            if (viewState.isDragging)
            {
                sf::Vector2f oldPos = window.mapPixelToCoords(viewState.lastMousePos, *view);
                sf::Vector2f newPos = window.mapPixelToCoords(mouseMove->position, *view);

                viewState.center += (oldPos - newPos);
                viewState.lastMousePos = mouseMove->position;
            }
        }
    }

    void update(sf::RenderWindow &window)
    {
        viewState.applyTo(*view, window);
        window.setView(*view);

        const float TILE_WIDTH = 128.0f;
        const float TILE_HEIGHT = 64.0f;

        for (auto &square : lotpack.squareMap)
        {
            if (square.coord.z() != 2)
                continue;

            int chunkX = square.coord.chunk_idx() / constants::CELL_SIZE_IN_BLOCKS;
            int chunkY = square.coord.chunk_idx() % constants::CELL_SIZE_IN_BLOCKS;

            if (chunkX > 5 || chunkY > 5)
                continue;

            int gx = square.coord.x() + chunkX * constants::BLOCK_SIZE_IN_SQUARE;
            int gy = square.coord.y() + chunkY * constants::BLOCK_SIZE_IN_SQUARE;
            int gz = square.coord.z();

            float screenX = (gx - gy) * (TILE_WIDTH / 2.0f);
            float screenY = (gx + gy) * (TILE_HEIGHT / 2.0f) - gz * (TILE_HEIGHT * 3.0f);

            for (auto &spriteId : square.tiles)
            {
                std::string &spriteName = lotheader.tileNames[spriteId];
                TexturePack::Texture *textureData = tilesheetService->getTextureByName(spriteName);

                if (textureData == nullptr)
                    continue;

                auto sprite = createSprite(textureData);

                if (!sprite.has_value())
                    continue;

                sf::Vector2i posInSheet = { textureData->x, textureData->y };
                sf::Vector2i texSize = { textureData->width, textureData->height };

                sprite->setTextureRect(sf::IntRect(posInSheet, texSize));
                sprite->setOrigin({ 0, 0 });
                sprite->setPosition({ screenX + textureData->ox, screenY + textureData->oy });

                window.draw(*sprite);
            }
        }

        firstFrame = false;
    }
};