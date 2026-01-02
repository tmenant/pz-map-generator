#include "loading_spinner.h"
#include "theme.h"

LoadingSpinner::LoadingSpinner(tgui::Gui &gui)
{
    loadingSpinner = tgui::Picture::create("ignore/spinner.png");
    loadingSpinner->setPosition("45%", "50%");
    loadingSpinner->setOrigin(0.5f, 0.5f);
    loadingSpinner->setSize(20, 20);

    loadingLabel = tgui::Label::create("Loading...");
    loadingLabel->setPosition("50%", "50%");
    loadingLabel->setOrigin(0.5f, 0.5f);
    loadingLabel->getRenderer()->setTextColor(Colors::fontColor.tgui());

    gui.add(loadingSpinner);
    gui.add(loadingLabel);
}

void LoadingSpinner::update(std::string message)
{
    loadingLabel->setText(message);
    loadingSpinner->setRotation(loadingSpinner->getRotation() + 5);
}

void LoadingSpinner::setVisible(bool visible)
{
    loadingLabel->setVisible(visible);
    loadingSpinner->setVisible(visible);
}

bool LoadingSpinner::isVisible()
{
    return loadingLabel->isVisible() || loadingSpinner->isVisible();
}