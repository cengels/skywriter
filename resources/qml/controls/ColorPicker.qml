import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Dialogs 1.3
import QtQuick.Window 2.14
import QtGraphicalEffects 1.14
import "." as Sky

Column {
    id: root
    width: 25
    height: 25
    Layout.alignment: Qt.AlignBottom
    Layout.bottomMargin: 3
    Layout.topMargin: 3
    smooth: true
    property string title: ""
    property color color: "#000000"

    readonly property Button delegate: Button {
        id: delegate
        height: root.height
        width: root.width
        hoverEnabled: true

        MouseArea {
            id: mouseArea
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            onPressed: mouse.accepted = false
        }

        contentItem: Rectangle {
            id: circle
            width: delegate.width * 0.75
            height: delegate.height * 0.75
            radius: width * 0.5
            color: root.color
            border.color: color === palette.dark ? Qt.hsla(color.hslHue, color.hslSaturation, color.hslLightness  - 0.1, color.a) : "#00000000"
            border.width: color === palette.dark ? 1 : 0
        }

        background: Rectangle {
            width: delegate.width
            height: delegate.height
            radius: width * 0.5
            color: "black"
            opacity: 0.35
            visible: delegate.hovered
        }

        onPressed: dialog.open()
    }

    readonly property Text textDelegate: Text { text: title }

    readonly property Sky.ColorPickerDialog dialog: Sky.ColorPickerDialog {
        initialColor: color
    }

    children: [textDelegate, delegate, dialog]
}

