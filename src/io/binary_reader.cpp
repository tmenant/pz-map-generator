#include "binary_reader.h"

std::string BinaryReader::read_n_chars(const BytesBuffer &buffer, size_t size, size_t &offset)
{
    if (offset + size > buffer.size())
        throw std::runtime_error("buffer is too small");

    std::string result(reinterpret_cast<const char *>(buffer.data() + offset), size);
    offset += size;
    return result;
}

uint32_t BinaryReader::read_int32(const BytesBuffer &buffer, size_t &offset)
{
    if (offset + 4 > buffer.size())
        throw std::runtime_error("buffer is too small");

    uint32_t value;
    std::memcpy(&value, buffer.data() + offset, 4);
    offset += 4;
    return value;
}