#include "mainwindow_client.h"
#include "ui_mainwindow_client.h"
#include <QMessageBox>
#include <QTime>
#include <QStringList>
#include <QtSql/QSqlDatabase>

MainWindowClient::MainWindowClient(QWidget *parent) : QMainWindow(parent),
    _ui(new Ui::MainWindowClient)
{
    _ui->setupUi(this);
    this->setWindowTitle("Chat Client");
    _ui->ipLineEdit->setText("127.0.0.1");
    _ui->portLineEdit->setText("5555");
    _ui->messageToAllRadioButton->setEnabled(false);
    _ui->privateMessageRadioButton->setEnabled(false);
    _ui->privateMessageFromRadioButton->setEnabled(false);
    _ui->allPrivateMessagesRadioButton->setEnabled(false);
    _ui->privateMessageRadioButton->setChecked(false);
    _ui->privateMessageFromRadioButton->setChecked(false);
    _ui->loginButton->setEnabled(false);
    _ui->registrationButton->setEnabled(false);
    _ui->exitUserButton->setEnabled(false);
    _ui->messageLabel->setEnabled(false);
    _ui->messageLineEdit->setEnabled(false);
    _ui->sendButton->setEnabled(false);
    _ui->currentUserLabel->setText("<font color=red>Current user:</font>");
    _ui->chatWithTextEdit->hide();

    connect(_ui->registrationButton, &QPushButton::clicked, this,
        &MainWindowClient::openRegistrationWidget);
    connect(_ui->loginButton, &QPushButton::clicked, this, &MainWindowClient::openLoginWidget);
    connect(_socket,&QTcpSocket::readyRead,this,&MainWindowClient::readData);
    connect(_socket, &QTcpSocket::disconnected, this, &MainWindowClient::
        disconnect);
    connect(_ui->closeButton, &QPushButton::clicked, this, &MainWindowClient::
        close);
    connect(_ui->exitUserButton, &QPushButton::clicked, this, &MainWindowClient
        ::exitUser);
    auto* const usersCB = _ui->usersComboBox;
    connect(_ui->privateMessageRadioButton, &QRadioButton::toggled, this,
        [this, usersCB](const auto on)
    {
        if(on)
        {
            usersCB->setEnabled(true);
            usersCB->setCurrentIndex(-1);
        }
        else
        {
            usersCB->setEnabled(false);
            usersCB->setCurrentIndex(-1);
            usersCB->setCurrentText("");
            _ui->usersStateLabel->setText("");
        }
    });
    connect(usersCB, &QComboBox::currentTextChanged, this, [this,usersCB]
        (QString login)
    {
        if(usersCB->currentIndex() != -1 && _ui->privateMessageRadioButton->
            isChecked())
        {
            QString message = "146;" + login;
            _socket->write(message.toUtf8());
        }
    });
    auto* const usersCB_private = _ui->privateUsersComboBox;
    connect(_ui->privateMessageFromRadioButton, &QRadioButton::toggled, this,
        [this, usersCB_private](const auto on)
    {
        if(on)
        {
            usersCB_private->setEnabled(true);
            usersCB_private->setCurrentIndex(-1);
            _ui->chatWithTextEdit->show();
            _ui->chatWithTextEdit->clear();
        }
        else
        {
            usersCB_private->setCurrentIndex(-1);
            usersCB_private->setCurrentText("");
            usersCB_private->setEnabled(false);
            _ui->chatWithTextEdit->hide();
        }
    });
    connect(usersCB_private, &QComboBox::currentTextChanged, this,
        [this, usersCB_private](QString login)
    {
        _ui->chatWithTextEdit->clear();
        if(usersCB_private->currentIndex() != -1 && _ui->
            privateMessageFromRadioButton->isChecked())
        {
            QString message = "141;" + login;
            _socket->write(message.toUtf8());
        }
    });
}

MainWindowClient::~MainWindowClient()
{
    delete _socket;
    delete _ui;
}

auto MainWindowClient::getSocket()->QTcpSocket* const
{
    return _socket;
}

auto MainWindowClient::getCurLogin()->QString const
{
    return _curLogin;
}

void MainWindowClient::setCurLogin(const QString& newCurLogin)
{
    _curLogin = newCurLogin;
}

