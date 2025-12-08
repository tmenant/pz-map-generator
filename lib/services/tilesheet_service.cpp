#include <fmt/base.h>
#include <fmt/format.h>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

#include "constants.h"
#include "files/texturepack.h"
#include "files/tiledefinition.h"
#include "tilesheet_service.h"

namespace fs = std::filesystem;

TilesheetService::TilesheetService(std::string _gamePath)
{
    gamePath = _gamePath;

    readTileDefinitions();
    readTexturePacks();
}

TexturePack::Texture *TilesheetService::getTextureByName(const std::string &textureName)
{
    if (texturesByName.contains(textureName))
    {
        return texturesByName[textureName];
    }

    return nullptr;
}

TexturePack::Page *TilesheetService::getPageByName(const std::string &name)
{
    if (pagesByName.contains(name))
    {
        return pagesByName[name];
    }

    return nullptr;
}

TexturePack::Page *TilesheetService::getPageByTextureName(const std::string &textureName)
{
    if (textureToPageName.contains(textureName) && pagesByName.contains(textureToPageName[textureName]))
    {
        return pagesByName[textureToPageName[textureName]];
    }

    return nullptr;
}

void TilesheetService::readTileDefinitions()
{
    std::string tilesDefDirectory = gamePath + "/media";

    tiledefinitions = std::vector<TileDefinition>{};
    tilesDefByName = std::unordered_map<std::string, TileDefinition::TileData *>{};
    tileSheetsByName = std::unordered_map<std::string, TileDefinition::TileSheet *>{};

    fmt::println("Loading tileDefinitions...");

    for (const auto &entry : fs::directory_iterator(tilesDefDirectory))
    {
        fs::path path = entry.path();

        if (path.string().find(".patch") != std::string::npos)
            continue;

        if (path.extension().string() != constants::TILE_DEF_EXT)
            continue;

        auto tileDef = TileDefinition::read(path);

        tiledefinitions.push_back(tileDef);

        for (auto &tilesheet : tileDef.tileSheets)
        {
            tileSheetsByName[tilesheet.name] = &tilesheet;

            for (auto &tileData : tilesheet.tileDatas)
            {
                tilesDefByName[tileData.name] = &tileData;
            }
        }
    }
}

void TilesheetService::readTexturePacks()
{
    std::string texturesDirectory = gamePath + "/media/texturepacks";

    texturePacks = std::vector<TexturePack>{};
    textureToPageName = std::unordered_map<std::string, std::string>{};
    texturesByName = std::unordered_map<std::string, TexturePack::Texture *>{};
    pagesByName = std::unordered_map<std::string, TexturePack::Page *>{};

    fmt::println("Loading texturePacks...");

    for (const auto &entry : fs::directory_iterator(texturesDirectory))
    {
        fs::path path = entry.path();

        if (path.extension().string() != constants::TEXT_PACK_EXT)
            continue;

        texturePacks.emplace_back(std::move(TexturePack::read(path)));

        for (auto &page : texturePacks.back().pages)
        {
            pagesByName[page.name] = &page;

            for (auto &texture : page.textures)
            {
                texturesByName[texture.name] = &texture;
                textureToPageName[texture.name] = page.name;
            }
        }
    }
}
