#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

#include "core/cell_coord.h"
#include "files/lotheader.h"

struct SquareData
{
    uint32_t roomId;
    std::vector<int32_t> tiles;
};

class Lotpack
{
public:
    const LotHeader *header;

    std::string magic;
    uint32_t version;
    std::unordered_map<CellCoord, SquareData> squareMap;

    Lotpack() = default;
    Lotpack(const LotHeader *header);

    static Lotpack read(const std::string &filename, const LotHeader *header);
    static Lotpack read(const BytesBuffer &buffer, const LotHeader *header);

    void readSquareMap(const BytesBuffer &buffer, size_t &offset);
    void readBlockSquares(const BytesBuffer &buffer, uint16_t blockIndex, size_t &offset);
    static SquareData readSquare(const BytesBuffer &buffer, int32_t count,size_t &offset);
};
