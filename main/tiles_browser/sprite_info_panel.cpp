#include <string>
#include <vector>

#include <fmt/base.h>

#include <TGUI/Color.hpp>
#include <TGUI/Cursor.hpp>
#include <TGUI/Layout.hpp>
#include <TGUI/Outline.hpp>
#include <TGUI/String.hpp>
#include <TGUI/TextStyle.hpp>
#include <TGUI/Widgets/EditBox.hpp>
#include <TGUI/Widgets/Grid.hpp>
#include <TGUI/Widgets/Label.hpp>
#include <TGUI/Widgets/ListBox.hpp>
#include <TGUI/Widgets/VerticalLayout.hpp>

#include "files/texturepack.h"
#include "sprite_info_panel.h"
#include "theme.h"

SpriteInfoPanel::SpriteInfoPanel(tgui::Gui &gui)
{
    panel = tgui::Panel::create();
    panel->getRenderer()->setBackgroundColor(Colors::panelColor.tgui());
    panel->getRenderer()->setPadding(tgui::Padding(5));

    vLayout = tgui::VerticalLayout::create();

    pageNameLabel = tgui::Label::create();
    pageNameLabel->setSize({ "100%", "100%" });
    pageNameLabel->getRenderer()->setTextSize(20);
    pageNameLabel->getRenderer()->setTextColor(Colors::fontColor.tgui());
    pageNameLabel->getRenderer()->setTextStyle(tgui::TextStyle::Bold);
    pageNameLabel->setHorizontalAlignment(tgui::HorizontalAlignment::Center);

    texureInfoGrid = tgui::Grid::create();
    texureInfoGrid->setSize({ "100%", "100%" });

    searchInput = tgui::EditBox::create();
    searchInput->setSize({ "100%", "100%" });
    searchInput->setDefaultText("Filter sprite names");
    searchInput->setMouseCursor(tgui::Cursor::Type::Text);
    searchInput->getRenderer()->setBackgroundColor(Colors::buttonColor.tgui());
    searchInput->getRenderer()->setBackgroundColorHover(Colors::buttonColor.tgui());
    searchInput->getRenderer()->setTextColor(Colors::fontColor.tgui());
    searchInput->getRenderer()->setCaretColor(Colors::fontColor.tgui());
    searchInput->getRenderer()->setCaretColor(Colors::fontColor.tgui());
    searchInput->onTextChange(&SpriteInfoPanel::onFilterChange, this);

    spritesList = tgui::ListBox::create();
    spritesList->setSize({ "100%", "100%" });
    spritesList->getRenderer()->setBorderColor(tgui::Color::Transparent);
    spritesList->getRenderer()->setBackgroundColor(tgui::Color::Transparent);
    spritesList->getRenderer()->setBackgroundColorHover(Colors::hoveredItem.tgui());
    spritesList->getRenderer()->setSelectedBackgroundColor(Colors::selectedItem.tgui());
    spritesList->getRenderer()->setSelectedBackgroundColorHover(Colors::selectedItem.tgui());
    spritesList->getRenderer()->setTextColor(Colors::fontColor.tgui());
    spritesList->getRenderer()->setTextColorHover(Colors::fontColor.tgui());
    spritesList->getScrollbar()->setScrollAmount(100);

    InitTextureInfo();

    vLayout->add(pageNameLabel, 0.05f);
    vLayout->add(texureInfoGrid, 0.2f);
    vLayout->addSpace(0.01f);
    vLayout->add(searchInput, 0.025f);
    vLayout->addSpace(0.01f);
    vLayout->add(spritesList);
    vLayout->setVisible(false);

    panel->add(vLayout);
    gui.add(panel);
}

void SpriteInfoPanel::setPage(const TexturePack::Page *page)
{
    vLayout->setVisible(page != nullptr);

    if (page != nullptr)
    {
        pageNameLabel->setText(page->name);
        InitSpritesList(page);
    }
}

void SpriteInfoPanel::onFilterChange(tgui::String keyword)
{
    spritesList->removeAllItems();

    for (const auto &spriteName : spriteNames)
    {
        if (keyword == "" || spriteName.toLower().contains(keyword.toLower()))
        {
            spritesList->addItem(spriteName);
        }
    }

    spritesList->getScrollbar()->setValue(0);
}

void SpriteInfoPanel::setItemSelected(std::string itemName)
{
    spritesList->setSelectedItem(itemName);
}

void SpriteInfoPanel::onTextureSelect(std::function<void(tgui::String)> func)
{
    spritesList->onItemSelect(func);
}

void SpriteInfoPanel::InitTextureInfo()
{
    int row = 0;
    for (const auto &key : names)
    {
        auto labelTitle = tgui::Label::create(key + " :");
        auto labelValue = tgui::Label::create("");

        labelTitle->getRenderer()->setTextColor(tgui::Color::White);
        labelValue->getRenderer()->setTextColor(tgui::Color::White);

        fields[key] = labelValue;

        texureInfoGrid->addWidget(labelTitle, row, 0, tgui::Grid::Alignment::Right);
        texureInfoGrid->addWidget(labelValue, row, 1, tgui::Grid::Alignment::Left);

        row++;
    }
}

void SpriteInfoPanel::InitSpritesList(const TexturePack::Page *page)
{
    spriteNames = std::vector<tgui::String>();

    spritesList->removeAllItems();

    for (const auto &texture : page->textures)
    {
        spriteNames.push_back(texture.name);
        spritesList->addItem(texture.name);
    }

    spritesList->getScrollbar()->setValue(0);
}

void SpriteInfoPanel::update(sf::RenderWindow &window, const TexturePack::Page *page, const TexturePack::Texture *texture)
{
    auto winsize = window.getSize();
    panel->setSize(250, winsize.y);
    panel->setPosition(winsize.x - 250, 0);

    if (texture == nullptr)
    {
        texureInfoGrid->setVisible(false);
        return;
    }

    texureInfoGrid->setVisible(true);

    fields["name"]->setText(texture->name);
    fields["x"]->setText(std::to_string(texture->x));
    fields["y"]->setText(std::to_string(texture->y));
    fields["width"]->setText(std::to_string(texture->width));
    fields["height"]->setText(std::to_string(texture->height));
    fields["ox"]->setText(std::to_string(texture->ox));
    fields["oy"]->setText(std::to_string(texture->oy));
    fields["ow"]->setText(std::to_string(texture->ow));
    fields["oh"]->setText(std::to_string(texture->oh));
}