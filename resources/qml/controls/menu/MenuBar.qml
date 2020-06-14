import QtQuick 2.12
import QtQuick.Controls 2.14
import Skywriter.Theming 1.0
import "qrc:/js/color.js" as Color

MenuBar {
    font.pointSize: 10

    background: Rectangle {
        color: ThemeManager.activeTheme.uiBackground

        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.ArrowCursor
        }
    }

    delegate: MenuBarItem {
        id: menuBarItem
        implicitHeight: 25

        background: Rectangle {
            opacity: enabled ? 1 : 0.3
            color: menuBarItem.highlighted ? Color.highlight(ThemeManager.activeTheme.uiBackground, 0.7) : "transparent"
        }

        contentItem: Text {
            text: menuBarItem.text
            font: menuBarItem.font
            opacity: enabled ? 1.0 : 0.3
            color: menuBarItem.highlighted ? Color.highlight(ThemeManager.activeTheme.uiColor, 1.2) : ThemeManager.activeTheme.uiColor
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
        }
    }

    Behavior on y {
        animation: NumberAnimation {
            easing {
                type: Easing.InOutSine
                amplitude: 1.0
                period: 0.5
            }
        }
    }

    Behavior on opacity {
        animation: NumberAnimation {
            easing {
                type: Easing.InOutSine
                amplitude: 1.0
                period: 0.4
            }
        }
    }
}
