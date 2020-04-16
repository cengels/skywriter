import QtQuick 2.12
import QtQuick.Controls 2.14

MenuBar {
    font.pointSize: 10
    background: Rectangle {
        color: palette.base
    }
    delegate: MenuBarItem {
        id: menuBarItem
        implicitHeight: 25
//        visible: true

//        contentItem: Text {
//            text: menuBarItem.text
//            font: menuBarItem.font
//            opacity: enabled ? 1.0 : 0.3
//            color: menuBarItem.highlighted ? "#ffffff" : "#ffffff"
//            horizontalAlignment: Text.AlignLeft
//            verticalAlignment: Text.AlignVCenter
//            elide: Text.ElideRight
//            visible: true
//        }

//        background: Rectangle {
//            anchors.fill: parent
//            implicitWidth: 40
//            implicitHeight: 40
//            opacity: enabled ? 1 : 0.3
//            color: menuBarItem.highlighted ? "#21be2b" : "#000000"
//            visible: true
//        }
    }
}
