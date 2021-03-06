import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Shapes 1.14
import "qrc:/qml/shapes" as Sky

ShapePath {
    id: root
    strokeWidth: 32
    property real width: parent.implicitWidth
    property real height: parent.implicitHeight
    strokeColor: "black"
    fillColor: "transparent"
    capStyle: ShapePath.RoundCap

    PathLine {
        relativeX: root.width
        relativeY: root.height
    }

    PathMove {
        relativeX: 0
        relativeY: -root.height
    }

    PathLine {
        relativeX: -root.width
        relativeY: root.height
    }
}
