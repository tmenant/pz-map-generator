#include "io/binary_reader.h"
#include <doctest/doctest.h>
#include <stdexcept>

TEST_SUITE("BinaryReader")
{
    TEST_CASE("read_n_chars")
    {
        BytesBuffer buf = { 'H', 'e', 'l', 'l', 'o' };
        size_t offset = 0;

        // normal read
        std::string result = BinaryReader::read_n_chars(buf, 5, offset);
        CHECK(result == "Hello");
        CHECK(offset == 5);

        // reading beyond buffer should throw
        offset = 3;
        CHECK_THROWS_AS(BinaryReader::read_n_chars(buf, 5, offset), std::runtime_error);
    }

    TEST_CASE("readInt32")
    {
        BytesBuffer buf = { 0x78, 0x56, 0x34, 0x12 }; // little-endian 0x12345678
        size_t offset = 0;

        int32_t value = BinaryReader::readInt32(buf, offset);
        CHECK(value == 0x12345678);
        CHECK(offset == 4);

        // reading beyond buffer should throw
        offset = 2;
        CHECK_THROWS_AS(BinaryReader::readInt32(buf, offset), std::runtime_error);
    }

    TEST_CASE("readLineTrimmed")
    {
        BytesBuffer buf = { 'H', 'e', 'l', 'l', 'o', '\n', 'W', 'o', 'r', 'l', 'd', '\n' };
        size_t offset = 0;

        std::string line1 = BinaryReader::readLineTrimmed(buf, offset);
        CHECK(line1 == "Hello");
        CHECK(offset == 6);

        std::string line2 = BinaryReader::readLineTrimmed(buf, offset);
        CHECK(line2 == "World");
        CHECK(offset == 12);

        // no newline left
        CHECK_THROWS_AS(BinaryReader::readLineTrimmed(buf, offset), std::runtime_error);
    }

    TEST_CASE("readStringWithLength")
    {
        // string length = 5, data = "Hello"
        BytesBuffer buf = { 5, 0, 0, 0, 'H', 'e', 'l', 'l', 'o' };
        size_t offset = 0;

        std::string result = BinaryReader::readStringWithLength(buf, offset);
        CHECK(result == "Hello");
        CHECK(offset == 9);

        // length bigger than buffer
        buf = { 10, 0, 0, 0, 'H', 'i' };
        offset = 0;
        CHECK_THROWS_AS(BinaryReader::readStringWithLength(buf, offset), std::runtime_error);
    }

    TEST_CASE("readBytesWithLength")
    {
        // length = 4, data = {1,2,3,4}
        BytesBuffer buf = { 4, 0, 0, 0, 1, 2, 3, 4 };
        size_t offset = 0;

        BytesBuffer result = BinaryReader::readBytesWithLength(buf, offset);
        CHECK(result.size() == 4);
        CHECK(result[0] == 1);
        CHECK(result[3] == 4);
        CHECK(offset == 8);

        // length bigger than buffer
        buf = { 10, 0, 0, 0, 1, 2, 3 };
        offset = 0;
        CHECK_THROWS_AS(BinaryReader::readBytesWithLength(buf, offset), std::runtime_error);
    }

    TEST_CASE("readExact")
    {
        BytesBuffer buf = { 1, 2, 3, 4, 5 };
        size_t offset = 1;

        BytesBuffer result = BinaryReader::readExact(buf, 3, offset);
        CHECK(result.size() == 3);
        CHECK(result[0] == 2);
        CHECK(result[2] == 4);
        CHECK(offset == 4);

        // reading beyond buffer
        offset = 4;
        CHECK_THROWS_AS(BinaryReader::readExact(buf, 2, offset), std::runtime_error);
    }

    TEST_CASE("readUntil extracts correct slice and updates offset")
    {
        BytesBuffer buffer = { 0x01, 0x02, 0x03, 0xAA, 0xBB, 0xCC, 0x04 };
        BytesBuffer pattern = { 0xAA, 0xBB };

        size_t offset = 0;

        auto result = BinaryReader::readUntil(buffer, pattern, offset);

        BytesBuffer expected = { 0x01, 0x02, 0x03 };

        CHECK(result == expected);
        CHECK(offset == 5);
    }

    TEST_CASE("readUntil throws when pattern not found")
    {
        BytesBuffer buffer = { 0x10, 0x20, 0x30 };
        BytesBuffer pattern = { 0xAA, 0xBB };

        size_t offset = 0;

        CHECK_THROWS_AS(BinaryReader::readUntil(buffer, pattern, offset),
            std::runtime_error);
    }
}
