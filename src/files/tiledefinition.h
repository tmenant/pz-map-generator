#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "types.h"

struct TileData
{
    std::string name;
    int32_t spriteID;
    std::unordered_map<std::string, std::string> properties;
};

struct TileSheet
{
    std::string name;
    std::string imageName;
    int32_t tileWidth;
    int32_t tileHeight;
    int32_t number;
    int32_t tilesCount;
    std::vector<TileData> tileDatas;
};

class TileDefinition
{
public:
    std::string magic;
    uint32_t version;
    std::vector<TileSheet> tileSheets;

    TileDefinition() = default;

    static TileDefinition read(const BytesBuffer &buffer);
    static std::vector<TileSheet> readTileSheets(const BytesBuffer &buffer, size_t &offset);
    static std::unordered_map<std::string, std::string> readProperties(const BytesBuffer &buffer, size_t &offset);
    static int32_t generateSpriteID();
};