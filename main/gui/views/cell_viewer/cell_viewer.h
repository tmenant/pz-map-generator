#pragma once

#include <SFML/Graphics/Texture.hpp>
#include <cstdint>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <vector>

#include "TGUI/Backend/SFML-Graphics.hpp"
#include "algorithms/rect_pack/rect_structs.h"
#include "files/lotheader.h"
#include "files/lotpack.h"
#include "files/texturepack.h"
#include "math/math.h"
#include "services/map_files_service.h"
#include "services/tilesheet_service.h"

#include "gui/components/debug_panel.h"

class CellViewer
{
private:
    struct ViewState
    {
        sf::Vector2i lastMousePos;
        sf::Vector2f center = { .0f, .0f };
        float zoomLevel = 1.0f;
        bool isDragging = false;
        bool firstFrame = true;
        int currentLayer = 2;

        void applyTo(sf::View &view, sf::RenderWindow &window)
        {
            zoomLevel = Math::fastClamp(zoomLevel, 0.05f, 10.f);

            view.setSize({ window.getSize().x * zoomLevel, window.getSize().y * zoomLevel });
            view.setCenter(center);
        }
    };

    struct RenderTile
    {
        int8_t layer;
        sf::Sprite sprite;
    };

    LotHeader lotheader;
    Lotpack lotpack;

    TilesheetService *tilesheetService;

    ViewState viewState;
    sf::View *view;
    std::vector<RenderTile> renderTiles;

    sf::Texture atlasTexture;
    std::vector<TexturePack::Texture *> spriteDatas;
    std::vector<rectpack2D::rect_xywh> rectangles;

    float framerate = 0;

    DebugPanel debugPanel;

public:
    CellViewer(sf::View *view, MapFilesService *mapFileService, TilesheetService *tilesheetService, tgui::Gui &gui, int x, int y);

    void handleEvents(const sf::Event &event, sf::RenderWindow &window);
    void update(sf::RenderWindow &window);

    void preComputeSprites();
    void packCellSprites();

    sf::Image loadTexture(TexturePack::Page *page);
};