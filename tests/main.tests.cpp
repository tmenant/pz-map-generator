#include "math/vector2i.h"
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <fmt/format.h>

#include "constants.h"
#include "files/lotheader.h"
#include "files/lotpack.h"
#include "files/texturepack.h"
#include "files/tiledefinition.h"
#include "io/file_reader.h"
#include "math/md5.h"
#include "types.h"

TEST_CASE("readGameFiles")
{
    BytesBuffer headerBuffer = FileReader::read(constants::LOTHEADER_PATH);
    BytesBuffer lotpackBuffer = FileReader::read(constants::LOTHPACK_PATH);
    BytesBuffer packBuffer = FileReader::read(constants::TEXTUREPACK_PATH);
    BytesBuffer tileDefBuffer = FileReader::read(constants::TILESDEF_PATH);

    std::string headerHash = MD5::toHash(headerBuffer);
    std::string lotpackHash = MD5::toHash(lotpackBuffer);

    LotHeader header = LotHeader::read(headerBuffer, Vector2i(0, 0));
    Lotpack lotpack = Lotpack::read(lotpackBuffer, &header);
    TexturePack texturePack = TexturePack::read("texurepack", packBuffer);
    TileDefinition tileDefinition = TileDefinition::read("", tileDefBuffer);

    fmt::println("magic: {}, version: {}, md5: {}", header.magic, header.version, headerHash);
    fmt::println("magic: {}, version: {}, md5: {}", lotpack.magic, lotpack.version, lotpackHash);
    fmt::println("magic: {}, version: {}", texturePack.magic, texturePack.version);

    fmt::println("squareMap size: {}", lotpack.squareMap.size());
    fmt::println("texture pages: {}", texturePack.pages.size());
    fmt::println("tilesheets: {}", tileDefinition.tileSheets.size());
}
