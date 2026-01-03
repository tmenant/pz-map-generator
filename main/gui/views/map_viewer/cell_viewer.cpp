#include "cell_viewer.h"

#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/VertexBuffer.hpp>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include <fmt/base.h>

#include "algorithms/rect_pack/rect_structs.h"
#include "algorithms/rect_pack/rectpack_2d.h"
#include "files/texturepack.h"

#include "constants.h"
#include "services/map_files_service.h"
#include "timer.h"

CellViewer::CellViewer(int x, int y, TilesheetService *tilesheetService)
{
    MapFilesService mapFileService(constants::GAME_PATH, MapNames::Muldraugh);

    lotheader = mapFileService.LoadLotheaderByPosition(x, y);
    lotpack = mapFileService.LoadLotpackByPosition(x, y, &lotheader);

    packCellSprites(tilesheetService);
    preComputeSprites();
}

void CellViewer::packCellSprites(TilesheetService *tilesheetService)
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
    std::unordered_map<uint8_t, std::vector<sf::Vertex>> vertexArrays;

    for (int layer = lotheader.minLayer; layer < lotheader.maxLayer; layer++)
    {
        vertexArrays[layer] = {};
    }

    for (auto &square : lotpack.squareMap)
    {
        int chunkX = square.coord.chunk_idx() / constants::CELL_SIZE_IN_BLOCKS;
        int chunkY = square.coord.chunk_idx() % constants::CELL_SIZE_IN_BLOCKS;

        int gx = square.coord.x() + chunkX * constants::BLOCK_SIZE_IN_SQUARE + lotheader.position.x * constants::CELL_SIZE_IN_SQUARE;
        int gy = square.coord.y() + chunkY * constants::BLOCK_SIZE_IN_SQUARE + lotheader.position.y * constants::CELL_SIZE_IN_SQUARE;
        int gz = square.coord.z();

        float screenX = (gx - gy) * (constants::TILE_WIDTH / 2);
        float screenY = (gx + gy) * (constants::TILE_HEIGHT / 2) - gz * (constants::TILE_HEIGHT * 3);

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
            vertexArrays[gz].push_back(v0);
            vertexArrays[gz].push_back(v2);
            vertexArrays[gz].push_back(v1);

            // Triangle 2
            vertexArrays[gz].push_back(v1);
            vertexArrays[gz].push_back(v2);
            vertexArrays[gz].push_back(v3);
        }
    }

    vertexBuffers.clear();
    vertexBuffers.reserve(lotheader.maxLayer - lotheader.minLayer);

    for (int layer = lotheader.minLayer; layer < lotheader.maxLayer; layer++)
    {
        vertexBuffers[layer] = sf::VertexBuffer(sf::PrimitiveType::Triangles);

        if (!vertexBuffers[layer].create(vertexArrays[layer].size())) continue;
        if (!vertexBuffers[layer].update(vertexArrays[layer].data())) continue;
    }
}

int CellViewer::update(sf::RenderWindow &window, int currentLayer)
{
    int drawCalls = 0;

    for (int layer = lotheader.minLayer; layer < lotheader.maxLayer; layer++)
    {
        if (layer <= currentLayer)
        {
            window.draw(vertexBuffers[layer], &atlasTexture);
            drawCalls++;
        }
    }

    return drawCalls;
}
