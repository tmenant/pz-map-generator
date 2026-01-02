#pragma once

#include "app_context.h"
#include "gui/app_window.h"
#include "gui/components/loading_spinner.h"
#include "sprite_explorer_panel.h"
#include "sprite_info_panel.h"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <memory>

class WindowTilesBrowser : public AppWindow
{
private:
    std::unique_ptr<SpriteInfoPanel> spriteInfoPanel = nullptr;
    std::unique_ptr<SpriteExplorerPanel> spriteExplorerPanel = nullptr;
    std::unique_ptr<LoadingSpinner> loadingSpinner = nullptr;

    sf::Texture texture;
    sf::Sprite sprite;

    TexturePack::Page *currentPage = nullptr;
    TexturePack::Texture *hoveredTexture = nullptr;

    bool initialized = false;

    static WindowConfig windowConfig()
    {
        return {
            sf::VideoMode::getDesktopMode(),
            "PZ Tiles Browser",
        };
    }

public:
    WindowTilesBrowser(AppContext &_appContext);

    void ready();
    void update() override;

    void updateWindow();
    void updateSpriteTexture();
    void drawSpriteOutline(sf::RenderWindow &window, sf::Sprite &sprite, TexturePack::Page *page, TexturePack::Texture *&hoveredTexture);

    sf::RectangleShape getOutlineRectangle(TexturePack::Texture *textureData, sf::Transform transform);
    TexturePack::Texture *getTextureByPosition(TexturePack::Page *page, sf::Vector2f pos);
};