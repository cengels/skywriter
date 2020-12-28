import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import "qrc:/qml/controls/dialog" as Sky
import "qrc:/qml/controls/text" as Sky

Sky.Dialog {
    id: dialog
    property string text
    width: 360
    height: 105
    minimumWidth: 360
    minimumHeight: 105
    maximumWidth: 360
    maximumHeight: 105
    buttonAlignment: undefined
    padding: 15
    property var horizontalAlignment: Text.AlignLeft
    property var verticalAlignment: Text.AlignVCenter

    Sky.Text {
        text: dialog.text
        font.pointSize: 11.5
        horizontalAlignment: dialog.horizontalAlignment
        verticalAlignment: dialog.verticalAlignment
        anchors.fill: parent
    }
}
