#pragma once
#include <QWidget>
#include <memory>
#include <QTcpSocket>

namespace Ui
{
class RegistrationWindow;
}

class RegistrationWindow : public QWidget
{
    Q_OBJECT
    Ui::RegistrationWindow *_ui;
    QTcpSocket* _socket;
    QString _userName;
    QString _userLogin;
    QString _userPassword;

public:
    explicit RegistrationWindow(QTcpSocket* socket, QWidget *parent =
        nullptr);
    ~RegistrationWindow();

    auto getName()->QString const
    {
        return _userName;
    }

    auto getLogin()->QString const
    {
        return _userLogin;
    }

    auto getPassword()->QString const
    {
        return _userPassword;
    }

    void RegistrationResult(size_t command);

private slots:
    void closeWidget();
    void on_okButton_clicked();
};
