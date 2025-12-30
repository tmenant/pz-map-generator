#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace Math
{
    int fastMin(int a, int b);
    int fastMax(int a, int b);
    float fastClamp(float value, float min, float max);
    uint32_t hashFnv1a(const std::string &str);
    uint32_t hashFnv1a(const std::vector<int32_t> &values);
}