void MainWindowClient::EnterUserInChat(const QString& login, const QString&
    users)
{
    QStringList lst;
    _online = true;
    setCurLogin(login);
    lst = users.split(",");
    _ui->usersOnlineListWidget->clear();
    for(auto& l : lst)
        _ui->usersOnlineListWidget->addItem(l);
    _ui->loginButton->setEnabled(false);
    _ui->registrationButton->setEnabled(false);
    _ui->exitUserButton->setEnabled(true);
    _ui->currentUserLabel->setText(QString("<font color=green>Current user: %1"
        "</font>").arg(login));
    _ui->messageToAllRadioButton->setEnabled(true);
    _ui->messageToAllRadioButton->setChecked(true);
    _ui->privateMessageRadioButton->setEnabled(true);
    _ui->privateMessageFromRadioButton->setEnabled(true);
    _ui->allPrivateMessagesRadioButton->setEnabled(true);
    _ui->allPrivateMessagesRadioButton->setChecked(true);
    _ui->privateUsersComboBox->setEnabled(false);
    _ui->usersComboBox->setEnabled(false);
    _ui->messageLabel->setEnabled(true);
    _ui->messageLineEdit->setEnabled(true);
    _ui->sendButton->setEnabled(true);
    _socket->write("145;");
}

void MainWindowClient::BanUser()
{
    _ui->currentUserLabel->setText(QString("<font color=red>Current user: %1 "
        "(in ban)</font>").arg(getCurLogin()));
    _ui->messageToAllRadioButton->setChecked(true);
    _ui->messageToAllRadioButton->setEnabled(false);
    _ui->privateMessageRadioButton->setEnabled(false);
    _ui->allPrivateMessagesRadioButton->setChecked(true);
    _ui->allPrivateMessagesRadioButton->setEnabled(false);
    _ui->privateMessageFromRadioButton->setEnabled(false);
    _ui->privateUsersComboBox->setEnabled(false);
    _ui->messageLabel->setEnabled(false);
    _ui->messageLineEdit->setEnabled(false);
    _ui->sendButton->setEnabled(false);
}

void MainWindowClient::exitUser()
{
    _online = false;
    setCurLogin("");
    _ui->allMessagesTextEdit->clear();
    _ui->privateMessagesTextEdit->clear();
    _ui->usersOnlineListWidget->clear();
    _ui->loginButton->setEnabled(true);
    _ui->registrationButton->setEnabled(true);
    _ui->exitUserButton->setEnabled(false);
    _ui->currentUserLabel->setText("<font color=red>Current user: </font>");
    _ui->messageToAllRadioButton->setChecked(true);
    _ui->messageToAllRadioButton->setEnabled(false);
    _ui->privateMessageRadioButton->setEnabled(false);
    _ui->allPrivateMessagesRadioButton->setChecked(true);
    _ui->allPrivateMessagesRadioButton->setEnabled(false);
    _ui->privateMessageFromRadioButton->setEnabled(false);
    _ui->privateUsersComboBox->setEnabled(false);
    _ui->messageLabel->setEnabled(false);
    _ui->messageLineEdit->setEnabled(false);
    _ui->sendButton->setEnabled(false);
    _socket->write("200");
}

void MainWindowClient::AddUsersInCombobox(QString str)
{
    QStringList users = str.split(",");
    _ui->usersComboBox->clear();
    _ui->privateUsersComboBox->clear();
    _ui->usersComboBox->addItems(users);
    _ui->usersComboBox->setCurrentIndex(-1);
    _ui->usersComboBox->setCurrentText("");
    _ui->privateUsersComboBox->addItems(users);
    _ui->privateUsersComboBox->setCurrentIndex(-1);
    _ui->privateUsersComboBox->setCurrentText("");
}

void MainWindowClient::ReceiveMessagesBetweenTwoUsers(QString str)
{
    if(_ui->privateUsersComboBox->currentIndex() != -1)
    {
        if(str.isEmpty())
        {
            QMessageBox msgBox;
            msgBox.setWindowTitle("Private messages");
            msgBox.setText("No messages with selected user");
            msgBox.resize(60,40);
            msgBox.exec();
        }
        else
        {
            QStringList messages = str.split("***");
            QString user1, user2, text;
            for(auto& message : messages)
            {
                user1 = message.section(';',0,0);
                user2 = message.section(';',1,1);
                text = message.section(';',2);
                _ui->chatWithTextEdit->append(tr("%1 write to %2:").arg(user1).
                    arg(user2));
                _ui->chatWithTextEdit->append(QString("<font color=gray>%1"
                    "</font>").arg(text));
            }
        }
    }
}

