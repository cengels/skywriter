import QtQuick 2.14
import QtQuick.Controls 2.14 as QtQuickControls
import QtGraphicalEffects 1.14
import "." as Sky

QtQuickControls.Dialog {
    id: root
    padding: 20
    margins: 10
    topInset: -2
    leftInset: -2
    rightInset: -8
    bottomInset: -8
    header: Sky.Heading {
        topPadding: 8
        horizontalAlignment: Qt.AlignCenter
        level: 2
        text: root.title
    }

    footer: QtQuickControls.DialogButtonBox {
        background: null
        delegate: Sky.Button {}
    }

    background: Item {
        Rectangle {
            anchors.fill: parent
            id: background
            radius: 30
            color: palette.mid
        }

        DropShadow {
            anchors.fill: background
            source: background
            horizontalOffset: 4
            verticalOffset: 4
            radius: 12
            samples: 24.0
            color: "#62000000"
        }
    }

    enter: Transition {
        NumberAnimation { property: "scale"; easing.type: Easing.InOutSine; from: 0.95; to: 1; duration: 200 }
        NumberAnimation { property: "opacity"; easing.type: Easing.InOutSine; from: 0.0; to: 1.0; duration: 200 }
    }
    exit: Transition {
        NumberAnimation { property: "scale"; easing.type: Easing.InOutSine; from: 1.0; to: 0.95; duration: 200 }
        NumberAnimation { property: "opacity"; easing.type: Easing.InOutSine; from: 1.0; to: 0.0; duration: 200 }
    }
}

