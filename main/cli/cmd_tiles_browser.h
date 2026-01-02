#pragma once

#include "command_manager.h"
#include "gui/views/window_tiles_browser.h"
#include "gui/window_manager.h"


class CmdTilesBrowser : public BaseCommand
{
    CLI::App *registerCommand(CLI::App &app) override
    {
        return app.add_subcommand("tiles-browser", "Opens the tilesheets browser gui.");
    }

    void executeCommand() override
    {
        WindowManager manager;

        manager.createWindow<WindowTilesBrowser>();
        manager.run();
    }
};