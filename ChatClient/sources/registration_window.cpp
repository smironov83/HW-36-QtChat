#include "registration_window.h"
#include "ui_registration_window.h"
#include <QMessageBox>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

RegistrationWindow::RegistrationWindow (QTcpSocket* socket, QWidget *parent) :
    QWidget(parent), _ui(new Ui::RegistrationWindow), _socket(socket)
{
    _ui->setupUi(this);
    connect(_ui->cancelButton, &QPushButton::clicked, this, &RegistrationWindow
        ::closeWidget);
}

RegistrationWindow::~RegistrationWindow()
{
    delete _socket;
    delete _ui;
}

void RegistrationWindow::RegistrationResult(size_t command)
{
    switch (command)
    {
    case 111:
        _ui->registrationResultLabel->setText("<font color=red>User with "
            "login " + _userLogin + " not added in database! Registration "
            "failed</font>");
        break;
    case 112:
        _ui->registrationResultLabel->setText("<font color=green>User with "
            "login " + _userLogin + " add in database! Registration success"
            "</font>");
        QMessageBox(QMessageBox::Information, QObject::tr("Registration"),
            QObject::tr("Registration of a new user was successful!"),
            QMessageBox::Ok).exec();
        this->close();
        break;
    case 113:
        _ui->registrationResultLabel->setText("<font color=red>User with "
            "login " + _userLogin + " is already exists! Try again</font>");
        break;
    default:
        break;
    }
}

void RegistrationWindow::closeWidget()
{
    this->close();
}

void RegistrationWindow::on_okButton_clicked()
{
    QString message;
    if(!_ui->nameEdit->text().isEmpty() && !_ui->loginEdit->text().isEmpty()
        && !_ui->passwordEdit->text().isEmpty() && !_ui->confirmPasswordEdit->
        text().isEmpty())
    {
        if(_ui->passwordEdit->text() == _ui->confirmPasswordEdit->text())
        {
            _userName = _ui->nameEdit->text();
            _userLogin = _ui->loginEdit->text();
            _userPassword = _ui->passwordEdit->text();
            message = "110;" + _userName + ";" + _userLogin + ";" +
                _userPassword;
            _socket->write(message.toUtf8());
        }
        else
            _ui->registrationResultLabel->setText(tr("<font color=red>"
                "Passwords don't match. Try again.</font>"));
    }
    else
        _ui->registrationResultLabel->setText(tr("<font color=red>All fields "
            "must be filled in</font>"));
}
