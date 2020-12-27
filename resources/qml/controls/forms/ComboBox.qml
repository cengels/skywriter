import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Window 2.14
import "qrc:/qml/controls/forms" as Sky
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
    property color popupFontColor: palette.brightText
    property real popupWidth: 0
    property alias validator: delegate.validator
    property alias textRole: delegate.textRole
    property alias valueRole: delegate.valueRole
    property alias highlightedIndex: delegate.highlightedIndex
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

        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            acceptedButtons: Qt.NoButton
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

        popup: Popup {
            id: popup
            y: delegate.height + 1
            x: Math.min(0, delegate.width - width)
            width: root.popupWidth === 0 ? Math.max(delegate.width, contentItem.contentWidth) : root.popupWidth
            implicitHeight: contentItem.implicitHeight
            padding: 1

            contentItem: root.popupContent

            background: Rectangle {
                color: palette.alternateBase
                radius: 16
            }
        }

        delegate: ItemDelegate {
            id: itemDelegate
            width: popup.width - popup.padding * 2
            highlighted: delegate.currentIndex === index

            contentItem: Sky.Text {
                color: root.popupFontColor
                text: delegate.textRole != null && delegate.textRole != "" ? modelData[delegate.textRole] : modelData
            }

            background: Rectangle {
                color: itemDelegate.hovered ? palette.button : itemDelegate.highlighted ? Qt.darker(palette.mid, 1.2) : "transparent"
                radius: 16
            }
        }
    }

    property ListView popupContent: ListView {
        clip: true
        width: popup.width
        implicitHeight: contentHeight
        model: popup.visible ? delegate.delegateModel : null
        currentIndex: root.currentIndex

        ScrollIndicator.vertical: ScrollIndicator { }
    }

    readonly property Sky.SectionLabel textDelegate: Sky.SectionLabel {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: delegate.height / 2
        text: title
    }

    children: [textDelegate, delegate]
}
