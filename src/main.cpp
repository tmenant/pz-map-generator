#include <crtdbg.h>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include <fmt/format.h>
#include <cpptrace/from_current.hpp>

#include "files/lotheader.h"
#include "io/binary_reader.h"
#include "io/file_reader.h"
#include "math/md5.h"

const std::string LOTHEADER_PATH = "data/B42/27_38.lotheader";

void read_header()
{
    BytesBuffer buffer = FileReader::read(LOTHEADER_PATH);
    LotHeader header = LotHeader::read(buffer);

    std::string hash = MD5::toHash(buffer);

    fmt::println("magic: {}, version: {}, md5: {}", header.magic, header.version, hash);

    for (const std::string &tilename : header.tileNames)
    {
        fmt::println("{}", tilename);
    }
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