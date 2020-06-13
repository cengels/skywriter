import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Window 2.14
import QtGraphicalEffects 1.14
import "." as Sky

Dialog {
    id: root
    width: 400
    height: 300
    title: "Pick a color"
    standardButtons: Dialog.Ok | Dialog.Cancel

    //! Will be shown to the user underneath the main color picker.
    property var suggestedColors: []
    property color initialColor: '#FF0000'
    property color color: '#FF0000'

    onVisibleChanged: {
        if (visible) {
            color = initialColor;
        }
    }

    contentItem: Column {
        id: column
        spacing: 5
        Sky.HueSlider {
            id: hueSlider
            anchors.left: parent.left; anchors.right: parent.right
            onColorChanged: console.log(color)
        }
        Sky.ColorSlider {
            anchors.left: parent.left; anchors.right: parent.right
            fromColor: 'transparent'
            toColor: hueSlider.displayColor
        }
    }
}

