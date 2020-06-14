import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Window 2.14
import "../controls" as Sky
import Skywriter.Theming 1.0

Sky.Dialog {
    id: root
    title: qsTr("Edit theme") + " " + theme.name
    modality: Qt.ApplicationModal
    width: Math.min(1000, Screen.width)
    height: 600
    minimumWidth: Math.min(1000, Screen.width)
    minimumHeight: 270
    standardButtons: Dialog.Ok | Dialog.Cancel
    property Theme theme: Theme {}
    property var suggestedColors: [theme.fontColor, theme.documentBackground, theme.windowBackground, theme.uiBackground, theme.uiColor]

    onAccepted: ThemeManager.saveChanges(theme)

    RowLayout {
        anchors.fill: parent

        ScrollView {
            width: Math.max(Math.min(parent.width * 0.4, 300), 400)
            Layout.fillHeight: true
            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

            Column {
                anchors.fill: parent

                Sky.Section {
                    title: qsTr("General")

                    GridLayout {
                        anchors.fill: parent
                        columns: 3

                        Sky.Field {
                            Layout.columnSpan: 3
                            title: qsTr("Name")
                            text: theme.name
                            onTextChanged: theme.name = text
                        }

                        Sky.ComboBox {
                            title: qsTr("Font")
                            items: Qt.fontFamilies()
                            editable: true
                            currentValue: theme.fontFamily
                            onCurrentValueChanged: theme.fontFamily = currentValue
                        }

                        Sky.ComboBox {
                            Layout.fillWidth: false
                            width: 75
                            items: [6, 7, 8, 9, 10, 11, 12, 14, 16, 18, 20, 22, 24, 28, 32, 36, 42, 48, 72]
                            editable: true
                            currentValue: theme.fontSize
                            onAccepted: theme.fontSize = text
                            validator: IntValidator {
                                bottom: 6
                                top: 200
                            }
                        }

                        Sky.ColorPicker {
                            color: theme.fontColor
                            suggestedColors: root.suggestedColors
                            onUserColorChanged: theme.fontColor = userColor
                        }
                    }
                }
            }
        }

        Sky.FullThemePreview {
            Layout.fillWidth: true
            Layout.fillHeight: true
            theme: root.theme
        }
    }
}
