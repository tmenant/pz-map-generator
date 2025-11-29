#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <fmt/core.h>

#include "files/lotheader.h"

#include "io/binary_reader.h"
#include "io/file_reader.h"

const std::string LOTHEADER_PATH = "data/B42/27_38.lotheader";

struct MyStruct
{
    std::string magic;
    int32_t version;
};

void read_header(std::string path)
{
    BytesBuffer buffer = FileReader::read(path);
    LotHeader header = LotHeader::read(buffer);

    fmt::print("magic: {}, version: {}\n", header.magic, header.version);
}

int main()
{
    read_header(LOTHEADER_PATH);
}
