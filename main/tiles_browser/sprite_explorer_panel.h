#pragma once

#include <functional>
#include <vector>

#include <SFML/Graphics/RenderWindow.hpp>

#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/String.hpp>
#include <TGUI/Widgets/EditBox.hpp>
#include <TGUI/Widgets/Panel.hpp>
#include <TGUI/Widgets/TreeView.hpp>
#include <TGUI/Widgets/VerticalLayout.hpp>

#include "services/game_files_service.h"

class SpriteExplorerPanel
{
private:
    tgui::Panel::Ptr panel;
    tgui::VerticalLayout::Ptr vlayout;
    tgui::EditBox::Ptr searchInput;
    tgui::TreeView::Ptr tilesTree;

    std::vector<std::vector<tgui::String>> treeNodes;

public:
    SpriteExplorerPanel(tgui::Gui &gui, GameFilesService &gameFilesService);

    void onItemSelect(const std::vector<tgui::String> &selectedItem);
    void onFilterChange(const tgui::String &keyword);
    void onPageSelect(std::function<void(const tgui::String &pageName)>);
    void InitTilesTree(GameFilesService &gameFilesService);
    void update(const sf::RenderWindow &window);
};