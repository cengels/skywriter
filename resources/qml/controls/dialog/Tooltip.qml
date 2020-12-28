import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import Skywriter.Theming
import "qrc:/qml/controls/dialog" as Sky
import "qrc:/qml/controls/forms" as Sky

ToolTip {
    id: root
    delay: 1000
    timeout: 10000
    verticalPadding: 6
    horizontalPadding: 20
    property bool useTheme: true

    contentItem: Text {
        text: root.text
        font: root.font
        color: useTheme ? ThemeManager.activeTheme.uiColor : palette.text
    }

    background: Rectangle {
        color: Qt.lighter(useTheme ? ThemeManager.activeTheme.uiBackground : palette.base, 1.2)
        border.color: useTheme ? Qt.darker(ThemeManager.activeTheme.uiBackground, 1.1) : palette.alternateBase
        radius: 20
    }
}
