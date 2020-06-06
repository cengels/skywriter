import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Window 2.14
import "../controls" as Sky
import "../types" as SkyT
import Skywriter.Theming 1.0
import Skywriter.Settings 1.0 as Settings

Sky.Dialog {
    title: qsTr("Appearance")
    padding: 5
    width: 800
    height: 600
    minimumWidth: 450
    minimumHeight: 270
    standardButtons: DialogButtonBox.Ok | DialogButtonBox.Cancel;

    onVisibleChanged: {
        if (visible) {
            grid.currentIndex = ThemeManager.activeThemeIndex;
        }
    }

    onAccepted: {
        Settings.Application.theme = ThemeManager.activeTheme.name;
    }

    GridView {
        id: grid
        anchors.fill: parent
        model: ThemeManager.themes
        cellWidth: 140
        cellHeight: 145
        focus: true

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

                    Text {
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
                    grid.currentIndex = index;
                    ThemeManager.activeThemeIndex = index;
                }

                hoverEnabled: true
                preventStealing: true
                z: 99
            }
        }
    }
}
