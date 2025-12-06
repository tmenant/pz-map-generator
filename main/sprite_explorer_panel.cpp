#include <fmt/format.h>

#include <sprite_explorer_panel.h>
#include <theme.h>

SpriteExplorerPanel::SpriteExplorerPanel(tgui::Gui &gui, GameFilesService &gameFilesService)
{
    panel = tgui::Panel::create();
    panel->getRenderer()->setBackgroundColor(Colors::panelColor.tgui());

    searchInput = tgui::EditBox::create();
    searchInput->getRenderer()->setTextColor(Colors::fontColor.tgui());

    tilesTree = tgui::TreeView::create();
    tilesTree->getRenderer()->setBackgroundColor(tgui::Color::Transparent);
    tilesTree->getRenderer()->setTextColor(Colors::fontColor.tgui());

    InitTilesTree(gameFilesService);

    tilesTree->onItemSelect(&SpriteExplorerPanel::onItemSelect, this);

    // panel->add(searchInput);
    panel->add(tilesTree);
    gui.add(panel);
}

void SpriteExplorerPanel::onItemSelect(const std::vector<tgui::String> &selectedItem)
{
    if (selectedItem.size() > 1)
    {
        fmt::println("selected: {} / {}", selectedItem[0].toStdString(), selectedItem[1].toStdString());
    }
}

void SpriteExplorerPanel::InitTilesTree(GameFilesService &gameFilesService)
{
    for (auto &texturePack : gameFilesService.texturePacks)
    {
        for (auto &page : texturePack.pages)
        {
            std::string packName = fmt::format("{} ({})", texturePack.name, texturePack.pages.size());
            tilesTree->addItem({ packName, page.name });
        }
    }

    tilesTree->collapseAll();
}

void SpriteExplorerPanel::update(const sf::RenderWindow &window)
{
    auto winsize = window.getSize();
    panel->setPosition(0, 0);
    panel->setSize(250, winsize.y);

    tilesTree->setSize(panel->getInnerSize());
}