#include <vector>

#include <fmt/format.h>

#include <TGUI/Color.hpp>
#include <TGUI/Layout.hpp>
#include <TGUI/Outline.hpp>
#include <TGUI/String.hpp>
#include <TGUI/Widgets/Panel.hpp>
#include <TGUI/Widgets/VerticalLayout.hpp>

#include "sprite_explorer_panel.h"
#include "theme.h"

SpriteExplorerPanel::SpriteExplorerPanel(tgui::Gui &gui, const TilesheetService &tilesheetService)
{
    panel = tgui::Panel::create();
    panel->getRenderer()->setBackgroundColor(Colors::panelColor.tgui());
    panel->getRenderer()->setPadding(tgui::Padding(5));
    panel->setSize({ "20%", "100%" });

    vlayout = tgui::VerticalLayout::create();

    searchInput = tgui::EditBox::create();
    searchInput->setSize({ "100%", "100%" });
    searchInput->setDefaultText("Filter page names");
    searchInput->setMouseCursor(tgui::Cursor::Type::Text);
    searchInput->getRenderer()->setBackgroundColor(Colors::buttonColor.tgui());
    searchInput->getRenderer()->setBackgroundColorHover(Colors::buttonColor.tgui());
    searchInput->getRenderer()->setTextColor(Colors::fontColor.tgui());
    searchInput->getRenderer()->setCaretColor(Colors::fontColor.tgui());
    searchInput->getRenderer()->setCaretColor(Colors::fontColor.tgui());
    searchInput->onTextChange(&SpriteExplorerPanel::onFilterChange, this);

    tilesTree = tgui::TreeView::create();
    tilesTree->setSize({ "100%", "100%" });
    tilesTree->getRenderer()->setBackgroundColor(tgui::Color::Transparent);
    tilesTree->getRenderer()->setBorderColor(tgui::Color::Transparent);
    tilesTree->getRenderer()->setTextColor(Colors::fontColor.tgui());
    tilesTree->getRenderer()->setBackgroundColorHover(Colors::hoveredItem.tgui());
    tilesTree->getRenderer()->setSelectedBackgroundColor(Colors::selectedItem.tgui());
    tilesTree->getRenderer()->setSelectedBackgroundColorHover(Colors::selectedItem.tgui());
    tilesTree->getRenderer()->setTextColorHover(Colors::fontColor.tgui());
    tilesTree->getHorizontalScrollbar()->setPolicy(tgui::Scrollbar::Policy::Never);

    InitTilesTree(tilesheetService);

    vlayout->add(searchInput, 0.025f);
    vlayout->addSpace(0.01f);
    vlayout->add(tilesTree);
    panel->add(vlayout);
    gui.add(panel);
}

void SpriteExplorerPanel::onPageSelect(std::function<void(const tgui::String &pageName)> func)
{
    tilesTree->onItemSelect(func);
}

void SpriteExplorerPanel::onFilterChange(const tgui::String &keyword)
{
    if (keyword == "")
    {
        tilesTree->removeAllItems();

        for (const auto &node : treeNodes)
        {
            tilesTree->addItem(node);
        }

        tilesTree->collapseAll();
        return;
    }

    tgui::String lowerKeyword = keyword.toLower();

    for (const std::vector<tgui::String> &node : treeNodes)
    {
        if (node.size() < 2)
            continue;

        if (node[1].toLower().contains(lowerKeyword))
        {
            tilesTree->expand(node);
        }
        else
        {
            tilesTree->removeItem(node);
        }
    }
}

void SpriteExplorerPanel::InitTilesTree(const TilesheetService &tilesheetService)
{
    treeNodes = std::vector<std::vector<tgui::String>>();

    for (const auto &texturePack : tilesheetService.texturePacks)
    {
        for (const auto &page : texturePack.pages)
        {
            std::string packName = fmt::format("{} ({})", texturePack.name, texturePack.pages.size());

            tilesTree->addItem({ packName, page.name });
            treeNodes.push_back({ packName, page.name });
        }
    }

    tilesTree->collapseAll();
}

void SpriteExplorerPanel::update(const sf::RenderWindow &window)
{
    auto winsize = window.getSize();
    panel->setPosition(0, 0);
    panel->setSize(250, winsize.y);

    vlayout->setSize(panel->getInnerSize());
}