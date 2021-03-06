#include <QPalette>
#include <QColor>

#include "colors.h"
#include "numbers.h"

QPalette skywriter_palette;
bool palette_initialized = false;

const QPalette& colors::palette()
{
    if (!palette_initialized) {
        // This is necessary because custom style palettes cannot be defined
        // in qtquickcontrols2.conf.

        skywriter_palette.setColor(QPalette::ColorRole::Base, QColor { "#32285E" });
        skywriter_palette.setColor(QPalette::ColorRole::AlternateBase, QColor { "#2b2252" });
        skywriter_palette.setColor(QPalette::ColorRole::BrightText, QColor { "#d9d9d9" });
        skywriter_palette.setColor(QPalette::ColorRole::Button, QColor { "#4D3DAD" }); // accent color
        skywriter_palette.setColor(QPalette::ColorRole::ButtonText, QColor { "#A8A8A9" });
        skywriter_palette.setColor(QPalette::ColorRole::Dark, QColor { "#473C77" });
        skywriter_palette.setColor(QPalette::ColorRole::Highlight, QColor { "#5f4bbd" });
        skywriter_palette.setColor(QPalette::ColorRole::HighlightedText, QColor { "#FCFCFC" });
        skywriter_palette.setColor(QPalette::ColorRole::Light, QColor { "#ACACAC" });
        skywriter_palette.setColor(QPalette::ColorRole::Link, QColor { "#A8A8A9" });
        skywriter_palette.setColor(QPalette::ColorRole::LinkVisited, QColor { "#A8A8A9" });
        skywriter_palette.setColor(QPalette::ColorRole::Mid, QColor { "#3a336b" });
        skywriter_palette.setColor(QPalette::ColorRole::Midlight, QColor { "#584C8D" });
        skywriter_palette.setColor(QPalette::ColorRole::Shadow, QColor { "#1a1a1a" });
        skywriter_palette.setColor(QPalette::ColorRole::Text, QColor { "#b6b5bd" });
        skywriter_palette.setColor(QPalette::ColorRole::ToolTipBase, QColor { "#403668" });
        skywriter_palette.setColor(QPalette::ColorRole::ToolTipText, QColor { "#CACACA" });
        skywriter_palette.setColor(QPalette::ColorRole::Window, QColor { "#372D65" });
        skywriter_palette.setColor(QPalette::ColorRole::WindowText, QColor { "#A8A8A9" });
        palette_initialized = true;
    }

    return skywriter_palette;
}

QColor colors::lighten(const QColor& color, int absoluteValue)
{
    QColor newColor = QColor(color);
    newColor.setHsl(color.hue(), color.saturation(), numbers::clamp(color.lightness() + absoluteValue, 0, 255));

    return newColor;
}

QColor colors::darken(const QColor& color, int absoluteValue)
{
    return colors::lighten(color, -absoluteValue);
}

QColor colors::saturate(const QColor& color, int absoluteValue)
{
    QColor newColor = QColor(color);
    newColor.setHsl(color.hue(), numbers::clamp(color.saturation() + absoluteValue, 0, 255), color.lightness());

    return newColor;
}

QColor colors::desaturate(const QColor& color, int absoluteValue)
{
    return colors::saturate(color, -absoluteValue);
}

QColor colors::decreaseEmphasis(const QColor& color, double strength)
{
    const int lightnessDifference = 40 * strength;
    const int saturationDifference = 32 * strength;
    QColor newColor = QColor(color);
    newColor.setHsl(color.hue(),
                    numbers::clamp(color.saturation() - saturationDifference, 0, 255),
                    numbers::clamp(color.lightness() <= 128 ? color.lightness() + lightnessDifference * 2 : color.lightness() - lightnessDifference, 0, 255));

    return newColor;
}
