import QtQuick
import QtQuick.Controls
import "qrc:/qml/controls/text" as Sky
import "qrc:/qml/effects" as Sky

Button {
    id: root
    implicitWidth: 100
    property int prominence: 0
    property bool outline: prominence >= 2
    property bool useTheme: false

    enum ButtonProminence {
        Normal,
        Primary,
        Secondary,
        Destructive
    }

    scale: down ? 0.85 : 1.0

    Behavior on scale {
        ScaleAnimator { easing.type: Easing.InOutSine; duration: 100 }
    }

    function ifEnabled(color, elseColor = "#3b3463") {
        return root.enabled ? color : elseColor;
    }

    contentItem: Text {
        text: root.text
        font: root.font
        opacity: enabled ? 1.0 : 0.7
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
        color: prominence === 0 ? palette.text : palette.buttonText;
    }

    background: Rectangle {
        id: background

        readonly property color normalColor: switch (prominence) {
                                                case 0: return ifEnabled(palette.base);
                                                case 3: return ifEnabled("#ef5350");
                                                default: return ifEnabled(palette.button);
                                            }
        readonly property color hoverColor: switch (prominence) {
                                                case 0: return palette.alternateBase;
                                                case 1: default: return palette.highlight;
                                            }
        anchors.fill: parent
        border.width: root.outline ? 2 : 0
        border.color: hovered && !root.outline ? hoverColor : normalColor
        color: root.outline ? "transparent" : normalColor

        // layer.enabled: true
        // layer.effect: Sky.CornerRadius { radius: 10 }
        radius: 10

        Rectangle {
            // This rectangle is used to animate the button fill.
            anchors.centerIn: parent
            height: Math.max(parent.height + parent.height / 2, parent.width + parent.width / 2)
            width: height
            radius: height / 2
            color: root.outline ? background.normalColor : background.hoverColor
            scale: hovered ? 1.0 : 0.0

            Behavior on scale {
                ScaleAnimator { easing.type: Easing.InOutSine; duration: 300 }
            }
        }

        Behavior on color {
            ColorAnimation { duration: 200 }
        }

        Behavior on border.color {
            ColorAnimation { duration: 200 }
        }
    }

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.NoButton
        cursorShape: hovered ? Qt.PointingHandCursor : Qt.ArrowCursor
    }
}
