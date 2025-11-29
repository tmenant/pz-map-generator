#include "lotheader.h"
#include "io/binary_reader.h"

LotHeader LotHeader::read(const std::string &filename)
{
    return LotHeader();
}

LotHeader LotHeader::read(const BytesBuffer &buffer)
{
    LotHeader header = LotHeader();
    size_t offset = 0;

    header.magic = BinaryReader::read_n_chars(buffer, 4, offset);
    header.version = BinaryReader::read_int32(buffer, offset);

    return header;
}
