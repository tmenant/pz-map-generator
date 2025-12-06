#include <crtdbg.h>

#include <iostream>
#include <stdexcept>
#include <string>

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window.hpp>
#include <SFML/Window/WindowEnums.hpp>

#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/String.hpp>
#include <TGUI/TGUI.hpp>

#include <fmt/format.h>
#include <lodepng.h>
#include <cpptrace/from_current.hpp>

#include "files/texturepack.h"
#include "services/game_files_service.h"

#include "tiles_browser/sprite_explorer_panel.h"
#include "tiles_browser/sprite_info_panel.h"

#include "constants.h"
#include "platform.h"
#include "theme.h"

sf::Vector2u getPNGSize(const BytesBuffer &data)
{
    if (data.size() < 24)
        throw std::runtime_error("Not enough data for PNG");

    const uint8_t pngSig[8] = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };
    for (int i = 0; i < 8; ++i)
        if (data[i] != pngSig[i])
            throw std::runtime_error("Invalid PNG signature");

    uint32_t width = (data[16] << 24) | (data[17] << 16) | (data[18] << 8) | data[19];
    uint32_t height = (data[20] << 24) | (data[21] << 16) | (data[22] << 8) | data[23];

    return sf::Vector2u{ width, height };
}

TexturePack::Texture *getTextureByName(TexturePack::Page *page, std::string textureName)
{
    if (page == nullptr)
        return nullptr;

    TexturePack::Texture *textureData = nullptr;

    for (int i = 0; i < page->textures.size(); i++)
    {
        if (page->textures[i].name == textureName)
        {
            textureData = &page->textures[i];
            break;
        }
    }

    return textureData;
}

TexturePack::Texture *getTextureByPosition(TexturePack::Page *page, sf::Vector2f pos)
{
    for (int i = 0; i < page->textures.size(); i++)
    {
        TexturePack::Texture text = page->textures[i];

        if (pos.x >= text.x && pos.y >= text.y && pos.x < text.x + text.width && pos.y < text.y + text.height)
        {
            return &page->textures[i];
        }
    }

    return nullptr;
}

sf::RectangleShape getOutlineRectangle(TexturePack::Texture *textureData, sf::Transform transform)
{
    sf::FloatRect rect(
        { static_cast<float>(textureData->x), static_cast<float>(textureData->y) },
        { static_cast<float>(textureData->width), static_cast<float>(textureData->height) });

    rect = transform.transformRect(rect);

    sf::RectangleShape rectangle(rect.size);
    rectangle.setPosition(rect.position);
    rectangle.setOutlineColor(sf::Color::Red);
    rectangle.setOutlineThickness(1.0f);
    rectangle.setFillColor(sf::Color::Transparent);

    return rectangle;
}

void drawSpriteOutline(sf::RenderWindow &window, sf::Sprite &sprite, TexturePack::Page *page, TexturePack::Texture *&hoveredTexture)
{
    sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    sf::Transform inverse = sprite.getInverseTransform();
    sf::Vector2f local = inverse.transformPoint(mouse);
    sf::FloatRect bounds = sprite.getLocalBounds();

    if (bounds.contains(local))
    {
        hoveredTexture = getTextureByPosition(page, local);
    }

    if (hoveredTexture != nullptr)
    {
        sf::RectangleShape spriteOutiline = getOutlineRectangle(hoveredTexture, sprite.getTransform());
        window.draw(spriteOutiline);
    }
}

void updateSpriteTexture(sf::Sprite &sprite, sf::Texture &texture, TexturePack::Page *page, const sf::RenderWindow &window)
{
    sf::Vector2u textureSize = getPNGSize(page->png);

    if (!texture.loadFromMemory(page->png.data(), page->png.size()))
    {
        throw std::runtime_error("Erreur : Échec du chargement de la texture depuis le buffer PNG.");
    }

    sprite.setTexture(texture, true);
    sf::FloatRect bounds = sprite.getLocalBounds();
    sprite.setOrigin({ bounds.size.x / 2.f, bounds.size.y / 2.f });
}

void main_window()
{
    sf::RenderWindow window(sf::VideoMode({ 1920, 1080 }), "PZ Map Generator");
    sf::Vector2u winsize = window.getSize();
    sf::Clock deltaClock;

    window.setFramerateLimit(60);

    sf::Texture texture;
    sf::Sprite sprite(texture);

    platform::windows::setWindowDarkMode(window);

    GameFilesService gamefileService(constants::GAME_PATH);

    TexturePack::Page *currentPage = nullptr;
    TexturePack::Texture *hoveredTexture = nullptr;

    tgui::Gui gui{ window };

    SpriteExplorerPanel spriteExplorer(gui, gamefileService);
    SpriteInfoPanel spriteInfoPanel(gui);

    spriteExplorer.onPageSelect([&](const tgui::String &pageName)
    {
        currentPage = gamefileService.getPageByName(pageName.toStdString());

        if (currentPage != nullptr)
        {
            updateSpriteTexture(sprite, texture, currentPage, window);
            spriteInfoPanel.setPage(currentPage);
            hoveredTexture = nullptr;
        }
    });

    spriteInfoPanel.onTextureSelect([&](const tgui::String &item)
    {
        hoveredTexture = getTextureByName(currentPage, item.toStdString());
    });

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            gui.handleEvent(*event);

            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
            else if (const auto *resized = event->getIf<sf::Event::Resized>())
            {
                sf::FloatRect visibleArea({ 0.f, 0.f }, { static_cast<float>(resized->size.x), static_cast<float>(resized->size.y) });
                window.setView(sf::View(visibleArea));
            }
        }

        // viewport drawings
        window.clear(Colors::backgroundColor.sfml());

        if (currentPage != nullptr)
        {
            window.draw(sprite);
            drawSpriteOutline(window, sprite, currentPage, hoveredTexture);
        }

        sprite.setPosition({ window.getSize().x / 2.f, window.getSize().y / 2.f });

        // ui drawing
        spriteInfoPanel.update(window, currentPage, hoveredTexture);
        spriteExplorer.update(window);

        gui.draw();
        window.display();
    }
}

int main()
{
    CPPTRACE_TRY
    {
        main_window();
    }
    CPPTRACE_CATCH(const std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        cpptrace::from_current_exception().print();
    }
}