import QtQuick 2.14
import QtQuick.Shapes 1.14

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
