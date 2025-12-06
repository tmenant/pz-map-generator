#include <filesystem>

#include <fmt/format.h>

#include "constants.h"
#include "game_files_service.h"

namespace fs = std::filesystem;

GameFilesService::GameFilesService(std::string _gamePath) : gamePath(_gamePath)
{
    readTileDefinitions();
    readTexturePacks();
}

TexturePack::Page GameFilesService::getPageByName(const std::string &name) const
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

std::vector<std::string> GameFilesService::getPageNames() const
{
    std::vector<std::string> pageNames{};

    for (const TexturePack &texturePack : texturePacks)
    {
        for (const TexturePack::Page &page : texturePack.pages)
        {
            pageNames.push_back(page.name);
        }
    }

    return pageNames;
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
            TexturePack texturePack = TexturePack::read(path);

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
