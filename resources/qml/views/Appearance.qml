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
        cellHeight: 140
        focus: true
        highlightMoveDuration: 0

        highlight: Item {
            Rectangle {
                color: palette.highlight
                radius: 5

                width: grid.cellWidth
                height: grid.cellHeight
            }
        }

        delegate: Item {
            height: grid.cellHeight
            width: grid.cellWidth

            Column {
                id: column
                anchors.fill: parent
                Rectangle { anchors.horizontalCenter: parent.horizontalCenter }
                Sky.Text {
                    text: model.modelData.name
                    width: grid.cellWidth
                    horizontalAlignment: Text.AlignHCenter
                }
            }

            MouseArea {
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
