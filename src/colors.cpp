#include <QPalette>
#include <QColor>
#include "colors.h"

QPalette skywriter_palette;
bool palette_initialized = false;

const QPalette& Skywriter::palette() {
    if (!palette_initialized) {
        // This is necessary because custom style palettes cannot be defined
        // in qtquickcontrols2.conf.

        skywriter_palette.setColor(QPalette::ColorRole::Base, QColor { "#403668" });
        skywriter_palette.setColor(QPalette::ColorRole::AlternateBase, QColor { "#443a6e" });
        skywriter_palette.setColor(QPalette::ColorRole::BrightText, QColor { "#CACACA" });
        skywriter_palette.setColor(QPalette::ColorRole::Button, QColor { "#403668" });
        skywriter_palette.setColor(QPalette::ColorRole::ButtonText, QColor { "#A8A8A9" });
        skywriter_palette.setColor(QPalette::ColorRole::Dark, QColor { "#473C77" });
        skywriter_palette.setColor(QPalette::ColorRole::Highlight, QColor { "#3B3261" });
        skywriter_palette.setColor(QPalette::ColorRole::HighlightedText, QColor { "#FCFCFC" });
        skywriter_palette.setColor(QPalette::ColorRole::Light, QColor { "#ACACAC" });
        skywriter_palette.setColor(QPalette::ColorRole::Link, QColor { "#A8A8A9" });
        skywriter_palette.setColor(QPalette::ColorRole::LinkVisited, QColor { "#A8A8A9" });
        skywriter_palette.setColor(QPalette::ColorRole::Mid, QColor { "#473C77" });
        skywriter_palette.setColor(QPalette::ColorRole::Shadow, QColor { "#1a1a1a" });
        skywriter_palette.setColor(QPalette::ColorRole::Text, QColor { "#1a1a1a" });
        skywriter_palette.setColor(QPalette::ColorRole::ToolTipBase, QColor { "#403668" });
        skywriter_palette.setColor(QPalette::ColorRole::ToolTipText, QColor { "#CACACA" });
        skywriter_palette.setColor(QPalette::ColorRole::Window, QColor { "#584C8D" });
        skywriter_palette.setColor(QPalette::ColorRole::WindowText, QColor { "#A8A8A9" });
        palette_initialized = true;
    }

    return skywriter_palette;
}
