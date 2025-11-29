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
    std::vector<uint32_t> tiles;
};

class Lotpack
{
public:
    const LotHeader &header;

    std::string magic;
    uint32_t version;
    std::unordered_map<CellCoord, SquareData> squareMap;

    Lotpack(const LotHeader &header);

    static Lotpack read(const std::string &filename, const LotHeader &header);
    static Lotpack read(const BytesBuffer &buffer, const LotHeader &header);

    static std::unordered_map<CellCoord, SquareData> readSquareMap(const BytesBuffer &buffer, size_t offset);
};
