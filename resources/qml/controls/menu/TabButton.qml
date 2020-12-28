import QtQuick
import QtQuick.Controls
import "qrc:/qml/controls/text" as Sky

TabButton {
    id: root
    font.pointSize: 11.5
    font.capitalization: Font.AllUppercase
    font.letterSpacing: 1

    contentItem: Sky.Text {
        text: root.text
        font: root.font
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
        color: root.hovered ? palette.text : palette.brightText

        Behavior on color {
            ColorAnimation {
                duration: 50
            }
        }
    }

    background: Item {}

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        acceptedButtons: Qt.NoButton
        cursorShape: root.hovered ? Qt.PointingHandCursor : Qt.ArrowCursor
    }
}
