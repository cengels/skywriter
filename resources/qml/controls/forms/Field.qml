import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Window 2.14
import "qrc:/qml/controls/text" as Sky
import "qrc:/qml/controls/forms" as Sky

Column {
    id: root
    Layout.fillWidth: true
    Layout.alignment: Qt.AlignBottom
    property string title: ""

    readonly property Sky.SectionLabel textDelegate: Sky.SectionLabel {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: field.height / 2
        text: title
    }

    property Item field: Item {}

    children: [textDelegate, field]
}
