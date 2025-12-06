#define DOCTEST_CONFIG_NO_CAPTURE_STDOUT

#include <filesystem>
#include <string>

#include <doctest/doctest.h>
#include <fmt/format.h>

#include <constants.h>
#include <files/lotheader.h>
#include <io/file_reader.h>
#include <math/md5.h>
#include <types.h>

namespace fs = std::filesystem;

TEST_CASE("Lotheader")
{
    std::string mapDirectory = constants::GAME_PATH_B42 + "/media/maps/Muldraugh, KY";

    for (auto &entry : fs::directory_iterator(mapDirectory))
    {
        fs::path path = entry.path();

        if (path.extension().string() == constants::LOTHEADER_EXT)
        {
            BytesBuffer buffer = FileReader::read(path.string());
            std::string md5Hash = MD5::toHash(buffer);

            LotHeader header = LotHeader::read(buffer);

            CHECK_EQ(header.magic, "LOTH");
            CHECK_EQ(header.version, 1);
        }
    }
}