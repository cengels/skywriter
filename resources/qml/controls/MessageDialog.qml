import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Window 2.14
import "./" as Sky

Sky.Dialog {
    id: dialog
    property string text
    width: 350
    height: 100
    minimumWidth: 350
    minimumHeight: 100
    buttonAlignment: undefined
    padding: 10
    property var horizontalAlignment: Text.AlignLeft
    property var verticalAlignment: Text.AlignVCenter

    Sky.Text {
        text: dialog.text
        font.pointSize: 12
        horizontalAlignment: dialog.horizontalAlignment
        verticalAlignment: dialog.verticalAlignment
        anchors.fill: parent
    }
}
