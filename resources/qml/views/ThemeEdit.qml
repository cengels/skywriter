import QtQuick 2.14
import QtQuick.Controls 2.14
import "../controls" as Sky
import Skywriter.Theming 1.0

Sky.Dialog {
    id: root
    title: qsTr("Edit theme") + " " + theme.name
    modality: Qt.ApplicationModal
    width: 800
    height: 600
    minimumWidth: 450
    minimumHeight: 270
    standardButtons: Dialog.Ok | Dialog.Cancel
    readonly property Theme emptyTheme: Theme {}
    property Theme theme: emptyTheme

    onRejected: theme = emptyTheme

    Sky.FullThemePreview {
        anchors.fill: parent
        theme: root.theme
    }
}
