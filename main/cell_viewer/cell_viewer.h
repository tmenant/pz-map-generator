#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
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
#include <fmt/format.h>

class CellViewer
{
public:
    LotHeader lotheader;
    Lotpack lotpack;

    TilesheetService *tilesheetService;

    std::unordered_map<std::string, std::optional<sf::Texture>> texturesByName;
    std::unordered_set<std::string> spritesNotFound;

    CellViewer(MapFilesService *mapFileService, TilesheetService *tilesheetService, int x, int y)
    {
        this->lotheader = mapFileService->LoadLotheaderByPosition(x, y);
        this->lotpack = mapFileService->LoadLotpackByPosition(x, y, &lotheader);
        this->tilesheetService = tilesheetService;

        texturesByName = std::unordered_map<std::string, std::optional<sf::Texture>>{};
        spritesNotFound = std::unordered_set<std::string>{};

        std::sort(lotpack.squareMap.begin(), lotpack.squareMap.end(), [](SquareData a, SquareData b)
        {
            int depthA = (int)(a.coord.x() + a.coord.y() + a.coord.z());
            int depthB = (int)(b.coord.x() + b.coord.y() + b.coord.z());

            return depthA < depthB;
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

    void update(sf::RenderWindow &window)
    {
        sf::View view;
        view.setSize({ window.getSize().x * 1.f, window.getSize().y * 1.f });
        view.setCenter({ 0.f, 0.f });
        window.setView(view);

        const float TILE_WIDTH = 128.0f;
        const float TILE_HEIGHT = 64.0f;

        for (auto &square : lotpack.squareMap)
        {
            if (square.coord.chunk_idx() != 3 || square.coord.z() != 2)
                continue;

            int cx_local = square.coord.chunk_idx() % constants::CELL_SIZE_IN_BLOCKS;
            int cy_local = square.coord.chunk_idx() / constants::CELL_SIZE_IN_BLOCKS;

            int gx = square.coord.x(); // + cx_local * constants::BLOCK_SIZE_IN_SQUARE;
            int gy = square.coord.y(); // + cx_local * constants::BLOCK_SIZE_IN_SQUARE;
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
    }
};