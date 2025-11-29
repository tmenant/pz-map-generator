#pragma once

#include <cstdint>
#include <functional>

struct CellCoord
{
private:
    uint32_t rawData;

public:
    explicit CellCoord(uint32_t data) : rawData(data) {}

    uint32_t getRawData() const { return rawData; }

    bool operator==(const CellCoord &other) const
    {
        return rawData == other.rawData;
    }
};

namespace std
{
    template <>
    struct hash<CellCoord>
    {
        std::size_t operator()(const CellCoord &c) const noexcept
        {
            return std::hash<uint32_t>()(c.getRawData());
        }
    };
}