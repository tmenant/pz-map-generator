#pragma once

#include <string>

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window.hpp>

#include "TGUI/Widgets/Grid.hpp"
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>

#include <fmt/base.h>

#include "files/texturepack.h"

class SpritePanel
{
private:
    tgui::Panel::Ptr panel;
    tgui::Grid::Ptr grid;

    std::map<std::string, tgui::Label::Ptr> fields;

    const std::vector<std::string> names = { "name", "x", "y", "width", "height", "ox", "oy", "ow", "oh" };

public:
    SpritePanel(tgui::Gui &gui);
    void update(sf::RenderWindow &window, TexturePack::Texture *texture);
};
