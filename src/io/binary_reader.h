#pragma once

#include <cstdint>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "types.h"

namespace BinaryReader
{
    std::string read_n_chars(const BytesBuffer &buffer, size_t size, size_t &offset);

    uint32_t read_int32(const BytesBuffer &buffer, size_t &offset);
};
