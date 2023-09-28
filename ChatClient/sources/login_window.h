#pragma once
#include <QWidget>
#include <memory>
#include <QTcpSocket>

namespace Ui
{
class LoginWindow;
}

class LoginWindow : public QWidget
{
    Q_OBJECT
    Ui::LoginWindow* _ui;
    QTcpSocket* _socket;

public:
    explicit LoginWindow(QTcpSocket* socket, QWidget *parent = nullptr);
    ~LoginWindow();
    void LoginResult(size_t command);

private slots:
    void closeWidget();
    void on_okButton_clicked();
};
