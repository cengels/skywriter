import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Window 2.14
import "qrc:/qml/controls/dialog" as Sky
import "qrc:/qml/controls/forms" as Sky

Window {
    id: dialog
    minimumWidth: 250
    minimumHeight: 200
    flags: Qt.Dialog
    modality: Qt.WindowModal
    color: palette.window
    property int padding: 5
    property int standardButtons: 0
    property list<Sky.Button> buttons
    property var buttonAlignment: Qt.AlignRight | Qt.AlignVCenter
    default property var content

    signal accepted
    signal applied
    signal discarded
    signal rejected
    signal action
    signal helpRequested
    signal yes
    signal no
    signal reset

    Page {
        width: parent.width
        height: parent.height
        padding: dialog.padding

        contentChildren: dialog.content != null ? dialog.content : []

        footer: DialogButtonBox {
            alignment: dialog.buttonAlignment
            delegate: Sky.Button { }

            contentChildren: dialog.buttons != null ? dialog.buttons : {}
            standardButtons: dialog.standardButtons

            onClicked: {
                dialog.close();

                switch (button.DialogButtonBox.buttonRole) {
                    case DialogButtonBox.AcceptRole: dialog.accepted(); break;
                    case DialogButtonBox.RejectRole: dialog.rejected(); break;
                    case DialogButtonBox.DestructiveRole: dialog.discarded(); break;
                    case DialogButtonBox.ActionRole: dialog.action(); break;
                    case DialogButtonBox.HelpRole: dialog.helpRequested(); break;
                    case DialogButtonBox.YesRole: dialog.yes(); break;
                    case DialogButtonBox.NoRole: dialog.no(); break;
                    case DialogButtonBox.ResetRole: dialog.reset(); break;
                    case DialogButtonBox.ApplyRole: dialog.applied(); break;
                }
            }
        }
    }
}
