#include <crtdbg.h>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include <fmt/format.h>
#include <lodepng.h>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <cpptrace/from_current.hpp>

#include "constants.h"
#include "files/lotheader.h"
#include "files/lotpack.h"
#include "files/texturepack.h"
#include "files/tiledefinition.h"
#include "io/binary_reader.h"
#include "io/file_reader.h"
#include "math/md5.h"
#include "services/game_files_service.h"

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

sf::RectangleShape getOutlineRectangle(TexturePack::Texture *textureData, sf::Transform transform)
{
    sf::Vector2f size = transform.transformPoint({ static_cast<float>(textureData->width), static_cast<float>(textureData->height) });
    sf::Vector2f position = transform.transformPoint({ static_cast<float>(textureData->x), static_cast<float>(textureData->y) });

    sf::RectangleShape rectangle(size);
    rectangle.setPosition(position);
    rectangle.setOutlineColor(sf::Color::Red);
    rectangle.setOutlineThickness(1.0f);
    rectangle.setFillColor(sf::Color::Transparent);

    return rectangle;
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

void main_window()
{
    sf::RenderWindow window(sf::VideoMode({ 1920, 1080 }), "My window");

    GameFilesService gamefileService(constants::GAME_PATH);
    TexturePack::Page page = gamefileService.getPageByName("Tiles1x1");

    sf::Vector2u textureSize = getPNGSize(page.png);
    sf::Texture texture = loadTexture(page);
    sf::Sprite sprite(texture);
    const float size = 1.5f;
    sprite.setScale({size, size});

    TexturePack::Texture *hoveredTexture = nullptr;

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
            else if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>())
            {
                if (keyPressed->scancode == sf::Keyboard::Scancode::Up)
                    window.close();
            }
            else if (const auto *mouseReleased = event->getIf<sf::Event::MouseButtonReleased>())
            {
                if (hoveredTexture != nullptr)
                {
                    fmt::println("{}", hoveredTexture->name);
                }
            }
            else if (const auto *resized = event->getIf<sf::Event::Resized>())
            {
                sf::FloatRect visibleArea({ 0.f, 0.f }, { static_cast<float>(resized->size.x), static_cast<float>(resized->size.y) });
                window.setView(sf::View(visibleArea));
            }
        }

        window.clear();
        window.draw(sprite);

        sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        sf::Transform inverse = sprite.getInverseTransform();
        sf::Vector2f local = inverse.transformPoint(mouse);

        hoveredTexture = getTextureByPosition(page, local);

        if (hoveredTexture != nullptr)
        {
            sf::RectangleShape spriteOutiline = getOutlineRectangle(hoveredTexture, sprite.getTransform());
            window.draw(spriteOutiline);
        }

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