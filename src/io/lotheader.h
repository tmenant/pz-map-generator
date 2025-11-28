#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <vector>

#include "types.h"

struct Room;
struct Building;

class LotHeaderFile
{
public:
    int CellSizeInBlock = 0;
    int BlockSizeInSquare = 0;
    int MinLayer = 0;
    int MaxLayer = 0;
    int Version = 0;
    int Width = 0;
    int Height = 0;

    std::vector<std::string> TileNames;
    std::vector<Room> Rooms;
    std::vector<Building> Buildings;
    std::vector<uint8_t> ZombieSpawns;

    LotHeaderFile() = default;

    static LotHeaderFile read(const std::string &filename);
    static LotHeaderFile read(const BytesBuffer &buffer);

    void read_magic(const BytesBuffer &buffer);
};