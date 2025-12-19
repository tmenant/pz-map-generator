#include <crtdbg.h>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Vector2.hpp>
#include <iostream>
#include <string>

#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>

#include "gui/views/cell_viewer/cell_viewer.h"
#include "constants.h"
#include "platform.h"
#include "services/map_files_service.h"
#include "services/tilesheet_service.h"
#include "theme.h"
#include <fmt/format.h>
#include <lodepng.h>
#include <cpptrace/from_current.hpp>


void main_window()
{
    sf::RenderWindow window(sf::VideoMode({ 1920, 1080 }), "PZ Map Generator");
    sf::View view = window.getDefaultView();

    platform::windows::setWindowDarkMode(window);
    window.setFramerateLimit(60);

    tgui::Gui gui{ window };
    TilesheetService tilesheetService(constants::GAME_PATH);
    MapFilesService mapFileService(constants::GAME_PATH, MapNames::Muldraugh);

    // TilesBrowser tilesBrowser(gui, window, tilesheetService);
    CellViewer cellViewer(&view, &mapFileService, &tilesheetService, gui, 32, 45);

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            gui.handleEvent(*event);
            cellViewer.handleEvents(*event, window);

            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
            else if (const auto *resized = event->getIf<sf::Event::Resized>())
            {
                // sf::FloatRect visibleArea({ 0.f, 0.f }, { (float)resized->size.x, (float)resized->size.y });
                view.setSize({ (float)resized->size.x, (float)resized->size.y });
                window.setView(view);
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
        main_window();
    }
    CPPTRACE_CATCH(const std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        cpptrace::from_current_exception().print();
    }
}