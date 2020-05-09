pragma Singleton

import QtQuick 2.0
import "./settings"
import "."

QtObject {
    objectName: "properties"
    property Theme activeTheme
    property list themes

    Component.onCompleted: {
        themes = [DefaultTheme]

        const matchingTheme = themes.find(x => x.name === ApplicationSettings.theme);

        activeTheme = matchingTheme != null ? matchingTheme : DefaultTheme;
    }
}
