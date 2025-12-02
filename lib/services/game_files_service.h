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

    std::vector<TileDefinition> tiledefinitionFiles;
    std::vector<TexturePack> texturePackFiles;
    std::vector<Lotpack> lotpackFiles;
    std::vector<LotHeader> lotheaderFiles;

    GameFilesService(std::string _gamePath);

private:
    void readTileDefinitions();
    void readTexturePacks();
    void readLotpacks();
    void readLotHeaders();
};