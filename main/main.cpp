#include <crtdbg.h>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector2.hpp>
#include <atomic>
#include <iostream>
#include <memory>
#include <string>

#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>

#include "constants.h"
#include "gui/views/cell_viewer/cell_viewer.h"
#include "platform.h"
#include "services/map_files_service.h"
#include "services/tilesheet_service.h"
#include "theme.h"
#include <fmt/format.h>
#include <lodepng.h>
#include <cpptrace/from_current.hpp>
#include <thread>

struct AppContext
{
    std::atomic<bool> isDone = false;
    std::string currentStatus;
    std::unique_ptr<TilesheetService> tilesheetService;
    std::unique_ptr<MapFilesService> mapFileService;
};

void main_window()
{
    sf::RenderWindow window(sf::VideoMode({ 1920, 1080 }), "PZ Map Generator");
    sf::View view = window.getDefaultView();
    tgui::Gui gui{ window };

    platform::windows::setWindowDarkMode(window);
    window.setFramerateLimit(60);

    auto loadingLabel = tgui::Label::create("Chargement des services PZ...");
    loadingLabel->setPosition("50%", "50%");
    loadingLabel->setOrigin(0.5f, 0.5f);
    gui.add(loadingLabel);

    AppContext ctx;
    std::thread loadingThread([&ctx]()
    {
        ctx.tilesheetService = std::make_unique<TilesheetService>(constants::GAME_PATH);
        ctx.mapFileService = std::make_unique<MapFilesService>(constants::GAME_PATH, MapNames::Muldraugh);
        ctx.isDone = true;
    });

    std::unique_ptr<CellViewer> cellViewer = nullptr;

    bool initialized = false;

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            gui.handleEvent(*event);

            if (ctx.isDone && initialized)
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

        if (ctx.isDone && !initialized)
        {
            if (loadingThread.joinable()) loadingThread.join();

            cellViewer = std::make_unique<CellViewer>(&view, ctx.mapFileService.get(), ctx.tilesheetService.get(), gui, 31, 45);
            gui.remove(loadingLabel);
            initialized = true;
        }
        else if (ctx.isDone)
        {
            cellViewer->update(window);
        }

        gui.draw();
        window.display();
    }

    if (loadingThread.joinable())
    {
        loadingThread.join();
    }
}

int main()
{
    CPPTRACE_TRY
    {
        main_window();
    }
    CPPTRACE_CATCH(const std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        cpptrace::from_current_exception().print();
    }
}