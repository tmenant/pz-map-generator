#pragma once

#include "app_context.h"
#include "cell_viewer.h"
#include "gui/app_window.h"
#include "gui/components/debug_panel.h"
#include "gui/components/loading_spinner.h"
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <memory>
#include <vector>

class WindowMapViewer : public AppWindow
{
private:
    struct ViewState
    {
        sf::Clock clock;
        sf::Vector2i lastMousePos;
        sf::Vector2f center;
        sf::Vector2i currentCell = { 31, 45 };

        float zoomLevel = 10.0f;
        bool initialized = false;
        bool isDragging = false;
        bool firstFrame = true;

        int cellsDisplayRange = 3;
        int currentLayer = 32;
        int maxLayer = 32;
        int minLayer = -32;

        void applyTo(sf::View &view, sf::RenderWindow &window);
    };

    ViewState viewState;

    std::unique_ptr<DebugPanel> debugPanel = nullptr;
    std::unique_ptr<LoadingSpinner> loadingSpinner = nullptr;

    std::vector<std::unique_ptr<CellViewer>> cellViewers;

    static WindowConfig windowConfig()
    {
        return {
            sf::VideoMode::getDesktopMode(),
            "PZ Cells Viewer",
        };
    }

public:
    WindowMapViewer(AppContext &_appContext);

    void handleEvents(const sf::Event &event) override;
    void ready() override;
    void update() override;

    void createCells();
    void updateCells();
    void drawDebugGrid(int displayRange);
};