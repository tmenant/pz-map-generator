#pragma once

#include <SFML/System/Clock.hpp>
#include <vector>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/VertexArray.hpp>

#include "TGUI/Backend/SFML-Graphics.hpp"

#include "algorithms/rect_pack/rect_structs.h"
#include "files/lotheader.h"
#include "files/lotpack.h"
#include "files/texturepack.h"
#include "math/math.h"
#include "services/tilesheet_service.h"

#include "gui/components/debug_panel.h"

class CellViewer
{
private:
    struct ViewState
    {
        sf::Clock clock;
        sf::Vector2i lastMousePos;
        sf::Vector2f center;

        float zoomLevel = 10.0f;
        bool isDragging = false;
        bool firstFrame = true;
        int currentLayer = 32;

        void applyTo(sf::View &view, sf::RenderWindow &window)
        {
            zoomLevel = Math::fastClamp(zoomLevel, 0.05f, 50.f);

            view.setSize({ window.getSize().x * zoomLevel, window.getSize().y * zoomLevel });
            view.setCenter(center);
        }
    };

    LotHeader lotheader;
    Lotpack lotpack;

    TilesheetService *tilesheetService = nullptr;

    ViewState viewState;
    sf::View *view = nullptr;

    sf::Texture atlasTexture;
    std::vector<sf::VertexArray> vertexArrays;
    std::vector<TexturePack::Texture *> spriteDatas;
    std::vector<rectpack2D::rect_xywh> rectangles;

    float framerate = 0;

    DebugPanel debugPanel;

public:
    CellViewer(sf::View *view, TilesheetService *tilesheetService, tgui::Gui &gui, int x, int y);

    void handleEvents(const sf::Event &event, sf::RenderWindow &window);
    void update(sf::RenderWindow &window);

    void preComputeSprites();
    void packCellSprites();
};