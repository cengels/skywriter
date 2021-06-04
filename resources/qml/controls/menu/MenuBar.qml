import QtQuick
import QtQuick.Controls
import Skywriter.Theming
import "qrc:/js/color.js" as Color

MenuBar {
    id: menuBar
    font.pointSize: 10
    property var openMenus: []

    background: Rectangle {
        color: ThemeManager.activeTheme.uiBackground
    }

    delegate: MenuBarItem {
        id: menuBarItem
        implicitHeight: 25

        background: Rectangle {
            opacity: enabled ? 1 : 0.3
            color: menuBarItem.highlighted ? Color.highlight(ThemeManager.activeTheme.uiBackground, 0.7) : "transparent"

            MouseArea {
                id: mouseArea
                anchors.fill: parent
                hoverEnabled: true
                acceptedButtons: Qt.LeftButton
                onClicked: {
                    if (menuBarItem.menu.visible) {
                        menuBarItem.menu.close();
                    } else if (!menuBarItem.menu.opened) {
                        menuBarItem.menu.open();
                    }
                }

                onHoveredChanged: {
                    if (!menuBarItem.menu.opened && mouseArea.containsMouse && menuBar.openMenus.length > 0) {
                        for (const menu of menuBar.openMenus) {
                            menu.close();
                        }

                        menuBarItem.menu.open();
                    }
                }
            }
        }

        contentItem: Text {
            text: menuBarItem.text
            font: menuBarItem.font
            opacity: enabled ? 1.0 : 0.3
            color: menuBarItem.highlighted ? Color.highlight(ThemeManager.activeTheme.uiColor, 1.2) : ThemeManager.activeTheme.uiColor
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
        }

        Connections {
            target: menuBarItem.menu
            // A menu is visible as soon as a part of it is visible
            // and doesn't become invisible until it is completely hidden.
            // Opened, on the other hand, is only true if the menu is fully
            // open and all transitions are complete.
            function onVisibleChanged() {
                if (menuBarItem.menu.visible) {
                    openMenus.push(menuBarItem.menu);
                }
            }
            function onOpenedChanged() {
                if (!menuBarItem.menu.opened) {
                    const index = openMenus.indexOf(menuBarItem.menu);

                    if (index !== -1) {
                        openMenus.splice(index, 1);
                    }
                }
            }
        }
    }

    Behavior on y {
        animation: NumberAnimation {
            easing {
                type: Easing.InOutSine
                amplitude: 1.0
                period: 0.5
            }
        }
    }

    Behavior on opacity {
        animation: NumberAnimation {
            easing {
                type: Easing.InOutSine
                amplitude: 1.0
                period: 0.4
            }
        }
    }
}
