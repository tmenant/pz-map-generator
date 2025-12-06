#pragma once

#include <fmt/format.h>
#include <stdexcept>

namespace Exceptions
{
    class FileEndNotReached : public std::runtime_error
    {
    public:
        explicit FileEndNotReached(size_t offset, size_t size) noexcept :
                std::runtime_error(fmt::format("File end not reached: {} / {}", offset, size)) {}
    };

    class NotImplemented : public std::runtime_error
    {
    public:
        explicit NotImplemented() noexcept :
                std::runtime_error("Not Implemented.") {}
    };
}
