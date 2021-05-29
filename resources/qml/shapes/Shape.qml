import QtQuick
import QtQuick.Shapes

Shape {
    property color fill: "black"
    property color stroke: "transparent"
    property real strokeWidth: 2
    antialiasing: true
    smooth: true
    anchors.centerIn: parent

    Behavior on fill {
        ColorAnimation { duration: 200 }
    }

    Behavior on stroke {
        ColorAnimation { duration: 200 }
    }
}
