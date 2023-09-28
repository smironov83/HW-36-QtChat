#include "mainwindow_server.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindowServer win;
    win.setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint |
        Qt::WindowMinimizeButtonHint);
    win.show();
    return app.exec();
}
