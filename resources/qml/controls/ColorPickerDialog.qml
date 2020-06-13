import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Window 2.14
import QtGraphicalEffects 1.14
import "." as Sky

Dialog {
    id: root
    width: 500
    height: 400
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

    contentItem: ColumnLayout {
        id: column
        spacing: 5
        Sky.ColorFlow {
            Layout.fillHeight: true
            Layout.fillWidth: true
            // Prevents the handle from being clipped by the title rectangle
            Layout.topMargin: 6
            hue: hueSlider.hue
            alpha: alphaSlider.value
        }

        Sky.HueSlider {
            id: hueSlider
            Layout.fillWidth: true
        }
        Sky.ColorSlider {
            id: alphaSlider
            Layout.fillWidth: true
            fromColor: 'transparent'
            toColor: hueSlider.displayColor
            value: 1.0
        }
    }
}

