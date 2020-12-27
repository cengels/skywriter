import QtQuick 2.14
import QtQuick.Controls 2.14
import QtGraphicalEffects 1.14

Switch {
    id: root
    property int animationDuration: 200

    indicator: Rectangle {
        implicitWidth: 52
        implicitHeight: 26
        x: root.leftPadding
        y: parent.height / 2 - height / 2
        radius: 13
        color: root.checked ? palette.highlight : palette.base
        border.color: root.checked ? palette.highlight : palette.alternateBase

        Behavior on color {
            PropertyAnimation {
                duration: root.animationDuration
                easing.type: Easing.InOutSine
            }
        }

        Behavior on border.color {
            PropertyAnimation {
                duration: root.animationDuration
                easing.type: Easing.InOutSine
            }
        }

        Rectangle {
            id: circle
            x: root.checked ? parent.width - width - 4 : 4
            y: root.down ? 5 : 4
            width: height
            height: parent.height - 8
            radius: height / 2
            color: root.checked ? palette.buttonText : palette.text
            border.width: 0

            Behavior on x {
                PropertyAnimation {
                    duration: root.animationDuration
                    easing.type: Easing.InOutSine
                }
            }

            Behavior on y {
                PropertyAnimation {
                    duration: 50
                    easing.type: Easing.InOutSine
                }
            }

            Behavior on color {
                PropertyAnimation {
                    duration: root.animationDuration
                    easing.type: Easing.InOutSine
                }
            }
        }

        DropShadow {
            anchors.fill: circle
            horizontalOffset: 2
            verticalOffset: 2
            radius: 4.0
            samples: 17
            color: "#30000000"
            source: circle
        }
    }

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.NoButton
        cursorShape: hovered ? Qt.PointingHandCursor : Qt.ArrowCursor
    }
}
