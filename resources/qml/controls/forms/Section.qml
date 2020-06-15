import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Window 2.14
import "qrc:/qml/controls/text" as Sky

ColumnLayout {
    id: root
    anchors.left: parent.left
    anchors.right: parent.right
    property string title: ""
    default property list<Item> content
    property alias columns: gridLayout.columns
    property alias rows: gridLayout.rows

    Sky.Heading {
        Layout.leftMargin: 6
        text: title
        level: 2
        Layout.fillWidth: true
        font.pointSize: 13
    }

    GridLayout {
        id: gridLayout
        Layout.fillHeight: true
        Layout.fillWidth: true
        Layout.leftMargin: 12
        Layout.rightMargin: 12
        children: content
    }
}
