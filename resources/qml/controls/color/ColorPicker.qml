import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "qrc:/qml/controls/color" as Sky

Column {
    id: root
    width: 27
    height: 27
    Layout.alignment: Qt.AlignBottom
    Layout.bottomMargin: 3
    Layout.topMargin: 3
    smooth: true
    property string title: ""
    property color color: "black"
    // Must have two properties here or setting color will break the binding.
    // Unfortunately, there is no way to provide a custom setter for a property
    // in QML alone, so a ColorPicker that behaves like a TextField (where
    // the binding is not broken when the user types something) is not possible
    // without turning this into a C++ component.
    property color userColor: "black"
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
        onAccepted: root.userColor = color
    }

    children: [textDelegate, delegate, dialog]
}
