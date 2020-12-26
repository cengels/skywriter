import QtQml 2.14
import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Window 2.14
import QtQuick.Dialogs 1.3
import Qt.labs.platform 1.1 as Platform
import Skywriter.Text 1.0
import Skywriter.Theming 1.0
import Skywriter.Progress 1.0
import Skywriter.Settings 1.0 as Settings
import "qrc:/qml/controls/dialog" as Sky
import "qrc:/qml/controls/forms" as Sky

QtObject {
    id: root
    property FormattableTextArea textArea;
    property ApplicationWindow mainWindow;

    function saveWithPrompt() {
        if (textArea.fileUrl != null
                && textArea.fileUrl !== ''
                && textArea.fileName !== 'untitled') {
            saveDocument(textArea.fileUrl);
        } else {
            saveDialog.open();
        }
    }

    function saveDocument(url) {
        textArea.saveAs(url, Settings.User.groups.files.removeBackupOnSave);
        ProgressTracker.save();
    }

    function loadDocument(url) {
        textArea.load(url);
        ProgressTracker.changeActiveFile(url);
    }

    function reset() {
        textArea.reset();
        ProgressTracker.changeActiveFile(textArea.fileUrl);
    }

    readonly property FileDialog openDialog: FileDialog {
        title: qsTr("Open...")
        modality: Qt.ApplicationModal
        nameFilters: ["Text files (*.txt)", "Markdown files (*.md)", "HTML files (*.html *.htm)"]
        selectedNameFilter: "Markdown files (*.md)"
        selectExisting: true
        folder: textArea.fileExists
                ? textArea.directoryUrl
                : Platform.StandardPaths.writableLocation(Platform.StandardPaths.DocumentsLocation);
        onAccepted: {
            if (openDialog.fileUrl == null || openDialog.fileUrl === '') {
                return;
            }

            if (textArea.modified) {
                unsavedOpenHandlers.enabled = true;

                unsavedDialog.show();
            } else {
                loadDocument(openDialog.fileUrl);
            }
        }
    }

    readonly property FileDialog saveDialog: FileDialog {
        title: qsTr("Save As...")
        modality: Qt.ApplicationModal
        defaultSuffix: "md"
        nameFilters: openDialog.nameFilters
        selectedNameFilter: "All files (*)"
        selectExisting: false
        folder: textArea.fileExists
                ? textArea.directoryUrl
                : Platform.StandardPaths.writableLocation(Platform.StandardPaths.DocumentsLocation);
        onVisibleChanged: {
            if (visible && textArea.fileExists) {
                renameDialog.folder = textArea.directoryUrl;
            }
        }

        onAccepted: {
            if (saveDialog.fileUrl == null || saveDialog.fileUrl === '') {
                return;
            }

            saveDocument(saveDialog.fileUrl);

            if (saveDialog.fileUrl !== ProgressTracker.fileUrl) {
                ProgressTracker.changeActiveFile(saveDialog.fileUrl);
            }
        }
    }

    readonly property FileDialog renameDialog: FileDialog {
        title: qsTr("Rename...")
        modality: Qt.ApplicationModal
        defaultSuffix: saveDialog.defaultSuffix
        nameFilters: saveDialog.nameFilters
        selectedNameFilter: saveDialog.selectedNameFilter
        selectExisting: saveDialog.selectExisting
        folder: saveDialog.folder
        onVisibleChanged: {
            if (visible && textArea.fileExists) {
                renameDialog.folder = textArea.directoryUrl;
            }
        }

        onAccepted: {
            if (renameDialog.fileUrl == null || renameDialog.fileUrl === '') {
                return;
            }

            if (textArea.rename(renameDialog.fileUrl)) {
                ProgressTracker.renameActiveFile(renameDialog.fileUrl);
            }
        }
    }

    readonly property Sky.MessageDialog unsavedDialog: Sky.MessageDialog {
        text: qsTr("You have unsaved changes. Do you want to save your changes before quitting?")
        modality: Qt.ApplicationModal
        buttons: [
            Sky.Button {
                text: qsTr("Save")
                prominence: Sky.Button.Primary
                DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
            },

            Sky.Button {
                text: qsTr("Don't Save")
                prominence: Sky.Button.Destructive
                DialogButtonBox.buttonRole: DialogButtonBox.DestructiveRole
            },

            Sky.Button {
                text: qsTr("Cancel")
                prominence: Sky.Button.Secondary
                DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
            }
        ]
    }

    readonly property Connections unsavedNewHandlers: Connections {
        id: unsavedNewHandlers
        target: unsavedDialog
        enabled: false
        onAccepted: {
            saveWithPrompt();
            reset();
        }
        onDiscarded: reset()
        onVisibleChanged: unsavedNewHandlers.enabled = unsavedNewHandlers.enabled && visible
    }

    readonly property Connections unsavedOpenHandlers: Connections {
        id: unsavedOpenHandlers
        target: unsavedDialog
        enabled: false
        onAccepted: {
            saveWithPrompt();
            loadDocument(openDialog.fileUrl);
        }
        onDiscarded: {
            loadDocument(openDialog.fileUrl);
        }
        onVisibleChanged: unsavedNewHandlers.enabled = unsavedNewHandlers.enabled && visible
    }

    readonly property Connections unsavedQuitHandlers: Connections {
        id: unsavedQuitHandlers
        target: unsavedDialog
        enabled: false
        onAccepted: {
            saveWithPrompt();
            mainWindow.close();
        }
        onDiscarded: {
            mainWindow.forceClose = true;
            mainWindow.close();
        }
        onVisibleChanged: unsavedNewHandlers.enabled = unsavedNewHandlers.enabled && visible
    }

    readonly property Action fullscreen: Action {
        property var previousVisibility: mainWindow.visibility
        text: qsTr("Fullscreen")
        shortcut: "F11"
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

    readonly property Action newDocument: Action {
        text: qsTr("New")
        shortcut: StandardKey.New
        onTriggered: {
            if (textArea.modified) {
                root.unsavedNewHandlers.enabled = true;

                root.unsavedDialog.show();
            } else {
                reset();
            }
        }

        // Remember ProgressTracker integration
    }
    readonly property Action open: Action {
        text: qsTr("Open...")
        shortcut: StandardKey.Open
        onTriggered: openDialog.open()
    }
    readonly property Action save: Action {
        text: qsTr("Save")
        shortcut: StandardKey.Save
        onTriggered: saveWithPrompt()
    }
    readonly property Action saveAs: Action {
        text: qsTr("Save As...")
        shortcut: StandardKey.SaveAs
        onTriggered: saveDialog.open()
    }
    readonly property Action rename: Action {
        text: qsTr("Rename...")
        enabled: textArea.fileExists
        onTriggered: renameDialog.open()
    }

    readonly property Action quit: Action {
        text: qsTr("Quit")
        shortcut: StandardKey.Quit
        onTriggered: mainWindow.close();
    }

    readonly property Action undo: Action {
        text: qsTr("Undo")
        shortcut: StandardKey.Undo
        onTriggered: textArea.undo()
        enabled: textArea.canUndo
    }
    readonly property Action redo: Action {
        text: qsTr("Redo")
        shortcut: StandardKey.Redo
        onTriggered: textArea.redo()
        enabled: textArea.canRedo
    }

    readonly property Action cut: Action {
        text: qsTr("Cut")
        shortcut: StandardKey.Cut
        onTriggered: textArea.cut()
        enabled: textArea.selectedText
    }
    readonly property Action copy: Action {
        text: qsTr("Copy")
        shortcut: StandardKey.Copy
        onTriggered: textArea.copy()
        enabled: textArea.selectedText
    }
    readonly property Action paste: Action {
        text: qsTr("Paste")
        shortcut: StandardKey.Paste
        onTriggered: textArea.paste()
        enabled: textArea.canPaste
    }
    readonly property Action pasteUnformatted: Action {
        text: qsTr("Paste Unformatted")
        shortcut: "Ctrl+Shift+V"
        onTriggered: textArea.pasteUnformatted()
        enabled: textArea.canPaste
    }
    readonly property Action pasteUntracked: Action {
        text: qsTr("Paste Untracked")
        enabled: textArea.canPaste
        onTriggered: {
            textArea.progressSuspended = true;
            root.paste.trigger();
        }
    }
    readonly property Action deleteSelected: Action {
        text: qsTr("Delete")
        onTriggered: textArea.remove()
        enabled: textArea.selectedText
    }
    readonly property Action deleteUntracked: Action {
        text: qsTr("Delete Untracked")
        enabled: textArea.selectedText
        onTriggered: {
            textArea.progressSuspended = true;
            root.deleteSelected.trigger();
        }
    }
    readonly property Action finishProgressItem: Action {
        text: qsTr("Finish progress item")
        onTriggered: {
            ProgressTracker.finishCurrent();
        }
    }

    readonly property Action selectWord:  Action {
        text: qsTr("Select Word")
        onTriggered: textArea.selectWord()
    }

    readonly property Action selectParagraph: Action {
        text: qsTr("Select Paragraph")
        onTriggered: textArea.selectParagraph()
    }

    readonly property Action selectAll: Action {
        text: qsTr("Select All")
        shortcut: StandardKey.SelectAll
        onTriggered: textArea.selectAll()
    }

    readonly property Action italics: Action {
        text: qsTr("Italic")
        shortcut: StandardKey.Italic
        onTriggered: textArea.toggleItalics();
    }

    readonly property Action bold: Action {
        text: qsTr("Bold")
        shortcut: StandardKey.Bold
        onTriggered: textArea.toggleBold()
    }

    readonly property Action strikethrough: Action {
        text: qsTr("Strikethrough")
        onTriggered: textArea.toggleStrikethrough()
    }

    function getHeadingName(level) {
        const base = qsTr("Heading") + " " + level;
        const name = ThemeManager.activeTheme.headingName(level);

        if (name === '') {
            return base;
        }

        return base + ": " + name;
    }

    readonly property Action noHeading: Action {
        text: qsTr("No heading")
        shortcut: "Ctrl+0"
        onTriggered: textArea.applyHeading(0)
    }
    readonly property Action heading1: Action {
        text: root.getHeadingName(1)
        shortcut: "Ctrl+1"
        onTriggered: textArea.applyHeading(1)
    }
    readonly property Action heading2: Action {
        text: root.getHeadingName(2)
        shortcut: "Ctrl+2"
        onTriggered: textArea.applyHeading(2)
    }
    readonly property Action heading3: Action {
        text: root.getHeadingName(3)
        shortcut: "Ctrl+3"
        onTriggered: textArea.applyHeading(3)
    }
    readonly property Action heading4: Action {
        text: root.getHeadingName(4)
        shortcut: "Ctrl+4"
        onTriggered: textArea.applyHeading(4)
    }
    readonly property Action heading5: Action {
        text: root.getHeadingName(5)
        shortcut: "Ctrl+5"
        onTriggered: textArea.applyHeading(5)
    }
    readonly property Action heading6: Action {
        text: root.getHeadingName(6)
        shortcut: "Ctrl+6"
        onTriggered: textArea.applyHeading(6)
    }

    readonly property Action sceneBreak: Action {
        text: qsTr("Scene break")
        shortcut: "Ctrl+#"
        onTriggered: textArea.insertSceneBreak()
    }

    readonly property Action find: Action {
        text: qsTr("Find")
        shortcut: StandardKey.Find
        onTriggered: {
            if (mainWindow.searchHasFocus) {
                mainWindow.searchBar.collapsed = true;
            } else {
                mainWindow.searchBar.collapsed = false;
                mainWindow.searchBar.forceActiveFocus();
            }

            if (mainWindow.searchBar.collapsed && !mainWindow.replaceBar.collapsed) {
                mainWindow.replaceBar.collapsed = true;
            }
        }
    }

    readonly property Action findReplace: Action {
        text: qsTr("Find and replace")
        shortcut: StandardKey.Replace
        onTriggered: {
            if (mainWindow.searchHasFocus && !mainWindow.replaceBar.collapsed) {
                mainWindow.replaceBar.collapsed = true;
            } else {
                mainWindow.replaceBar.collapsed = false;
                mainWindow.replaceBar.forceActiveFocus();
            }

            if (mainWindow.searchBar.collapsed && !mainWindow.replaceBar.collapsed) {
                mainWindow.searchBar.collapsed = false;
            }
        }
    }
}
