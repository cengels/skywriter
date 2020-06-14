import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import "../controls" as Sky
import "../controls/dialog" as Sky
import "../controls/forms" as Sky
import "../controls/text" as Sky
import "../views" as Sky
import Skywriter.Theming 1.0
import Skywriter.Settings 1.0 as Settings

Sky.Dialog {
    id: root
    title: qsTr("Appearance")
    padding: 5
    width: 800
    height: 600
    minimumWidth: 450
    minimumHeight: 270
    standardButtons: DialogButtonBox.Ok | DialogButtonBox.Cancel

    readonly property Sky.ThemeEdit themeEdit: Sky.ThemeEdit { }
    property int oldThemeIndex;

    Component.onCompleted: oldThemeIndex = ThemeManager.activeThemeIndex

    onAccepted: {
        Settings.Application.theme = ThemeManager.activeTheme.name;
        oldThemeIndex = ThemeManager.activeThemeIndex;
    }

    onRejected: ThemeManager.activeThemeIndex = oldThemeIndex;

    RowLayout {
        anchors.fill: parent

        GridView {
            id: grid
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: ThemeManager.themes
            cellWidth: 140
            cellHeight: 145
            focus: true
            currentIndex: ThemeManager.activeThemeIndex

            delegate: Item {
                id: rootDelegate
                height: grid.cellHeight
                width: grid.cellWidth
                readonly property bool selected: GridView.isCurrentItem

                Rectangle {
                    anchors.fill: parent
                    radius: 5
                    color: rootDelegate.selected
                           ? palette.highlight
                           : mouseArea.containsMouse
                             ? palette.alternateBase
                             : 'transparent'
                }

                ColumnLayout {
                    id: column
                    anchors.fill: parent
                    anchors.topMargin: 10
                    anchors.leftMargin: 10
                    anchors.rightMargin: 10
                    anchors.bottomMargin: 5
                    spacing: 5

                    Sky.ThemePreview {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        theme: model.modelData

                        content: Text {
                            text: 'A'
                            font.family: model.modelData.fontFamily
                            font.pointSize: model.modelData.fontSize * 2
                            color: model.modelData.fontColor
                            anchors.centerIn: parent
                        }
                    }
                    Sky.Text {
                        id: caption
                        text: model.modelData.name
                        Layout.fillWidth: true
                        horizontalAlignment: Text.AlignHCenter
                    }
                }

                MouseArea {
                    id: mouseArea
                    anchors.fill: parent
                    onClicked: {
                        ThemeManager.activeThemeIndex = index;
                    }

                    hoverEnabled: true
                    preventStealing: true
                    z: 99
                }
            }
        }

        Column {
            Layout.fillHeight: true
            padding: 10
            spacing: 10

            Sky.MessageDialog {
                id: deleteConfirmationDialog
                text: qsTr("Are you sure you want to delete this theme? This operation cannot be undone.")
                standardButtons: Dialog.Ok | Dialog.Cancel
                onAccepted: {
                    ThemeManager.remove();
                    // Must reset oldThemeIndex here, or clicking Cancel
                    // may cause an out of bounds exception.
                    root.oldThemeIndex = ThemeManager.activeThemeIndex;
                }
            }

            Sky.Button {
                text: qsTr("New")
                onClicked: ThemeManager.createNew()
            }

            Sky.Button {
                text: qsTr("Duplicate")
                onClicked: ThemeManager.duplicate()
            }

            Sky.Button {
                text: qsTr("Edit")
                enabled: !ThemeManager.activeTheme.isReadOnly
                onClicked: {
                    themeEdit.theme = ThemeManager.editableTheme();
                    themeEdit.show();
                }
            }

            Sky.Button {
                text: qsTr("Remove")
                enabled: !ThemeManager.activeTheme.isReadOnly
                onClicked: deleteConfirmationDialog.show()
            }
        }
    }
}
