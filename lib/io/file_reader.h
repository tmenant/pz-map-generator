#pragma once

#include "types.h"
#include <string>

namespace FileReader
{
    BytesBuffer read(std::string path);
    void save(const BytesBuffer &buffer, std::string path);
}