import QtQuick
import QtQuick.Controls
import QtQuick.Shapes
import Skywriter.Theming
import "qrc:/qml/controls/text" as Sky
import "qrc:/qml/effects" as Sky
import "qrc:/qml/shapes" as Sky

AbstractButton {
    id: root
    property color fill: "transparent"
    property color stroke: useTheme ? ThemeManager.activeTheme.uiColor : "#7c87ff"
    property bool circle: true
    implicitHeight: height
    implicitWidth: width
    hoverEnabled: true
    padding: Math.round(height * 0.34375)
    property Sky.Shape shape: Sky.Shape {}
    property bool useTheme: false
    opacity: enabled ? 1.0 : 0.3

    scale: down ? 0.85 : 1.0

    Behavior on scale {
        ScaleAnimator { easing.type: Easing.InOutSine; duration: 100 }
    }

    Behavior on opacity { OpacityAnimator { duration: 200 } }

    background: Rectangle {
        radius: height / 2
        color: "transparent"
        border.width: root.shape.strokeWidth;
        border.color: root.stroke
        visible: root.circle

        Rectangle {
            anchors.fill: parent
            radius: height / 2
            color: root.fill.a > 0 ? root.fill : root.stroke
            scale: root.hovered ? 1.0 : 0.0

            Behavior on scale {
                ScaleAnimator { easing.type: Easing.InOutSine; duration: 200 }
            }
        }

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.NoButton
            cursorShape: hovered ? Qt.PointingHandCursor : Qt.ArrowCursor
        }
    }

    onShapeChanged: {
        root.shape.fill = Qt.binding(() => root.fill);
        root.shape.stroke = Qt.binding(() => root.hovered ? palette.button : root.stroke);
        root.shape.height = Qt.binding(() => root.height - root.padding * 2);
        root.shape.width = Qt.binding(() => root.width - root.padding * 2);

        root.children = [root.shape];
    }
}
