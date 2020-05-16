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
    property int result
    property var buttonAlignment: Qt.AlignRight | Qt.AlignVCenter
    default property var content

    signal accepted
    signal applied
    signal discarded
    signal helpRequested
    signal rejected

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
            alignment: dialog.buttonAlignment
            standardButtons: dialog.standardButtons
            delegate: Sky.Button {
                onTextChanged: {
                    if (text === qsTr("Save")) {
                        DialogButtonBox.buttonRole = DialogButtonBox.ApplyRole
                    }
                }
            }

            onAccepted: { dialog.result = Sky.Dialog.Result.Accepted; dialog.accepted(); }
            onApplied: { dialog.result = Sky.Dialog.Result.Applied; dialog.applied(); }
            onDiscarded: { dialog.result = Sky.Dialog.Result.Discarded; dialog.discarded(); }
            onHelpRequested: { dialog.result = Sky.Dialog.Result.HelpRequested; dialog.helpRequested(); }
            onRejected: { dialog.result = Sky.Dialog.Result.Rejected; dialog.rejected(); }
        }
    }
}
