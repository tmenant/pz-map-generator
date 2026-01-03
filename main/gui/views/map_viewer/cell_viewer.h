#pragma once

#include <SFML/Graphics/VertexBuffer.hpp>
#include <cstdint>
#include <unordered_map>
#include <vector>

#include <SFML/Graphics.hpp>

#include "algorithms/rect_pack/rect_structs.h"
#include "files/lotheader.h"
#include "files/lotpack.h"
#include "files/texturepack.h"
#include "services/tilesheet_service.h"

class CellViewer
{
private:
    LotHeader lotheader;
    Lotpack lotpack;

    sf::Texture atlasTexture;

    std::vector<TexturePack::Texture *> spriteDatas;
    std::vector<rectpack2D::rect_xywh> rectangles;
    std::unordered_map<int8_t, sf::VertexBuffer> vertexBuffers;

public:
    CellViewer(int x, int y, TilesheetService *tilesheetService);

    int minLayer() { return lotheader.minLayer; }
    int maxLayer() { return lotheader.maxLayer; }

    void packCellSprites(TilesheetService *tilesheetService);
    void preComputeSprites();

    int update(sf::RenderWindow &window, int currentLayer);
};