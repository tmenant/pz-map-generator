#pragma once

#include "TGUI/Backend/SFML-Graphics.hpp"
#include "TGUI/Widgets/Label.hpp"
#include "TGUI/Widgets/Picture.hpp"

class LoadingSpinner
{
private:
    tgui::Label::Ptr loadingLabel = nullptr;
    tgui::Picture::Ptr loadingSpinner = nullptr;

public:
    LoadingSpinner(tgui::Gui &gui);

    void update(std::string message);
    void setVisible(bool visible);
    bool isVisible();
};