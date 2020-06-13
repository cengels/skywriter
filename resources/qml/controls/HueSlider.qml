import QtQuick 2.14
import QtQuick.Controls 2.14
import "." as Sky
import "../../js/color.js" as Color

Sky.AbstractColorSlider {
    id: root
    property alias hue: root.value
    property color color: Qt.hsla(hue, 1.0, 0.5, 1.0)
    //! A desaturated variant of the color for display in a material UI.
    property color displayColor: Qt.hsla(hue, 0.7, 0.5, 1.0)
    handleColor: root.displayColor

    gradient: Gradient {
        orientation: Gradient.Horizontal
        GradientStop { position: 0.0; color: "#E64D5E" }
        GradientStop { position: 0.16666666666; color: "#FDFA5F" }
        GradientStop { position: 0.33333333333; color: "#A8F974" }
        GradientStop { position: 0.5; color: "cyan" }
        GradientStop { position: 0.66666666666; color: "#3969F9" }
        GradientStop { position: 0.83333333333; color: "#E66CF4" }
        GradientStop { position: 1.0; color: "#E64D5E" }
    }
}
