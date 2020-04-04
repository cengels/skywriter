import QtQuick 2.14
import QtQuick.Controls 2.14
import Qt.labs.settings 1.0
import QtQuick.Window 2.14

import com.skywriter.text 1.0

ApplicationWindow {
    id: mainWindow
    title: "Skywriter"
    visible: true

    readonly property int defaultWidth: 800
    readonly property int defaultHeight: 600

    minimumWidth: 500
    minimumHeight: 200
    width: defaultWidth
    height: defaultHeight

    onVisibilityChanged: {
        // Ensure that the window does not retain screen size
        // when going from FullScreen/Maximized to Windowed
        // only width is checked because the height may be reduced by
        // the height of the taskbar/titlebar when coming out of Maximized
        if (visibility === Window.Windowed && width >= Screen.width) {
            mainWindow.setGeometry(Screen.width / 2 - defaultWidth / 2,
                                   Screen.height / 2 - defaultHeight / 2,
                                   defaultWidth,
                                   defaultHeight)
        }
    }

    Settings {
        category: "window"
        fileName: "skywriter.settings"
        property alias x: mainWindow.x
        property alias y: mainWindow.y
        property alias width: mainWindow.width
        property alias height: mainWindow.height
        property alias visibility: mainWindow.visibility
    }

    menuBar: MenuBar {
        Menu {
            title: qsTr("File")
            Action {
                text: qsTr("New")
                shortcut: StandardKey.New
            }
            Action {
                text: qsTr("Open...")
                shortcut: StandardKey.Open
            }
            Action {
                text: qsTr("Save")
                shortcut: StandardKey.Save
            }
            Action {
                text: qsTr("Save As...")
                shortcut: StandardKey.SaveAs
            }
            Action {
                text: qsTr("Rename...")
                shortcut: StandardKey.Replace
            }
            Action {
                text: qsTr("Rename...")
            }
            Action {
                property var previousVisibility: mainWindow.visibility
                text: qsTr("Fullscreen")
                shortcut: StandardKey.FullScreen
                onTriggered: {
                    if (mainWindow.visibility === Window.FullScreen) {
                        mainWindow.visibility = previousVisibility === Window.FullScreen ? Window.AutomaticVisibility : previousVisibility;
                        previousVisibility = null;
                    } else {
                        previousVisibility = mainWindow.visibility;
                        mainWindow.showFullScreen();
                    }
                }
            }
            Action {
                text: qsTr("Preferences...")
                shortcut: StandardKey.Preferences
            }
            Action {
                text: qsTr("Quit")
                shortcut: StandardKey.Quit
                onTriggered: Qt.quit()
            }
        }
    }

    footer: ToolBar {
        // ...
    }

    FormattableTextArea {
        // Binds itself to the text area below and modifies its document
        id: document
        document: textArea.textDocument
        cursorPosition: textArea.cursorPosition
        selectionStart: textArea.selectionStart
        selectionEnd: textArea.selectionEnd
//        Component.onCompleted: document.load("qrc:/texteditor.html")
//        onLoaded: {
//            textArea.text = text
//        }
//        onError: {
//            errorDialog.text = message
//            errorDialog.visible = true
//        }
    }

    ScrollView {
        anchors.fill: parent
        clip: true
        ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
        ScrollBar.vertical.minimumSize: 0.06

        TextArea {
            id: textArea
            selectByMouse: true
            text: "test"
            textFormat: TextEdit.RichText
            verticalAlignment: TextEdit.AlignTop
            wrapMode: TextEdit.Wrap
            persistentSelection: true
            Keys.onPressed: {
                if (event.key === Qt.Key_B) {
                    document.toggleBold();
                    event.accepted = true;
                }
            }
        }
    }
}
