#include "vector2i.h"
#include "math.h"
#include <vector>

uint32_t Vector2i::hashcode() const
{
    std::vector<int32_t> values = { x, y };
    return Math::hashFnv1a(values);
}