void MainWindowClient::ReceiveCommandMessageFromServer(QString cmd)
{
    switch(cmd.toInt())
    {
    case 1:
        QMessageBox(QMessageBox::Information, QObject::tr("Message"), QObject::
            tr("Message was send!"), QMessageBox::Ok).exec();
        break;
    case 2:
        QMessageBox(QMessageBox::Information, QObject::tr("Message"), QObject::
                    tr("Private message was send!"), QMessageBox::Ok).exec();
        break;
    case 3:
        QMessageBox(QMessageBox::Information, QObject::tr("Logout user"),
            QObject::tr("You have logged out of the chat"), QMessageBox::Ok).
            exec();
        break;
    case 4:
        QMessageBox(QMessageBox::Critical, QObject::tr("Error"), QObject::tr
            ("Error send message"), QMessageBox::Ok).exec();
        break;
    default:
        break;
    }
}

void MainWindowClient::disconnect()
{
    _ui->sendButton->setEnabled(false);
    _ui->connectButton->setText("Connect");
    QMessageBox msgBox;
    msgBox.setWindowTitle("CAUTION");
    msgBox.setText("You are disconnect from server!");
    msgBox.resize(60,30);
    msgBox.exec();
    _ui->loginButton->setEnabled(false);
    _ui->registrationButton->setEnabled(false);
    _ui->exitUserButton->setEnabled(false);
    _ui->messageToAllRadioButton->setEnabled(false);
    _ui->messageToAllRadioButton->setChecked(true);
    _ui->privateMessageRadioButton->setEnabled(false);
    _ui->allPrivateMessagesRadioButton->setEnabled(false);
    _ui->allPrivateMessagesRadioButton->setChecked(true);
    _ui->privateMessageFromRadioButton->setEnabled(false);
    _ui->messageLabel->setEnabled(false);
    _ui->messageLineEdit->setEnabled(false);
    _ui->sendButton->setEnabled(false);
    _ui->allMessagesTextEdit->clear();
    _ui->privateMessagesTextEdit->clear();
    _ui->usersOnlineListWidget->clear();
    _ui->currentUserLabel->setText("<font color=red>Current user:</font>");
    _ui->ipLineEdit->setEnabled(true);
    _ui->portLineEdit->setEnabled(true);
}

void MainWindowClient::openLoginWidget()
{
    _loginWindow = new LoginWindow(_socket);
    _loginWindow->setWindowModality(Qt::ApplicationModal);
    _loginWindow->setWindowFlags(Qt::CustomizeWindowHint |
        Qt::WindowTitleHint);
    _loginWindow->show();
}

void MainWindowClient::openRegistrationWidget()
{
    _registrationWindow = new RegistrationWindow(_socket);
    _registrationWindow->setWindowModality(Qt::ApplicationModal);
    _registrationWindow->setWindowFlags(Qt::CustomizeWindowHint |
        Qt::WindowTitleHint);
    _registrationWindow->show();
}

