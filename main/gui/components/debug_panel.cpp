#include "debug_panel.h"
#include "TGUI/Widgets/Label.hpp"
#include "theme.h"

DebugPanel::DebugPanel(tgui::Gui &gui)
{
    panel = tgui::Panel::create();
    panel->getRenderer()->setBackgroundColor(Colors::backgroundColor.tgui());
    panel->getRenderer()->setPadding(tgui::Padding(5));
    panel->setSize({ 200, 50 });

    fpsLabel = tgui::Label::create();
    fpsLabel->getRenderer()->setTextColor(Colors::fontColor.tgui());

    drawCallsLabel = tgui::Label::create();
    drawCallsLabel->getRenderer()->setTextColor(Colors::fontColor.tgui());
    drawCallsLabel->setPosition({ 0, 20 });

    panel->add(fpsLabel);
    panel->add(drawCallsLabel);

    gui.add(panel);
}

void DebugPanel::setFPS(float value)
{
    fpsLabel->setText(fmt::format("{:.0f} fps", value));
}

void DebugPanel::setDrawCalls(int value)
{
    drawCallsLabel->setText(fmt::format("{} draw calls", value));
}