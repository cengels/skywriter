import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.14
import QtQuick.Window 2.14
import "qrc:/qml/controls" as Sky
import "qrc:/qml/controls/dialog" as Sky
import "qrc:/qml/controls/forms" as Sky
import "qrc:/qml/controls/color" as Sky
import "qrc:/qml/shapes" as Shapes
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
    canAccept: form.valid
    property Theme theme: Theme {}
    property var suggestedColors: [theme.fontColor, theme.documentBackground, theme.windowBackground, theme.uiBackground, theme.uiColor]

    onAccepted: ThemeManager.saveChanges(theme)

    RowLayout {
        anchors.fill: parent

        ScrollView {
            width: Math.max(Math.min(parent.width * 0.4, 300), 400)
            Layout.fillHeight: true
            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

            Sky.Form {
                id: form

                Sky.Section {
                    title: qsTr("General")
                    anchors.left: parent.left
                    anchors.right: parent.right
                    columns: 3

                    Sky.Field {
                        Layout.columnSpan: 3
                        title: qsTr("Name")

                        field: Sky.TextField {
                            text: theme.name
                            onTextChanged: theme.name = text
                            valid: text !== ''
                        }
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

                Sky.Section {
                    title: qsTr("Document")
                    anchors.left: parent.left
                    anchors.right: parent.right
                    columns: 3

                    readonly property FileDialog imageDialog: FileDialog {
                        id: imageDialog
                        title: qsTr("Select image...")
                        nameFilters: ["Image files (*.jpg *.png *.bmp *.gif *.jpeg)"]
                        selectedNameFilter: "Image files (*.jpg *.png *.bmp *.gif *.jpeg)"
                        selectExisting: true
                        folder: shortcuts.pictures
                        onAccepted: {
                            theme.backgroundImage = imageDialog.fileUrl
                        }
                    }

                    Sky.SectionLabel {
                        Layout.column: 0
                        Layout.row: 0
                        text: qsTr("Window background")
                    }

                    Sky.ColorPicker {
                        Layout.column: 1
                        Layout.columnSpan: 2
                        Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                        color: theme.windowBackground
                        suggestedColors: root.suggestedColors
                        onUserColorChanged: theme.windowBackground = userColor
                    }

                    Sky.SectionLabel {
                        Layout.column: 0
                        Layout.row: 1
                        text: qsTr("Background image")
                    }

                    Sky.Button {
                        id: imageButton
                        Layout.column: 1
                        Layout.row: 1
                        implicitWidth: Math.min(implicitContentWidth + 20, 200)
                        text: theme.backgroundImage != null && theme.backgroundImage !== '' ? fileName(theme.backgroundImage) : qsTr("No image")
                        onClicked: imageDialog.open()
                    }

                    Sky.IconButton {
                        Layout.column: 2
                        Layout.row: 1
                        Layout.leftMargin: 5
                        height: imageButton.height * 0.8
                        width: height
                        shape: Shapes.CloseIcon {}
                        onClicked: theme.backgroundImage = ""
                    }

                    Sky.SectionLabel {
                        Layout.column: 0
                        Layout.row: 2
                        text: qsTr("Document background")
                    }

                    Sky.ColorPicker {
                        Layout.column: 1
                        Layout.columnSpan: 2
                        Layout.row: 2
                        Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                        color: theme.documentBackground
                        suggestedColors: root.suggestedColors
                        onUserColorChanged: theme.documentBackground = userColor
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
