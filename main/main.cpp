#include <crtdbg.h>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Vector2.hpp>
#include <iostream>
#include <stdexcept>
#include <string>

#include <fmt/base.h>
#include <fmt/format.h>
#include <lodepng.h>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Window/WindowEnums.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <cpptrace/from_current.hpp>

#include "TGUI/Color.hpp"
#include "TGUI/Widgets/Panel.hpp"
#include "constants.h"
#include "files/texturepack.h"
#include "services/game_files_service.h"
#include "sprite_panel.h"
#include "types.h"

#include <dwmapi.h>
#include <windows.h>

WNDPROC g_originalProc = nullptr;

LRESULT CALLBACK CustomWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    return CallWindowProcA(g_originalProc, hwnd, msg, wParam, lParam);
}

void setWindowDarkMode(const sf::RenderWindow &window)
{
    HWND hwnd = reinterpret_cast<HWND>(window.getNativeHandle());

    g_originalProc = reinterpret_cast<WNDPROC>(
        SetWindowLongPtrW(hwnd, GWLP_WNDPROC, (LONG_PTR)CustomWndProc));

    BOOL dark = TRUE;
    DwmSetWindowAttribute(hwnd, 20, &dark, sizeof(dark));
    DwmSetWindowAttribute(hwnd, 19, &dark, sizeof(dark));

    ShowWindow(hwnd, SW_MINIMIZE);
    ShowWindow(hwnd, SW_RESTORE);

    SendMessageW(hwnd, WM_NCACTIVATE, TRUE, 0);
}

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

TexturePack::Texture *getTextureByName(TexturePack::Page &page, std::string textureName)
{
    TexturePack::Texture *textureData = nullptr;

    for (int i = 0; i < page.textures.size(); i++)
    {
        if (page.textures[i].name == textureName)
        {
            textureData = &page.textures[i];
            break;
        }
    }

    if (textureData == nullptr)
    {
        throw std::runtime_error("texture not found");
    }

    return textureData;
}

TexturePack::Texture *getTextureByPosition(TexturePack::Page &page, sf::Vector2f pos)
{
    for (int i = 0; i < page.textures.size(); i++)
    {
        TexturePack::Texture text = page.textures[i];

        if (pos.x >= text.x && pos.y >= text.y && pos.x < text.x + text.width && pos.y < text.y + text.height)
        {
            return &page.textures[i];
        }
    }

    return nullptr;
}

sf::Texture loadTexture(TexturePack::Page &page)
{
    sf::Texture texture;

    if (!texture.loadFromMemory(page.png.data(), page.png.size()))
    {
        throw std::runtime_error("Erreur : Échec du chargement de la texture depuis le buffer PNG.");
    }

    return texture;
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

void drawSpriteOutline(sf::RenderWindow &window, sf::Sprite &sprite, TexturePack::Page &page, TexturePack::Texture *&hoveredTexture)
{
    sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    sf::Transform inverse = sprite.getInverseTransform();
    sf::Vector2f local = inverse.transformPoint(mouse);

    hoveredTexture = getTextureByPosition(page, local);

    if (hoveredTexture != nullptr)
    {
        sf::RectangleShape spriteOutiline = getOutlineRectangle(hoveredTexture, sprite.getTransform());
        window.draw(spriteOutiline);
    }
}

void main_window()
{
    sf::RenderWindow window(sf::VideoMode({ 1920, 1080 }), "PZ Map Generator");
    sf::Vector2u winsize = window.getSize();
    sf::Clock deltaClock;

    setWindowDarkMode(window);

    GameFilesService gamefileService(constants::GAME_PATH);
    TexturePack::Page page = gamefileService.getPageByName("Tiles1x1");

    sf::Vector2u textureSize = getPNGSize(page.png);
    sf::Texture texture = loadTexture(page);
    sf::Sprite sprite(texture);
    sf::FloatRect bounds = sprite.getLocalBounds();
    const float size = 1.0f;
    sprite.setScale({ size, size });
    sprite.setOrigin({ bounds.size.x / 2.f, bounds.size.y / 2.f });
    sprite.setPosition({ winsize.x / 2.f, winsize.y / 2.f });

    TexturePack::Texture *hoveredTexture = nullptr;

    bool firstFrame = true;

    tgui::Gui gui{ window };

    auto explorerPanel = tgui::Panel::create();
    explorerPanel->getRenderer()->setBackgroundColor(tgui::Color(54, 61, 74));
    gui.add(explorerPanel);

    auto spritePanel = SpritePanel(gui);

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            gui.handleEvent(*event);

            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
            // else if (const auto *resized = event->getIf<sf::Event::Resized>())
            // {
            //     sf::FloatRect visibleArea({ 0.f, 0.f }, { static_cast<float>(resized->size.x), static_cast<float>(resized->size.y) });
            //     window.setView(sf::View(visibleArea));
            // }
        }

        // viewport drawings
        window.clear(sf::Color(33, 38, 46));
        window.draw(sprite);
        drawSpriteOutline(window, sprite, page, hoveredTexture);

        explorerPanel->setSize(250, winsize.y);
        explorerPanel->setPosition(0, 0);

        spritePanel.update(window, hoveredTexture);

        gui.draw();
        window.display();
        firstFrame = false;
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