#pragma once

#include <string>
#include <unordered_map>

#include "files/texturepack.h"
#include "files/tiledefinition.h"
#include "threading/loading_payload.h"

class TilesheetService
{
public:
    std::string gamePath;

    std::vector<TileDefinition> tiledefinitions;
    std::vector<TexturePack> texturePacks;

    std::unordered_map<std::string, TexturePack::Page *> pagesByName;
    std::unordered_map<std::string, TexturePack::Texture *> texturesByName;
    std::unordered_map<std::string, TileDefinition::TileSheet *> tileSheetsByName;
    std::unordered_map<std::string, TileDefinition::TileData *> tilesDefByName;
    std::unordered_map<std::string, std::string> textureToPageName;

    TilesheetService(std::string _gamePath, LoadingPayload &loadingPayload);

    TexturePack::Texture *getTextureByName(const std::string &textureName, TexturePack::Page *page);
    TexturePack::Texture *getTextureByName(const std::string &textureName);
    TexturePack::Page *getPageByTextureName(const std::string &textureName);
    TexturePack::Page *getPageByName(const std::string &name);

private:
    void readTileDefinitions();
    void readTexturePacks();
};