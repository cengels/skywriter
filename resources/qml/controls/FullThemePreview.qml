import QtQuick 2.14

import '.' as Sky
import Skywriter.Theming 1.0

Sky.ThemePreview {
    id: root
    showUI: true

    Sky.LoremIpsum {
        anchors.fill: parent
        font.family: theme.fontFamily
        color: theme.fontColor
        font.pointSize: theme.fontSize
        lineHeight: theme.lineHeight
        lineHeightMode: theme.lineHeight > 3.0 ? Text.FixedHeight : Text.ProportionalHeight
        horizontalAlignment: theme.textAlignment
        leftPadding: theme.paddingHorizontal
        rightPadding: theme.paddingHorizontal
        topPadding: theme.paddingVertical
        bottomPadding: theme.paddingVertical
        paragraphSpacing: theme.paragraphSpacing
        textIndent: theme.firstLineIndent
    }
}
