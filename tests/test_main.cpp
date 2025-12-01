#pragma once
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "core/cell_coord.h"
#include <doctest/doctest.h>

TEST_CASE("CellCoord tests")
{
    struct CoordTest
    {
        int chunk;
        int x;
        int y;
        int z;
    };

    CoordTest coords[] = {
        { 0, 0, 0, -32 },    // z min
        { 1023, 7, 7, 31 },  // chunk_idx, x, y, z max
        { 512, 2, 2, -5 },   // neutral values
        { 0, 0, 7, 31 },     // mix min/max: min chunk, min x, max y, max z
        { 1023, 7, 0, -32 }, // mix min/max: max chunk, max x, min y, min z
        { 0, 7, 0, 0 },      // mix of x/y/z: min chunk, max x, min y, neutral z
        { 1023, 7, 7, -32 }, // combo max/min
        { 512, 0, 0, 0 },    // all neutral
        { 512, 7, 7, 0 },    // neutral z, max x/y
    };

    for (const auto &testCoord : coords)
    {
        CellCoord cellCoord(testCoord.chunk, testCoord.x, testCoord.y, testCoord.z);

        CHECK_EQ(cellCoord.chunk_idx(), testCoord.chunk);
        CHECK_EQ(cellCoord.x(), testCoord.x);
        CHECK_EQ(cellCoord.y(), testCoord.y);
        CHECK_EQ(cellCoord.z(), testCoord.z);
    }
}
