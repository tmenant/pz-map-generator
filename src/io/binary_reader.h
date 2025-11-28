#pragma once

#include <cstdint>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

class BinaryReader
{
private:
    const std::vector<uint8_t> &data;

public:
    BinaryReader(const std::vector<uint8_t> &buffer);

    std::string read_n_chars(size_t size, size_t &offset);
    uint32_t read_int32(size_t &offset);
};
