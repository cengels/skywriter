import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import "qrc:/qml/controls/forms" as Sky

Sky.TextField {
    id: root
    inputMethodHints: Qt.ImhFormattedNumbersOnly
    // Can't use a direct binding here because bindings are only evaluated
    // after all other signals have finished processing, which would be too late
    property real value: min !== Number.NaN ? min : 0
    property bool clamp: false
    property real min: Number.NaN
    property real max: Number.NaN
    property int decimals: 0

    onValidate: {
        if (event.text === '') {
            return;
        }

        // "value" isn't updated yet at this point
        const num = parseFloat(event.text);

        if (!isNumeric(event.text)) {
            event.accepted = false;
        } else if (event.text.indexOf('.') !== event.text.lastIndexOf('.')) {
            // multiple periods, invalid
            event.accepted = false;
        } else if (decimals === 0 && event.text.includes('.')) {
            event.accepted = false;
        } else if (decimals > 0 && event.text.slice(event.text.indexOf('.')).length > decimals) {
            // too many decimal places
            event.accepted = false;
        } else if (num === Number.NaN || (min !== Number.NaN && num < min) || (max !== Number.NaN && num > max)) {
            event.accepted = false;
        }

        if (!event.accepted && clamp) {
            if (num > max) {
                previousText = max.toString();
            } else {
                previousText = min.toString();
            }
        }
    }

    onAccepted: value = text === '' ? (min !== Number.NaN ? min : 0) : parseFloat(text);

    onEditingFinished: {
        if (text === '') {
            text = '0';
        }
    }
}
