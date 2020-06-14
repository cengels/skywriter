import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Window 2.14
import "qrc:/qml/controls/text" as Sky

Column {
    id: root
    Layout.fillWidth: true
    Layout.minimumWidth: Layout.fillWidth ? delegate.implicitWidth : 0
    Layout.alignment: Qt.AlignBottom
    property string title: ""
    property alias items: delegate.model
    property alias editable: delegate.editable
    property alias currentIndex: delegate.currentIndex
    property alias itemDelegate: delegate.delegate
    property alias validator: delegate.validator
    readonly property string text: delegate.acceptableInput ? delegate.editText : delegate.currentText

    signal accepted()

    property var currentValue: null
    onCurrentValueChanged: {
        if (delegate.currentValue !== currentValue) {
            currentIndex = delegate.indexOfValue(currentValue);

            if (currentIndex === -1) {
                delegate.editText = currentValue;
            }
        }
    }

    readonly property ComboBox delegate: ComboBox {
        id: delegate
        editable: false
        height: 30
        width: root.width
        // enable in 2.15
        // selectByMouse: true
        onCurrentValueChanged: {
            if (root.currentValue !== delegate.currentValue) {
                root.currentValue = delegate.currentValue
                root.accepted();
            }
        }

        onAccepted: {
            if (find(editText) === -1) {
                root.accepted();
            }
        }
    }

    readonly property Sky.Text textDelegate: Sky.Text {
        anchors.left: parent.left
        anchors.right: parent.right
        text: title
    }

    children: [textDelegate, delegate]
}
