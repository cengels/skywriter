import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Window 2.14
import QtGraphicalEffects 1.14
import "qrc:/qml/utility" as Utility
import "qrc:/js/color.js" as Color

TextField {
    id: root
    Layout.fillWidth: true
    selectByMouse: true
    leftPadding: 10
    rightPadding: 10
    hoverEnabled: true
    property bool markValid: false
    property bool valid: acceptableInput

    background: Rectangle {
        id: rectangle
        radius: height / 2
        width: root.width
        height: root.height
        color: root.activeFocus ? palette.alternateBase : palette.base
        border.width: 1
        border.color: {
            if (root.valid && root.markValid) {
                return "#4caf50";
            }

            if (!root.valid) {
                return "#ef5350";
            }

            if (root.activeFocus) {
                return Color.adjustAlpha(palette.highlight, 0.65);
            }

            if (root.hovered) {
                return palette.alternateBase;
            }

            return palette.base;
        }

        Behavior on color {
            ColorAnimation { duration: 100 }
        }

        Behavior on border.color {
            ColorAnimation { duration: 150 }
        }
    }

    signal validate(var event)
    signal editAccepted();

    Utility.ValidationEvent {
        id: validationEvent
    }

    QtObject {
        id: props
        property bool isManualEdit: false
    }

    property string previousText: ""
    onTextChanged: {
        validationEvent.text = root.text;
        validationEvent.accepted = true;

        validate(validationEvent);

        if (validationEvent.accepted) {
            previousText = root.text;
            accepted();

            if (props.isManualEdit) {
                editAccepted();
                props.isManualEdit = false;
            }
        } else {
            root.text = previousText;
        }
    }

    onTextEdited: {
        props.isManualEdit = true;
    }
}
