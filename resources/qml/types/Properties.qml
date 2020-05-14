pragma Singleton

import QtQuick 2.0
import "./settings"
import "." as Sky

QtObject {
    objectName: "properties"
    property Sky.Theme activeTheme
    property list themes

    Component.onCompleted: {
        themes = [Sky.DefaultTheme]

        const matchingTheme = themes.find(x => x.name === ApplicationSettings.theme);

        activeTheme = matchingTheme != null ? matchingTheme : Sky.DefaultTheme;
    }
}
