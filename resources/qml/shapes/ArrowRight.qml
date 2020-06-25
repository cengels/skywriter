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
