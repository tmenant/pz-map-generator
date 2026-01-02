#include "app_window.h"
#include "platform.h"
#include "theme.h"

AppWindow::AppWindow(AppContext &_appContext, WindowConfig &&windowConfig) : appContext(_appContext)
{
    window.create(windowConfig.videoMode, windowConfig.title);
    window.setFramerateLimit(60);

    platform::windows::setWindowDarkMode(window);

    view = window.getDefaultView();
    gui.setWindow(window);
}

void AppWindow::update()
{
    window.clear(Colors::backgroundColor.sfml());
}

void AppWindow::render()
{
    gui.draw();
    window.display();
}

void AppWindow::processEvents()
{
    while (const auto event = window.pollEvent())
    {
        gui.handleEvent(*event);
        this->handleEvents(*event);

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
}