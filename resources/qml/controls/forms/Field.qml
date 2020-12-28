import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
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

    Component.onCompleted: {
        field.anchors.left = root.left;
        field.anchors.right = root.right;
    }
}
