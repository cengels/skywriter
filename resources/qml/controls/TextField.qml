import QtQuick 2.14
import QtQuick.Controls 2.14 as QtQuick
import QtQuick.Layouts 1.14
import QtQuick.Window 2.14
import "." as Sky
import "../types" as Types

QtQuick.TextField {
    id: root
    selectByMouse: true

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
