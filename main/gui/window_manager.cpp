#include "window_manager.h"
#include "constants.h"
#include <thread>

void WindowManager::cleanupClosedWindows()
{
    std::erase_if(windows, [](auto &win)
    {
        return !win->isOpen();
    });
}

void WindowManager::run()
{
    if (isRunning)
    {
        throw std::runtime_error("Window Manager is already running.");
    }

    isRunning = true;

    std::thread loadingThread([this]()
    {
        appContext.tilesheetService = std::make_unique<TilesheetService>(constants::GAME_PATH, appContext.loadingPayload);
        appContext.isLoaded = true;
    });

    while (!windows.empty())
    {
        cleanupClosedWindows();

        if (appContext.isLoaded && loadingThread.joinable())
        {
            loadingThread.join();
        }

        for (auto &window : windows)
        {
            window->processEvents();
            window->update();
            window->render();
        }
    }

    if (loadingThread.joinable()) loadingThread.join();
}