/*
MIT License

Copyright (c) 2025 Patryk Czachurski

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#pragma once

#include "finder_interfaces.h"
#include "rect_structs.h"
#include <fmt/base.h>
#include <fmt/format.h>

namespace rectpack2D
{
    inline rect_wh packRectangles(std::vector<rectpack2D::rect_xywh> &rectangles, int max_side = 8192, int discard_step = -4)
    {
        constexpr bool allow_flip = false;
        const auto runtime_flipping_mode = flipping_option::DISABLED;

        using spaces_type = rectpack2D::empty_spaces<allow_flip, default_empty_spaces>;
        using rect_type = output_rect_t<spaces_type>;

        auto report_successful = [](rect_type &)
        {
            return callback_result::CONTINUE_PACKING;
        };

        auto report_unsuccessful = [](rect_type &)
        {
            return callback_result::ABORT_PACKING;
        };

        return find_best_packing<spaces_type>(
            rectangles,
            make_finder_input(
                max_side,
                discard_step,
                report_successful,
                report_unsuccessful,
                runtime_flipping_mode));
    }
};