import QtQuick 2.14
import "qrc:/qml/controls/text" as Sky

Sky.Text {
    linkColor: palette.link
    onLinkActivated: Qt.openUrlExternally(link)
    textFormat: Text.RichText

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.NoButton
        cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
    }
}
