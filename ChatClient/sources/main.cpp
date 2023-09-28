#include "mainwindow_client.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindowClient win;
    win.setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint |
        Qt::WindowMinimizeButtonHint);
    win.show();
    return app.exec();
}
