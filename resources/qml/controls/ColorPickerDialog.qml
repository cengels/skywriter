import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Window 2.14
import QtGraphicalEffects 1.14
import "." as Sky

Dialog {
    id: root
    padding: 20
    margins: 10
    title: "Pick a color"
    standardButtons: Dialog.Ok | Dialog.Cancel

    //! Will be shown to the user underneath the main color picker.
    property var suggestedColors: []
    property color initialColor: '#FF0000'
    readonly property alias color: colorFlow.color

    onVisibleChanged: {
        if (visible) {
            colorFlow.setColor(initialColor);
        }
    }

    contentItem: GridLayout {
        id: column
        rowSpacing: 20
        columnSpacing: 5
        columns: 3

        Sky.ColorFlow {
            id: colorFlow
            height: 240
            width: 400
            Layout.rowSpan: 5
            onHueChanged: {
                if (hueSlider.hue !== hue) {
                    hueSlider.hue = hue;
                }
            }
            onAlphaChanged: {
                if (alphaSlider.value !== alpha) {
                    alphaSlider.value = alpha;
                }
            }
        }

        Sky.Text {
            text: "RGB"
            horizontalAlignment: Qt.AlignCenter
            Layout.fillWidth: true
            Layout.column: 1
            Layout.columnSpan: 2
            Layout.leftMargin: 20
            Layout.row: 0
        }

        Sky.Text {
            text: "R"
            Layout.leftMargin: 20
            Layout.column: 1
            Layout.row: 1
        }

        Sky.NumberField {
            implicitWidth: 60
            Layout.column: 2
            Layout.row: 1
            min: 0
            max: 255
            text: Math.round(root.color.r * 255)
            onEditAccepted: colorFlow.setColor(Qt.rgba(value / 255, root.color.g, root.color.b, root.color.a));
        }

        Sky.Text {
            text: "G"
            Layout.leftMargin: 20
            Layout.column: 1
            Layout.row: 2
        }

        Sky.NumberField {
            implicitWidth: 60
            Layout.column: 2
            Layout.row: 2
            min: 0
            max: 255
            text: Math.round(root.color.g * 255)
            onEditAccepted: colorFlow.setColor(Qt.rgba(root.color.r, value / 255, root.color.b, root.color.a));
        }

        Sky.Text {
            text: "B"
            Layout.leftMargin: 20
            Layout.column: 1
            Layout.row: 3
        }

        Sky.NumberField {
            implicitWidth: 60
            Layout.column: 2
            Layout.row: 3
            min: 0
            max: 255
            text: Math.round(root.color.b * 255)
            onEditAccepted: colorFlow.setColor(Qt.rgba(root.color.r, root.color.g, value / 255, root.color.a));
        }

        Sky.Text {
            text: "A"
            Layout.leftMargin: 20
            Layout.column: 1
            Layout.row: 4
        }

        Sky.NumberField {
            implicitWidth: 60
            Layout.column: 2
            Layout.row: 4
            min: 0
            max: 255
            text: Math.round(root.color.a * 255)
            onEditAccepted: if (alphaSlider.value * 255 !== value) alphaSlider.value = value / 255
        }

        Sky.HueSlider {
            id: hueSlider
            Layout.fillWidth: true
            Layout.columnSpan: 3
            hue: initialColor.hslHue
            onHueChanged: {
                if (colorFlow.hue !== hue) {
                    colorFlow.hue = hue;
                }
            }
        }
        Sky.ColorSlider {
            id: alphaSlider
            Layout.fillWidth: true
            Layout.columnSpan: 3
            fromColor: 'transparent'
            toColor: hueSlider.displayColor
            value: initialColor.a
            onValueChanged: {
                if (colorFlow.alpha !== value) {
                    colorFlow.alpha = value;
                }
            }
        }

        Row {
            Layout.columnSpan: 3
            Layout.alignment: Qt.AlignCenter
            spacing: 40

            Sky.TextField {
                implicitWidth: 250
                horizontalAlignment: Qt.AlignHCenter
                text: root.color.toString()
                onEditingFinished: {
                    if (isValidHex(text)) {
                        colorFlow.setColor(colorFromHex(text));
                    }
                }
            }

            ListView {
                id: otherColors
                implicitWidth: contentWidth
                height: 28
                spacing: 5
                orientation: Qt.Horizontal
                model: root.suggestedColors

                delegate: Sky.ColorButton {
                    height: 28
                    width: 28
                    color: modelData
                    onClicked: colorFlow.setColor(modelData);
                }
            }
        }
    }
}

