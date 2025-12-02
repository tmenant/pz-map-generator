#include <filesystem>
#include <iostream>

#include "constants.h"
#include "exceptions.h"
#include "game_files_service.h"
#include "io/file_reader.h"

namespace fs = std::filesystem;

GameFilesService::GameFilesService(std::string _gamePath) : gamePath(_gamePath)
{
    readTileDefinitions();
    readTexturePacks();
}

TexturePack::Page GameFilesService::getPageByName(const std::string &name)
{
    for (const TexturePack &texturePack : texturePacks)
    {
        for (const TexturePack::Page &page : texturePack.pages)
        {
            if (page.name == name)
            {
                return page;
            }
        }
    }

    throw std::runtime_error("Page not found: " + name);
}

void GameFilesService::readTileDefinitions()
{
    // not implemented
}

void GameFilesService::readTexturePacks()
{
    std::string texturesDirectory = gamePath + "/media/texturepacks";

    texturePacks = std::vector<TexturePack>{};

    fmt::println("Loading texturePacks...");

    for (const auto &entry : fs::directory_iterator(texturesDirectory))
    {
        fs::path path = entry.path();

        if (path.extension().string() == constants::TEXT_PACK_EXT)
        {
            TexturePack texturePack = TexturePack::read(path.string());

            // fmt::println("{}: version: {}, pages: {}", path.filename().string(), texturePack.version, texturePack.pages.size());

            // for (const auto &page : texturePack.pages)
            // {
            //     fmt::println("{} {}", texturePack.version, page.name);
            //     FileReader::save(page.png, "ignore/tilesheets/" + page.name + ".png");
            // }

            texturePacks.push_back(texturePack);
        }
    }
}
