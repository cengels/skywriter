#!/bin/bash
# Deploys the application on Windows.
# Make sure Makefile.Release exists.
# You may have to adjust the Qt paths on your system.

cd build

# "/mnt/e/Qt/5.14.2/mingw73_64/bin/qmake.exe" ../skywriter.pro
# "/mnt/e/Qt/Tools/mingw730_64/bin/mingw32-make.exe" -f Makefile.Release
"/mnt/e/Qt/5.14.2/mingw73_64/bin/windeployqt.exe" release --qmldir ../resources/qml

cd ..
