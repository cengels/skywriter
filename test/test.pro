QT += quick quickcontrols2 quick-private concurrent testlib

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

HEADERS += \
        $$files(../src/*.h, true) \
        ../libs/md4c/src/md4c.h \
        ../libs/gtest/googletest/include/gtest/gtest.h

SOURCES += \
        $$files(../src/*.cpp, true) \
        ../libs/md4c/src/md4c.c \
        ../libs/gtest/googletest/src/gtest-all.cc \
        ../libs/gtest/googletest/src/gtest_main.cc \
        unit/FormattableTextArea/test_selection.cpp \
        unit/test_symbols.cpp
SOURCES -= ../src/main.cpp

INCLUDEPATH += ../src
INCLUDEPATH += ../libs
INCLUDEPATH += ../libs/gtest/googletest
INCLUDEPATH += ../libs/gtest/googletest/include
DEPENDPATH += ../src

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
