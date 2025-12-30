#include "math.h"
#include <cstdint>
#include <vector>

int Math::fastMin(int a, int b)
{
    if (a < b) return a;
    return b;
}

int Math::fastMax(int a, int b)
{
    if (a > b) return a;
    return b;
}

float Math::fastClamp(float value, float min, float max)
{
    if (value <= min) return min;
    if (value >= max) return max;

    return value;
}

uint32_t Math::hashFnv1a(const std::string &str)
{
    uint32_t hash = 0x811C9DC5; // offset basis

    for (char c : str)
    {
        hash ^= static_cast<uint8_t>(c);
        hash *= 0x01000193; // prime
    }

    return hash;
}

uint32_t Math::hashFnv1a(const std::vector<int32_t> &values)
{
    uint32_t hash = 0x811C9DC5; // offset basis

    for (int32_t c : values)
    {
        hash ^= c;
        hash *= 0x01000193; // prime
    }

    return hash;
}
