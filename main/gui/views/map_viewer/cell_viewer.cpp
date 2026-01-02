#include "cell_viewer.h"

#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include <fmt/base.h>

#include "TGUI/Backend/SFML-Graphics.hpp"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

#include "algorithms/rect_pack/rect_structs.h"
#include "algorithms/rect_pack/rectpack_2d.h"
#include "files/texturepack.h"
#include "gui/components/debug_panel.h"

#include "constants.h"
#include "services/map_files_service.h"
#include "timer.h"

CellViewer::CellViewer(sf::View *view, TilesheetService *tilesheetService, tgui::Gui &gui, int x, int y) :
        debugPanel(gui)
{
    MapFilesService mapFileService(constants::GAME_PATH, MapNames::Muldraugh);

    this->lotheader = mapFileService.LoadLotheaderByPosition(x, y);
    this->lotpack = mapFileService.LoadLotpackByPosition(x, y, &lotheader);
    this->tilesheetService = tilesheetService;
    this->view = view;

    this->viewState.currentLayer = lotheader.maxLayer;
    this->viewState.center = { .0f, 128.f * 64 };

    packCellSprites();
    preComputeSprites();
}

void CellViewer::packCellSprites()
{
    auto timer = Timer::start();
    auto tilesCount = lotheader.tileNames.size();

    std::unordered_map<TexturePack::Page *, std::vector<size_t>> groupedSprites = {};
    std::vector<sf::Image> sprites(tilesCount);

    rectangles.resize(tilesCount);
    spriteDatas.resize(tilesCount, nullptr);

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

        spriteDatas[i] = textureData;
        rectangles[i] = rectpack2D::rect_xywh(0, 0, textureData->width, textureData->height);
    }

    auto atlasSize = rectpack2D::packRectangles(rectangles);
    auto packedSize = atlasSize.w * atlasSize.h * 4.f / 1024 / 1024;

    fmt::println("{} tiles packed in {:.3f}ms, AtlasSize = {}x{} ({:.1f}Mo)", tilesCount, timer.elapsedMiliseconds(true), atlasSize.w, atlasSize.h, packedSize);

    sf::Image atlasImage;
    atlasImage.resize({ (uint32_t)atlasSize.w, (uint32_t)atlasSize.h });

    for (const auto &entry : groupedSprites)
    {
        sf::Image &image = entry.first->image;

        for (const auto &index : entry.second)
        {
            std::string tilename = lotheader.tileNames[index];
            TexturePack::Texture *textureData = tilesheetService->getTextureByName(tilename);

            if (textureData == nullptr)
                continue;

            sf::IntRect areaToCopy = sf::IntRect({ textureData->x, textureData->y }, { textureData->width, textureData->height });
            sf::Vector2u atlasPos = { (uint32_t)rectangles[index].x, (uint32_t)rectangles[index].y };

            if (!atlasImage.copy(image, atlasPos, areaToCopy, false))
            {
                throw std::runtime_error("failed to copy");
            }
        }
    }

    if (!atlasTexture.loadFromImage(atlasImage))
    {
        throw std::runtime_error("failed loading atlas.");
    }

    fmt::println("{} textures loaded in {:.1f}ms", groupedSprites.size(), timer.elapsedMiliseconds());
}

void CellViewer::preComputeSprites()
{
    const float TILE_WIDTH = 128.0f;
    const float TILE_HEIGHT = 64.0f;

    vertexArrays.clear();
    vertexArrays.resize(lotheader.maxLayer - lotheader.minLayer);

    for (int i = 0; i < vertexArrays.size(); i++)
    {
        vertexArrays[i] = sf::VertexArray(sf::PrimitiveType::Triangles);
    }

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
            TexturePack::Texture *textureData = spriteDatas[spriteId];

            if (textureData == nullptr)
                continue;

            sf::Sprite sprite(atlasTexture);
            rectpack2D::rect_xywh &rectangle = rectangles[spriteId];

            float x = screenX + textureData->ox;
            float y = screenY + textureData->oy;
            float w = static_cast<float>(rectangle.w);
            float h = static_cast<float>(rectangle.h);

            float tx = static_cast<float>(rectangle.x);
            float ty = static_cast<float>(rectangle.y);
            float tw = static_cast<float>(rectangle.w);
            float th = static_cast<float>(rectangle.h);

            sf::Vertex v0 = {
                .position = { x, y },
                .texCoords = { tx, ty },
            };

            sf::Vertex v1 = {
                .position = { x, y + h },
                .texCoords = { tx, ty + th },
            };

            sf::Vertex v2 = {
                .position = { x + w, y },
                .texCoords = { tx + tw, ty },
            };

            sf::Vertex v3 = {
                .position = { x + w, y + h },
                .texCoords = { tx + tw, ty + th },
            };

            // Triangle 1
            vertexArrays[gz].append(v0);
            vertexArrays[gz].append(v2);
            vertexArrays[gz].append(v1);

            // Triangle 2
            vertexArrays[gz].append(v1);
            vertexArrays[gz].append(v2);
            vertexArrays[gz].append(v3);
        }
    }
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
            viewState.currentLayer = Math::fastMin(viewState.currentLayer + 1, lotheader.maxLayer - 1);
        }
        else if (keyPressed->code == sf::Keyboard::Key::Down)
        {
            viewState.currentLayer = Math::fastMax(viewState.currentLayer - 1, lotheader.minLayer);
        }
    }
}

void CellViewer::update(sf::RenderWindow &window)
{
    viewState.applyTo(*view, window);
    window.setView(*view);

    int drawCalls = 0;

    Timer timer = Timer::start();

    for (int layer = 0; layer < (lotheader.maxLayer - lotheader.minLayer); layer++)
    {
        if (layer <= viewState.currentLayer)
        {
            window.draw(vertexArrays[layer], &atlasTexture);
            drawCalls++;
        }
    }

    if (viewState.clock.getElapsedTime() > sf::milliseconds(750))
    {
        debugPanel.setFPS(1000 / timer.elapsedMiliseconds());
        debugPanel.setTimer(timer.elapsedMiliseconds());
        debugPanel.setDrawCalls(drawCalls);

        viewState.clock.restart();
    }

    viewState.firstFrame = false;
}
