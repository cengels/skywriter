import QtQuick
import QtQuick.Controls
import "qrc:/qml/controls/color" as Sky

Sky.AbstractColorSlider {
    id: root
    property color fromColor: 'black'
    property color toColor: 'white'
    property color color: Qt.rgba(
                              fromColor.r + value * (toColor.r - fromColor.r),
                              fromColor.g + value * (toColor.g - fromColor.g),
                              fromColor.b + value * (toColor.b - fromColor.b),
                              fromColor.a + value * (toColor.a - fromColor.a)
                          )
    handleColor: root.color

    gradient: Gradient {
        orientation: Gradient.Horizontal
        GradientStop { position: 0.0; color: fromColor }
        GradientStop { position: 1.0; color: toColor }
    }
}
