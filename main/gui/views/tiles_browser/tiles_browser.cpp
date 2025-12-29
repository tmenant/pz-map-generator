#include "tiles_browser.h"
#include "sprite_info_panel.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>

TilesBrowser::TilesBrowser(tgui::Gui &gui, sf::RenderWindow &window, TilesheetService &tilesheetService) :
        spriteInfoPanel(gui),
        spriteExplorerPanel(gui, tilesheetService),
        texture(sf::Texture()),
        sprite(texture)
{
    spriteExplorerPanel.onPageSelect([&](const tgui::String &pageName)
    {
        currentPage = tilesheetService.getPageByName(pageName.toStdString());

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
}

void TilesBrowser::update(sf::RenderWindow &window)
{
    if (currentPage != nullptr)
    {
        window.draw(sprite);
        drawSpriteOutline(window, sprite, currentPage, hoveredTexture);
    }

    sprite.setPosition({ window.getSize().x / 2.f, window.getSize().y / 2.f });

    spriteInfoPanel.update(window, currentPage, hoveredTexture);
    spriteExplorerPanel.update(window);
}

void TilesBrowser::updateSpriteTexture(sf::Sprite &sprite, sf::Texture &texture, TexturePack::Page *page, const sf::RenderWindow &window)
{
    if (!texture.loadFromImage(page->image))
    {
        throw std::runtime_error("Erreur : Échec du chargement de la texture depuis le buffer PNG.");
    }

    sprite.setTexture(texture, true);
    sf::FloatRect bounds = sprite.getLocalBounds();
    sprite.setOrigin({ bounds.size.x / 2.f, bounds.size.y / 2.f });
}

sf::Vector2u TilesBrowser::getPNGSize(const BytesBuffer &data)
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

TexturePack::Texture *TilesBrowser::getTextureByName(TexturePack::Page *page, std::string textureName)
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

TexturePack::Texture *TilesBrowser::getTextureByPosition(TexturePack::Page *page, sf::Vector2f pos)
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

sf::RectangleShape TilesBrowser::getOutlineRectangle(TexturePack::Texture *textureData, sf::Transform transform)
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

void TilesBrowser::drawSpriteOutline(sf::RenderWindow &window, sf::Sprite &sprite, TexturePack::Page *page, TexturePack::Texture *&hoveredTexture)
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
