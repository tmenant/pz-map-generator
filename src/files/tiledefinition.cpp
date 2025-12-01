#include <fmt/format.h>

#include "exceptions.h"
#include "io/binary_reader.h"
#include "tiledefinition.h"

TileDefinition TileDefinition::read(const BytesBuffer &buffer)
{
    TileDefinition tileDefinition{};
    size_t offset = 0;

    tileDefinition.magic = BinaryReader::read_n_chars(buffer, 4, offset);
    tileDefinition.version = BinaryReader::readInt32(buffer, offset);
    tileDefinition.tileSheets = TileDefinition::readTileSheets(buffer, offset);

    if (offset != buffer.size())
    {
        throw Exceptions::FileEndNotReached(offset, buffer.size());
    }

    return tileDefinition;
}

std::vector<TileSheet> TileDefinition::readTileSheets(const BytesBuffer &buffer, size_t &offset)
{
    int32_t tileSheetsCount = BinaryReader::readInt32(buffer, offset);
    std::vector<TileSheet> tileSheets(tileSheetsCount);

    for (int i = 0; i < tileSheetsCount; i++)
    {
        TileSheet tileSheet{};

        tileSheet.name = BinaryReader::readLineTrimmed(buffer, offset);
        tileSheet.imageName = BinaryReader::readLineTrimmed(buffer, offset);
        tileSheet.tileWidth = BinaryReader::readInt32(buffer, offset);
        tileSheet.tileHeight = BinaryReader::readInt32(buffer, offset);
        tileSheet.number = BinaryReader::readInt32(buffer, offset);
        tileSheet.tilesCount = BinaryReader::readInt32(buffer, offset);
        tileSheet.tileDatas = std::vector<TileData>(tileSheet.tilesCount);

        for (int j = 0; j < tileSheet.tilesCount; j++)
        {
            TileData tileData{};

            tileData.spriteID = TileDefinition::generateSpriteID();
            tileData.name = tileSheet.name + "_" + std::to_string(j);
            tileData.properties = TileDefinition::readProperties(buffer, offset);

            tileSheet.tileDatas[j] = tileData;
        }

        tileSheets[i] = tileSheet;
    }

    return tileSheets;
}

std::unordered_map<std::string, std::string> TileDefinition::readProperties(const BytesBuffer &buffer, size_t &offset)
{
    uint32_t propertiesCount = BinaryReader::readInt32(buffer, offset);
    std::unordered_map<std::string, std::string> properties(propertiesCount);

    for (int i = 0; i < propertiesCount; i++)
    {
        std::string name = BinaryReader::readLineTrimmed(buffer, offset);
        std::string value = BinaryReader::readLineTrimmed(buffer, offset);

        properties[name] = value;
    }

    return properties;
}

int32_t TileDefinition::generateSpriteID()
{
    return -1;
}