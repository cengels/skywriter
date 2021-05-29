import QtQuick
import QtQuick.Controls

TabBar {
    id: root
    height: 32

    background: Rectangle {
        x: root.currentItem.x
        width: root.currentItem.width
        height: 2
        anchors.bottom: parent.bottom
        color: palette.highlight

        Behavior on color { ColorAnimation { duration: 200 } }
        Behavior on x {
            PropertyAnimation {
                duration: 200
                easing.type: Easing.InOutSine
            }
        }
        Behavior on width {
            PropertyAnimation {
                duration: 200
                easing.type: Easing.InOutSine
            }
        }
    }
}
