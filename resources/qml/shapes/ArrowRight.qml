import QtQuick
import QtQuick.Controls
import QtQuick.Shapes
import "qrc:/qml/shapes" as Sky

Sky.Shape {
    id: root
    implicitWidth: 32
    implicitHeight: 32

    ShapePath {
        strokeWidth: 2
        strokeColor: root.stroke
        fillColor: root.fill
        capStyle: ShapePath.RoundCap
        startX: root.width * 0.7

        PathLine {
            x: root.width
            y: root.height / 2
        }

        PathLine {
            x: root.width * 0.7
            y: root.height
        }

        PathMove {
            x: root.width
            y: root.height / 2
        }

        PathLine {
            x: 0
            relativeY: 0
        }
    }
}
