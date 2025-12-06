#pragma once

#include "TGUI/Color.hpp"
#include <SFML/Graphics/Color.hpp>
#include <cstdint>

namespace Colors
{
    struct Color
    {
        uint8_t r;
        uint8_t g;
        uint8_t b;

        sf::Color sfml() const
        {
            return sf::Color(r, g, b);
        }

        tgui::Color tgui() const
        {
            return tgui::Color(r, g, b);
        }
    };

    const Color panelColor = Color(54, 61, 74);
    const Color buttonColor = Color(37, 43, 52);
    const Color backgroundColor = Color(33, 38, 46);
    const Color fontColor = Color(255, 255, 255);
    const Color selectedItem = Color(55, 79, 102);
    const Color hoveredItem = Color(42, 54, 68);
};