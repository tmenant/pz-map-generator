#define DOCTEST_CONFIG_NO_CAPTURE_STDOUT

#include <filesystem>
#include <string>

#include <doctest/doctest.h>
#include <fmt/format.h>

#include "math/vector2i.h"
#include <constants.h>
#include <files/lotheader.h>
#include <io/file_reader.h>
#include <math/md5.h>
#include <types.h>


namespace fs = std::filesystem;

TEST_CASE("Muldraugh lotheaders B42")
{
    std::string mapDirectory = constants::GAME_PATH_B42 + "/media/maps/Muldraugh, KY";

    for (auto &entry : fs::directory_iterator(mapDirectory))
    {
        fs::path path = entry.path();

        if (path.extension().string() == constants::LOTHEADER_EXT)
        {
            BytesBuffer buffer = FileReader::read(path.string());
            std::string md5Hash = MD5::toHash(buffer);

            LotHeader header = LotHeader::read(buffer, Vector2i(0, 0));

            CHECK_EQ(header.magic, "LOTH");
            CHECK_EQ(header.version, 1);
            CHECK_EQ(header.position.x, 0);
            CHECK_EQ(header.position.y, 0);
            CHECK_EQ(header.width, 8);
            CHECK_EQ(header.height, 8);
        }
    }
}

TEST_CASE("getPositionFromFilename")
{
    SUBCASE("File '10_20.lotheader'")
    {
        Vector2i pos = LotHeader::getPositionFromFilename("10_20.lotheader");
        CHECK(pos.x == 10);
        CHECK(pos.y == 20);
    }

    SUBCASE("File '0_0.lotheader'")
    {
        Vector2i pos = LotHeader::getPositionFromFilename("0_0.lotheader");
        CHECK(pos.x == 0);
        CHECK(pos.y == 0);
    }

    SUBCASE("File with big number '12345_67890.lotheader'")
    {
        Vector2i pos = LotHeader::getPositionFromFilename("12345_67890.lotheader");
        CHECK(pos.x == 12345);
        CHECK(pos.y == 67890);
    }

    SUBCASE("File path")
    {
        Vector2i pos = LotHeader::getPositionFromFilename("Path/with/number (x86)/To/32_64.lotheader");
        CHECK(pos.x == 32);
        CHECK(pos.y == 64);
    }

    SUBCASE("File without underscore")
    {
        CHECK_THROWS_AS(LotHeader::getPositionFromFilename("1020.lotheader"), std::runtime_error);
    }

    SUBCASE("File with letters instead of coords")
    {
        CHECK_THROWS_AS(LotHeader::getPositionFromFilename("a_b.lotheader"), std::runtime_error);
    }

    SUBCASE("File with invalid extension")
    {
        CHECK_THROWS_AS(LotHeader::getPositionFromFilename("10_20.txt"), std::runtime_error);
    }
}