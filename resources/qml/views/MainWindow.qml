import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Window 2.14
import QtQuick.Dialogs 1.3
import QtQuick.Layouts 1.14
import Qt.labs.platform 1.1 as Platform
import Skywriter.Settings 1.0 as Settings
import Skywriter.Text 1.0
import Skywriter.Progress 1.0
import Skywriter.Theming 1.0
import "../controls" as Controls
import "../types" as Sky
import "." as View

ApplicationWindow {
    id: mainWindow
    title: document.fileName + (document.modified ? "*" : "") + "  •  Skywriter"
    visible: true

    readonly property int defaultWidth: 800
    readonly property int defaultHeight: 600

    minimumWidth: 500
    minimumHeight: 200

    onVisibilityChanged: {
        // Ensure that the window does not retain screen size
        // when going from FullScreen/Maximized to Windowed
        // only width is checked because the height may be reduced by
        // the height of the taskbar/titlebar when coming out of Maximized
        if (visibility === Window.Windowed && width >= Screen.width) {
            mainWindow.setGeometry(Screen.width / 2 - defaultWidth / 2,
                                   Screen.height / 2 - defaultHeight / 2,
                                   defaultWidth,
                                   defaultHeight);
        }
    }

    function save() {
        if (document.fileUrl != null
                && document.fileUrl !== ''
                && document.fileName !== 'untitled.txt') {
            document.saveAs(document.fileUrl);
        } else {
            saveDialog.open();
        }
    }

    function loadDocument(url) {
        document.progressSuspended = true;
        document.load(url);
        ProgressTracker.changeActiveFile(url);
        document.progressSuspended = false;
    }

    x: Settings.Window.x;
    y: Settings.Window.y;
    width: Settings.Window.width;
    height: Settings.Window.height;
    // By default, Settings.Window.visibility is Window.Hidden.
    // This results in a "ghost process," i.e. an application that is running
    // a process but does not have a window. It's not visible in the taskbar
    // either.
    //
    // As for Minimized: If a window starts minimized, it usually leads to
    // user frustration as the first thought is that the program crashed.
    visibility: Settings.Window.visibility === Window.Hidden
             || Settings.Window.visibility === Window.Minimized
                ? Window.AutomaticVisibility
                : Settings.Window.visibility;

    property bool forceClose: false;

    onClosing: {
        if (document.modified && !forceClose) {
            close.accepted = false;
            quitDialog.show();

            return;
        }

        Settings.Window.x = x;
        Settings.Window.y = y;
        Settings.Window.width = width;
        Settings.Window.height = height;
        Settings.Window.visibility = visibility;

        // Ensures that, if the user chose to quit without saving, their
        // unsaved progress is not saved. If they *did* save their progress,
        // (progressAtLastSave - progressToday) will be 0.
        ProgressTracker.addProgress(document.progressAtLastSave - ProgressTracker.progressToday);
        ProgressTracker.save();
    }

    Component.onCompleted: {
        ProgressTracker.maximumIdleMinutes = Qt.binding(() => Settings.Application.maximumProgressIdleMinutes);
        ProgressTracker.autosaveMinutes = Qt.binding(() => Settings.Application.progressAutosaveMinutes);
        ProgressTracker.dailyReset = Qt.binding(() => Settings.Application.dailyReset);
        ProgressTracker.load();
    }

    FileDialog {
        id: openDialog
        title: qsTr("Open...")
        nameFilters: ["Text files (*.txt)", "Markdown files (*.md)", "HTML files (*.html *.htm)", "All files (*)"]
        selectedNameFilter: "All files (*)"
        selectExisting: true
        folder: Platform.StandardPaths.writableLocation(Platform.StandardPaths.DocumentsLocation)
        onAccepted: loadDocument(openDialog.fileUrl)
    }

    FileDialog {
        id: saveDialog
        title: qsTr("Save As...")
        defaultSuffix: "md"
        nameFilters: openDialog.nameFilters
        selectedNameFilter: "All files (*)"
        selectExisting: false
        folder: Platform.StandardPaths.writableLocation(Platform.StandardPaths.DocumentsLocation)
        onAccepted: {
            document.saveAs(saveDialog.fileUrl);

            if (saveDialog.fileUrl !== ProgressTracker.fileUrl) {
                ProgressTracker.renameActiveFile(saveDialog.fileUrl);
            }
        }
    }

    Controls.MessageDialog {
        id: errorDialog
        title: qsTr("Error")
    }

    Controls.MessageDialog {
        id : quitDialog
        title: qsTr("Quit?")
        text: qsTr("You have unsaved changes. Do you want to save your changes before quitting?")
        buttons: [
            Controls.Button {
                text: qsTr("Save")
                DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
            },

            Controls.Button {
                text: qsTr("Don't Save")
                DialogButtonBox.buttonRole: DialogButtonBox.DestructiveRole
            },

            Controls.Button {
                text: qsTr("Cancel")
                DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
            }
        ]


        onAccepted: {
            save();
            mainWindow.close();
        }
        onDiscarded: {
            forceClose = true;
            mainWindow.close();
        }
    }

    View.AboutQt { id: aboutQt }
    View.AboutSkywriter { id: aboutSkywriter }
    View.Appearance {
        id: appearance
        property int oldThemeIndex;
        onRejected: ThemeManager.activeThemeIndex = oldThemeIndex;
    }

    menuBar: Controls.MenuBar {
        font.pointSize: 10
        background: Rectangle {
            color: palette.base
        }
        delegate: MenuBarItem {
            implicitHeight: 25
        }

        Controls.Menu {
            title: qsTr("File")
            Action {
                text: qsTr("New")
                shortcut: StandardKey.New
                // Remember ProgressTracker integration
            }
            Action {
                text: qsTr("Open...")
                shortcut: StandardKey.Open
                onTriggered: openDialog.open()
            }
            MenuSeparator {}
            Action {
                text: qsTr("Save")
                shortcut: StandardKey.Save
                onTriggered: save()
            }
            Action {
                text: qsTr("Save As...")
                shortcut: StandardKey.SaveAs
                onTriggered: saveDialog.open()
            }
            Action {
                text: qsTr("Rename...")
                // Remember to rename file in ProgressTracker as well
            }
            MenuSeparator {}
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
                onTriggered: mainWindow.close();
            }
        }
        Controls.Menu {
            title: qsTr("Edit")
            Action {
                text: qsTr("Undo")
                onTriggered: textArea.undo()
                enabled: textArea.canUndo
            }
            Action {
                text: qsTr("Redo")
                onTriggered: textArea.redo()
                enabled: textArea.canRedo
            }
            MenuSeparator {}
            Action {
                text: qsTr("Cut")
                onTriggered: textArea.cut()
                enabled: textArea.selectedText
            }
            Action {
                text: qsTr("Copy")
                onTriggered: textArea.copy()
                enabled: textArea.selectedText
            }
            Action {
                id: pasteAction
                text: qsTr("Paste")
                onTriggered: textArea.paste()
                enabled: textArea.canPaste
            }
            Action {
                text: qsTr("Paste Unformatted")
                shortcut: "Ctrl+Shift+V"
                enabled: textArea.canPaste
            }
            Action {
                text: qsTr("Paste Untracked")
                enabled: textArea.canPaste
                onTriggered: {
                    document.progressSuspended = true;
                    pasteAction.trigger();
                    document.progressSuspended = false;
                }
            }
            Action {
                id: deleteAction
                text: qsTr("Delete")
                onTriggered: textArea.remove(textArea.selectionStart, textArea.selectionEnd)
                enabled: textArea.selectedText
            }
            Action {
                text: qsTr("Delete Untracked")
                enabled: textArea.selectedText
                onTriggered: {
                    document.progressSuspended = true;
                    deleteAction.trigger();
                    document.progressSuspended = false;
                }
            }
            MenuSeparator {}
            Action {
                text: qsTr("Select Word")
                shortcut: "Ctrl+W"
                onTriggered: textArea.selectWord()
            }
            Action {
                text: qsTr("Select Line")
            }
            Action {
                text: qsTr("Select All")
            }
            MenuSeparator {}
            Action {
                text: qsTr("Find")
            }
            Action {
                text: qsTr("Find and Replace")
            }
        }

        Controls.Menu {
            title: qsTr("Formatting")
            Action {
                text: qsTr("Bold")
                shortcut: StandardKey.Bold
                onTriggered: document.toggleBold()
            }
            Action {
                text: qsTr("Italic")
                shortcut: StandardKey.Italic
                onTriggered: document.toggleItalics()
            }
            Action {
                text: qsTr("Strikethrough")
                onTriggered: document.toggleStrikethrough()
            }
            MenuSeparator {}
            Action {
                text: qsTr("No heading")
                shortcut: "Ctrl+0"
            }
            Action {
                text: qsTr("Heading 1")
                shortcut: "Ctrl+1"
            }
            Action {
                text: qsTr("Heading 2")
                shortcut: "Ctrl+2"
            }
            Action {
                text: qsTr("Heading 3")
                shortcut: "Ctrl+3"
            }
            Action {
                text: qsTr("Heading 4")
                shortcut: "Ctrl+4"
            }
            Action {
                text: qsTr("Heading 5")
                shortcut: "Ctrl+5"
            }
            Action {
                text: qsTr("Heading 6")
                shortcut: "Ctrl+6"
            }
        }
        Controls.Menu {
            title: qsTr("Tools")
            Action {
                text: qsTr("Appearance...")
                onTriggered: {
                    appearance.oldThemeIndex = ThemeManager.activeThemeIndex;
                    appearance.show();
                }
            }
            Action {
                text: qsTr("Progress...")
            }
        }
        Controls.Menu {
            title: qsTr("Help")
            Action {
                text: qsTr("About Skywriter...")
                onTriggered: aboutSkywriter.show()
            }
            Action {
                text: qsTr("About Qt...")
                onTriggered: aboutQt.show()
            }
        }
    }

    footer: Controls.StatsBar {
        document: document
    }

    FormattableTextArea {
        // Binds itself to the text area below and modifies its document
        id: document
        document: textArea.textDocument
        cursorPosition: textArea.cursorPosition
        selectionStart: textArea.selectionStart
        selectionEnd: textArea.selectionEnd
        firstLineIndent: ThemeManager.activeTheme.firstLineIndent

        Component.onCompleted: {
            if (Settings.Document.lastFile != null) {
                loadDocument(Qt.resolvedUrl(Settings.Document.lastFile));
            }
        }

        onFileUrlChanged: {
            Settings.Document.lastFile = document.fileUrl.toString();
        }

        property int oldWordCount;
        property bool progressSuspended: false;
        onWordCountChanged: {
            if (!progressSuspended && oldWordCount !== wordCount) {
                ProgressTracker.addProgress(wordCount - oldWordCount);
            }

            oldWordCount = wordCount;
        }

        onError: {
            errorDialog.text = message;
            errorDialog.show();
        }

        property int progressAtLastSave;
        onLastModifiedChanged: {
            // called whenever the file is saved
            progressAtLastSave = ProgressTracker.progressToday;
        }
    }

    Image {
        id: backgroundImage
        anchors.fill: container
        source: ThemeManager.activeTheme.backgroundImage
        fillMode: ThemeManager.activeTheme.fillMode
    }

    Rectangle {
        id: backgroundRectangle
        anchors.fill: container
        color: ThemeManager.activeTheme.windowBackground
    }

    Control {
        id: container
        background: ThemeManager.activeTheme.backgroundImage === '' ? backgroundRectangle : backgroundImage
        anchors.fill: parent

        ScrollBar {
            id: verticalScrollbar
            z: 99
            hoverEnabled: true
            parent: container
            anchors.top: container.top
            anchors.right: container.right
            anchors.bottom: container.bottom
            contentItem.opacity: 1
            size: scrollView.height / textArea.height
            visible: textArea.height > scrollView.height
        }

        MouseArea {
            anchors.fill: parent
            onWheel: {
                wheel.accepted = true;

                if (wheel.modifiers === Qt.NoModifier) {
                    verticalScrollbar.setPosition(wheel.angleDelta.y < 0
                            ? Math.min(verticalScrollbar.position + 0.05, 1.0 - verticalScrollbar.size)
                            : Math.max(verticalScrollbar.position - 0.05, 0.0));
                }
            }
        }

        Rectangle {
            width: ThemeManager.activeTheme.documentWidth > 1 ? ThemeManager.activeTheme.documentWidth : ThemeManager.activeTheme.documentWidth * container.width
            height: ThemeManager.activeTheme.documentHeight > 1 ? ThemeManager.activeTheme.documentHeight : ThemeManager.activeTheme.documentHeight * container.height
            anchors.centerIn: parent
            color: ThemeManager.activeTheme.documentBackground

            Item {
                id: scrollView
                height: parent.height - ThemeManager.activeTheme.paddingVertical * 2
                width: parent.width - ThemeManager.activeTheme.paddingHorizontal * 2
                anchors.centerIn: parent
                clip: true

                Flickable {
                    boundsBehavior: Flickable.StopAtBounds
                    ScrollBar.vertical: verticalScrollbar
                    contentWidth: textArea.width
                    contentHeight: textArea.height

                    TextEdit {
                        id: textArea
                        width: scrollView.width
                        height: contentHeight

                        selectByMouse: true
                        textFormat: TextEdit.MarkdownText
                        verticalAlignment: TextEdit.AlignTop
                        horizontalAlignment: ThemeManager.activeTheme.textAlignment
                        wrapMode: TextEdit.Wrap
                        persistentSelection: true
                        color: ThemeManager.activeTheme.fontColor
                        font: ThemeManager.activeTheme.font

                        onTextChanged: {
                            const caretPosition = cursorRectangle.y;
                            // parent.y is always negative
                            const visibleStartY = -parent.y;
                            const visibleEndY = -parent.y + scrollView.height;

                            if (caretPosition < visibleStartY) {
                                verticalScrollbar.position = caretPosition / parent.height;
                            } else if (caretPosition > visibleEndY) {
                                verticalScrollbar.position = (caretPosition + cursorRectangle.height - visibleEndY) / parent.height;
                            }
                        }
                    }
                }
            }
        }
    }
}
