#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>

#include "files/lotheader.h"
#include "files/lotpack.h"
#include "services/map_files_service.h"

class CellViewer
{
public:
    LotHeader lotheader;
    Lotpack lotpack;

    CellViewer(tgui::Gui &gui, sf::RenderWindow &window, MapFilesService &mapFileService, int x, int y)
    {
        lotheader = mapFileService.LoadLotheaderByPosition(x, y);
        lotpack = mapFileService.LoadLotpackByPosition(x, y, &lotheader);
    }

    void update(sf::RenderWindow &window)
    {
    }
};