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

#include "files/lotheader.h"
#include "files/lotpack.h"
#include "files/texturepack.h"
#include "files/tiledefinition.h"
#include "io/binary_reader.h"
#include "io/file_reader.h"
#include "math/md5.h"
#include "services/game_files_service.h"

const std::string GAME_PATH = "C:/SteamLibrary/steamapps/common/ProjectZomboidB42";
const std::string LOTHEADER_PATH = "data/B42/27_38.lotheader";
const std::string LOTHPACK_PATH = "data/B42/world_27_38.lotpack";
const std::string TILESDEF_PATH = "data/B42/newtiledefinitions.tiles";
const std::string TEXTUREPACK_PATH = "/media/texturepacks/Tiles2x.pack";

void readGameFiles()
{
    BytesBuffer headerBuffer = FileReader::read(LOTHEADER_PATH);
    BytesBuffer lotpackBuffer = FileReader::read(LOTHPACK_PATH);
    BytesBuffer packBuffer = FileReader::read(TEXTUREPACK_PATH);
    BytesBuffer tileDefBuffer = FileReader::read(TILESDEF_PATH);

    std::string headerHash = MD5::toHash(headerBuffer);
    std::string lotpackHash = MD5::toHash(lotpackBuffer);

    LotHeader header = LotHeader::read(headerBuffer);
    Lotpack lotpack = Lotpack::read(lotpackBuffer, header);
    TexturePack texturePack = TexturePack::read(packBuffer);
    TileDefinition tileDefinition = TileDefinition::read(tileDefBuffer);

    fmt::println("magic: {}, version: {}, md5: {}", header.magic, header.version, headerHash);
    fmt::println("magic: {}, version: {}, md5: {}", lotpack.magic, lotpack.version, lotpackHash);
    fmt::println("magic: {}, version: {}", texturePack.magic, texturePack.version);

    fmt::println("squareMap size: {}", lotpack.squareMap.size());
    fmt::println("texture pages: {}", texturePack.pages.size());
    fmt::println("tilesheets: {}", tileDefinition.tileSheets.size());
}

BytesBuffer create_png(size_t width, size_t height)
{
    BytesBuffer image(width * height * 4); // RGBA

    for (uint8_t y = 0; y < height; ++y)
    {
        for (uint8_t x = 0; x < width; ++x)
        {
            image[4 * (y * width + x) + 0] = x;   // R
            image[4 * (y * width + x) + 1] = y;   // G
            image[4 * (y * width + x) + 2] = 128; // B
            image[4 * (y * width + x) + 3] = 255; // A
        }
    }

    BytesBuffer result;
    unsigned error = lodepng::encode(result, image.data(), width, height);

    if (error)
    {
        std::cout << "Erreur PNG: " << lodepng_error_text(error) << std::endl;
    }

    return result;
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

    return { width, height };
}

void main_window()
{
    GameFilesService gamefileService(GAME_PATH);

    TexturePack::Page page = gamefileService.getPageByName("Tiles1x1");
    int textureIndex = -1;

    for (int i = 0; i < page.textures.size(); i++)
    {
        if (page.textures[i].name == "blends_natural_01_1")
        {
            textureIndex = i;
            break;
        }
    }

    if(textureIndex == -1)
    {
        throw std::runtime_error("texture not found");
    }

    sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "My window", sf::Style::Close | sf::Style::Titlebar);

    sf::Vector2u size = getPNGSize(page.png);
    sf::Texture texture;

    fmt::println("png size: {}, {}", size.x, size.y);

    if (!texture.loadFromMemory(page.png.data(), page.png.size()))
    {
        std::cerr << "Erreur : Échec du chargement de la texture depuis le buffer PNG." << std::endl;
        return;
    }

    sf::Sprite sprite(texture);

    std::cout << "Texture chargée avec succès." << std::endl;

    // run the program as long as the window is open
    while (window.isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (event->is<sf::Event::MouseButtonReleased>())
                fmt::println("Mouse pressed!");
        }

        window.clear();
        window.draw(sprite);
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