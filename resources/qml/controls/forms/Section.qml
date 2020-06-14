import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Window 2.14
import "qrc:/qml/controls/text" as Sky

Item {
    id: root
    anchors.left: parent.left
    anchors.right: parent.right
    property string title: ""
    default property list<Item> content

    ColumnLayout {
        anchors.fill: parent

        Sky.Text {
            text: title
            Layout.fillWidth: true
            font.pointSize: 13
        }

        Item {
            width: parent.width
            Layout.fillHeight: true
            Layout.fillWidth: true
            children: content
        }
    }

}
