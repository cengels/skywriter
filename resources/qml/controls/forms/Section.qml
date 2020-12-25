import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Window 2.14
import "qrc:/qml/controls/text" as Sky

ColumnLayout {
    id: root
    anchors.left: parent.left
    anchors.right: parent.right
    property string title: ""
    property bool collapsible: true
    property bool collapsed: false
    default property list<Item> content
    property alias columns: gridLayout.columns
    property alias rows: gridLayout.rows
    property int columns
    property int rows
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
            colorFactor: mouseArea.containsMouse ? 1.5 : 1
        }

        MouseArea {
            id: mouseArea
            anchors.fill: heading
            cursorShape: Qt.PointingHandCursor
            hoverEnabled: true
            onClicked: {
                if (root.collapsible) {
                    collapsed = !collapsed;
                }
            }
        }
    }

    // This layout seems to cause a recursive rearrange.
    // It still works, but should definitely be fixed at
    // some point.
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
