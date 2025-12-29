#include <regex>

#include "constants.h"
#include "exceptions.h"
#include "io/binary_reader.h"
#include "io/file_reader.h"
#include "lotheader.h"
#include "math/vector2i.h"
#include "types.h"

LotHeader LotHeader::read(const std::string &filename)
{
    Vector2i cellPosition = getPositionFromFilename(filename);
    BytesBuffer buffer = FileReader::read(filename);

    return read(buffer, cellPosition);
}

LotHeader LotHeader::read(const BytesBuffer &buffer, Vector2i position)
{
    LotHeader header = LotHeader();
    size_t offset = 0;

    header.position = position;
    header.magic = BinaryReader::read_n_chars(buffer, 4, offset);
    header.version = BinaryReader::readInt32(buffer, offset);
    header.tileNames = LotHeader::readTileNames(buffer, offset);
    header.width = BinaryReader::readInt32(buffer, offset);
    header.height = BinaryReader::readInt32(buffer, offset);
    header.minLayer = BinaryReader::readInt32(buffer, offset);
    header.maxLayer = BinaryReader::readInt32(buffer, offset) + 1;
    header.rooms = LotHeader::readRooms(buffer, offset);
    header.buildings = LotHeader::readBuildings(buffer, offset);
    header.spawns = BinaryReader::readExact(buffer, constants::BLOCKS_PER_CELL, offset);

    if (offset != buffer.size())
    {
        throw Exceptions::FileEndNotReached(offset, buffer.size());
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

std::vector<LotHeader::Room> LotHeader::readRooms(const BytesBuffer &buffer, size_t &offset)
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

std::vector<LotHeader::Rectangle> LotHeader::readRectangles(const BytesBuffer &buffer, size_t &offset)
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

std::vector<LotHeader::RoomObject> LotHeader::readRoomObjects(const BytesBuffer &buffer, size_t &offset)
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

std::vector<LotHeader::Building> LotHeader::readBuildings(const BytesBuffer &buffer, size_t &offset)
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
    throw Exceptions::NotImplemented();
}

Vector2i LotHeader::getPositionFromFilename(const std::string& filename)
{
    static const std::regex pattern(R"((\d+)_(\d+)\.lotheader)");

    std::smatch match;
    if (!std::regex_search(filename, match, pattern))
        throw std::runtime_error("Impossible d'extraire la position depuis : " + filename);

    return { std::stoi(match[1]), std::stoi(match[2]) };
}