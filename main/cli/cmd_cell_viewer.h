#pragma once

#include "command_manager.h"
#include "gui/views/window_cell_viewer.h"
#include "gui/window_manager.h"

class CmdCellViewer : public BaseCommand
{
public:
    CLI::App *registerCommand(CLI::App &app) override
    {
        return app.add_subcommand("cell-viewer", "Open the cells viewer gui.");
    }

    void executeCommand() override
    {
        WindowManager manager;

        manager.createWindow<WindowCellViewer>();
        manager.run();
    }
};