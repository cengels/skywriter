import QtQuick
import QtQuick.Controls
import QtQuick.Shapes
import "qrc:/qml/shapes" as Sky

Sky.Shape {
    id: root
    implicitWidth: 32
    implicitHeight: 32

    Sky.ShapeCross {
        strokeColor: root.stroke
        strokeWidth: root.strokeWidth
        width: root.width
        height: root.height
    }
}
