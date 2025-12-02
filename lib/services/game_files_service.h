#pragma once

#include <string>
#include <vector>

#include "files/lotheader.h"
#include "files/lotpack.h"
#include "files/texturepack.h"
#include "files/tiledefinition.h"

class GameFilesService
{
public:
    std::string gamePath;

    std::vector<TileDefinition> tiledefinitions;
    std::vector<TexturePack> texturePacks;
    std::vector<Lotpack> lotpacks;
    std::vector<LotHeader> lotheaders;

    GameFilesService(std::string _gamePath);

    TexturePack::Page getPageByName(const std::string &name);

private:
    void readTileDefinitions();
    void readTexturePacks();
    void readLotpacks();
    void readLotHeaders();
};