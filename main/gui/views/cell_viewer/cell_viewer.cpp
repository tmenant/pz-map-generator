#include "cell_viewer.h"

#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include <fmt/base.h>

#include "TGUI/Backend/SFML-Graphics.hpp"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

#include "algorithms/rect_pack/rectpack_2d.h"
#include "files/texturepack.h"
#include "gui/components/debug_panel.h"

#include "constants.h"
#include "timer.h"
#include "types.h"

CellViewer::CellViewer(sf::View *view, MapFilesService *mapFileService, TilesheetService *tilesheetService, tgui::Gui &gui, int x, int y) :
        debugPanel(gui)
{
    this->lotheader = mapFileService->LoadLotheaderByPosition(x, y);
    this->lotpack = mapFileService->LoadLotpackByPosition(x, y, &lotheader);
    this->tilesheetService = tilesheetService;

    this->viewState = ViewState();
    this->view = view;

    packCellSprites();
    preComputeSprites();
}

void CellViewer::packCellSprites()
{
    auto timer = Timer::start();
    auto tilesCount = lotheader.tileNames.size();

    std::unordered_map<TexturePack::Page *, std::vector<size_t>> groupedSprites = {};
    std::vector<rectpack2D::rect_xywh> rectangles(tilesCount);
    std::vector<sf::Image> sprites(tilesCount);

    for (size_t i = 0; i < tilesCount; i++)
    {
        const std::string &tilename = lotheader.tileNames[i];
        TexturePack::Texture *textureData = tilesheetService->getTextureByName(tilename);

        if (textureData == nullptr)
        {
            fmt::println("texture not found: '{}'", tilename);
            continue;
        }

        auto page = tilesheetService->getPageByTextureName(tilename);
        if (page == nullptr)
        {
            throw std::runtime_error("page not found: " + page->name);
        }

        if (!groupedSprites.contains(page))
        {
            groupedSprites[page] = {};
        }

        groupedSprites[page].push_back(i);

        rectangles[i] = rectpack2D::rect_xywh(0, 0, textureData->width, textureData->height);
    }

    auto atlasSize = rectpack2D::packRectangles(rectangles);
    auto packedSize = atlasSize.w * atlasSize.h * 4.f / 1024 / 1024;

    fmt::println("{} tiles packed in {:.3f}ms, AtlasSize = {}x{} ({:.1f}Mo)", tilesCount, timer.elapsedMiliseconds(true), atlasSize.w, atlasSize.h, packedSize);

    sf::Image atlasTexture;
    atlasTexture.resize({ (uint32_t)atlasSize.w, (uint32_t)atlasSize.h });

    for (const auto &entry : groupedSprites)
    {
        sf::Image image = loadTexture(entry.first);

        for (const auto &index : entry.second)
        {
            std::string tilename = lotheader.tileNames[index];
            TexturePack::Texture *textureData = tilesheetService->getTextureByName(tilename);

            if (textureData == nullptr)
                continue;

            sf::IntRect areaToCopy = sf::IntRect({ textureData->x, textureData->y }, { textureData->width, textureData->height });
            sf::Vector2u atlasPos = { (uint32_t)rectangles[index].x, (uint32_t)rectangles[index].y };

            if (!atlasTexture.copy(image, atlasPos, areaToCopy, false))
            {
                // throw std::runtime_error("failed to copy");
                continue;
            }
        }
    }

    fmt::println("{} textures loaded in {:.1f}ms", groupedSprites.size(), timer.elapsedMiliseconds());

    // if (!atlasTexture.saveToFile("ignore/atlas.png"))
    // {
    //     throw std::runtime_error("failed saving atlas");
    // }
}

sf::Image CellViewer::loadTexture(TexturePack::Page *page)
{
    sf::Image texture;

    if (!texture.loadFromMemory(page->png.data(), page->png.size()))
    {
        throw std::runtime_error("Failed loading page: " + page->name);
    }

    return texture;
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

        for (int32_t &spriteId : square.tiles)
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

    Timer timer = Timer::start();

    for (auto &renderTile : renderTiles)
    {
        if (renderTile.layer <= currentLayer)
        {
            window.draw(renderTile.sprite);
        }
    }

    firstFrame = false;

    debugPanel.setFPS(1000 / timer.elapsedMiliseconds());
}
