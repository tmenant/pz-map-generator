#include "lotheader.h"

LotHeaderFile LotHeaderFile::read(const std::string &filename)
{
    return LotHeaderFile();
}

LotHeaderFile LotHeaderFile::read(const std::vector<uint8_t> &buffer)
{
    LotHeaderFile header = LotHeaderFile();

    header.read_magic(buffer);

    return header;
}
