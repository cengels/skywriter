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
import "qrc:/qml/controls" as Sky
import "qrc:/qml/controls/text" as Sky
import "qrc:/qml/controls/dialog" as Sky
import "qrc:/qml/controls/menu" as Sky
import "qrc:/qml/controls/forms" as Sky
import "qrc:/qml/utility" as Sky
import "qrc:/qml/views" as View
import "qrc:/qml/shapes" as Shapes
import "qrc:/js/color.js" as Color

ApplicationWindow {
    id: mainWindow
    title: textArea.fileName + (textArea.modified ? "*" : "") + "  •  Skywriter"
    visible: true
    color: ThemeManager.activeTheme.windowBackground

    readonly property int windowSizeSubtraction: 250
    readonly property int defaultWidth: Screen.width - windowSizeSubtraction
    readonly property int defaultHeight: Screen.height - windowSizeSubtraction
    // The number in pixels away from a collapsed control's original edge
    // that the collapsed control is uncollapsed.
    readonly property int edgeTolerance: 16

    minimumWidth: 820
    minimumHeight: 200

    readonly property Sky.Actions actions: Sky.Actions {
        textArea: textArea
        mainWindow: mainWindow
    }

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
        function onError() {
            errorDialog.text = message;
            errorDialog.show();
        }
        function onWarning() {
            errorDialog.text = message;
            errorDialog.show();
        }
        function onInfo() {
            errorDialog.text = message;
            errorDialog.show();
        }
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
    property int previousHeight

    onWidthChanged: {
        // See verticalScrollbar. This works because QML calls this function
        // last, after all bindings and layouts are updated.
        previousWidth = width;
    }

    onHeightChanged: previousHeight = height

    property bool forceClose: false;

    onClosing: {
        if (textArea.modified && !forceClose) {
            close.accepted = false;

            actions.unsavedQuitHandlers.enabled = true;

            actions.unsavedDialog.show();

            return;
        }

        Settings.Window.x = x;
        Settings.Window.y = y;
        Settings.Window.width = width;
        Settings.Window.height = height;
        Settings.Window.visibility = visibility;

        // safety call in case the active theme was renamed
        Settings.Application.theme = ThemeManager.activeTheme.name;
    }

    Component.onCompleted: {
        const foundIndex = ThemeManager.themes.findIndex(x => x.name === Settings.Application.theme);
        if (foundIndex !== -1) {
            ThemeManager.activeThemeIndex = foundIndex;
        }

        ProgressTracker.minimumIdleMinutes = Qt.binding(() => Settings.User.groups.progress.minimumIdleMinutes);
        ProgressTracker.dailyReset = Qt.binding(() => Settings.User.groups.progress.dailyReset);
        ProgressTracker.load();
    }

    Sky.MessageDialog {
        id: errorDialog
        title: qsTr("Error")
    }

    View.AboutQt { id: aboutQt }
    View.AboutSkywriter { id: aboutSkywriter }
    View.Appearance { id: appearance }
    View.Preferences { id: preferences }

    menuBar: Item {
        height: mainWindow.visibility === Window.FullScreen ? 0 : menuBar.implicitHeight
        Sky.MenuBar {
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

            Sky.Menu {
                id: fileMenu
                title: qsTr("File")
                Sky.MenuItem { action: actions.newDocument }
                Sky.MenuItem { action: actions.open }
                MenuSeparator {}
                Sky.MenuItem { action: actions.save }
                Sky.MenuItem { action: actions.saveAs }
                Sky.MenuItem { action: actions.rename }
                MenuSeparator {}
                Sky.MenuItem { action: actions.fullscreen }
                Sky.MenuItem { action: actions.quit }
            }
            Sky.Menu {
                title: qsTr("Edit")
                Sky.MenuItem { action: actions.undo }
                Sky.MenuItem { action: actions.redo }
                MenuSeparator {}
                Sky.MenuItem { action: actions.cut }
                Sky.MenuItem { action: actions.copy }
                Sky.MenuItem { action: actions.paste }
                Sky.MenuItem { action: actions.pasteUnformatted }
                Sky.MenuItem { action: actions.pasteUntracked }
                Sky.MenuItem { action: actions.deleteSelected }
                Sky.MenuItem { action: actions.deleteUntracked }
                MenuSeparator {}
                Sky.MenuItem { action: actions.selectWord }
                Sky.MenuItem { action: actions.selectParagraph }
                Sky.MenuItem { action: actions.selectAll }
                MenuSeparator {}
                Sky.MenuItem { action: actions.find }
                Sky.MenuItem { action: actions.findReplace }
            }

            Sky.Menu {
                id: formattingMenu

                title: qsTr("Formatting")
                Sky.MenuItem { action: actions.bold }
                Sky.MenuItem { action: actions.italics }
                Sky.MenuItem { action: actions.strikethrough }
                MenuSeparator {}
                Sky.MenuItem { action: actions.noHeading }
                Sky.MenuItem { action: actions.heading1 }
                Sky.MenuItem { action: actions.heading2 }
                Sky.MenuItem { action: actions.heading3 }
                Sky.MenuItem { action: actions.heading4 }
                Sky.MenuItem { action: actions.heading5 }
                Sky.MenuItem { action: actions.heading6 }
                MenuSeparator {}
                Sky.MenuItem { action: actions.sceneBreak }
            }
            Sky.Menu {
                title: qsTr("Tools")
                Sky.MenuItem {
                    text: qsTr("Appearance...")
                    onTriggered: {
                        appearance.oldThemeIndex = ThemeManager.activeThemeIndex;
                        appearance.show();
                    }
                }
                MenuSeparator {}
                Sky.MenuItem {
                    text: qsTr("Progress...")
                }
                Sky.MenuItem { action: actions.finishProgressItem }
                MenuSeparator {}
                Sky.MenuItem {
                    text: qsTr("Preferences...")
                    onTriggered: {
                        preferences.show();
                    }
                }
            }
            Sky.Menu {
                title: qsTr("Help")
                Sky.MenuItem {
                    text: qsTr("About Skywriter...")
                    onTriggered: aboutSkywriter.show()
                }
                Sky.MenuItem {
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
        visible: ThemeManager.activeTheme.backgroundImage !== ''
    }

    Control {
        id: container
        background: backgroundImage
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
            property real scrollStart: 0

            function scrollTo(y) {
                if (scrollAnimation.running && scrollStart != 0) {
                    scrollAnimation.stop();

                    const difference = Math.abs(scrollAnimation.to - verticalScrollbar.position);

                    if (position - y < 0) {
                        scrollAnimation.to = y + difference;
                    } else {
                        scrollAnimation.to = y - difference;
                    }
                } else {
                    scrollAnimation.to = y;
                }

                scrollStart = verticalScrollbar.position;
                scrollAnimation.duration = Math.min(Math.abs(verticalScrollbar.position - y) * textArea.contentHeight * 2.5, 1000);
                scrollAnimation.start();
            }

            NumberAnimation on position {
                id: scrollAnimation
                easing.type: Easing.OutQuad
            }

            readonly property int middleMouseThreshold: 32

            /* Section: maintain scroll position if window width changes */
            property double lastSize: 0
            property double lastPosition: 0
            property string lastTheme: ""
            property bool fixPositionNext: false
            onPositionChanged: {
                if (mainWindow.active) {
                    adjustPosition();
                }
            }

            onSizeChanged: {
                if (mainWindow.active) {
                    adjustPosition();
                }
            }

            function adjustPosition() {
                if (mainWindow.width !== previousWidth
                        || mainWindow.height !== previousHeight
                        || lastTheme !== ThemeManager.activeTheme.name
                        || fixPositionNext) {
                    // scroll position changed because window width was changed
                    // or because theme was changed

                    lastTheme = ThemeManager.activeTheme.name;
                    fixPositionNext = lastSize === size || lastPosition === position;

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
                let visibleStartY = textArea.contentY;
                let visibleEndY = textArea.contentY + scrollView.height;

                if (!menuBar.collapsed && mainWindow.visibility === Window.FullScreen) {
                    visibleStartY += menuBar.y;
                }

                if (!searchBar.collapsed) {
                    visibleEndY -= searchBar.height;
                }

                if (!replaceBar.collapsed) {
                    visibleEndY -= replaceBar.height;
                }

                if (!statsBar.collapsed && mainWindow.visibility === Window.FullScreen) {
                    visibleEndY -= statsBar.height;
                }

                if (caretPosition < visibleStartY) {
                    verticalScrollbar.position = caretPosition / textArea.contentHeight;
                } else if (caretPosition > visibleEndY) {
                    verticalScrollbar.position = (caretPosition + textArea.caretRectangle().height - visibleEndY + visibleStartY) / textArea.contentHeight;
                }
            }

            function centerOnCaret(smooth) {
                const verticalCaretCenter = textArea.caretRectangle().y + textArea.contentY + textArea.caretRectangle().height / 2;
                const relativeCaretPosition = verticalCaretCenter / textArea.contentHeight;
                const verticalScrollViewCenter = textArea.contentY + scrollView.height / 2;
                const relativeScrollViewCenter = verticalScrollViewCenter / textArea.contentHeight;
                const newPosition = verticalScrollbar.position + relativeCaretPosition - relativeScrollViewCenter;

                if (smooth) {
                    verticalScrollbar.scrollTo(newPosition);
                } else {
                    verticalScrollbar.position = newPosition;
                }
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
                    verticalScrollbar.position = clamp(verticalScrollbar.position + verticalScrollbar.movementFactor, 0, 1 - verticalScrollbar.size);
                }
            }

            Connections {
                target: Mouse

                property bool isDragging: false

                function onMove() {
                    if (Mouse.buttons & Qt.LeftButton && isDragging) {
                        const textAreaPosition = textArea.mapFromGlobal(Mouse.globalPosition.x, Mouse.globalPosition.y);

                        if (textAreaPosition.y < 0 || textAreaPosition.y > textArea.height) {
                            const delta = textAreaPosition.y < 0 ? -textAreaPosition.y : textArea.height - textAreaPosition.y;

                            verticalScrollbar.movementFactor = -(delta / container.height / 70);
                        } else {
                            verticalScrollbar.movementFactor = 0;
                        }
                    }

                    if (verticalScrollbar.middleMouseOriginY >= 0) {
                        const yDelta = verticalScrollbar.middleMouseOriginY - Mouse.globalPosition.y;

                        if (Math.abs(yDelta) > verticalScrollbar.middleMouseThreshold) {
                            verticalScrollbar.movementFactor = -(yDelta / container.height / 150);
                        } else {
                            verticalScrollbar.movementFactor = 0;
                        }
                    }
                }

                function onPressed() {
                    if (Mouse.isInside(textArea)) {
                        isDragging = true;
                    }
                }

                function onReleased(button, longPress) {
                    if (button === Qt.MiddleButton && longPress) {
                        verticalScrollbar.middleMouseOriginY = -1;
                        verticalScrollbar.movementFactor = 0;
                        Mouse.resetCursor();
                    } else if (button === Qt.LeftButton) {
                        isDragging = false;
                        verticalScrollbar.movementFactor = 0;
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

                    verticalScrollbar.scrollTo(wheel.angleDelta.y < 0
                            ? Math.min(verticalScrollbar.position - delta, 1.0 - verticalScrollbar.size)
                            : Math.max(verticalScrollbar.position - delta, 0.0));
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
                    focus: true
                    contentY: verticalScrollbar.position * contentHeight
                    overflowArea: height * 0.8
                    clip: true
                    property bool changedSinceLastAutosave: false

                    Keys.onPressed: {
                        const maxScroll = 1.0 - verticalScrollbar.size
                        if (event.key === Qt.Key_PageUp) {
                            verticalScrollbar.position = Math.max(0.0, verticalScrollbar.position - (textArea.height / contentHeight));
                        } else if (event.key === Qt.Key_PageDown) {
                            verticalScrollbar.position = Math.min(maxScroll, verticalScrollbar.position + (textArea.height / contentHeight));
                        }
                    }

                    property bool suspended: false;
                    // This is necessary because signals are always queued
                    // and only emitted if there is nothing else happening
                    // on the UI thread. If we simply used textArea.loading
                    // in the other signal handlers, the loading state might
                    // be "in the future", so to speak.
                    onLoadingChanged: suspended = textArea.loading;

                    property double previousContentHeight: 0
                    onContentHeightChanged: {
                        // activeFocus makes sure the text area does not adjust its scroll
                        // until the text area has fully initialized
                        if (textArea.activeFocus && !suspended && mainWindow.active) {
                            // "restore" the previous scroll position
                            let newScroll = verticalScrollbar.position / contentHeight * previousContentHeight;

                            if (Settings.User.groups.editor.keepScrollOnType) {
                                // ... and add the difference of old and new content height
                                newScroll += (contentHeight - previousContentHeight) / contentHeight;
                            }

                            verticalScrollbar.position = newScroll;
                        }

                        previousContentHeight = contentHeight;
                    }

                    Component.onCompleted: {
                        if (Settings.Document.lastFile != null) {
                            actions.loadDocument(fromLocalFileString(Settings.Document.lastFile));
                            textArea.caretPosition = Settings.Document.caretPosition;
                            verticalScrollbar.centerOnCaret();
                        }

                        Settings.Document.caretPosition = Qt.binding(() => textArea.caretPosition);

                        for (const replacement of Settings.Document.autoReplacements) {
                            if (replacement.length === 2) {
                                setReplacement(replacement[0], replacement[1]);
                            } else {
                                setSmartReplacement(replacement[0], replacement[1], replacement[2]);
                            }
                        }

                        forceActiveFocus();

                        if (!textArea.loading) {
                            suspended = false;
                        }
                    }

                    onFileUrlChanged: {
                        Settings.Document.lastFile = toLocalFileString(textArea.fileUrl)
                    }

                    property int oldWordCount;
                    property bool progressSuspended: false
                    onWordCountChanged: {
                        // If neither canUndo nor canRedo are true, the word count change
                        // was not prompted by the user (e.g. during loading).
                        if (!progressSuspended && (canUndo || canRedo) && oldWordCount !== wordCount) {
                            ProgressTracker.addProgress(wordCount - oldWordCount);
                        }

                        oldWordCount = wordCount;

                        if (progressSuspended) {
                            progressSuspended = false;
                        }
                    }

                    property int progressAtLastSave;
                    onLastModifiedChanged: {
                        // called whenever the file is saved
                        progressAtLastSave = ProgressTracker.progressToday;
                    }

                    onCaretPositionChanged: {
                        if (!suspended) {
                            verticalScrollbar.scrollToCaret();
                        }
                    }

                    onTextChanged: textArea.changedSinceLastAutosave = true;

                    Timer {
                        interval: Settings.User.groups.files.backupInterval * 1000
                        repeat: true
                        running: Settings.User.groups.files.backupInterval !== 0 && textArea.modified && textArea.changedSinceLastAutosave && textArea.fileExists
                        onTriggered: {
                            textArea.backup();
                            textArea.changedSinceLastAutosave = false;
                        }
                    }

                    onContextMenuRequested: contextMenu.popup()

                    onSearchResultCountChanged: {
                        if (Settings.User.groups.editor.instantSearch) {
                            if (textArea.searchResultCount > 0) {
                                textArea.jumpToNext();
                            } else {
                                // Clears the selection.
                                textArea.caretPosition = textArea.caretPosition;
                            }
                        }
                    }

                    Menu {
                        id: contextMenu
                        Sky.MenuItem { action: actions.undo }
                        Sky.MenuItem { action: actions.redo }
                        MenuSeparator {}
                        Sky.MenuItem { action: actions.cut }
                        Sky.MenuItem { action: actions.copy }
                        Sky.MenuItem { action: actions.paste }
                        Sky.MenuItem { action: actions.pasteUnformatted }
                        Sky.MenuItem { action: actions.pasteUntracked }
                        Sky.MenuItem { action: actions.deleteSelected }
                        Sky.MenuItem { action: actions.deleteUntracked }
                        MenuSeparator {}
                        Sky.MenuItem { action: actions.selectWord }
                        Sky.MenuItem { action: actions.selectParagraph }
                        Sky.MenuItem { action: actions.selectAll }

                        enter: Transition {
                            NumberAnimation { property: "opacity"; from: 0.0; to: 1.0; duration: 50 }
                        }

                        exit: Transition {
                            NumberAnimation { property: "opacity"; from: 1.0; to: 0.0; duration: 50 }
                        }
                    }
                }
            }
        }
    }

    readonly property Pane searchBar: searchBar
    readonly property Pane replaceBar: replaceBar
    readonly property alias searchHasFocus: searchBarFocusScope.activeFocus

    footer: Item {
        height: mainWindow.visibility === Window.FullScreen ? 0 : statsBar.implicitHeight

        FocusScope {
            anchors.fill: parent
            id: searchBarFocusScope
            focus: !searchBar.collapsed

            Sky.CollapsiblePane {
                id: searchBar
                anchors.left: parent.left
                anchors.right: parent.right
                height: row.height + 15
                y: (mainWindow.visibility === Window.FullScreen && !statsBar.collapsed ? -statsBar.height : 0) - (replaceBar.collapsed ? 0 : replaceBar.height) - (collapsed ? 0 : height)
                horizontalPadding: 10
                onCollapsedChanged: {
                    if (collapsed) {
                        textArea.clearMatches();
                        textArea.forceActiveFocus();
                    } else {
                        searchString.forceActiveFocus();

                        if (searchString.text !== '') {
                            searchString.selectAll();
                            find();
                        }
                    }
                }

                function find() {
                    let flags = FormattableTextArea.SearchOption.None;

                    if (useRegEx.checked) {
                        flags |= FormattableTextArea.SearchOption.RegEx;
                    }

                    if (caseSensitive.checked) {
                        flags |= FormattableTextArea.SearchOption.CaseSensitive;
                    }

                    if (inSelection.checked) {
                        flags |= FormattableTextArea.SearchOption.InSelection;
                    }

                    if (wholeWords.checked) {
                        flags |= FormattableTextArea.SearchOption.WholeWords;
                    }

                    textArea.find(searchString.text, flags);
                }

                Row {
                    id: row
                    spacing: 15

                    Sky.TextField {
                        id: searchString
                        width: Math.max(300, searchBar.width * 0.3)
                        placeholderText: qsTr("Find")
                        onTextChanged: searchBar.find()

                        KeyNavigation.tab: replaceBar.collapsed ? null : replaceString
                        onEnterPressed: textArea.jumpToNext()
                    }

                    Sky.IconButton {
                        width: searchString.height - 3
                        height: searchString.height - 3
                        anchors.verticalCenter: parent.verticalCenter
                        useTheme: true
                        shape: Shapes.ArrowLeft {}
                        enabled: textArea.searchResultCount > 0
                        // If the user clicks twice in quick succession,
                        // the second click will be captured by onDoubleClicked.
                        // onClicked will not see it. Therefore, using onReleased
                        // here.
                        onReleased: textArea.jumpToPrevious()
                    }

                    Sky.IconButton {
                        width: searchString.height - 3
                        height: searchString.height - 3
                        anchors.verticalCenter: parent.verticalCenter
                        useTheme: true
                        shape: Shapes.ArrowRight {}
                        enabled: textArea.searchResultCount > 0
                        onReleased: textArea.jumpToNext()
                    }

                    Sky.TextSwitch {
                        id: caseSensitive
                        height: searchString.height
                        text: qsTr("Case-sensitive")
                        tooltip: qsTr("Any characters that are uppercase in the searched text must also be uppercase in the found text.")
                        useTheme: true
                        onToggled: searchBar.find()
                    }

                    Sky.TextSwitch {
                        id: wholeWords
                        height: searchString.height
                        text: qsTr("Whole words")
                        tooltip: qsTr("Match only whole words, i.e. don't match if the found text is only part of a word.")
                        useTheme: true
                        onToggled: searchBar.find()
                    }

                    Sky.TextSwitch {
                        id: inSelection
                        height: searchString.height
                        text: qsTr("In selection")
                        tooltip: qsTr("Search the current selection only.")
                        enabled: textArea.selectedText !== ""
                        useTheme: true
                        onToggled: searchBar.find()
                    }

                    Sky.TextSwitch {
                        id: useRegEx
                        height: searchString.height
                        text: qsTr("Regular expression")
                        tooltip: qsTr("Use a Perl regular expression to match text. Note that this is considerably slower than matching text directly.")
                        useTheme: true
                        onToggled: searchBar.find()
                    }

                    Sky.Text {
                        anchors.verticalCenter: parent.verticalCenter
                        text: qsTr("%L1 results found").arg(textArea.searchResultCount)
                        opacity: searchString.text !== '' ? 1.0 : 0.0
                        font.pointSize: 10
                        color: ThemeManager.activeTheme.uiColor
                        wrapMode: Text.NoWrap

                        Behavior on opacity { OpacityAnimator { } }
                    }
                }

                Sky.IconButton {
                    anchors.right: parent.right
                    width: 32
                    height: 32
                    circle: false
                    fill: ThemeManager.activeTheme.uiColor
                    // For some reason Qt.darker()/Qt.lighter() don't work here
                    // so we have to use opacity.
                    opacity: 0.6
                    shape: Shapes.CloseIcon {}
                    onClicked: {
                        searchBar.collapsed = true;
                        replaceBar.collapsed = true;
                    }
                }
            }

            Sky.CollapsiblePane {
                id: replaceBar
                anchors.left: parent.left
                anchors.right: parent.right
                y: (mainWindow.visibility === Window.FullScreen && !statsBar.collapsed ? -statsBar.height : 0) - (collapsed ? 0 : height)
                horizontalPadding: 10
                onCollapsedChanged: {
                    if (!searchBar.collapsed) {
                        searchString.forceActiveFocus();
                    }
                }

                Row {
                    anchors.fill: parent
                    spacing: 15

                    Sky.TextField {
                        id: replaceString
                        width: Math.max(300, searchBar.width * 0.3)
                        height: searchString.height
                        placeholderText: qsTr("Replace with...")
                        onEnterPressed: {
                            if (textArea.searchResultCount > 0) {
                                textArea.replaceNext(replaceString.text);
                                searchBar.find();
                            }
                        }
                    }

                    Sky.Button {
                        id: replaceNext
                        text: qsTr("Replace next")
                        enabled: textArea.searchResultCount > 0
                        height: replaceString.height
                        onReleased: {
                            textArea.replaceNext(replaceString.text);
                            // Calling find again so that the ranges are updated
                            // to accommodate the replacement.
                            // If the performance of this isn't good enough,
                            // replace it with a proper implementation in replaceNext().
                            searchBar.find();
                        }
                    }

                    Sky.Button {
                        text: qsTr("Replace all")
                        enabled: replaceNext.enabled
                        height: replaceString.height
                        onReleased: textArea.replaceAll(replaceString.text)
                    }
                }
            }
        }

        // The StatsBar is wrapped in an Item because otherwise it would not be
        // possible to manipulate the y property (as a footer element).
        // Simply detaching it as footer is also not possible, as this will
        // cause an ugly snap when the window is fullscreened (and therefore
        // grows).
        Sky.StatsBar {
            id: statsBar
            y: collapsed || mainWindow.visibility !== Window.FullScreen ? 0 : -height
            anchors.left: parent.left
            anchors.right: parent.right
            document: textArea
            collapsed: searchBar.collapsed && mainWindow.visibility === Window.FullScreen
                       && Mouse.windowPosition.y <= mainWindow.height - height - edgeTolerance
        }
    }

    Drawer {
        id: documentStructureDrawer
        y: menuBar.collapsed ? 0 : menuBar.height
        width: 0.2 * mainWindow.width
        height: mainWindow.height - y - (statsBar.collapsed ? 0 : statsBar.height) + (searchBar.collapsed ? 0 : searchBar.y)
        modal: false
        clip: true

        background: Rectangle { color: Qt.darker(ThemeManager.activeTheme.uiBackground, 1.1) }

        Connections {
            target: Mouse

            function onWindowPositionChanged() {
                if (!documentStructureDrawer.opened
                        && Mouse.windowPosition.y >= documentStructureDrawer.y + edgeTolerance
                        && Mouse.windowPosition.y < documentStructureDrawer.y + documentStructureDrawer.height
                        && Mouse.windowPosition.x < edgeTolerance
                        && !fileMenu.visible
                        && Mouse.buttons === Qt.NoButton) {
                    documentStructureDrawer.open();
                } else if (documentStructureDrawer.opened
                        && !drawerScrollBar.active
                        && (Mouse.windowPosition.y < documentStructureDrawer.y
                        || Mouse.windowPosition.y >= documentStructureDrawer.y + documentStructureDrawer.height
                        || Mouse.windowPosition.x > documentStructureDrawer.width)) {
                    documentStructureDrawer.close();
                }
            }
        }

        ListView {
            id: listView
            anchors.fill: parent
            anchors.topMargin: 12
            anchors.bottomMargin: 12
            boundsBehavior: Flickable.StopAtBounds
            model: textArea.documentStructure
            spacing: 12
            ScrollBar.vertical: ScrollBar { id: drawerScrollBar; width: 12 }
            delegate: Control {
                width: listView.width - leftPadding
                height: button.height
                leftPadding: (modelData.depth - 1) * 20 + 12
                rightPadding: 12

                Sky.Button {
                    id: button
                    x: parent.leftPadding
                    width: parent.width - parent.rightPadding
                    height: column.implicitHeight + 20
                    prominence: textArea.currentDocumentSegment === modelData ? Sky.Button.Primary : Sky.Button.Secondary

                    onClicked: {
                        textArea.suspended = true;
                        textArea.caretPosition = modelData.position;
                        textArea.suspended = false;
                        verticalScrollbar.centerOnCaret(true);
                    }

                    Column {
                        id: column
                        anchors.fill: parent
                        anchors.margins: 10

                        Row {
                            Sky.Text {
                                id: headingLabel
                                font.pointSize: 13
                                width: column.width - segmentWordCount.width
                                text: modelData.heading === '' ? qsTr('No heading') : modelData.heading

                            }
                            Sky.Text {
                                id: segmentWordCount
                                font.pointSize: 9
                                color: Qt.darker(palette.text, 1.3)
                                width: 75
                                height: headingLabel.height
                                horizontalAlignment: Qt.AlignRight
                                verticalAlignment: Qt.AlignBottom
                                text: qsTr("%L1 words").arg(modelData.totalWordCount)
                            }
                        }

                        Sky.Text {
                            color: Qt.darker(palette.text, 1.15)
                            font.pointSize: 10.5
                            width: parent.width
                            visible: modelData.subheading !== ''
                            text: modelData.subheading
                        }
                    }
                }
            }
        }
    }
}
