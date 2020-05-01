import QtQuick 2.14
import QtQuick.Controls 2.14

Button {
    background: Rectangle {
        implicitWidth: 100
        color: palette.button
        radius: 5
    }

    onHoveredChanged: background.color = hovered ? palette.highlight : palette.button

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.NoButton
        cursorShape: hovered ? Qt.PointingHandCursor : Qt.ArrowCursor
    }
}
