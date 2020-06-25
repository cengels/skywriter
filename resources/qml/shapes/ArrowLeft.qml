import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Shapes 1.14
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
        startX: root.width * 0.3

        PathLine {
            x: 0
            y: root.height / 2
        }

        PathLine {
            x: root.width * 0.3
            y: root.height
        }

        PathMove {
            x: 0
            y: root.height / 2
        }

        PathLine {
            x: root.width
            relativeY: 0
        }
    }
}
