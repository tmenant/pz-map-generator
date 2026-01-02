#pragma once

#include "app_context.h"
#include "app_window.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/VideoMode.hpp>

class WindowManager
{
private:
    bool isRunning = false;

    AppContext appContext;
    std::vector<std::unique_ptr<AppWindow>> windows;

public:
    WindowManager() = default;

    template <typename T>
    void createWindow()
    {
        windows.emplace_back(std::make_unique<T>(appContext));
    }

    void cleanupClosedWindows();
    void run();
};