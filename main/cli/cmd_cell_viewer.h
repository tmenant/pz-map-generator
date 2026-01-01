#pragma once

#include <atomic>
#include <memory>
#include <thread>

#include <SFML/Graphics/RenderWindow.hpp>

#include "TGUI/Backend/SFML-Graphics.hpp"
#include "TGUI/Widgets/Label.hpp"
#include "TGUI/Widgets/Picture.hpp"

#include "gui/views/cell_viewer/cell_viewer.h"

#include "services/map_files_service.h"
#include "services/tilesheet_service.h"

#include "command_manager.h"
#include "constants.h"
#include "platform.h"
#include "theme.h"

struct AppContext
{
    std::atomic<int> progression = 0;
    std::atomic<bool> isLoaded = false;
    std::string currentStatus;
    std::unique_ptr<TilesheetService> tilesheetService;
    std::unique_ptr<MapFilesService> mapFileService;
    LoadingPayload loadingPayload;
};

class CmdCellViewer : public BaseCommand
{
public:
    CLI::App *registerCommand(CLI::App &app) override
    {
        return app.add_subcommand("cell-viewer", "Open the cells viewer gui.");
    }

    void executeCommand() override
    {
        sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "PZ Cells Viewer");
        sf::View view = window.getDefaultView();
        tgui::Gui gui{ window };

        platform::windows::setWindowDarkMode(window);
        window.setFramerateLimit(60);

        auto loadingSpinner = tgui::Picture::create("ignore/spinner.png");
        loadingSpinner->setPosition("45%", "50%");
        loadingSpinner->setOrigin(0.5f, 0.5f);
        loadingSpinner->setSize(20, 20);
        gui.add(loadingSpinner);

        auto loadingLabel = tgui::Label::create("Chargement des services PZ...");
        loadingLabel->setPosition("50%", "50%");
        loadingLabel->setOrigin(0.5f, 0.5f);
        loadingLabel->getRenderer()->setTextColor(Colors::fontColor.tgui());
        gui.add(loadingLabel);

        AppContext ctx;
        std::thread loadingThread([&ctx]()
        {
            ctx.tilesheetService = std::make_unique<TilesheetService>(constants::GAME_PATH, ctx.loadingPayload);
            ctx.mapFileService = std::make_unique<MapFilesService>(constants::GAME_PATH, MapNames::Muldraugh);
            ctx.isLoaded = true;
        });

        std::unique_ptr<CellViewer> cellViewer = nullptr;

        bool initialized = false;

        while (window.isOpen())
        {
            while (const std::optional event = window.pollEvent())
            {
                gui.handleEvent(*event);

                if (ctx.isLoaded && initialized)
                {
                    cellViewer->handleEvents(*event, window);
                }

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

            if (ctx.isLoaded && !initialized)
            {
                if (loadingThread.joinable()) loadingThread.join();

                cellViewer = std::make_unique<CellViewer>(&view, ctx.mapFileService.get(), ctx.tilesheetService.get(), gui, 31, 45);
                gui.remove(loadingLabel);
                gui.remove(loadingSpinner);
                initialized = true;
            }
            else if (ctx.isLoaded)
            {
                cellViewer->update(window);
            }
            else
            {
                loadingLabel->setText(ctx.loadingPayload.getMessage());
                loadingSpinner->setRotation(loadingSpinner->getRotation() + 5);
            }

            gui.draw();
            window.display();
        }

        if (loadingThread.joinable())
        {
            loadingThread.join();
        }
    }
};