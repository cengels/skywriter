import QtQuick 2.14
import QtQuick.Controls 2.14
import QtGraphicalEffects 1.14
import "." as Sky

Slider {
    id: root
    implicitHeight: 20
    implicitWidth: 200
    from: 0.0
    to: 1.0
    property alias gradient: rectangle.gradient
    property alias handleColor: handle.color

    background: Rectangle {
        id: rectangle
        anchors.topMargin: 2
        anchors.bottomMargin: 2
        anchors.leftMargin: handle.width / 2 - 1
        anchors.rightMargin: handle.width / 2 - 1
        anchors.fill: root
        radius: root.height / 2

        Image {
            anchors.fill: rectangle
            z: -1
            fillMode: Image.Tile
            source: "qrc:///images/checkered.svg"
            layer.enabled: true
            layer.effect: Sky.CornerRadius { radius: rectangle.radius }
        }
    }

    handle: Rectangle {
        id: handle
        x: root.leftPadding + root.visualPosition * (root.availableWidth - width)
        y: root.topPadding + root.availableHeight / 2 - height / 2
        height: root.height
        width: height
        radius: height / 2
        border.width: 2
        border.color: mouseArea.containsMouse || root.pressed ? palette.brightText : palette.highlightedText

        MouseArea {
            id: mouseArea
            cursorShape: Qt.PointingHandCursor
            anchors.fill: handle
            hoverEnabled: true
            onPressed: mouse.accepted = false
        }
    }

    DropShadow {
        anchors.fill: handle
        horizontalOffset: 1
        verticalOffset: 1
        radius: 4.0
        samples: 8
        color: "#80000000"
        source: handle
    }
}

