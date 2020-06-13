import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Window 2.14
import "." as Sky

Column {
    id: root
    Layout.fillWidth:true
    Layout.alignment: Qt.AlignBottom
    property string title: ""
    property alias text: delegate.text
    property alias placeholderText: delegate.placeholderText
    property alias validator: delegate.validator

    readonly property Sky.Text textDelegate: Sky.Text {
        anchors.left: parent.left
        anchors.right: parent.right
        text: title
    }

    readonly property Item delegate: TextField {
        id: delegate
        width: root.width
        selectByMouse: true
        placeholderText: placeholderText
    }

    children: [textDelegate, delegate]
}
