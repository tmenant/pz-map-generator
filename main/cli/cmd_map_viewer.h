#pragma once

#include "command_manager.h"
#include "gui/views/map_viewer/window_map_viewer.h"
#include "gui/window_manager.h"

class CmdMapViewer : public BaseCommand
{
public:
    CLI::App *registerCommand(CLI::App &app) override
    {
        return app.add_subcommand("map-viewer", "Open the map viewer gui.");
    }

    void executeCommand() override
    {
        WindowManager manager;

        manager.createWindow<WindowMapViewer>();
        manager.run();
    }
};