import QtQuick 2.14
import QtQuick.Window 2.14
import QtGraphicalEffects 1.14

import '.' as Sky

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
