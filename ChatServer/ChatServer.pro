QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17


INCLUDEPATH += $$PWD/include

SOURCES += \
    $$PWD/sources/client.cpp \
    $$PWD/sources/main.cpp \
    $$PWD/sources/mainwindow_server.cpp \
    $$PWD/sources/sql_db.cpp

HEADERS += \
    $$PWD/sources/client.h \
    $$PWD/sources/mainwindow_server.h \
    $$PWD/sources/sql_db.h

FORMS += \
    $$PWD/gui/mainwindow_server.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
