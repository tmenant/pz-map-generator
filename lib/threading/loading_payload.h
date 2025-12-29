#pragma once

#include <atomic>
#include <string>

struct LoadingPayload
{
private:
    std::atomic<int> progression{ 0 };
    std::string message;
    mutable std::mutex messageMutex;

public:
    void updateProgression(int value) noexcept
    {
        progression.store(value, std::memory_order_relaxed);
    }

    void updateMessage(const std::string &value)
    {
        std::lock_guard<std::mutex> lock(messageMutex);
        message = value;
    }

    void update(int value, const std::string &valueMessage)
    {
        {
            std::lock_guard<std::mutex> lock(messageMutex);
            message = valueMessage;
        }
        progression.store(value, std::memory_order_relaxed);
    }

    std::string getMessage() const
    {
        std::lock_guard<std::mutex> lock(messageMutex);
        return message;
    }

    int getProgression() const noexcept
    {
        return progression.load(std::memory_order_relaxed);
    }
};