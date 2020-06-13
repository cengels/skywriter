import QtQuick 2.14
import QtQuick.Controls 2.14
import QtGraphicalEffects 1.14
import "." as Sky

Item {
    id: root
    implicitHeight: 300
    implicitWidth: 400
    property real hue: 0.0
    property real alpha: 1.0
    property color color: Qt.hsva(hue, point.x, 1.0 - point.y, alpha)
    property point point: Qt.point(1.0, 0.0)

    Rectangle {
        id: horizontalGradient
        anchors.fill: root
        gradient: Gradient {
            orientation: Gradient.Horizontal
            GradientStop { position: 0.0; color: Qt.rgba(1, 1, 1, root.alpha) }
            GradientStop { position: 1.0; color: Qt.hsla(root.hue, 1.0, 0.5, root.alpha) }
        }
    }

    Rectangle {
        id: verticalGradient
        anchors.fill: root
        z: 1
        gradient: Gradient {
            orientation: Gradient.Vertical
            GradientStop { position: 0.0; color: "transparent" }
            GradientStop { position: 1.0; color: Qt.rgba(0, 0, 0, root.alpha) }
        }
    }

    Image {
        anchors.fill: root
        z: -1
        fillMode: Image.Tile
        source: "qrc:///images/checkered.svg"
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onPositionChanged: root.point = Qt.point(clamp(mouseX / width, 0.0, 1.0), clamp(mouseY / height, 0.0, 1.0))
        onPressed: root.point = Qt.point(clamp(mouseX / width, 0.0, 1.0), clamp(mouseY / height, 0.0, 1.0))
    }

    Rectangle {
        id: handle
        x: root.point.x * root.width - width / 2
        y: root.point.y * root.height - height / 2
        z: 99
        height: 24
        width: 24
        radius: height / 2
        color: root.color
        border.width: 2
        border.color: palette.highlightedText
    }

    DropShadow {
        anchors.fill: handle
        horizontalOffset: 1
        verticalOffset: 1
        z: 99
        radius: 4.0
        samples: 8
        color: "#80000000"
        source: handle
    }
}

