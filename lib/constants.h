#pragma once

#include <string>

namespace constants
{
    constexpr int MIN_LAYER = -32;
    constexpr int MAX_LAYER = 32;
    constexpr int MAX_CELLS_SIZE = 1024;

    constexpr int BLOCK_SIZE_IN_SQUARE = 8;
    constexpr int SQUARE_PER_BLOCK = BLOCK_SIZE_IN_SQUARE * BLOCK_SIZE_IN_SQUARE;

    constexpr int CELL_SIZE_IN_BLOCKS = 32;
    constexpr int BLOCKS_PER_CELL = CELL_SIZE_IN_BLOCKS * CELL_SIZE_IN_BLOCKS;

    constexpr char LINE_END = 0xA;

    const std::string GAME_PATH_B42 = "C:/Program Files (x86)/Steam/steamapps/common/ProjectZomboid";

    constexpr std::string_view LOTHEADER_EXT = ".lotheader";
    constexpr std::string_view LOTPACK_EXT = ".lotpack";
    constexpr std::string_view TEXT_PACK_EXT = ".pack";
    constexpr std::string_view TILE_DEF_EXT = ".tiles";

    constexpr std::string_view MAGIC_LOTHEADER = "LOTH";
    constexpr std::string_view MAGIC_LOTPACK = "LOTP";
    constexpr std::string_view MAGIC_PACK = "PZPK";
    constexpr std::string_view MAGIC_TILEDEF = "tdef";

    const std::string GAME_PATH = GAME_PATH_B42;
    const std::string LOTHEADER_PATH = "data/B42/27_38.lotheader";
    const std::string LOTHPACK_PATH = "data/B42/world_27_38.lotpack";
    const std::string TILESDEF_PATH = "data/B42/newtiledefinitions.tiles";
    const std::string TEXTUREPACK_PATH = GAME_PATH + "/media/texturepacks/Tiles2x.pack";
}

namespace MapNames
{
    const std::string Muldraugh = "Muldraugh, KY";
    const std::string EchoCreek = "Echo Creek, KY";
    const std::string Riverside = "Riverside, KY";
    const std::string Rosewood = "Rosewood, KY";
    const std::string WestPoint = "West Point, KY";
    const std::string Challenge1 = "challengemaps/Challenge1";
    const std::string Challenge2 = "challengemaps/Challenge2";
    const std::string Kingsmouth = "challengemaps/Kingsmouth";
    const std::string KnoxCounty = "challengemaps/KnoxCounty";
    const std::string Studio = "challengemaps/Studio";
    const std::string TheForest = "challengemaps/The Forest";
    const std::string Tutorial = "challengemaps/Tutorial";
}