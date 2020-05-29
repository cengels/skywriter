QT += quick quickcontrols2 quick-private

CONFIG += c++20

#Application version
VERSION_MAJOR = 0
VERSION_MINOR = 0
VERSION_BUILD = 1

DEFINES += "VERSION_MAJOR=$$VERSION_MAJOR"\
       "VERSION_MINOR=$$VERSION_MINOR"\
       "VERSION_BUILD=$$VERSION_BUILD"

#Target version
VERSION = $${VERSION_MAJOR}.$${VERSION_MINOR}.$${VERSION_BUILD}

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Refer to the documentation for the
# deprecated API to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS \
    QT_USE_QSTRINGBUILDER # redefines + into QStringBuilder's more efficient %

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
#        src/document_handler.cpp \
        src/Mouse.cpp \
        src/numbers.cpp \
        src/persistence.cpp \
        src/progress/ProgressItem.cpp \
        src/text/symbols.cpp \
        src/text/FormattableTextArea/FormattableTextArea.cpp \
        src/text/FormattableTextArea/counter.cpp \
        src/text/FormattableTextArea/keyevents.cpp \
        src/text/FormattableTextArea/mouseevents.cpp \
        src/text/FormattableTextArea/painting.cpp \
        src/text/FormattableTextArea/props.cpp \
        src/text/TextHighlighter.cpp \
        src/text/TextIterator.cpp \
        src/text/format.cpp \
        src/progress/ProgressTracker.cpp \
        src/theming/ThemeManager.cpp \
        src/theming/Theme.cpp \
        src/colors.cpp \
        src/main.cpp

RESOURCES += qml.qrc

TRANSLATIONS += \
    resources/translations/skywriter-qt_en_US.ts

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH += $$PWD

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
#    src/document_handler.h \
    src/Mouse.h \
    src/numbers.h \
    src/persistence.h \
    src/progress/ProgressItem.h \
    src/text/FormattableTextArea/FormattableTextArea.h \
    src/text/TextHighlighter.h \
    src/text/TextIterator.h \
    src/text/format.h \
    src/progress/ProgressTracker.h \
    src/text/symbols.h \
    src/theming/ThemeManager.h \
    src/theming/Theme.h \
    src/colors.h
