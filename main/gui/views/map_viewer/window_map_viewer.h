#pragma once

#include "TGUI/Widgets/Label.hpp"
#include "TGUI/Widgets/Picture.hpp"

#include "app_context.h"
#include "cell_viewer.h"
#include "gui/app_window.h"
#include "theme.h"
#include <SFML/Window/VideoMode.hpp>
#include <memory>

class WindowMapViewer : public AppWindow
{
private:
    std::unique_ptr<CellViewer> cellViewer = nullptr;

    tgui::Label::Ptr loadingLabel = nullptr;
    tgui::Picture::Ptr loadingSpinner = nullptr;

    static WindowConfig windowConfig()
    {
        return {
            sf::VideoMode::getDesktopMode(),
            "PZ Cells Viewer",
        };
    }

public:
    WindowMapViewer(AppContext &_appContext) : AppWindow(_appContext, windowConfig())
    {
        loadingSpinner = tgui::Picture::create("ignore/spinner.png");
        loadingSpinner->setPosition("45%", "50%");
        loadingSpinner->setOrigin(0.5f, 0.5f);
        loadingSpinner->setSize(20, 20);

        loadingLabel = tgui::Label::create("Loading...");
        loadingLabel->setPosition("50%", "50%");
        loadingLabel->setOrigin(0.5f, 0.5f);
        loadingLabel->getRenderer()->setTextColor(Colors::fontColor.tgui());

        gui.add(loadingSpinner);
        gui.add(loadingLabel);
    }

    void handleEvents(const sf::Event &event) override
    {
        if (cellViewer != nullptr)
        {
            cellViewer->handleEvents(event, window);
        }
    }

    void update() override
    {
        AppWindow::update();

        if (appContext.isLoaded && cellViewer == nullptr)
        {
            cellViewer = std::make_unique<CellViewer>(&view, appContext.tilesheetService.get(), gui, 31, 45);

            gui.remove(loadingLabel);
            gui.remove(loadingSpinner);
        }
        else if (appContext.isLoaded)
        {
            cellViewer->update(window);
        }
        else
        {
            loadingLabel->setText(appContext.loadingPayload.getMessage());
            loadingSpinner->setRotation(loadingSpinner->getRotation() + 5);
        }
    }
};