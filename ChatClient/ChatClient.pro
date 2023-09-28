QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

INCLUDEPATH += $$PWD/include

SOURCES += \
    $$PWD/sources/login_window.cpp \
    $$PWD/sources/main.cpp \
    $$PWD/sources/mainwindow_client.cpp \
    $$PWD/sources/registration_window.cpp

HEADERS += \
    $$PWD/sources/mainwindow_client.h \
    $$PWD/sources/login_window.h \
    $$PWD/sources/registration_window.h

FORMS += \
    $$PWD/gui/mainwindow_client.ui \
    $$PWD/gui/login_window.ui \
    $$PWD/gui/registration_window.ui


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
