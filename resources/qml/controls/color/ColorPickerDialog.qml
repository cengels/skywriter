import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Window 2.14
import QtGraphicalEffects 1.14
import "qrc:/qml/controls/color" as Sky
import "qrc:/qml/controls/dialog" as Sky
import "qrc:/qml/controls/forms" as Sky
import "qrc:/qml/controls/text" as Sky

Sky.PopupDialog {
    id: root
    title: qsTr("Pick a color")
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
        columnSpacing: 20
        columns: 2

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

        GridLayout {
            id: rgba
            Layout.column: 1
            // Layout.alignment doesn't work here for some reason
            Layout.topMargin: (colorFlow.height - height) / 2
            rowSpacing: 10
            columnSpacing: 15
            columns: 2

            Sky.Text { text: qsTr("R") }

            Sky.NumberField {
                implicitWidth: 60
                min: 0
                max: 255
                text: Math.round(root.color.r * 255)
                onEditAccepted: colorFlow.setColor(Qt.rgba(value / 255, root.color.g, root.color.b, root.color.a));
            }

            Sky.Text { text: qsTr("G") }

            Sky.NumberField {
                implicitWidth: 60
                min: 0
                max: 255
                text: Math.round(root.color.g * 255)
                onEditAccepted: colorFlow.setColor(Qt.rgba(root.color.r, value / 255, root.color.b, root.color.a));
            }

            Sky.Text { text: qsTr("B") }

            Sky.NumberField {
                implicitWidth: 60
                min: 0
                max: 255
                text: Math.round(root.color.b * 255)
                onEditAccepted: colorFlow.setColor(Qt.rgba(root.color.r, root.color.g, value / 255, root.color.a));
            }

            Sky.Text {  text: qsTr("A") }

            Sky.NumberField {
                implicitWidth: 60
                min: 0
                max: 255
                text: Math.round(root.color.a * 255)
                onEditAccepted: if (alphaSlider.value * 255 !== value) alphaSlider.value = value / 255
            }
        }

        Sky.HueSlider {
            id: hueSlider
            Layout.fillWidth: true
            Layout.columnSpan: 2
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
            Layout.columnSpan: 2
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
            Layout.columnSpan: 2
            Layout.alignment: Qt.AlignCenter
            spacing: 40

            Sky.TextField {
                implicitWidth: 250
                horizontalAlignment: Qt.AlignHCenter
                text: otherColors.hoveredColor != null ? otherColors.hoveredColor.toString() : root.color.toString()
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
                property var hoveredColor

                delegate: Sky.ColorButton {
                    height: 28
                    width: 28
                    color: modelData
                    onHoveredChanged: {
                        if (hovered) {
                            otherColors.hoveredColor = modelData;
                        } else {
                            otherColors.hoveredColor = undefined;
                        }
                    }
                    onClicked: colorFlow.setColor(modelData);
                }
            }
        }
    }
}

