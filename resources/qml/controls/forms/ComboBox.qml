import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Window 2.14
import "qrc:/qml/controls/text" as Sky
import "qrc:/js/color.js" as Color

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
        flat: true
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

        indicator: Item {
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: 24
            anchors.right: delegate.right

            Image {
                cache: true
                anchors.centerIn: parent
                height: 12
                width: 12
                source: "qrc:/images/arrow-down.svg"
            }

            MouseArea {
                id: indicatorMouseArea
                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor
                onPressed: mouse.accepted = false;
            }
        }

        contentItem: TextInput {
            leftPadding: rectangle.radius
            rightPadding: delegate.indicator.width + delegate.spacing
            enabled: delegate.editable

            text: delegate.displayText
            color: palette.text
            font: delegate.font
            verticalAlignment: Text.AlignVCenter
        }


        background: Rectangle {
            id: rectangle
            radius: height / 2
            color: delegate.activeFocus ? palette.alternateBase : palette.base
            border.width: 1
            border.color: delegate.activeFocus ? Color.adjustAlpha(palette.highlight, 0.65) : (delegate.hovered ? palette.alternateBase : palette.base)

            Behavior on color {
                ColorAnimation { duration: 100 }
            }

            Behavior on border.color {
                ColorAnimation { duration: 100 }
            }
        }
    }

    readonly property Sky.Text textDelegate: Sky.Text {
        color: Qt.darker(palette.text, 1.25)
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: delegate.height / 2
        text: title
        font.pointSize: 10.5
        font.letterSpacing: 2
    }

    children: [textDelegate, delegate]
}
