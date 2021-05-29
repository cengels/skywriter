import QtQuick
import QtQuick.Controls
import QtQuick.Shapes
import "qrc:/qml/shapes" as Sky

ShapePath {
    id: root
    strokeWidth: 32
    property real radius: 32

    PathMove {
        relativeX: root.radius
    }

    PathArc {
        relativeX: 0
        relativeY: root.radius * 2
        radiusX: root.radius
        radiusY: root.radius
    }
    PathArc {
        relativeX: 0
        relativeY: -root.radius * 2
        radiusX: root.radius
        radiusY: root.radius
    }
}
