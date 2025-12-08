#include <fmt/format.h>
#include <vector>

#include "constants.h"
#include "exceptions.h"
#include "io/binary_reader.h"
#include "io/file_reader.h"
#include "lotpack.h"
#include "types.h"

Lotpack::Lotpack(const LotHeader *header) : header(header) {}

Lotpack Lotpack::read(const std::string &filename, const LotHeader *header)
{
    BytesBuffer buffer = FileReader::read(filename);

    return read(buffer, header);
}

Lotpack Lotpack::read(const BytesBuffer &buffer, const LotHeader *header)
{
    Lotpack lotpack(header);
    size_t offset = 0;

    lotpack.magic = BinaryReader::read_n_chars(buffer, 4, offset);
    lotpack.version = BinaryReader::readInt32(buffer, offset);
    lotpack.readSquareMap(buffer, offset);

    if (offset != buffer.size())
    {
        throw Exceptions::FileEndNotReached(offset, buffer.size());
    }

    return lotpack;
}

void Lotpack::readSquareMap(const BytesBuffer &buffer, size_t &offset)
{
    squareMap = std::vector<SquareData>();

    uint32_t blocksCount = BinaryReader::readInt32(buffer, offset);
    uint32_t tableOffset = static_cast<uint32_t>(offset);

    for (uint16_t blockIndex = 0; blockIndex < blocksCount; blockIndex++)
    {
        offset = tableOffset + blockIndex * 8;
        offset = BinaryReader::readInt32(buffer, offset);

        this->readBlockSquares(buffer, blockIndex, offset);
    }
}

void Lotpack::readBlockSquares(const BytesBuffer &buffer, uint16_t blockIndex, size_t &offset)
{
    int32_t skip = 0;

    for (int8_t z = 0; z < header->maxLayer - header->minLayer; z++)
    {
        if (skip >= constants::SQUARE_PER_BLOCK)
        {
            skip -= constants::SQUARE_PER_BLOCK;
            continue;
        }
        for (int8_t x = 0; x < constants::BLOCK_SIZE_IN_SQUARE; x++)
        {
            if (skip >= constants::BLOCK_SIZE_IN_SQUARE)
            {
                skip -= constants::BLOCK_SIZE_IN_SQUARE;
                continue;
            }
            for (int8_t y = 0; y < constants::BLOCK_SIZE_IN_SQUARE; y++)
            {
                if (skip > 0)
                {
                    skip--;
                    continue;
                }

                int32_t count = BinaryReader::readInt32(buffer, offset);

                if (count == -1)
                {
                    skip = BinaryReader::readInt32(buffer, offset);

                    if (skip > 0)
                    {
                        skip--;
                        continue;
                    }
                }
                else if (count > 1)
                {

                    SquareData squareData = Lotpack::readSquare(buffer, count - 1, offset);
                    squareData.coord = CellCoord(blockIndex, x, y, z);

                    squareMap.push_back(squareData);
                }
            }
        }
    }
}

SquareData Lotpack::readSquare(const BytesBuffer &buffer, int32_t count, size_t &offset)
{
    SquareData squareData{};

    squareData.roomId = BinaryReader::readInt32(buffer, offset);
    squareData.tiles = std::vector<int32_t>(count);

    for (int i = 0; i < count; i++)
    {
        squareData.tiles[i] = BinaryReader::readInt32(buffer, offset);
    }

    return squareData;
}
