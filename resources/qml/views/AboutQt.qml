import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Window 2.14
import "../controls/dialog" as Sky
import "../controls/text" as Sky

Sky.Dialog {
    title: qsTr("About Qt")
    padding: 5
    width: 800
    height: 440
    minimumWidth: 800
    minimumHeight: 440

    Sky.LinkText {
        anchors.fill: parent
        leftPadding: 5
        rightPadding: 10
        text: qsTr("<p>This program uses Qt version %1.</p>" +
              "<p>Qt is a C++ toolkit for cross-platform application " +
              "development.</p>" +
              "<p>Qt provides single-source portability across all major desktop " +
              "operating systems. It is also available for embedded Linux and other " +
              "embedded and mobile operating systems.</p>" +
              "<p>Qt is available under multiple licensing options designed " +
              "to accommodate the needs of our various users.</p>" +
              "<p>Qt licensed under our commercial license agreement is appropriate " +
              "for development of proprietary/commercial software where you do not " +
              "want to share any source code with third parties or otherwise cannot " +
              "comply with the terms of GNU (L)GPL.</p>" +
              "<p>Qt licensed under GNU (L)GPL is appropriate for the " +
              "development of Qt&nbsp;applications provided you can comply with the terms " +
              "and conditions of the respective licenses.</p>" +
              "<p>Please see <a href=\"http://%3/\">%3</a> " +
              "for an overview of Qt licensing.</p>" +
              "<p>Copyright (C) %2 The Qt Company Ltd and other " +
              "contributors.</p>" +
              "<p>Qt and the Qt logo are trademarks of The Qt Company Ltd.</p>" +
              "<p>Qt is The Qt Company Ltd product developed as an open source " +
              "project. See <a href=\"http://%4/\">%4</a> for more information.</p>")
              .arg(QT_VERSION).arg(new Date().getFullYear()).arg("qt.io/licensing").arg("qt.io")
    }
}
