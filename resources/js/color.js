function highlight(color, factor) {
    if (color.hslLightness <= 0.5) {
        // dark color

        return Qt.darker(color, factor);
    } else {
        // light color

        return Qt.lighter(color, factor);
    }
}

function middle(color1, color2) {
    return Qt.rgba(
        (color1.r + color2.r) / 2,
        (color1.g + color2.g) / 2,
        (color1.b + color2.b) / 2,
        (color1.a + color2.a) / 2
    );
}

function difference(color1, color2) {
    return Qt.rgba(
        color1.r - color2.r,
        color1.g - color2.g,
        color1.b - color2.b,
        color1.a - color2.a
    );
}


function relativeLuminance(color) {
    const r = color.r <= 0.03928 ? color.r / 12.92 : Math.pow((color.r + 0.055) / 1.055, 2.4);
    const g = color.g <= 0.03928 ? color.g / 12.92 : Math.pow((color.g + 0.055) / 1.055, 2.4);
    const b = color.b <= 0.03928 ? color.b / 12.92 : Math.pow((color.b + 0.055) / 1.055, 2.4);

    return r * 0.2126 + g * 0.7152 + b * 0.0722;
}

function contrast(source1, source2) {
    const luminance1 = relativeLuminance(source1);
    const luminance2 = relativeLuminance(source2);

    return (Math.max(luminance1, luminance2) + 0.05) / (Math.min(luminance1, luminance2) + 0.05);
}

/**
  * Returns a new color where sourceColor has its lightness adjusted if and only
  * if sourceColor and comparisonColor have an insufficient contrast with each
  * other.
  */
function adjustContrast(sourceColor, comparisonColor) {
    const theContrast = contrast(sourceColor, comparisonColor);
    if (theContrast < 1.2) {
        return sourceColor.hslLightness > comparisonColor.hslLightness
            ? Qt.lighter(sourceColor, 1 + 0.5 / theContrast)
            : Qt.darker(sourceColor, 1 + 0.5 / theContrast);
    }

    return sourceColor;
}

function isDark(color) {
    return color.hslLightness < 0.5;
}

function adjustAlpha(color, alpha) {
    return Qt.rgba(color.r, color.g, color.b, alpha);
}
