#include <crtdbg.h>
#include <iostream>
#include <string>

#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>

#include <fmt/base.h>
#include <fmt/format.h>
#include <lodepng.h>
#include <cpptrace/from_current.hpp>

#include "cell_viewer/cell_viewer.h"
#include "constants.h"
#include "files/lotheader.h"
#include "files/lotpack.h"
#include "platform.h"
#include "services/tilesheet_service.h"
#include "services/map_files_service.h"
#include "theme.h"

#include <psapi.h>
#include <windows.h>

void main_window()
{
    sf::RenderWindow window(sf::VideoMode({ 1920, 1080 }), "PZ Map Generator");
    platform::windows::setWindowDarkMode(window);
    window.setFramerateLimit(60);

    tgui::Gui gui{ window };
    TilesheetService tilesheetService(constants::GAME_PATH);
    MapFilesService mapFileService(constants::GAME_PATH, MapNames::Muldraugh);

    // TilesBrowser tilesBrowser(gui, window, tilesheetService);
    CellViewer cellViewer(gui, window, mapFileService, 32, 45);

    return;

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

size_t getMemoryUsage()
{
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc)))
        return pmc.WorkingSetSize; // Mémoire physique utilisée en octets
    return 0;
}

void test_cellViewer()
{
    TilesheetService tilesheetService(constants::GAME_PATH_B42);
    MapFilesService mapFileService(constants::GAME_PATH, MapNames::Muldraugh);

    int x = 32;
    int y = 45;

    LotHeader lotheader = mapFileService.LoadLotheaderByPosition(x, y);
    Lotpack lotpack = mapFileService.LoadLotpackByPosition(x, y, &lotheader);

    for (auto const &squareData : lotpack.squareMap)
    {
        if (squareData.coord.chunk_idx() != 0)
            continue;

        for (const auto &tileIndex : squareData.tiles)
        {
            auto textureName = lotheader.tileNames[tileIndex];
            auto textureData = tilesheetService.getTextureByName(textureName);
            auto page = tilesheetService.getPageByTextureName(textureName);

            if (textureData == nullptr || page == nullptr)
                continue;

            fmt::println("texture: {}, page: {}", textureName, page->name);
        }
    }
}

int main()
{
    CPPTRACE_TRY
    {
        test_cellViewer();
    }
    CPPTRACE_CATCH(const std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        cpptrace::from_current_exception().print();
    }
}