#include <filesystem>
#include <iostream>

#include "constants.h"
#include "exceptions.h"
#include "game_files_service.h"

namespace fs = std::filesystem;

GameFilesService::GameFilesService(std::string _gamePath) : gamePath(_gamePath)
{
    readTileDefinitions();
    readTexturePacks();
}

void GameFilesService::readTileDefinitions()
{
    // not implemented
}

void GameFilesService::readTexturePacks()
{
    std::string texturesDirectory = gamePath + "/media/texturepacks";

    texturePackFiles = std::vector<TexturePack>{};

    for (const auto &entry : fs::directory_iterator(texturesDirectory))
    {
        fs::path path = entry.path();

        if (path.extension().string() == constants::TEXT_PACK_EXT)
        {
            TexturePack texturePack = TexturePack::read(path.string());

            fmt::println("{}: version: {}, pages: {}", path.filename().string(), texturePack.version, texturePack.pages.size());

            texturePackFiles.push_back(texturePack);
        }
    }
}
