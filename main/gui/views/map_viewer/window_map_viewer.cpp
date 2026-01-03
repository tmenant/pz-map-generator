#include "window_map_viewer.h"
#include "constants.h"
#include "gui/components/debug_panel.h"
#include "gui/components/loading_spinner.h"
#include "gui/views/map_viewer/cell_viewer.h"
#include "math/math.h"
#include "timer.h"
#include <memory>

WindowMapViewer::WindowMapViewer(AppContext &_appContext) : AppWindow(_appContext, windowConfig())
{
    loadingSpinner = std::make_unique<LoadingSpinner>(gui);
}

void WindowMapViewer::ViewState::applyTo(sf::View &view, sf::RenderWindow &window)
{
    zoomLevel = Math::fastClamp(zoomLevel, 0.05f, 50.f);

    view.setSize({ window.getSize().x * zoomLevel, window.getSize().y * zoomLevel });
    view.setCenter(center);

    window.setView(view);
}

void WindowMapViewer::ready()
{
    debugPanel = std::make_unique<DebugPanel>(gui);
    loadingSpinner->setVisible(false);

    createCells();

    float halfMapSize = constants::CELL_SIZE_IN_SQUARE / 2.0f;

    float globalGridCenterX = (viewState.currentCell.x * constants::CELL_SIZE_IN_SQUARE) + halfMapSize;
    float globalGridCenterY = (viewState.currentCell.y * constants::CELL_SIZE_IN_SQUARE) + halfMapSize;

    float halfTileW = constants::TILE_WIDTH / 2.0f;
    float halfTileH = constants::TILE_HEIGHT / 2.0f;

    float targetCameraX = (globalGridCenterX - globalGridCenterY) * halfTileW;
    float targetCameraY = (globalGridCenterX + globalGridCenterY) * halfTileH;

    viewState.currentLayer = viewState.maxLayer;
    viewState.center = { targetCameraX, targetCameraY };
    viewState.initialized = true;
}

void WindowMapViewer::createCells()
{
    int cxMin = viewState.currentCell.x - viewState.cellsDisplayRange;
    int cxMax = viewState.currentCell.x + viewState.cellsDisplayRange;

    int cyMin = viewState.currentCell.y - viewState.cellsDisplayRange;
    int cyMax = viewState.currentCell.y + viewState.cellsDisplayRange;

    viewState.minLayer = 32;
    viewState.maxLayer = -32;

    for (int cy = cyMin; cy <= cyMax; cy++)
    {
        for (int cx = cxMin; cx <= cxMax; cx++)
        {
            cellViewers.emplace_back(std::make_unique<CellViewer>(cx, cy, appContext.tilesheetService.get()));

            viewState.minLayer = Math::fastMin(viewState.minLayer, cellViewers.back()->minLayer());
            viewState.maxLayer = Math::fastMax(viewState.maxLayer, cellViewers.back()->maxLayer());
        }
    }
}

void WindowMapViewer::handleEvents(const sf::Event &event)
{
    if (const auto *mouseWheel = event.getIf<sf::Event::MouseWheelScrolled>())
    {
        if (mouseWheel->wheel == sf::Mouse::Wheel::Vertical)
        {
            float factor = (mouseWheel->delta > 0) ? 0.9f : 1.1f;

            viewState.zoomLevel *= factor;
            viewState.lastMousePos = sf::Vector2i(window.mapPixelToCoords(mouseWheel->position, view));
        }
    }

    if (const auto *mouseButton = event.getIf<sf::Event::MouseButtonPressed>())
    {
        if (mouseButton->button == sf::Mouse::Button::Left)
        {
            viewState.isDragging = true;
            viewState.lastMousePos = mouseButton->position;
        }
    }

    if (const auto *mouseButton = event.getIf<sf::Event::MouseButtonReleased>())
    {
        if (mouseButton->button == sf::Mouse::Button::Left)
        {
            viewState.isDragging = false;
        }
    }

    if (const auto *mouseMove = event.getIf<sf::Event::MouseMoved>())
    {
        if (viewState.isDragging)
        {
            sf::Vector2f oldPos = window.mapPixelToCoords(viewState.lastMousePos, view);
            sf::Vector2f newPos = window.mapPixelToCoords(mouseMove->position, view);

            viewState.center += (oldPos - newPos);
            viewState.lastMousePos = mouseMove->position;
        }
    }

    if (const auto *keyPressed = event.getIf<sf::Event::KeyPressed>())
    {
        if (keyPressed->code == sf::Keyboard::Key::Up)
        {
            viewState.currentLayer = Math::fastMin(viewState.currentLayer + 1, viewState.maxLayer - 1);
        }
        else if (keyPressed->code == sf::Keyboard::Key::Down)
        {
            viewState.currentLayer = Math::fastMax(viewState.currentLayer - 1, viewState.minLayer);
        }
    }
}

void WindowMapViewer::update()
{
    AppWindow::update();

    if (appContext.isLoaded && !viewState.initialized)
    {
        ready();
    }
    else if (appContext.isLoaded)
    {
        updateCells();
    }
    else
    {
        loadingSpinner->update(appContext.loadingPayload.getMessage());
    }
}

void WindowMapViewer::updateCells()
{
    Timer timer = Timer::start();

    viewState.applyTo(view, window);

    int drawCalls = 0;

    for (auto &cellViewer : cellViewers)
    {
        drawCalls += cellViewer->update(window, viewState.currentLayer);
    }

    if (viewState.clock.getElapsedTime() > sf::milliseconds(750))
    {
        debugPanel->setFPS(1000 / timer.elapsedMiliseconds());
        debugPanel->setTimer(timer.elapsedMiliseconds());
        debugPanel->setDrawCalls(drawCalls);

        viewState.clock.restart();
    }

    viewState.firstFrame = false;
}
