#pragma once

#include <string>
#include <unordered_map>

#include "files/lotheader.h"
#include "files/lotpack.h"

class MapFilesService
{
private:
    std::string gamePath;
    std::string mapName;

    std::unordered_map<uint32_t, Lotpack> lotpacks;
    std::unordered_map<uint32_t, LotHeader> lotheaders;

public:
    MapFilesService(std::string _gamePath, std::string _mapName);

    void LoadMapFiles();

    LotHeader *getLotheaderByPosition(int x, int y);
    Lotpack *getLotpackByPosition(int x, int y);

    LotHeader LoadLotheaderByPosition(int x, int y);
    Lotpack LoadLotpackByPosition(int x, int y, LotHeader *header);
};