#include "lotheader.h"

LotHeaderFile LotHeaderFile::read(const std::string &filename)
{
    return LotHeaderFile();
}

LotHeaderFile LotHeaderFile::read(const std::vector<uint8_t> &buffer)
{
    return LotHeaderFile();
}

void LotHeaderFile::read_magic(const BytesBuffer &buffer)
{
}
