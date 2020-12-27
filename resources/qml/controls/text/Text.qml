import QtQuick 2.14
import QtQuick.Controls 2.14

Text {
    wrapMode: Text.Wrap
    color: palette.text
    linkColor: palette.link
    textFormat: Text.PlainText
    lineHeight: 0.8
    bottomPadding: padding !== -1 ? padding : 4
}
