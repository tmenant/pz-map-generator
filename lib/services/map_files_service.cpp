#include <fmt/format.h>
#include <filesystem>
#include <string>

#include "constants.h"
#include "files/lotheader.h"
#include "files/lotpack.h"
#include "map_files_service.h"

namespace fs = std::filesystem;

MapFilesService::MapFilesService(std::string _gamePath, std::string _mapName)
{
    gamePath = _gamePath;
    mapName = _mapName;
}

void MapFilesService::LoadMapFiles()
{
    fmt::println("Loading map '{}'", mapName);

    std::string mapDirectory = gamePath + "/media/maps/" + mapName;

    auto start = std::chrono::high_resolution_clock::now();
    auto filescount = 0;

    for (auto &entry : fs::directory_iterator(mapDirectory))
    {
        fs::path path = entry.path();

        if (path.extension().string() != constants::LOTHEADER_EXT)
            continue;

        std::string lotheaderPath = path.string();
        std::string lotpackPath = path.replace_filename("world_" + path.filename().replace_extension(constants::LOTPACK_EXT).string()).string();

        fmt::println("lotheader: '{} ({})'", lotheaderPath, fs::exists(lotheaderPath));
        fmt::println("lotpack  : '{} ({})'\n", lotpackPath, fs::exists(lotpackPath));

        LotHeader lotheader = LotHeader::read(lotheaderPath);
        Lotpack lotpack = Lotpack::read(lotpackPath, &lotheader);

        int hash = lotheader.position.x + lotheader.position.y * constants::MAX_CELLS_SIZE;
        lotheaders[hash] = lotheader;
        lotpacks[hash] = lotpack;

        filescount++;
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    fmt::println("{} files parsed in {} ms", filescount, duration);
}

LotHeader *MapFilesService::getLotheaderByPosition(int x, int y)
{
    int hash = x + y * constants::MAX_CELLS_SIZE;

    if (lotheaders.contains(hash))
    {
        return &lotheaders[hash];
    }

    return nullptr;
}

Lotpack *MapFilesService::getLotpackByPosition(int x, int y)
{
    int hash = x + y * constants::MAX_CELLS_SIZE;

    if (lotpacks.contains(hash))
    {
        return &lotpacks[hash];
    }

    return nullptr;
}

LotHeader MapFilesService::LoadLotheaderByPosition(int x, int y)
{
    std::string path = fmt::format("{}/media/maps/{}/{}_{}.lotheader", gamePath, mapName, x, y);

    return LotHeader::read(path);
}

Lotpack MapFilesService::LoadLotpackByPosition(int x, int y, LotHeader *header)
{
    std::string path = fmt::format("{}/media/maps/{}/world_{}_{}.lotpack", gamePath, mapName, x, y);

    return Lotpack::read(path, header);
}