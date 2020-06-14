import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Window 2.14
import "qrc:/qml/controls/text" as Sky
import "qrc:/qml/controls/forms" as Sky

Column {
    id: root
    Layout.fillWidth:true
    Layout.alignment: Qt.AlignBottom
    property string title: ""
    property alias text: delegate.text
    property alias placeholderText: delegate.placeholderText
    property alias validator: delegate.validator

    readonly property Sky.Text textDelegate: Sky.Text {
        color: Qt.darker(palette.text, 1.25)
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: delegate.height / 2
        text: title
        font.pointSize: 10.5
        font.letterSpacing: 2
    }

    readonly property Item delegate: Sky.TextField {
        id: delegate
        width: root.width
        selectByMouse: true
        placeholderText: placeholderText
    }

    children: [textDelegate, delegate]
}
