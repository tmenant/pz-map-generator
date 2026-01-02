#include "window_tiles_browser.h"

WindowTilesBrowser::WindowTilesBrowser(AppContext &_appContext) : AppWindow(_appContext, windowConfig()), sprite(texture)
{
    loadingSpinner = std::make_unique<LoadingSpinner>(gui);
}

void WindowTilesBrowser::ready()
{
    if (initialized)
    {
        throw std::runtime_error("window is already initialized");
    }

    spriteInfoPanel = std::make_unique<SpriteInfoPanel>(gui);
    spriteExplorerPanel = std::make_unique<SpriteExplorerPanel>(gui, *appContext.tilesheetService);

    spriteInfoPanel->onTextureSelect([this](const tgui::String &item)
    {
        hoveredTexture = appContext.tilesheetService->getTextureByName(item.toStdString(), currentPage);
    });

    spriteExplorerPanel->onPageSelect([this](const tgui::String &pageName)
    {
        currentPage = appContext.tilesheetService->getPageByName(pageName.toStdString());

        if (currentPage != nullptr)
        {
            updateSpriteTexture();
            spriteInfoPanel->setPage(currentPage);
            hoveredTexture = nullptr;
        }
    });

    loadingSpinner->setVisible(false);

    initialized = true;
}

void WindowTilesBrowser::update()
{
    AppWindow::update();

    if (appContext.isLoaded && !initialized)
    {
        ready();
    }
    else if (appContext.isLoaded)
    {
        updateWindow();
    }
    else
    {
        loadingSpinner->update(appContext.loadingPayload.getMessage());
    }
}

void WindowTilesBrowser::updateWindow()
{
    if (currentPage != nullptr)
    {
        window.draw(sprite);
        drawSpriteOutline(window, sprite, currentPage, hoveredTexture);
    }

    sprite.setPosition({ window.getSize().x / 2.f, window.getSize().y / 2.f });

    spriteInfoPanel->update(window, currentPage, hoveredTexture);
    spriteExplorerPanel->update(window);
}

void WindowTilesBrowser::updateSpriteTexture()
{
    if (!texture.loadFromImage(currentPage->image))
    {
        throw std::runtime_error("Failed loading image from page: " + currentPage->name);
    }

    sprite.setTexture(texture, true);
    sprite.setOrigin(sprite.getLocalBounds().size / 2.f);
}

void WindowTilesBrowser::drawSpriteOutline(sf::RenderWindow &window, sf::Sprite &sprite, TexturePack::Page *page, TexturePack::Texture *&hoveredTexture)
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

sf::RectangleShape WindowTilesBrowser::getOutlineRectangle(TexturePack::Texture *textureData, sf::Transform transform)
{
    sf::FloatRect rect(
        { (float)textureData->x, (float)textureData->y },
        { (float)textureData->width, (float)textureData->height });

    rect = transform.transformRect(rect);

    sf::RectangleShape rectangle(rect.size);
    rectangle.setPosition(rect.position);
    rectangle.setOutlineColor(sf::Color::Red);
    rectangle.setOutlineThickness(1.0f);
    rectangle.setFillColor(sf::Color::Transparent);

    return rectangle;
}

TexturePack::Texture *WindowTilesBrowser::getTextureByPosition(TexturePack::Page *page, sf::Vector2f pos)
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