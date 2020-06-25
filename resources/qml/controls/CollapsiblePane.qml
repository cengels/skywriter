import QtQuick 2.14
import QtQuick.Controls 2.14
import Skywriter.Theming 1.0

Pane {
    id: root
    y: collapsed ? (direction === 0 ? height : -height) : (direction === 0 ? 0 : -height)
    opacity: collapsed ? 0.0 : 1.0
    property bool collapsed: true
    property int direction: 0

    enum ExpandDirection {
        Up,
        Down
    }

    Behavior on y {
        animation: NumberAnimation {
            easing {
                type: Easing.InOutSine
                amplitude: 1.0
                period: 0.4
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

    background: Rectangle {
        color: ThemeManager.activeTheme.uiBackground

        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.ArrowCursor
        }
    }
}
