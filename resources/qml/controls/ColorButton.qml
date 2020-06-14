import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Dialogs 1.3
import QtQuick.Window 2.14
import QtGraphicalEffects 1.14
import "." as Sky

Button {
    id: root
    property color color: "white"
    hoverEnabled: true

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
        onPressed: mouse.accepted = false
    }

    contentItem: Rectangle {
        id: circle
        width: root.width * 0.75
        height: root.height * 0.75
        radius: width * 0.5
        color: root.color
        border.color: palette.highlight
        border.width: 2
    }

    background: DropShadow {
        anchors.fill: circle
        source: circle
        radius: 8
        samples: 16.0
        spread: 0.4
        color: "#77000000"
        opacity: root.hovered ? 1.0 : 0.0

        Behavior on opacity {
            OpacityAnimator { duration: 50 }
        }
    }
}

