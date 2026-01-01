#pragma once

#include "TGUI/Backend/SFML-Graphics.hpp"
#include <SFML/Graphics/RenderWindow.hpp>

#include "command_manager.h"
#include "constants.h"
#include "gui/views/tiles_browser/tiles_browser.h"
#include "services/tilesheet_service.h"
#include "theme.h"
#include "threading/loading_payload.h"

class CmdTilesBrowser : public BaseCommand
{
    CLI::App *registerCommand(CLI::App &app) override
    {
        return app.add_subcommand("tiles-browser", "Opens the tilesheets browser gui.");
    }

    void executeCommand() override
    {
        sf::RenderWindow window(sf::VideoMode({ 1920, 1080 }), "PZ Tiles Browser");
        sf::View view = window.getDefaultView();
        tgui::Gui gui{ window };

        LoadingPayload loadingPayload;
        TilesheetService tilesheetService(constants::GAME_PATH_B42, loadingPayload);
        TilesBrowser tilesBrowser(gui, window, tilesheetService);

        while (window.isOpen())
        {
            while (const std::optional event = window.pollEvent())
            {
                gui.handleEvent(*event);

                if (event->is<sf::Event::Closed>())
                {
                    window.close();
                }
                else if (const auto *resized = event->getIf<sf::Event::Resized>())
                {
                    view.setSize({ (float)resized->size.x, (float)resized->size.y });
                    window.setView(view);
                }
            }

            // viewport drawings
            window.clear(Colors::backgroundColor.sfml());

            tilesBrowser.update(window);

            gui.draw();
            window.display();
        }
    }
};