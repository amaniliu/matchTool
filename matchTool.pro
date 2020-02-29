#-------------------------------------------------
#
# Project created by QtCreator 2020-02-26T14:57:23
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = matchTool
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

win32{
    INCLUDEPATH += $$PWD/3rdparty/opencv/include
    DEPENDPATH += $$PWD/3rdparty/opencv/include

    contains(QT_ARCH, i386) {
        message("32-bit")
        LIBS += -L$$PWD/3rdparty/opencv/x86/lib
        CONFIG(release, debug|release){
            LIBS += -lopencv_world346
            DESTDIR += bin/x86/release
        }
        CONFIG(debug, debug|release){
            LIBS += -lopencv_world346d
            DESTDIR += bin/x86/debug
        }
    }else {
        message("64-bit")
        LIBS += -L$$PWD/3rdparty/opencv/x64/lib
        CONFIG(release, debug|release){
            LIBS += -lopencv_world346
            DESTDIR += bin/x64/release
        }
        CONFIG(debug, debug|release){
            LIBS += -lopencv_world346d
            DESTDIR += bin/x64/debug
        }
    }
}

SOURCES += \
        main.cpp \
        view.cpp \
        widget.cpp

HEADERS += \
        view.h \
        widget.h

FORMS += \
        widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
