import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Window 2.14
import "qrc:/qml/controls/text" as Sky
import "qrc:/qml/controls/forms" as Sky

Sky.Text {
    Layout.fillWidth: true
    color: Qt.darker(palette.text, 1.25)
    font.pointSize: 10.5
    font.letterSpacing: 2
}
