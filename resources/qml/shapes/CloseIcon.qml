import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Shapes 1.14
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
