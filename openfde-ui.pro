######################################################################
# Automatically generated by qmake (3.1) Fri Jan 17 16:59:42 2025
######################################################################

TEMPLATE = app
TARGET = openfde-ui
INCLUDEPATH += .
QT += widgets network dbus

# The following define makes your compiler warn you if you use any
# feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Input
HEADERS += mainwindow.h shapeButton.h download.h worker.h
SOURCES += main.cpp mainwindow.cpp shapeButton.cpp download.cpp worker.cpp
RESOURCES += openfde-ui.qrc
