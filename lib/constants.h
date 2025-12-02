#pragma once
#include <string>

namespace constants
{
    constexpr int MIN_LAYER = -32;
    constexpr int MAX_LAYER = 32;

    constexpr int BLOCK_SIZE_IN_SQUARE = 8;
    constexpr int SQUARE_PER_BLOCK = BLOCK_SIZE_IN_SQUARE * BLOCK_SIZE_IN_SQUARE;

    constexpr int CELL_SIZE_IN_BLOCKS = 32;
    constexpr int BLOCKS_PER_CELL = CELL_SIZE_IN_BLOCKS * CELL_SIZE_IN_BLOCKS;

    constexpr char LINE_END = 0xA;

    const std::string GAME_PATH_B42 = "C:/SteamLibrary/steamapps/common/ProjectZomboidB42";

    constexpr std::string_view LOTHEADER_EXT = ".lotheader";
    constexpr std::string_view LOTPACK_EXT = ".lotpack";
    constexpr std::string_view TEXT_PACK_EXT = ".pack";
    constexpr std::string_view TILE_DEF_EXT = ".tiles";

    constexpr std::string_view MAGIC_LOTHEADER = "LOTH";
    constexpr std::string_view MAGIC_LOTPACK = "LOTP";
    constexpr std::string_view MAGIC_PACK = "PZPK";
    constexpr std::string_view MAGIC_TILEDEF = "tdef";
}