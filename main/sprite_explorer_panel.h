#pragma once

#include <SFML/Graphics/RenderWindow.hpp>

#include "TGUI/Backend/SFML-Graphics.hpp"
#include "TGUI/Widgets/EditBox.hpp"
#include "TGUI/Widgets/Panel.hpp"
#include "TGUI/Widgets/TreeView.hpp"

#include "services/game_files_service.h"

class SpriteExplorerPanel
{
private:
    tgui::EditBox::Ptr searchInput;
    tgui::TreeView::Ptr tilesTree;
    tgui::Panel::Ptr panel;

public:
    SpriteExplorerPanel(tgui::Gui &gui, GameFilesService &gameFilesService);

    void onItemSelect(const std::vector<tgui::String> &selectedItem);
    void InitTilesTree(GameFilesService &gameFilesService);
    void update(const sf::RenderWindow &window);
};