void MainWindowClient::on_connectButton_clicked()
{
    QString ip;
    qint16 port;
    if (_ui->connectButton->text() == QString("Connect"))
    {
        ip=_ui->ipLineEdit->text();
        port=_ui->portLineEdit->text().toInt();
        _socket->abort();
        _socket->connectToHost(ip,port);
        if (!_socket->waitForConnected())
        {
            QMessageBox msgBox;
            msgBox.setWindowTitle("Client");
            msgBox.setText("Runtime Out");
            msgBox.resize(50,30);
            msgBox.exec();
            return;
        }
        QMessageBox msgBox;
        msgBox.setWindowTitle("Client");
        msgBox.setText("Successful Connection");
        msgBox.resize(50,40);
        msgBox.exec();
        _ui->connectButton->setText("Disconnect");
        _ui->loginButton->setEnabled(true);
        _ui->registrationButton->setEnabled(true);
        _ui->exitUserButton->setEnabled(false);
        _ui->ipLineEdit->setEnabled(false);
        _ui->portLineEdit->setEnabled(false);
    }
    else
    {
        _socket->disconnectFromHost();
        _ui->connectButton->setText("Connect");
        _ui->loginButton->setEnabled(false);
        _ui->registrationButton->setEnabled(false);
        _ui->exitUserButton->setEnabled(false);
        _ui->messageToAllRadioButton->setEnabled(false);
        _ui->messageToAllRadioButton->setChecked(true);
        _ui->privateMessageRadioButton->setEnabled(false);
        _ui->allPrivateMessagesRadioButton->setEnabled(false);
        _ui->allPrivateMessagesRadioButton->setChecked(true);
        _ui->privateMessageFromRadioButton->setEnabled(false);
        _ui->messageLabel->setEnabled(false);
        _ui->messageLineEdit->setEnabled(false);
        _ui->sendButton->setEnabled(false);
        _ui->allMessagesTextEdit->clear();
        _ui->privateMessagesTextEdit->clear();
        _ui->usersOnlineListWidget->clear();
        _ui->currentUserLabel->setText("<font color=red>Current user:</font>");
        _ui->ipLineEdit->setEnabled(true);
        _ui->portLineEdit->setEnabled(true);
    }
}

void MainWindowClient::on_sendButton_clicked()
{
    QString message;
    if(_ui->messageToAllRadioButton->isChecked())
    {
        if (_ui->messageLineEdit->text().isEmpty())
        {
            QMessageBox msgb;
            msgb.setText("Can't Send Empty Message");
            msgb.resize(60, 40);
            msgb.exec();
            return;
        }
        _ui->allMessagesTextEdit->append(tr("<font color=darkSlateBlue>%1 From"
            " Me:</font>").arg(QTime::currentTime().toString()));
        _ui->allMessagesTextEdit->append(QString("<font color=grey>%1</font>").
            arg(_ui->messageLineEdit->text()));
        message = "130;" + _ui->messageLineEdit->text();
        _socket->write(message.toUtf8());
        _socket->flush();
        _ui->messageLineEdit->clear();
        _ui->messageLineEdit->setFocus();
    }
    if(_ui->privateMessageRadioButton->isChecked())
    {
        QString reciever = _ui->usersComboBox->currentText();
        if(_ui->messageLineEdit->text().isEmpty())
        {
            QMessageBox msgb;
            msgb.setText("Can't Send Empty Message");
            msgb.resize(60, 40);
            msgb.exec();
            return;
        }
        if(_ui->usersComboBox->currentIndex() == -1)
        {
            QMessageBox msgb;
            msgb.setText("You need to select a user!");
            msgb.resize(60, 40);
            msgb.exec();
            return;
        }
        if(_ui->privateMessageFromRadioButton->isChecked() && _ui->
            privateUsersComboBox->currentText() == reciever)
        {
            _ui->chatWithTextEdit->append(tr("%1 write to %2:").
                arg(getCurLogin()).arg(reciever));
            _ui->chatWithTextEdit->append(QString("<font color=gray>%1</font>")
                .arg(_ui->messageLineEdit->text()));
        }
        message = "140;" + reciever + ";" + _ui->messageLineEdit->text();
        _socket->write(message.toUtf8());
        _socket->flush();
        _ui->messageLineEdit->clear();
        _ui->messageLineEdit->setFocus();
    }
}

/* COMMANDS FROM SERVER
100 - new user join to chat
111 - registration (user not addet in database)
112 - registration (user add in database)
113 - registration (user is already exists)
121 - login (Authentication success)
122 - login (Authentication failed)
123 - login (user already online)
131 - send message to all online clients
141 - private message
142 - receive messages between two users
146 - list of users add in combobox
147-148 - user's state for private message
200 - user left chat
300 - current user in ban
301 - user in ban
302 - current user unban
303 - user unban
400 - current user kicked
500 - command messages*/

