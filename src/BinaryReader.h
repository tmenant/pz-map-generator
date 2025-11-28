#pragma once

#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <string>
#include <vector>

namespace BinaryReader
{
    inline int32_t readInt32LE(const std::vector<uint8_t> &buffer, size_t offset)
    {
        if (offset + 4 > buffer.size())
            throw std::out_of_range("Buffer too small for int32");

        return static_cast<int32_t>(buffer[offset])
            | (static_cast<int32_t>(buffer[offset + 1]) << 8)
            | (static_cast<int32_t>(buffer[offset + 2]) << 16)
            | (static_cast<int32_t>(buffer[offset + 3]) << 24);
    }

    inline std::string readNChars(const std::vector<uint8_t> &buffer, size_t offset, size_t size)
    {
        if (offset + size > buffer.size())
            throw std::out_of_range("Buffer too small for " + std::to_string(size) + " chars");

        return std::string(reinterpret_cast<const char *>(buffer.data() + offset), size);
    }
}
