import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Window 2.14
import "./" as Sky

Window {
    enum Result {
        None,
        Accepted,
        Applied,
        Discarded,
        HelpRequested,
        Rejected
    }

    id: dialog
    minimumWidth: 250
    minimumHeight: 200
    modality: Qt.WindowModal
    color: palette.window
    property int padding: 5
    property int standardButtons: DialogButtonBox.Ok
    default property var content
    property int result

    onResultChanged: {
        if (result === Sky.Dialog.Result.Accepted
                || result === Sky.Dialog.Result.Applied
                || result === Sky.Dialog.Result.Discarded
                || result === Sky.Dialog.Result.Rejected) {
            close();
        }
    }

    onVisibleChanged: {
        if (visible) {
            result = Sky.Dialog.Result.None
        }
    }

    Page {
        width: parent.width
        height: parent.height
        padding: dialog.padding

        contentChildren: dialog.content

        footer: DialogButtonBox {
            standardButtons: dialog.standardButtons
            delegate: Sky.Button {}

            onAccepted: dialog.result = Sky.Dialog.Result.Accepted
            onApplied: dialog.result = Sky.Dialog.Result.Applied
            onDiscarded: dialog.result = Sky.Dialog.Result.Discarded
            onHelpRequested: dialog.result = Sky.Dialog.Result.HelpRequested
            onRejected: dialog.result = Sky.Dialog.Result.Rejected
        }
    }
}
