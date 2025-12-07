#include <crtdbg.h>
#include <iostream>
#include <string>

#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>

#include <fmt/format.h>
#include <lodepng.h>
#include <cpptrace/from_current.hpp>

#include "cell_viewer/cell_viewer.h"
#include "constants.h"
#include "platform.h"
#include "services/game_files_service.h"
#include "services/map_files_service.h"
#include "theme.h"

void main_window()
{
    sf::RenderWindow window(sf::VideoMode({ 1920, 1080 }), "PZ Map Generator");
    platform::windows::setWindowDarkMode(window);
    window.setFramerateLimit(60);

    tgui::Gui gui{ window };
    GameFilesService gamefileService(constants::GAME_PATH);
    MapFilesService mapFileService(constants::GAME_PATH, MapNames::Muldraugh);

    // TilesBrowser tilesBrowser(gui, window, gamefileService);
    CellViewer cellViewer(gui, window, mapFileService, 32, 45);

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
                sf::FloatRect visibleArea({ 0.f, 0.f }, { static_cast<float>(resized->size.x), static_cast<float>(resized->size.y) });
                window.setView(sf::View(visibleArea));
            }
        }

        // viewport drawings
        window.clear(Colors::backgroundColor.sfml());

        // tilesBrowser.update(window);
        cellViewer.update(window);

        gui.draw();
        window.display();
    }
}

int main()
{
    CPPTRACE_TRY
    {
        // main_window();
        MapFilesService service(constants::GAME_PATH_B42, MapNames::Muldraugh);
    }
    CPPTRACE_CATCH(const std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        cpptrace::from_current_exception().print();
    }
}