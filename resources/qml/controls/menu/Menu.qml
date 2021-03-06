import QtQuick 2.12
import QtQuick.Controls 2.14
import Skywriter.Theming 1.0
import "qrc:/js/color.js" as Color

Menu {
    id: menu
    font.pointSize: 10
    property real menuHeight
    // Necessary to ensure menu.height is not bound to menuHeight
    Component.onCompleted: {
        menuHeight = menu.height;
        background.color = Qt.binding(() => Color.isDark(ThemeManager.activeTheme.uiBackground)
                                      ? Qt.lighter(ThemeManager.activeTheme.uiBackground, 1.3)
                                      : Qt.lighter(ThemeManager.activeTheme.uiBackground, 1.05));
        background.border.width = 0;
    }

    delegate: MenuItem { }

    enter: Transition {
        NumberAnimation {
            from: 0
            to: menuHeight
            easing.type: Easing.OutCubic
            property: "height"
            duration: 300
        }
        NumberAnimation {
            from: 0.0
            to: 1.0
            property: "opacity"
            duration: 0
        }
    }

    exit: Transition {
        NumberAnimation {
            from: menuHeight
            to: 0
            property: "height"
            duration: 200
        }
        NumberAnimation {
            from: 1.0
            to: 0.0
            property: "opacity"
            duration: 100
        }
    }
}
