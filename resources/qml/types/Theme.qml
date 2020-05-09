import QtQuick 2.14
import QtGraphicalEffects 1.14

QtObject {
    objectName: "Theme"
    property string name: ""
    property bool defaultTheme: false
    property double fontSize: 12.0
    property string fontFamily: "Times New Roman"
    property string backgroundImage
    property int backgroundImageFillType: Image.PreserveAspectCrop
    property double documentWidth: 0.9
    property double documentHeight: 1.0
    property int paddingVertical: 20
    property int paddingHorizontal: 50
    property double firstLineIndent: 0.0
    property double lineHeight: 1.0
    property double paragraphSpacing: 0.0
    property color fontColor: black
    property color windowBackground: lightgray
    property color documentBackground: white
    property int textAlignment: Text.AlignLeft
    property DropShadow fontShadow
}
