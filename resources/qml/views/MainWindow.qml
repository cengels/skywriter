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
import Skywriter.Events 1.0
import Skywriter.Errors 1.0
import "../controls" as Controls
import "../types" as Sky
import "." as View
import "qrc:/js/color.js" as Color

ApplicationWindow {
    id: mainWindow
    title: textArea.fileName + (textArea.modified ? "*" : "") + "  •  Skywriter"
    visible: true

    readonly property int windowSizeSubtraction: 250
    readonly property int defaultWidth: Screen.width - windowSizeSubtraction
    readonly property int defaultHeight: Screen.height - windowSizeSubtraction
    // The number in pixels away from a collapsed control's original edge
    // that the collapsed control is uncollapsed.
    readonly property int edgeTolerance: 16

    minimumWidth: 820
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

    Connections {
        target: ErrorManager
        onError: {
            errorDialog.text = message;
            errorDialog.show();
        }
        onWarning: {
            errorDialog.text = message;
            errorDialog.show();
        }
        onInfo: {
            errorDialog.text = message;
            errorDialog.show();
        }
    }

    function save() {
        if (textArea.fileUrl != null
                && textArea.fileUrl !== ''
                && textArea.fileName !== 'untitled.txt') {
            textArea.saveAs(textArea.fileUrl);
        } else {
            saveDialog.open();
        }
    }

    function loadDocument(url) {
        textArea.load(url);
        ProgressTracker.changeActiveFile(url);
    }

    function clamp(value, min, max) {
        return Math.min(Math.max(value, min), max);
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

    property int previousWidth

    onWidthChanged: {
        // See verticalScrollbar. This works because QML calls this function
        // last, after all bindings and layouts are updated.
        previousWidth = width;
    }

    property bool forceClose: false;

    onClosing: {
        if (textArea.modified && !forceClose) {
            close.accepted = false;
            quitDialog.show();

            return;
        }

        Settings.Window.x = x;
        Settings.Window.y = y;
        Settings.Window.width = width;
        Settings.Window.height = height;
        Settings.Window.visibility = visibility;

        // safety call in case the active theme was renamed
        Settings.Application.theme = ThemeManager.activeTheme.name;

        // Ensures that, if the user chose to quit without saving, their
        // unsaved progress is not saved. If they *did* save their progress,
        // (progressAtLastSave - progressToday) will be 0.
        ProgressTracker.addProgress(textArea.progressAtLastSave - ProgressTracker.progressToday);
        ProgressTracker.save();
    }

    Component.onCompleted: {
        const foundIndex = ThemeManager.themes.findIndex(x => x.name === Settings.Application.theme);
        if (foundIndex !== -1) {
            ThemeManager.activeThemeIndex = foundIndex;
        }

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
        folder: textArea.fileName !== '' && textArea.fileName != null
                ? textArea.directoryUrl
                : Platform.StandardPaths.writableLocation(Platform.StandardPaths.DocumentsLocation);
        onAccepted: loadDocument(openDialog.fileUrl);
    }

    FileDialog {
        id: saveDialog
        title: qsTr("Save As...")
        defaultSuffix: "md"
        nameFilters: openDialog.nameFilters
        selectedNameFilter: "All files (*)"
        selectExisting: false
        folder: textArea.fileName !== '' && textArea.fileName != null
                ? textArea.directoryUrl
                : Platform.StandardPaths.writableLocation(Platform.StandardPaths.DocumentsLocation);
        onAccepted: {
            textArea.saveAs(saveDialog.fileUrl);

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

    menuBar: Item {
        height: mainWindow.visibility === Window.FullScreen ? 0 : menuBar.implicitHeight
        Controls.MenuBar {
            id: menuBar
            anchors.fill: mainWindow.visibility === Window.FullScreen ? undefined : parent
            y: collapsed ? -height : 0
            opacity: collapsed ? 0.0 : 1.0

            property bool collapsed: mainWindow.visibility === Window.FullScreen
                && Mouse.windowPosition.y >= height + edgeTolerance

            onCollapsedChanged: {
                if (collapsed) {
                    for (const i in menus) {
                        menus[i].close();
                    }
                }
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
                        textArea.progressSuspended = true;
                        pasteAction.trigger();
                        textArea.progressSuspended = false;
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
                        textArea.progressSuspended = true;
                        deleteAction.trigger();
                        textArea.progressSuspended = false;
                    }
                }
                MenuSeparator {}
                Action {
                    text: qsTr("Select Word")
                    onTriggered: textArea.selectWord()
                }
                Action {
                    text: qsTr("Select Paragraph")
                    onTriggered: textArea.selectParagraph()
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
                    onTriggered: textArea.toggleBold()
                }
                Action {
                    text: qsTr("Italic")
                    shortcut: StandardKey.Italic
                    onTriggered: textArea.toggleItalics()
                }
                Action {
                    text: qsTr("Strikethrough")
                    onTriggered: textArea.toggleStrikethrough()
                }
                MenuSeparator {}
                Action {
                    text: qsTr("No heading")
                    shortcut: "Ctrl+0"
                    onTriggered: textArea.applyHeading(0)
                }
                Action {
                    text: qsTr("Heading 1")
                    shortcut: "Ctrl+1"
                    onTriggered: textArea.applyHeading(1)
                }
                Action {
                    text: qsTr("Heading 2")
                    shortcut: "Ctrl+2"
                    onTriggered: textArea.applyHeading(2)
                }
                Action {
                    text: qsTr("Heading 3")
                    shortcut: "Ctrl+3"
                    onTriggered: textArea.applyHeading(3)
                }
                Action {
                    text: qsTr("Heading 4")
                    shortcut: "Ctrl+4"
                    onTriggered: textArea.applyHeading(4)
                }
                Action {
                    text: qsTr("Heading 5")
                    shortcut: "Ctrl+5"
                    onTriggered: textArea.applyHeading(5)
                }
                Action {
                    text: qsTr("Heading 6")
                    shortcut: "Ctrl+6"
                    onTriggered: textArea.applyHeading(6)
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
            size: scrollView.height / textArea.contentHeight
            visible: textArea.contentHeight > scrollView.height
            enabled: visible
            minimumSize: 50 / scrollView.height

            readonly property int middleMouseThreshold: 32

            /* Section: maintain scroll position if window width changes */
            property double lastSize: 0
            property double lastPosition: 0
            property string lastTheme: ""
            property bool fixPositionNext: false
            onPositionChanged: adjustPosition();
            onSizeChanged: adjustPosition();

            function adjustPosition() {
                if (mainWindow.width !== previousWidth
                        || lastTheme !== ThemeManager.activeTheme.name
                        || fixPositionNext) {
                    // scroll position changed because window width was changed
                    // or because theme was changed

                    lastTheme = ThemeManager.activeTheme.name;
                    fixPositionNext = lastSize === size || lastPosition === position;

                    // TODO: Fix a bug where changing theme still changes position
                    // despite this call. For some reason, there is another call
                    // that changes position to an arbitrary value, but the
                    // stack trace QML supplies is completely useless.
                    if (lastPosition !== 0 && !fixPositionNext) {
                        position = lastPosition - (size - lastSize) / 2;
                    }
                }

                if (!fixPositionNext) {
                    lastPosition = position;
                    lastSize = size;
                }
            }

            function scrollToCaret() {
                const caretPosition = textArea.caretRectangle().y + textArea.contentY;
                const visibleStartY = textArea.contentY;
                const visibleEndY = textArea.contentY + scrollView.height;

                if (caretPosition < visibleStartY) {
                    verticalScrollbar.position = caretPosition / textArea.contentHeight;
                } else if (caretPosition > visibleEndY) {
                    verticalScrollbar.position = (caretPosition + textArea.caretRectangle().height - visibleEndY + visibleStartY) / textArea.contentHeight;
                }
            }

            function centerOnCaret() {
                const verticalCaretCenter = textArea.caretRectangle().y + textArea.contentY + textArea.caretRectangle().height / 2;
                const relativeCaretPosition = verticalCaretCenter / textArea.contentHeight;
                const verticalScrollViewCenter = textArea.contentY + scrollView.height / 2;
                const relativeScrollViewCenter = verticalScrollViewCenter / textArea.contentHeight;
                verticalScrollbar.position = relativeCaretPosition - relativeScrollViewCenter;
            }

            Component.onCompleted: lastTheme = ThemeManager.activeTheme.name

            /* Section: handle mouse moves with pressed middle mouse */
            property double middleMouseOriginY: -1
            property double movementFactor: 0

            Timer {
                interval: 1
                running: verticalScrollbar.movementFactor !== 0
                repeat: true
                triggeredOnStart: true
                onTriggered: {
                    verticalScrollbar.position = clamp(verticalScrollbar.position + verticalScrollbar.movementFactor, 0, 1);
                }
            }

            Connections {
                target: Mouse
                onMove: {
                    if (verticalScrollbar.middleMouseOriginY >= 0) {
                        const yDelta = verticalScrollbar.middleMouseOriginY - Mouse.globalPosition.y;

                        if (Math.abs(yDelta) > verticalScrollbar.middleMouseThreshold) {
                            verticalScrollbar.movementFactor = -(yDelta / container.height / 150);
                        } else {
                            verticalScrollbar.movementFactor = 0;
                        }
                    }
                }
                onReleased: {
                    if (button === Qt.MiddleButton && longPress) {
                        verticalScrollbar.middleMouseOriginY = -1;
                        verticalScrollbar.movementFactor = 0;
                        Mouse.resetCursor();
                    }
                }
            }

            contentItem: Rectangle {
                opacity: 1
                implicitWidth: 10
                radius: width / 2
                color: verticalScrollbar.pressed
                         ? Color.adjustContrast(Color.highlight(ThemeManager.activeTheme.documentBackground, 0.5), ThemeManager.activeTheme.windowBackground)
                         : scrollBarHoverArea.containsMouse
                             ? Color.adjustContrast(Color.highlight(ThemeManager.activeTheme.documentBackground, 0.85), ThemeManager.activeTheme.windowBackground)
                             : Color.adjustContrast(ThemeManager.activeTheme.documentBackground, ThemeManager.activeTheme.windowBackground);


                MouseArea {
                    id: scrollBarHoverArea
                    anchors.fill: parent
                    hoverEnabled: true
                    propagateComposedEvents: true
                    acceptedButtons: Qt.NoButton
                }
            }
        }

        MouseArea {
            anchors.fill: parent
            preventStealing: true
            acceptedButtons: Qt.MiddleButton

            onWheel: {
                wheel.accepted = true;

                if (wheel.modifiers === Qt.NoModifier && verticalScrollbar.enabled) {
                    // Scrolls about 72 pixels per wheel "click"
                    const delta = (wheel.angleDelta.y * 0.6) / textArea.contentHeight;

                    verticalScrollbar.position = wheel.angleDelta.y < 0
                            ? Math.min(verticalScrollbar.position - delta, 1.0 - verticalScrollbar.size)
                            : Math.max(verticalScrollbar.position - delta, 0.0);
                }
            }
            onPressed: {
                if (verticalScrollbar.middleMouseOriginY >= 0) {
                    verticalScrollbar.middleMouseOriginY = -1;
                    verticalScrollbar.movementFactor = 0;
                    Mouse.resetCursor();
                } else {
                    verticalScrollbar.middleMouseOriginY = Mouse.globalPosition.y;
                    Mouse.setCursor(Qt.SizeAllCursor);
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

                FormattableTextArea {
                    id: textArea
                    width: scrollView.width
                    height: scrollView.height
                    firstLineIndent: ThemeManager.activeTheme.firstLineIndent
                    focus: true
                    contentY: verticalScrollbar.position * contentHeight
                    overflowArea: height * 0.8
                    clip: true

                    Component.onCompleted: {
                        if (Settings.Document.lastFile != null) {
                            loadDocument(Qt.resolvedUrl(Settings.Document.lastFile));
                            textArea.caretPosition = Settings.Document.caretPosition;
                            verticalScrollbar.centerOnCaret();
                        }

                         Settings.Document.caretPosition = Qt.binding(() => textArea.caretPosition);

                        forceActiveFocus();
                    }

                    onFileUrlChanged: {
                        Settings.Document.lastFile = textArea.fileUrl.toString();
                    }

                    property int oldWordCount;
                    property bool progressSuspended: false;
                    onWordCountChanged: {
                        if (!progressSuspended && isProgress && oldWordCount !== wordCount) {
                            ProgressTracker.addProgress(wordCount - oldWordCount);
                        }

                        oldWordCount = wordCount;
                    }

                    property int progressAtLastSave;
                    onLastModifiedChanged: {
                        // called whenever the file is saved
                        progressAtLastSave = ProgressTracker.progressToday;
                    }

                    onTextChanged: {
                        if (!loading) {
                            verticalScrollbar.scrollToCaret();
                        }
                    }
                }
            }
        }
    }

    footer: Item {
        height: mainWindow.visibility === Window.FullScreen ? 0 : statsBar.implicitHeight
        // The StatsBar is wrapped in an Item because otherwise it would not be
        // possible to manipulate the y property (as a footer element).
        // Simply detaching it as footer is also not possible, as this will
        // cause an ugly snap when the window is fullscreened (and therefore
        // grows).
        Controls.StatsBar {
            id: statsBar
            anchors.fill: mainWindow.visibility === Window.FullScreen ? undefined : parent
            document: textArea
            y: collapsed ? 0 : -height
            opacity: collapsed ? 0.0 : 1.0

            property bool collapsed: mainWindow.visibility === Window.FullScreen
                && Mouse.windowPosition.y <= mainWindow.height - height - edgeTolerance

            background: Rectangle {
                color: ThemeManager.activeTheme.uiBackground

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.ArrowCursor
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
    }
}
