import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
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
    property bool canAccept: true
    default property var content

    onVisibleChanged: {
        if (visible) {
            shown();
        }
    }

    signal shown

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
            delegate: Sky.Button {
                readonly property bool isAcceptRole: DialogButtonBox.buttonRole === DialogButtonBox.AcceptRole
                                                     || DialogButtonBox.buttonRole === DialogButtonBox.YesRole
                                                     || DialogButtonBox.buttonRole === DialogButtonBox.ApplyRole
                prominence: isAcceptRole ? Sky.Button.Primary : Sky.Button.Secondary
                enabled: isAcceptRole ? dialog.canAccept : true
            }

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
