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

#include "TGUI/Widgets/Picture.hpp"
#include "constants.h"
#include "core/atlas_graph.h"
#include "gui/views/cell_viewer/cell_viewer.h"
#include "math/vector2i.h"
#include "platform.h"
#include "services/map_files_service.h"
#include "services/tilesheet_service.h"
#include "theme.h"
#include "threading/loading_payload.h"
#include "timer.h"
#include <fmt/base.h>
#include <fmt/format.h>
#include <lodepng.h>
#include <cpptrace/from_current.hpp>
#include <thread>
#include <vector>

struct AppContext
{
    std::atomic<int> progression = 0;
    std::atomic<bool> isLoaded = false;
    std::string currentStatus;
    std::unique_ptr<TilesheetService> tilesheetService;
    std::unique_ptr<MapFilesService> mapFileService;
    LoadingPayload loadingPayload;
};

void main_window()
{
    sf::RenderWindow window(sf::VideoMode({ 1920, 1080 }), "PZ Map Generator");
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

void test_atlas_packing()
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

int main()
{
    CPPTRACE_TRY
    {
        test_atlas_packing();
    }
    CPPTRACE_CATCH(const std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        cpptrace::from_current_exception().print();
    }
}