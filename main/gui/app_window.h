#pragma once

#include <SFML/Graphics/View.hpp>
#include <SFML/Window/Event.hpp>

#include "TGUI/Backend/SFML-Graphics.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/VideoMode.hpp>

#include "app_context.h"

class AppWindow
{
protected:
    struct WindowConfig
    {
        sf::VideoMode videoMode = sf::VideoMode::getDesktopMode();
        std::string title = "Default";
    };

    sf::RenderWindow window;
    sf::View view;
    tgui::Gui gui;

    AppContext &appContext;
    AppWindow(AppContext &_appContext, WindowConfig &&config);

public:
    virtual ~AppWindow() = default;

    inline bool isOpen() const { return window.isOpen(); }

    virtual void handleEvents(const sf::Event &event) {}
    virtual void update();
    virtual void render();

    void processEvents();
};