import QtQuick
import QtQuick.Controls
import "qrc:/qml/controls/text" as Sky
import "qrc:/qml/controls/dialog" as Sky
import "qrc:/qml/effects" as Sky
import "qrc:/js/color.js" as Color
import Skywriter.Theming

AbstractButton {
    id: root
    checkable: true
    verticalPadding: 2
    horizontalPadding: 10
    property string tooltip: ""
    property bool useTheme: false
    readonly property color actualColor: {
        if (!root.enabled) {
            return useTheme ? Qt.darker(ThemeManager.activeTheme.uiColor, 1.2) : "#3b3463";
        }

        const enabledColor = useTheme ? ThemeManager.activeTheme.uiColor : "#584C8D";

        return root.hovered ? Qt.lighter(enabledColor, 1.1) : enabledColor;
    }

    scale: down ? 0.9 : 1.0

    Behavior on scale {
        ScaleAnimator { easing.type: Easing.InOutSine; duration: 100 }
    }

    contentItem: Text {
        text: root.text
        font: root.font
        opacity: enabled ? (root.checked ? 1.0 : 0.7) : 0.5
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
        color: root.checked ? root.actualColor : root.actualColor

        Behavior on color { ColorAnimation { duration: 200 } }
        Behavior on opacity { OpacityAnimator { duration: 200 } }
    }

    background: Rectangle {
        id: background
        anchors.fill: parent
        color: "transparent"

        // layer.enabled: true
        // layer.effect: Sky.CornerRadius { radius: 15 }
        border.width: root.checked ? 2 : 0
        border.color: Color.adjustAlpha(root.actualColor, 0.4)
        radius: 15

        Rectangle {
            // This rectangle is used to animate the button fill.
            anchors.centerIn: parent
            height: Math.max(parent.height + parent.height / 2, parent.width + parent.width / 2)
            width: height
            radius: height / 2
            color: root.hovered ? "#21584C8D" : "transparent"
            scale: root.hovered ? 1.0 : 0.0

            Behavior on scale {
                ScaleAnimator { easing.type: Easing.InOutSine; duration: 300 }
            }
        }

        Behavior on border.color { ColorAnimation { duration: 200 } }
        Behavior on border.width { NumberAnimation { duration: 200 } }
    }

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.NoButton
        cursorShape: hovered ? Qt.PointingHandCursor : Qt.ArrowCursor
    }

    Sky.Tooltip {
        parent: root
        visible: root.hovered && root.tooltip !== ""
        text: root.tooltip
    }
}
