#pragma once

#include "app_context.h"
#include "gui/app_window.h"
#include "gui/views/tiles_browser/tiles_browser.h"
#include "theme.h"
#include <memory>

class WindowTilesBrowser : public AppWindow
{
private:
    std::unique_ptr<TilesBrowser> tilesBrowser;

    tgui::Label::Ptr loadingLabel = nullptr;
    tgui::Picture::Ptr loadingSpinner = nullptr;

    static WindowConfig windowConfig()
    {
        return {
            sf::VideoMode::getDesktopMode(),
            "PZ Tiles Browser",
        };
    }

public:
    WindowTilesBrowser(AppContext &_app_context) : AppWindow(_app_context, windowConfig())
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

    void update() override
    {
        AppWindow::update();

        if (appContext.isLoaded && tilesBrowser == nullptr)
        {
            tilesBrowser = std::make_unique<TilesBrowser>(gui, window, *appContext.tilesheetService);

            gui.remove(loadingLabel);
            gui.remove(loadingSpinner);
        }
        else if (appContext.isLoaded)
        {
            tilesBrowser->update(window);
        }
        else
        {
            loadingLabel->setText(appContext.loadingPayload.getMessage());
            loadingSpinner->setRotation(loadingSpinner->getRotation() + 5);
        }
    }
};