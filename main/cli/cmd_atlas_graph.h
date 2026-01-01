#pragma once

#include "command_manager.h"
#include <fmt/base.h>

#include "constants.h"
#include "core/atlas_graph.h"
#include "files/lotheader.h"
#include "math/math.h"
#include "platform.h"
#include "timer.h"

class CmdAtlasGraph : public BaseCommand
{
private:
    std::string mapName;

public:
    CLI::App *registerCommand(CLI::App &app) override
    {
        return app.add_subcommand("atlas-graph", "Build the global atlas graph for a given map.");
    }

    void executeCommand() override
    {
        // auto loadingPayload = LoadingPayload();
        // auto tilesheetService = TilesheetService(constants::GAME_PATH_B42, loadingPayload);

        auto mapDirectory = constants::GAME_PATH_B42 + "/media/maps/" + MapNames::Muldraugh;
        auto timer = Timer::start();

        AtlasGraph atlasGraph;

        for (auto &entry : std::filesystem::directory_iterator(mapDirectory))
        {
            std::filesystem::path path = entry.path();

            if (path.extension().string() != constants::LOTHEADER_EXT)
                continue;

            LotHeader header = LotHeader::read(path.string());

            AtlasGraph::Node atlasData;

            for (const auto &tilename : header.tileNames)
            {
                atlasData.hashes.emplace_back(Math::hashFnv1a(tilename));
            }

            atlasGraph.addNode(header.position.hashcode(), std::move(atlasData));
        }

        atlasGraph.buildGraph();

        int rootNodes = 0;
        for (auto &node : atlasGraph)
        {
            rootNodes += node.parent == nullptr;
        }

        auto memory = platform::windows::getMemoryUsage() / 1024 / 1024;

        fmt::println("{} files parsed in {}ms, memory: {}MB", atlasGraph.size(), timer.elapsedMiliseconds(), memory);
        fmt::println("{} / {} root nodes", rootNodes, atlasGraph.size());
    }
};