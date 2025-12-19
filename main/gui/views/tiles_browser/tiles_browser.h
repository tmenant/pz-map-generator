#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>

#include "files/texturepack.h"
#include "services/tilesheet_service.h"
#include "sprite_explorer_panel.h"
#include "sprite_info_panel.h"

class TilesBrowser
{
private:
    SpriteInfoPanel spriteInfoPanel;
    SpriteExplorerPanel spriteExplorerPanel;

    sf::Texture texture;
    sf::Sprite sprite;

    TexturePack::Page *currentPage = nullptr;
    TexturePack::Texture *hoveredTexture = nullptr;

public:
    TilesBrowser(tgui::Gui &gui, sf::RenderWindow &window, TilesheetService &tilesheetService);

    void update(sf::RenderWindow &window);

    TexturePack::Texture *getTextureByName(TexturePack::Page *page, std::string textureName);
    TexturePack::Texture *getTextureByPosition(TexturePack::Page *page, sf::Vector2f pos);

    sf::Vector2u getPNGSize(const BytesBuffer &data);
    sf::RectangleShape getOutlineRectangle(TexturePack::Texture *textureData, sf::Transform transform);

    void drawSpriteOutline(sf::RenderWindow &window, sf::Sprite &sprite, TexturePack::Page *page, TexturePack::Texture *&hoveredTexture);
    void updateSpriteTexture(sf::Sprite &sprite, sf::Texture &texture, TexturePack::Page *page, const sf::RenderWindow &window);
};