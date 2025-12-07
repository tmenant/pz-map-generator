#pragma once

#include <string>

#include "files/texturepack.h"
#include "files/tiledefinition.h"

class GameFilesService
{
public:
    std::string gamePath;

    std::vector<TileDefinition> tiledefinitions;
    std::vector<TexturePack> texturePacks;

    GameFilesService(std::string _gamePath);

    TexturePack::Page *getPageByName(const std::string &name);
    std::vector<std::string> getPageNames();

private:
    void readTileDefinitions();
    void readTexturePacks();
};