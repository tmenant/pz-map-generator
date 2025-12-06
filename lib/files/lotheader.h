#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "types.h"

class LotHeader
{
public:
    struct Rectangle
    {
        uint32_t x;
        uint32_t y;
        uint32_t width;
        uint32_t heigth;
    };

    struct RoomObject
    {
        uint32_t room_type;
        uint32_t x;
        uint32_t y;
    };

    struct Building
    {
        uint32_t id;
        std::vector<uint32_t> room_ids;
    };

    struct Room
    {
        uint32_t id;
        std::string name;
        uint32_t layer;
        uint32_t area;
        std::vector<Rectangle> rectangles;
        std::vector<RoomObject> roomObjects;
    };

    std::string magic = "";
    int32_t version = 0;
    int32_t width = 0;
    int32_t height = 0;
    int32_t maxLayer = 0;
    int32_t minLayer = 0;

    std::vector<std::string> tileNames;
    std::vector<Room> rooms;
    std::vector<Building> buildings;
    std::vector<uint8_t> spawns;

    LotHeader() = default;

    static LotHeader read(const std::string &filename);
    static LotHeader read(const BytesBuffer &buffer);

private:
    static std::vector<std::string> readTileNames(const BytesBuffer &buffer, size_t &offset);
    static std::vector<Room> readRooms(const BytesBuffer &buffer, size_t &offset);
    static std::vector<Rectangle> readRectangles(const BytesBuffer &buffer, size_t &offset);
    static std::vector<RoomObject> readRoomObjects(const BytesBuffer &buffer, size_t &offset);
    static std::vector<Building> readBuildings(const BytesBuffer &buffer, size_t &offset);
    static BytesBuffer readSpawns(const BytesBuffer &buffer, size_t &offset);
};