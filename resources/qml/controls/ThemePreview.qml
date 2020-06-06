import QtQuick 2.14
import QtQuick.Window 2.14

import '.' as Sky

Rectangle {
    id: root
    property var theme
    default property var content
    clip: true
    layer.enabled: true
    layer.effect: Sky.CornerRadius { radius: 15 }
    color: theme.windowBackground

    function getRelativeWidth() {
        if (theme.documentWidth <= 1.0) {
            return width * theme.documentWidth;
        }

        return Math.min(width, theme.documentWidth / Screen.width * width);
    }

    function getRelativeHeight() {
        if (theme.documentHeight <= 1.0) {
            return height * theme.documentHeight;
        }

        return Math.min(height, theme.documentHeight / Screen.height * height);

    }

    Image {
        anchors.fill: parent
        source: theme.backgroundImage
        visible: theme.status === Image.Ready
    }

    Rectangle {
        anchors.centerIn: parent
        width: getRelativeWidth()
        height: getRelativeHeight()
        color: theme.documentBackground
        children: parent.content
    }
}
