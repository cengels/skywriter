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
import "qrc:/qml/controls/dialog" as Sky
import "qrc:/qml/controls/menu" as Sky
import "qrc:/qml/utility" as Sky
import "qrc:/qml/views" as View
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

        ProgressTracker.maximumIdleMinutes = Qt.binding(() => Settings.Application.maximumProgressIdleMinutes);
        ProgressTracker.dailyReset = Qt.binding(() => Settings.Application.dailyReset);
        ProgressTracker.load();
    }

    Sky.MessageDialog {
        id: errorDialog
        title: qsTr("Error")
    }

    View.AboutQt { id: aboutQt }
    View.AboutSkywriter { id: aboutSkywriter }
    View.Appearance { id: appearance }

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
                title: qsTr("File")
                Sky.MenuItem { action: actions.newDocument }
                Sky.MenuItem { action: actions.open }
                MenuSeparator {}
                Sky.MenuItem { action: actions.save }
                Sky.MenuItem { action: actions.saveAs }
                Sky.MenuItem { action: actions.rename }
                MenuSeparator {}
                Sky.MenuItem { action: actions.fullscreen }
                Sky.MenuItem {
                    text: qsTr("Preferences...")
                }
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
                Sky.MenuItem {
                    text: qsTr("Find")
                }
                Sky.MenuItem {
                    text: qsTr("Find and Replace")
                }
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
                Sky.MenuItem {
                    text: qsTr("Progress...")
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
                    verticalScrollbar.position = clamp(verticalScrollbar.position + verticalScrollbar.movementFactor, 0, 1 - verticalScrollbar.size);
                }
            }

            Connections {
                target: Mouse

                property bool isDragging: false

                onMove: {
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

                onPressed: {
                    if (Mouse.isInside(textArea)) {
                        isDragging = true;
                    }
                }

                onReleased: {
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
                    focus: true
                    contentY: verticalScrollbar.position * contentHeight
                    overflowArea: height * 0.8
                    clip: true

                    // User-configurable properties
                    firstLineIndent: ThemeManager.activeTheme.firstLineIndent
                    sceneBreak: Settings.Document.sceneBreak

                    Keys.onPressed: {
                        const maxScroll = 1.0 - verticalScrollbar.size
                        if (event.key === Qt.Key_Home) {
                            verticalScrollbar.position = 0.0;
                        } else if (event.key === Qt.Key_End) {
                            verticalScrollbar.position = maxScroll;
                        } else if (event.key === Qt.Key_PageUp) {
                            verticalScrollbar.position = Math.max(0.0, verticalScrollbar.position - (textArea.height / contentHeight));
                        } else if (event.key === Qt.Key_PageDown) {
                            verticalScrollbar.position = Math.min(maxScroll, verticalScrollbar.position + (textArea.height / contentHeight));
                        }
                    }

                    property double previousContentHeight: 0
                    onContentHeightChanged: {
                        // activeFocus makes sure the text area does not adjust its scroll
                        // until the text area has fully initialized
                        if (textArea.activeFocus) {
                            // "restore" the previous scroll position
                            let newScroll = verticalScrollbar.position / contentHeight * previousContentHeight;

                            if (Settings.Application.keepScrollWhenTyping) {
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
                    }

                    onFileUrlChanged: {
                        Settings.Document.lastFile = toLocalFileString(textArea.fileUrl)
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
        Sky.StatsBar {
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
