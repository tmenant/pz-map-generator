#include "binary_reader.h"

BinaryReader::BinaryReader(const std::vector<uint8_t> &buffer) : data(buffer) {}

std::string BinaryReader::read_n_chars(size_t size, size_t &offset)
{
    if (offset + size > data.size())
        throw std::runtime_error("Lecture incomplète du buffer.");

    std::string result(reinterpret_cast<const char *>(data.data() + offset), size);
    offset += size;
    return result;
}

uint32_t BinaryReader::read_int32(size_t &offset)
{
    if (offset + 4 > data.size())
        throw std::runtime_error("Lecture incomplète du buffer (int32).");

    uint32_t value;
    std::memcpy(&value, data.data() + offset, 4);
    offset += 4;
    return value;
}