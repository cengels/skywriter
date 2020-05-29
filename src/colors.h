#include <QPalette>

#ifndef COLORS_H
#define COLORS_H

namespace colors {
    //! Returns the palette used for Skywriter.
    const QPalette& palette();
    //! Returns a new QColor that is lightened by the specified absolute value.
    QColor lighten(const QColor& color, int absoluteValue);
    //! Returns a new QColor that is darkened by the specified absolute value.
    QColor darken(const QColor& color, int absoluteValue);
    //! Returns a new QColor with the saturation increased by the specified value.
    QColor saturate(const QColor& color, int absoluteValue);
    //! Returns a new QColor with the saturation decreased by the specified value.
    QColor desaturate(const QColor& color, int absoluteValue);

    //! Returns a new QColor that is "deemphasized." In other words, this
    //! function will attempt to decrease or increase the brightness slightly,
    //! depending on whether the color is dark or light, then decrease its
    //! saturation.
    QColor decreaseEmphasis(const QColor& color, double strength = 1.0);
}

#endif // COLORS_H
