#pragma once

#include <string>

#include <fmt/base.h>

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window.hpp>

#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/String.hpp>
#include <TGUI/TGUI.hpp>
#include <TGUI/Widgets/EditBox.hpp>
#include <TGUI/Widgets/Grid.hpp>
#include <TGUI/Widgets/ListBox.hpp>
#include <TGUI/Widgets/VerticalLayout.hpp>

#include "files/texturepack.h"

class SpriteInfoPanel
{
private:
    tgui::Panel::Ptr panel;
    tgui::VerticalLayout::Ptr vLayout;
    tgui::Label::Ptr pageNameLabel;
    tgui::Grid::Ptr texureInfoGrid;
    tgui::EditBox::Ptr searchInput;
    tgui::ListBox::Ptr spritesList;

    std::map<std::string, tgui::Label::Ptr> fields;
    std::vector<tgui::String> spriteNames;

    const std::vector<std::string> names = { "name", "x", "y", "width", "height", "ox", "oy", "ow", "oh" };

public:
    SpriteInfoPanel(tgui::Gui &gui);

    void update(sf::RenderWindow &window, const TexturePack::Page *page, const TexturePack::Texture *texture);

    void InitTextureInfo();
    void InitSpritesList(const TexturePack::Page *page);

    void onTextureSelect(std::function<void(tgui::String)> func);
    void onFilterChange(tgui::String keyword);
    void setItemSelected(std::string itemName);
    void setPage(const TexturePack::Page *page);
};
