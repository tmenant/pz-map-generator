#include "cell_viewer.h"

#include <fmt/base.h>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <optional>
#include <vector>

#include "constants.h"

CellViewer::CellViewer(sf::View *view, MapFilesService *mapFileService, TilesheetService *tilesheetService, int x, int y)
{
    this->lotheader = mapFileService->LoadLotheaderByPosition(x, y);
    this->lotpack = mapFileService->LoadLotpackByPosition(x, y, &lotheader);
    this->tilesheetService = tilesheetService;

    this->viewState = ViewState();
    this->view = view;

    this->texturesByName = std::unordered_map<std::string, std::optional<sf::Texture>>{};
    this->spritesNotFound = std::unordered_set<std::string>{};

    preComputeSprites();
}

void CellViewer::preComputeSprites()
{
    renderTiles.clear();

    const float TILE_WIDTH = 128.0f;
    const float TILE_HEIGHT = 64.0f;

    for (auto &square : lotpack.squareMap)
    {
        int chunkX = square.coord.chunk_idx() / constants::CELL_SIZE_IN_BLOCKS;
        int chunkY = square.coord.chunk_idx() % constants::CELL_SIZE_IN_BLOCKS;

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

            sf::Sprite sprite = createSprite(textureData);

            sf::Vector2i posInSheet = { textureData->x, textureData->y };
            sf::Vector2i texSize = { textureData->width, textureData->height };

            sprite.setTextureRect(sf::IntRect(posInSheet, texSize));
            sprite.setOrigin({ 0, 0 });
            sprite.setPosition({ screenX + textureData->ox, screenY + textureData->oy });

            renderTiles.push_back({ square.coord.z(), sprite });
        }
    }
}

sf::Texture *CellViewer::getOrCreateTexture(const std::string &textureName)
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

    texturesByName[page->name] = texture;
    return &texturesByName[page->name].value();
}

sf::Sprite CellViewer::createSprite(TexturePack::Texture *textureData)
{
    assert(textureData != nullptr);

    sf::Texture *texture = getOrCreateTexture(textureData->name);

    if (texture == nullptr)
        throw std::runtime_error("sprite not found");

    sf::Sprite sprite(*texture);

    return sprite;
}

void CellViewer::handleEvents(const sf::Event &event, sf::RenderWindow &window)
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

    if (const auto *keyPressed = event.getIf<sf::Event::KeyPressed>())
    {
        if (keyPressed->code == sf::Keyboard::Key::Up)
        {
            currentLayer = Math::fastMin(currentLayer + 1, lotheader.maxLayer + 1);
        }
        else if (keyPressed->code == sf::Keyboard::Key::Down)
        {
            currentLayer = Math::fastMax(currentLayer - 1, lotheader.minLayer + 1);
        }
    }
}

void CellViewer::update(sf::RenderWindow &window)
{
    viewState.applyTo(*view, window);
    window.setView(*view);

    for (auto &renderTile : renderTiles)
    {
        if (renderTile.layer <= currentLayer)
        {
            window.draw(renderTile.sprite);
        }
    }

    firstFrame = false;
}
