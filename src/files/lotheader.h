#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <vector>

#include "types.h"

struct Room
{
    uint32_t id;
};

struct Building
{
    uint32_t id;
};

class LotHeader
{
public:
    std::string magic = "";
    int version = 0;
    int width = 0;
    int height = 0;

    std::vector<std::string> TileNames;
    std::vector<Room> Rooms;
    std::vector<Building> Buildings;
    std::vector<uint8_t> ZombieSpawns;

    LotHeader() = default;

    static LotHeader read(const std::string &filename);
    static LotHeader read(const BytesBuffer &buffer);
};