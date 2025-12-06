#include "sprite_info_panel.h"
#include "theme.h"

SpritePanel::SpritePanel(tgui::Gui &gui)
{
    panel = tgui::Panel::create();
    panel->getRenderer()->setBackgroundColor(Colors::panelColor.tgui());

    grid = tgui::Grid::create();
    grid->setSize(panel->getSize());

    panel->add(grid);
    gui.add(panel);

    int row = 0;
    for (const auto &key : names)
    {
        auto labelTitle = tgui::Label::create(key + " :");
        auto labelValue = tgui::Label::create("");

        labelTitle->getRenderer()->setTextColor(tgui::Color::White);
        labelValue->getRenderer()->setTextColor(tgui::Color::White);

        fields[key] = labelValue;

        grid->addWidget(labelTitle, row, 0, tgui::Grid::Alignment::Right);
        grid->addWidget(labelValue, row, 1, tgui::Grid::Alignment::Left);

        row++;
    }
}

void SpritePanel::update(sf::RenderWindow &window, TexturePack::Texture *texture)
{
    auto winsize = window.getSize();
    panel->setSize(250, winsize.y);
    panel->setPosition(winsize.x - 250, 0);

    if (texture == nullptr)
    {
        grid->setVisible(false);
        return;
    }

    grid->setVisible(true);

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