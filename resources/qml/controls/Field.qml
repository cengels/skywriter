import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Window 2.14

Column {
    id: root
    Layout.fillWidth:true
    Layout.alignment: Qt.AlignBottom
    property string title: ""
    property alias text: delegate.text
    property alias placeholderText: delegate.placeholderText

    readonly property Text textDelegate: Text { text: title }

    readonly property Item delegate: TextField {
        id: delegate
        width: root.width
        selectByMouse: true
        placeholderText: placeholderText
    }

    children: [textDelegate, delegate]
}
