#include <crtdbg.h>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Vector2.hpp>
#include <iostream>
#include <stdexcept>
#include <string>

#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>

#include "cell_viewer/cell_viewer.h"
#include "constants.h"
#include "files/texturepack.h"
#include "platform.h"
#include "services/map_files_service.h"
#include "services/tilesheet_service.h"
#include "theme.h"
#include "types.h"
#include <fmt/format.h>
#include <lodepng.h>
#include <cpptrace/from_current.hpp>

#include <psapi.h>
#include <windows.h>

size_t getMemoryUsage()
{
    PROCESS_MEMORY_COUNTERS pmc;

    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc)))
    {
        return pmc.WorkingSetSize;
    }

    return 0;
}

sf::Texture LoadTexture(const BytesBuffer &png)
{
    sf::Texture texture;

    if (!texture.loadFromMemory(png.data(), png.size()))
    {
        throw std::runtime_error("failed loading texture");
    }

    return texture;
}

void draw_sprites(sf::RenderWindow &window, sf::Texture &texture, TexturePack::Texture *textureData)
{
    sf::Vector2i posInSheet = { textureData->x, textureData->y };
    sf::Vector2i texSize = { textureData->width, textureData->height };

    float tileW = textureData->ow * 0.5;
    float tileH = textureData->oh * 0.5 * 0.25;

    sf::View view;
    view.setSize({ window.getSize().x * 1.f, window.getSize().y * 1.f });
    view.setCenter({ 0.f, 0.f });
    window.setView(view);

    for (int gx = 0; gx < 8; gx++)
    {
        for (int gy = 0; gy < 8; gy++)
        {
            sf::Sprite sprite(texture);

            float screenX = (gx - gy) * tileW;
            float screenY = (gx + gy) * tileH;

            sprite.setTextureRect(sf::IntRect(posInSheet, texSize));
            sprite.setOrigin({ texSize.x / 2.f, texSize.y / 2.f });
            sprite.setPosition({ screenX, screenY });

            window.draw(sprite);
        }
    }
}

void test_texturePack()
{
    TexturePack pack = TexturePack::read("C:/SteamLibrary/steamapps/common/ProjectZomboidB42/media//texturepacks/Tiles2x.pack");

    for (auto &page : pack.pages)
    {
        for (auto &texture : page.textures)
        {
            fmt::println("{}, {}", page.name, texture.name);
        }
    }
}

void main_window()
{
    sf::RenderWindow window(sf::VideoMode({ 1920, 1080 }), "PZ Map Generator");
    sf::View view = window.getDefaultView();

    platform::windows::setWindowDarkMode(window);
    window.setFramerateLimit(60);

    tgui::Gui gui{ window };
    TilesheetService tilesheetService(constants::GAME_PATH);
    MapFilesService mapFileService(constants::GAME_PATH, MapNames::Muldraugh);

    // TilesBrowser tilesBrowser(gui, window, tilesheetService);
    CellViewer cellViewer(&view, &mapFileService, &tilesheetService, 32, 45);

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            gui.handleEvent(*event);
            cellViewer.handleEvents(*event, window);

            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
            else if (const auto *resized = event->getIf<sf::Event::Resized>())
            {
                // sf::FloatRect visibleArea({ 0.f, 0.f }, { (float)resized->size.x, (float)resized->size.y });
                view.setSize({ (float)resized->size.x, (float)resized->size.y });
                window.setView(view);
            }
        }

        // viewport drawings
        window.clear(Colors::backgroundColor.sfml());

        // tilesBrowser.update(window);
        cellViewer.update(window);

        gui.draw();
        window.display();
    }
}

void test_lotpack()
{
    std::string map = constants::GAME_PATH + "/media/maps/" + MapNames::Muldraugh;
    int x = 32;
    int y = 45;

    LotHeader header = LotHeader::read(map + fmt::format("/{}_{}.lotheader", x, y));
    Lotpack pack = Lotpack::read(map + fmt::format("/world_{}_{}.lotpack", x, y), &header);

    std::unordered_set<std::string> missingSprite{};

    TilesheetService tilesheetService(constants::GAME_PATH);

    for (auto &square : pack.squareMap)
    {
        // if (square.coord.chunk_idx() != 0)
        //     continue;

        for (auto &spriteId : square.tiles)
        {
            std::string spriteName = header.tileNames[spriteId];
            auto sprite = tilesheetService.getTextureByName(spriteName);

            if (sprite == nullptr && !missingSprite.contains(spriteName))
            {
                missingSprite.insert(spriteName);
                fmt::println("[{}, {}, {}]: {}", square.coord.x(), square.coord.y(), square.coord.z(), spriteName);
            }
        }
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