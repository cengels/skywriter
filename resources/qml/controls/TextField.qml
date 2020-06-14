import QtQuick 2.14
import QtQuick.Controls 2.14 as QtQuick
import QtQuick.Layouts 1.14
import QtQuick.Window 2.14
import QtGraphicalEffects 1.14
import "." as Sky
import "../types" as Types

QtQuick.TextField {
    id: root
    selectByMouse: true
    leftPadding: 10
    rightPadding: 10
    hoverEnabled: true

    background: Rectangle {
        id: rectangle
        radius: height / 2
        width: root.width
        height: root.height
        color: root.activeFocus ? palette.highlight : palette.base

        Behavior on color {
            ColorAnimation { duration: 100 }
        }
    }

    DropShadow {
        anchors.fill: rectangle
        cached: true
        opacity: root.hovered ? 1.0 : 0.0
        radius: 4.0
        samples: 8
        z: -1
        color: "#70000000"
        source: rectangle
        Behavior on opacity {
            OpacityAnimator { duration: 100 }
        }
    }

    signal validate(var event)
    signal editAccepted();

    Types.ValidationEvent {
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
