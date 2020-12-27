import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Window 2.14
import "qrc:/qml/controls/text" as Sky

ColumnLayout {
    id: root
    Layout.fillWidth: true
    property string title: ""
    property bool collapsible: true
    property bool collapsed: false
    default property list<Item> content
    property alias columns: gridLayout.columns
    property alias rows: gridLayout.rows
    state: "expanded"
    states: [
        State {
            name: "expanded"
            when: !root.collapsed
            PropertyChanges { target: gridLayout; Layout.preferredHeight: gridLayout.implicitHeight }
        },
        State {
            name: "collapsed"
            when: root.collapsed
            PropertyChanges { target: gridLayout; Layout.preferredHeight: 0 }
        }
    ]

    Item {
        id: headingContainer
        Layout.leftMargin: 6
        Layout.fillWidth: true
        Layout.preferredHeight: heading.height

        Sky.Heading {
            id: heading
            anchors.left: parent.left
            anchors.right: parent.right
            text: title
            level: 2
            font.pointSize: 13
            colorFactor: mouseArea.containsMouse && root.collapsible ? 1.5 : 1
        }

        MouseArea {
            id: mouseArea
            anchors.fill: heading
            cursorShape: root.collapsible ? Qt.PointingHandCursor : undefined
            hoverEnabled: root.collapsible
            onClicked: {
                if (root.collapsible) {
                    root.collapsed = !root.collapsed;
                }
            }
        }
    }

    GridLayout {
        id: gridLayout
        children: content
        Layout.leftMargin: 12
        Layout.rightMargin: 12
        Layout.fillWidth: true
        clip: true

        Behavior on Layout.preferredHeight {
            PropertyAnimation { duration: 200; easing.type: Easing.InOutQuad }
        }
    }
}
