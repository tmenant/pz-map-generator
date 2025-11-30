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
    int32_t readInt32(const BytesBuffer &buffer, size_t &offset);
    std::string readLineTrimmed(const BytesBuffer &buffer, size_t &offset);
    std::string readStringWithLength(const BytesBuffer &buffer, size_t &offset);
    BytesBuffer readBytesWithLength(const BytesBuffer &buffer, size_t &offset);
    BytesBuffer readExact(const BytesBuffer &buffer, uint32_t size, size_t &offset);
};
