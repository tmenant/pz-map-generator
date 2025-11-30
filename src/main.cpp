#include <crtdbg.h>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include <fmt/format.h>
#include <cpptrace/from_current.hpp>

#include "files/lotheader.h"
#include "files/lotpack.h"
#include "files/texturepack.h"
#include "io/binary_reader.h"
#include "io/file_reader.h"
#include "math/md5.h"

const std::string LOTHEADER_PATH = "data/B42/27_38.lotheader";
const std::string LOTHPACK_PATH = "data/B42/world_27_38.lotpack";
const std::string TEXTUREPACK_PATH = "C:/SteamLibrary/steamapps/common/ProjectZomboidB42/media/texturepacks/Tiles2x.pack";

void read_header()
{
    BytesBuffer headerBuffer = FileReader::read(LOTHEADER_PATH);
    BytesBuffer lotpackBuffer = FileReader::read(LOTHPACK_PATH);
    BytesBuffer packBuffer = FileReader::read(TEXTUREPACK_PATH);

    std::string headerHash = MD5::toHash(headerBuffer);
    std::string lotpackHash = MD5::toHash(lotpackBuffer);

    LotHeader header = LotHeader::read(headerBuffer);
    Lotpack lotpack = Lotpack::read(lotpackBuffer, header);
    TexturePack texturePack = TexturePack::read(packBuffer);

    fmt::println("magic: {}, version: {}, md5: {}", header.magic, header.version, headerHash);
    fmt::println("magic: {}, version: {}, md5: {}", lotpack.magic, lotpack.version, lotpackHash);
    fmt::println("magic: {}, version: {}", texturePack.magic, texturePack.version);

    fmt::println("squareMap size: {}", lotpack.squareMap.size());
    fmt::println("texture pages: {}", texturePack.pages.size());
}

int main()
{
    CPPTRACE_TRY
    {
        read_header();
    }
    CPPTRACE_CATCH(const std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        cpptrace::from_current_exception().print();
    }
}