void MainWindowClient::readData()
{
    QByteArray message;
    message = _socket->readAll();
    QString str = QString(message);
    QString cmd = str.section(';',0,0);
    int ind;
    QListWidgetItem* currentItem;
    switch (cmd.toInt())
    {
    case 100:
        _ui->allMessagesTextEdit->append(tr("<font color=green>%1 %2 join to "
            "Chat!</font>").arg(QTime::currentTime().toString()).
            arg(str.section(';',1)));
        _ui->usersOnlineListWidget->addItem(str.section(';',1));
        ind = _ui->usersComboBox->findText(str.section(';',1),
            Qt::MatchExactly);
        if(ind == -1)
            _ui->usersComboBox->addItem(str.section(';',1));
        ind = _ui->privateUsersComboBox->findText(str.section(';',1),
            Qt::MatchExactly);
        if(ind == -1)
            _ui->privateUsersComboBox->addItem(str.section(';',1));
        break;
    case 111:
        _registrationWindow->RegistrationResult(111);
        break;
    case 112:
        _registrationWindow->RegistrationResult(112);
        EnterUserInChat(str.section(';',1,1),str.section(';',2));
        break;
    case 113:
        _registrationWindow->RegistrationResult(113);
        break;
    case 121:
        _loginWindow->LoginResult(121);
        EnterUserInChat(str.section(';',1,1),str.section(';',2));
        break;
    case 122:
        _loginWindow->LoginResult(122);
        break;
    case 123:
        _loginWindow->LoginResult(123);
        break;
    case 131:
        _ui->allMessagesTextEdit->append(tr("%1 %2 write:").arg(QTime::
            currentTime().toString()).arg(str.section(';',1,1)));
        _ui->allMessagesTextEdit->append(QString("<font color=grey>%1</font>").
            arg(str.section(';',2)));
        break;
    case 141:
        if(_ui->privateMessageFromRadioButton->isChecked() && _ui->
            privateUsersComboBox->currentText() == str.section(";",1,1))
        {
            _ui->chatWithTextEdit->append(tr("%1 write to %2:")
                .arg(str.section(";",1,1)).arg(getCurLogin()));
            _ui->chatWithTextEdit->append(QString("<font color=gray>%1</font>")
                .arg(str.section(';',2)));
        }
        _ui->privateMessagesTextEdit->append(tr("%1 Message from %2:").arg
            (QTime::currentTime().toString()).arg(str.section(";",1,1)));
        _ui->privateMessagesTextEdit->append(QString("<font color=grey>%1"
            "</font>").arg(str.section(';',2)));
        break;
    case 142:
        ReceiveMessagesBetweenTwoUsers(str.section(';',1));
        break;
    case 146:
        AddUsersInCombobox(str.section(';',1));
        break;
    case 147:
        if(_ui->usersComboBox->currentIndex() != -1)
            _ui->usersStateLabel->setText("<font color=green>User online"
                "</font>");
        break;
    case 148:
        if(_ui->usersComboBox->currentIndex() != -1)
            _ui->usersStateLabel->setText("<font color=red>User offline"
                "</font>");
        break;
    case 200:
        _ui->allMessagesTextEdit->append(QString("<font color=red>%1 %2 left "
            "the Chat!</font>").arg(QTime::currentTime().toString())
            .arg(str.section(';',1)));
        currentItem = _ui->usersOnlineListWidget->findItems(str.section(';',1),
            Qt::MatchExactly)[0];
        delete _ui->usersOnlineListWidget->takeItem(_ui->usersOnlineListWidget
            ->row(currentItem));
        break;
    case 300:
        _ui->allMessagesTextEdit->append(QString("<font color=red>%1 You are "
            "in BAN!</font>").arg(QTime::currentTime().toString()));
        BanUser();
        break;
    case 301:
        _ui->allMessagesTextEdit->append(QString("<font color=red>%1 User with"
            " login %2 sent to ban!</font>").arg(QTime::currentTime()
            .toString()).arg(str.section(';',1)));
        break;
    case 302:
        _ui->allMessagesTextEdit->append(QString("<font color=green>%1 You are"
            " UNBAN!</font>").arg(QTime::currentTime().toString()));
        EnterUserInChat(str.section(';',1,1),str.section(';',2));
        break;
    case 303:
        _ui->allMessagesTextEdit->append(QString("<font color=green>%1 User "
            "with login %2 Unban!</font>").arg(QTime::currentTime().toString()).arg(str.section(';',1)));
        break;
    case 400:
        _ui->allMessagesTextEdit->append(QString("<font color=red>%1 You are kicked!</font>").arg(QTime::currentTime().toString()));
        break;
    case 500:
        ReceiveCommandMessageFromServer(str.section(';',1));
        break;
    default:
        break;
    }
}
