import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Window 2.14
import "../controls" as Sky

Sky.Dialog {
    title: qsTr("About Skywriter")
    padding: 5
    width: 450
    height: 270
    minimumWidth: 450
    minimumHeight: 270

    Sky.LinkText {
        anchors.fill: parent
        leftPadding: 5
        rightPadding: 10
        horizontalAlignment: Text.AlignHCenter
        text: qsTr("<h2>Skywriter %1</h2>" +
                   "<h3>© <a href=\"%3\">cengels</a> 2019-%2</h3>" +
              "<p>A minimalistic word processor for novelists that exports " +
              "to Markdown</p>" +
              "<p>Released under the <a href=\"%4\">GPLv3</a> license.</p>" +
              "<p>Skywriter is in active development. You can track the " +
              "development as well as suggest new features or report bugs " +
              "on Skywriter's public <a href=\"%5\">github</a> page.</p>")
              .arg(Qt.application.version)
              .arg(new Date().getFullYear())
              .arg("https://www.cengels.com")
              .arg("https://www.gnu.org/licenses/gpl-3.0.en.html")
              .arg("https://github.com/cengels/skywriter/issues")
    }
}
