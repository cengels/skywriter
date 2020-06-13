import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Dialogs 1.3
import QtQuick.Window 2.14
import QtGraphicalEffects 1.14
import "." as Sky

Column {
    id: root
    width: 27
    height: 27
    Layout.alignment: Qt.AlignBottom
    Layout.bottomMargin: 3
    Layout.topMargin: 3
    smooth: true
    property string title: ""
    property color color: "#000000"
    // Fun fact: If this is an alias instead of a new property, the program
    // crashes on startup.
    property var suggestedColors: []

    readonly property Sky.ColorButton delegate: Sky.ColorButton {
        height: root.height
        width: root.width
        color: root.color
        onPressed: dialog.open()
    }

    readonly property Text textDelegate: Text { text: title }

    readonly property Sky.ColorPickerDialog dialog: Sky.ColorPickerDialog {
        initialColor: root.color
        suggestedColors: root.suggestedColors
    }

    children: [textDelegate, delegate, dialog]
}

