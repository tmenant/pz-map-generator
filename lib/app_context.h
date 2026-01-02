#pragma once

#include <atomic>
#include <memory>
#include <string>

#include "services/tilesheet_service.h"

struct AppContext
{
    // loading state
    std::atomic<int> progression = 0;
    std::atomic<bool> isLoaded = false;
    std::string currentStatus;
    LoadingPayload loadingPayload;

    // data services
    std::unique_ptr<TilesheetService> tilesheetService = nullptr;
};