import QtQuick 2.12
import QtQuick.Controls 2.14
import Skywriter.Theming 1.0
import "qrc:/js/color.js" as Color

MenuItem {
    id: menuItem

    contentItem: Text {
        text: menuItem.text
        font: menuItem.font
        opacity: enabled ? 1.0 : 0.3
        color: menuItem.highlighted ? Color.highlight(ThemeManager.activeTheme.uiColor, 1.4) : Color.highlight(ThemeManager.activeTheme.uiColor, 1.1)
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    background: Rectangle {
        opacity: enabled ? 1 : 0.3
        color: menuItem.highlighted ? Color.highlight(ThemeManager.activeTheme.uiBackground, 0.5) : "transparent"
    }
}
