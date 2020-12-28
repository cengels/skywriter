import QtQuick
import QtQuick.Window
import QtGraphicalEffects

OpacityMask {
    id: root
    property real radius: 0
    cached: true

    maskSource: Item {
        width: root.parent.width
        height: root.parent.height
        Rectangle {
            anchors.fill: parent
            radius: root.radius
        }
    }
}
