#include "login_window.h"
#include "ui_login_window.h"
#include <QMessageBox>

LoginWindow::LoginWindow(QTcpSocket* socket, QWidget *parent) : QWidget(parent)
    , _ui(new Ui::LoginWindow), _socket(socket)
{
    _ui->setupUi(this);

    connect(_ui->cancelButton, &QPushButton::clicked, this, &LoginWindow::closeWidget);
}

LoginWindow::~LoginWindow()
{
    delete _socket;
    delete _ui;
}

void LoginWindow::LoginResult(size_t command)
{
    switch (command)
    {
    case 121:
        _ui->loginResultLabel->setText(tr("<font color=green>Authentication "
            "was successful</font>"));
        QMessageBox(QMessageBox::Information, QObject::tr("Authentication"),
            QObject::tr("Authentication user was successful!"),
            QMessageBox::Ok).exec();
        closeWidget();
        break;
    case 122:
        _ui->loginResultLabel->setText(tr("<font color=red>Authentication "
            "failed. Try again!</font>"));
        break;
    case 123:
        _ui->loginResultLabel->setText(tr("<font color=red>You are already "
            "online</font>"));
        break;
    default:
        break;
    }
}

void LoginWindow::closeWidget()
{
    this->close();
}


void LoginWindow::on_okButton_clicked()
{
    QString message;
    if(!_ui->loginEdit->text().isEmpty() && !_ui->passwordEdit->text().
        isEmpty())
    {
        message = "120;" +  _ui->loginEdit->text() + ";" + _ui->passwordEdit->
            text();
        _socket->write(message.toUtf8());
    }
    else
        _ui->loginResultLabel->setText(tr("<font color=red>All fields must be "
            "filled in</font>"));
}

