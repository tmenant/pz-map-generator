#include <fmt/base.h>

#include "constants.h"
#include "io/binary_reader.h"
#include "lotheader.h"

LotHeader LotHeader::read(const std::string &filename)
{
    throw std::runtime_error("not implemented");
}

LotHeader LotHeader::read(const BytesBuffer &buffer)
{
    LotHeader header = LotHeader();
    size_t offset = 0;

    header.magic = BinaryReader::read_n_chars(buffer, 4, offset);
    header.version = BinaryReader::readInt32(buffer, offset);
    header.tileNames = LotHeader::readTileNames(buffer, offset);
    header.rooms = LotHeader::readRooms(buffer, offset);
    header.buildings = LotHeader::readBuildings(buffer, offset);
    header.spawns = BinaryReader::readExact(buffer, constants::BLOCKS_PER_CELL, offset);

    if (offset != buffer.size())
    {
        throw std::runtime_error("File end not reached.");
    }

    return header;
}

std::vector<std::string> LotHeader::readTileNames(const BytesBuffer &buffer, size_t &offset)
{
    uint32_t tilesCount = BinaryReader::readInt32(buffer, offset);

    std::vector<std::string> tilenames(tilesCount);

    for (uint32_t i = 0; i < tilesCount; i++)
    {
        tilenames[i] = BinaryReader::readLineTrimmed(buffer, offset);
    }

    return tilenames;
}

std::vector<Room> LotHeader::readRooms(const BytesBuffer &buffer, size_t &offset)
{
    uint32_t roomsCount = BinaryReader::readInt32(buffer, offset);

    std::vector<Room> rooms(roomsCount);

    for (uint32_t i = 0; i < roomsCount; i++)
    {
        Room room = Room();

        room.id = i;
        room.name = BinaryReader::readLineTrimmed(buffer, offset);
        room.layer = BinaryReader::readInt32(buffer, offset);
        room.rectangles = LotHeader::readRectangles(buffer, offset);
        room.roomObjects = LotHeader::readRoomObjects(buffer, offset);
        room.area = 0;

        for (const Rectangle &rect : room.rectangles)
        {
            room.area += rect.width * rect.heigth;
        }

        rooms[i] = room;
    }

    return rooms;
}

std::vector<Rectangle> LotHeader::readRectangles(const BytesBuffer &buffer, size_t &offset)
{
    uint32_t rectanglesCount = BinaryReader::readInt32(buffer, offset);

    std::vector<Rectangle> rectangles(rectanglesCount);

    for (uint32_t i = 0; i < rectanglesCount; i++)
    {
        Rectangle rect = Rectangle();

        rect.x = BinaryReader::readInt32(buffer, offset);
        rect.y = BinaryReader::readInt32(buffer, offset);
        rect.width = BinaryReader::readInt32(buffer, offset);
        rect.heigth = BinaryReader::readInt32(buffer, offset);

        rectangles[i] = rect;
    }

    return rectangles;
}

std::vector<RoomObject> LotHeader::readRoomObjects(const BytesBuffer &buffer, size_t &offset)
{
    uint32_t objectsCount = BinaryReader::readInt32(buffer, offset);

    std::vector<RoomObject> roomObjects(objectsCount);

    for (uint32_t i = 0; i < objectsCount; i++)
    {
        RoomObject roomObject = RoomObject();

        roomObject.room_type = BinaryReader::readInt32(buffer, offset);
        roomObject.x = BinaryReader::readInt32(buffer, offset);
        roomObject.y = BinaryReader::readInt32(buffer, offset);

        roomObjects[i] = roomObject;
    }

    return roomObjects;
}

std::vector<Building> LotHeader::readBuildings(const BytesBuffer &buffer, size_t &offset)
{
    uint32_t buildingsCount = BinaryReader::readInt32(buffer, offset);

    std::vector<Building> buildings(buildingsCount);

    for (uint32_t i = 0; i < buildingsCount; i++)
    {
        uint32_t roomsCount = BinaryReader::readInt32(buffer, offset);

        Building building = Building();

        building.id = i;
        building.room_ids = std::vector<uint32_t>(roomsCount);

        for (uint32_t j = 0; j < roomsCount; j++)
        {
            building.room_ids[j] = BinaryReader::readInt32(buffer, offset);
        }

        buildings[i] = building;
    }

    return buildings;
}

BytesBuffer LotHeader::readSpawns(const BytesBuffer &buffer, size_t &offset)
{
    return BytesBuffer();
}
