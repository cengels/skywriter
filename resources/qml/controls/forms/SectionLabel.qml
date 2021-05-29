import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import "qrc:/qml/controls/text" as Sky
import "qrc:/qml/controls/forms" as Sky

Sky.Text {
    Layout.fillWidth: true
    color: Qt.darker(palette.text, 1.25)
    font.pointSize: 10.5
    font.letterSpacing: 1
}
