#include "mainwindow_server.h"
#include "ui_mainwindow_server.h"
#include <QTime>
#include <QMessageBox>
#include <QStringList>
#include <QFont>
#include <algorithm>

MainWindowServer::MainWindowServer(QWidget *parent) : QMainWindow(parent),
    _ui(new Ui::MainWindowServer)
{
    _ui->setupUi(this);
    this->setWindowTitle("Chat Server");
    _ui->portLineEdit->setText("5555");
    _server->setMaxPendingConnections(10);
    _ui->clientCountLineEdit->setText(QString::number(_users.size()));
    _usersOnline = 0;
    _ui->authUsersLineEdit->setText(QString::number(_usersOnline));
    _usersAll = 0;
    _ui->allUsersLineEdit->setText(QString::number(_usersAll));
    _ui->allUsersRadioButton->setEnabled(false);
    _ui->usersOnlineRadioButton->setEnabled(false);
    _ui->inBanRadioButton->setEnabled(false);
    _ui->banUserPushButton->setEnabled(false);
    _ui->disconnectUserPushButton->setEnabled(false);
    _ui->user1ComboBox->setEnabled(false);
    _ui->user2ComboBox->setEnabled(false);
    _ui->showChatPushButton->setEnabled(false);

    std::pair<bool, QString> result = _dataBase->OpenDB();
    if(result.first)
    {
        _ui->informationFromClientTextEdit->append(("<font color=darkGreen>%1"
            " " + result.second + "</font>").arg(QTime::currentTime()
            .toString()));
        result = _dataBase->CreateTable1();
        if(result.first)
            _ui->informationFromClientTextEdit->append(("<font color="
                "darkGreen>%1 " + result.second + "</font>").arg(QTime::
                currentTime().toString()));
        else
        {
            _ui->connectButton->setEnabled(false);
            _ui->informationFromClientTextEdit->append(("<font color="
                "darkRed>%1 " + result.second + "</font>").arg(QTime::
                currentTime().toString()));
        }
        result = _dataBase->CreateTable2();
        if(result.first)
            _ui->informationFromClientTextEdit->append(("<font color = "
                "darkGreen>%1 " + result.second + "</font>").arg(QTime::
                currentTime().toString()));
        else
        {
            _ui->connectButton->setEnabled(false);
            _ui->informationFromClientTextEdit->append(("<font color = "
                "darkRed>%1 " + result.second + "</font>").arg(QTime::
                currentTime().toString()));
        }
        result = _dataBase->CreateTable3();
        if(result.first)
            _ui->informationFromClientTextEdit->append(("<font color = "
                "darkGreen>%1 " + result.second + "</font>").arg(QTime::
                currentTime().toString()));
        else
        {
            _ui->connectButton->setEnabled(false);
            _ui->informationFromClientTextEdit->append(("<font color = "
                "darkRed>%1 " + result.second + "</font>").arg(QTime::
                currentTime().toString()));
        }
    }

    connect(_server,&QTcpServer::newConnection,this, &MainWindowServer::
        newConnect);
    connect(_ui->closeButton, &QPushButton::clicked, this, &MainWindowServer::
        close);
    connect(_ui->allUsersRadioButton, &QRadioButton::toggled, this, [this]
        (const auto on)
    {
        if(on)
        {
            QVector<QString> users = _dataBase->getAllUsers();
            for(auto& user : users)
                _ui->usersListWidget->addItem(user);
            _ui->banUserPushButton->setEnabled(false);
            _ui->disconnectUserPushButton->setEnabled(false);
            _ui->banUserPushButton->setText("Ban user");
        }
        else
            _ui->usersListWidget->clear();
    });

    connect(_ui->inBanRadioButton, &QRadioButton::toggled, this, [this]
        (const auto on)
    {
        if(on)
        {
            for(auto& user : _users)
                if(user->InBan())
                    _ui->usersListWidget->addItem(user->getName());
            _ui->banUserPushButton->setEnabled(false);
            _ui->disconnectUserPushButton->setEnabled(false);
        }
        else
            _ui->usersListWidget->clear();
    });

    connect(_ui->usersListWidget, &QListWidget::itemClicked, this, [this]
        (QListWidgetItem *item)
    {
        if(_ui->usersOnlineRadioButton->isChecked())
        {
            _ui->banUserPushButton->setEnabled(true);
            _ui->disconnectUserPushButton->setEnabled(true);
            for(auto& user : _users)
            {
                if((user->getName() == item->text()) && user->InBan())
                {
                    _ui->banUserPushButton->setText("Unban user");
                    break;
                }
                else
                    _ui->banUserPushButton->setText("Ban user");
            }
        }
        else if(_ui->inBanRadioButton->isChecked())
        {
            _ui->banUserPushButton->setEnabled(true);
            _ui->disconnectUserPushButton->setEnabled(false);
            _ui->banUserPushButton->setText("Unban user");
        }
        else
        {
            _ui->banUserPushButton->setEnabled(false);
            _ui->disconnectUserPushButton->setEnabled(false);
        }
    });

    connect(_ui->usersOnlineRadioButton, &QRadioButton::toggled, this, [this]
        (const auto on)
    {
        if(on)
        {
            for(auto& user : _users)
                if(user->Online())
                    _ui->usersListWidget->addItem(user->getName());
        }
        else
            _ui->usersListWidget->clear();
    });
}

MainWindowServer::~MainWindowServer()
{
    _server->close();
    _server->deleteLater();
    delete _ui;
}

auto MainWindowServer::getClientBySocket(QTcpSocket* socket)->Client* const
{
    for(auto& user : _users)
    {
        QTcpSocket* clientSocket = user->getSocket();
        if(socket == clientSocket)
            return user;
    }
    return nullptr;
}

void MainWindowServer::RemoveClient(QTcpSocket *socket)
{
    _users.remove(_users.indexOf(getClientBySocket(socket)));
}

void MainWindowServer::UserIsOnline(QTcpSocket* socket, QString login)
{
    if(!login.isEmpty())
    {
        for(auto& user : _users)
            if((user->getName() == login) && user->Online())
                socket->write("147;");
        socket->write("148;");
    }
}

void MainWindowServer::newConnect()
{
    QTcpSocket* clientSocket = _server->nextPendingConnection();
    Client* user = new Client(clientSocket);
    int port = user->getPort();
    connect(clientSocket, &QTcpSocket::readyRead, this, &MainWindowServer::
        readData);
    connect(clientSocket, &QTcpSocket::disconnected, this, &MainWindowServer::
        disconnect);
    _users.push_back(user);
    _ui->clientCountLineEdit->setText(QString::number(_users.size()));
    _ui->informationFromClientTextEdit->append(tr("<font color=darkGreen>%1 "
        "New client connect from port number %2</font>").arg(QTime::
        currentTime().toString()).arg(QString::number(port)));
}

void MainWindowServer::disconnect()
{
    QTcpSocket* currentSocket = qobject_cast<QTcpSocket*>(sender());
    for(auto& user : _users)
    {
        QTcpSocket* clientSocket = user->getSocket();
        if(currentSocket == clientSocket)
        {
            if(!user->getName().isEmpty())
            {
                QListWidgetItem* currentItem = _ui->usersListWidget->findItems
                    (user->getName(), Qt::MatchExactly)[0];
                if(user->Online() && _ui->usersOnlineRadioButton->isChecked())
                    delete _ui->usersListWidget->takeItem(_ui->usersListWidget
                        ->row(currentItem));
                if(user->InBan() && _ui->inBanRadioButton->isChecked())
                    delete _ui->usersListWidget->takeItem(_ui->usersListWidget
                        ->row(currentItem));
                _ui->authUsersLineEdit->setText(QString::number(--_usersOnline));
                QString message = "200;" + getClientBySocket(currentSocket)->
                    getName();
                SendMessageToAllButOne(message.toUtf8(), currentSocket);
            }
            RemoveClient(currentSocket);
            currentSocket->deleteLater();
            _ui->clientCountLineEdit->setText(QString::number(_users.size()));
            _ui->informationFromClientTextEdit->append(tr("<font color=darkRed"
                ">%1 Client from port number %2 disconnect</font>").arg(QTime::
                currentTime().toString()).arg(user->getPort()));
            break;
        }
    }
}

/* COMMANDS FROM CLIENTS
110 - Registration
120 - login
130 - message to all online clients
140 - send private message
141 - get chat between two users
145 - send list of users
146 - user is online or not
200 - logout user*/

void MainWindowServer::readData()
{
    QByteArray message;
    QTcpSocket* currentSocket = qobject_cast<QTcpSocket*>(sender());
    message =  currentSocket->readAll();
    QString str = QString(message);
    QString cmd = str.section(';',0,0);
    switch (cmd.toInt())
    {
    case 110:
        _ui->informationFromClientTextEdit->append(tr("%1 Requested command "
            "from client: Registration").arg(QTime::currentTime().toString()));
        Registration(currentSocket, str.section(';',1,1), str.section(';',2,2),
            str.section(';',3,3));
        break;
    case 120:
        _ui->informationFromClientTextEdit->append(tr("%1 Requested command "
            "from client: Login").arg(QTime::currentTime().toString()));
        LogIn(currentSocket, str.section(';',1,1), str.section(';',2,2));
        break;
    case 130:
        _ui->informationFromClientTextEdit->append(tr("%1 Requested command "
            "from client: Send message to all").arg(QTime::currentTime().
            toString()));
        _ui->allMessagesTextEdit->append(tr("%1 %2 write:").arg(QTime::
            currentTime().toString()).arg(getClientBySocket(currentSocket)->
            getName()));
        _ui->allMessagesTextEdit->append(QString("<font color=dimGray>%1"
            "</font>").arg(str.section(';',1)));
        SendMessage(currentSocket, str.section(';',1));
        break;
    case 140:
        _ui->informationFromClientTextEdit->append(tr("%1 Requested command "
            "from client: Send private message").arg(QTime::currentTime().
            toString()));
        SendPrivateMessage(currentSocket, str.section(';',1,1),
            str.section(';',2));
        break;
    case 141:
        _ui->informationFromClientTextEdit->append(tr("%1 Requested command "
            "from client: Get chat between two users").arg(QTime::currentTime()
            .toString()));
        GetChatBetweenTwoUsers(currentSocket,str.section(';',1));
        break;
    case 145:
        _ui->informationFromClientTextEdit->append(tr("%1 Requested command "
            "from client: Send list of users").arg(QTime::currentTime().
            toString()));
        SendListOfUsers(currentSocket);
        break;
    case 146:
        _ui->informationFromClientTextEdit->append(tr("%1 Requested command "
            "from client: User is online or not").arg(QTime::currentTime().
            toString()));
        UserIsOnline(currentSocket,str.section(';',1));
        break;
    case 200:
        _ui->informationFromClientTextEdit->append(tr("%1 Requested command "
            "from client: Logout").arg(QTime::currentTime().toString()));
        LogOut(currentSocket);
        break;
    default:
        break;
    }
}

void MainWindowServer::SendMessageToAllButOne(QByteArray message, QTcpSocket*
    socket)
{
    for (auto i = 0; i < _users.size(); ++i)
    {
        QTcpSocket* clientSocket = _users.at(i)->getSocket();
        if(socket != clientSocket && _users.at(i)->Online())
            clientSocket->write(message);
    }
}

void MainWindowServer::SendMessageToOne(QByteArray message, QTcpSocket *socket)
{
    for (auto i = 0; i < _users.size(); ++i)
    {
        QTcpSocket* clientSocket = _users.at(i)->getSocket();
        if(socket == clientSocket)
            clientSocket->write(message);
    }
}

void MainWindowServer::Registration(QTcpSocket* socket, QString name, QString
    login, QString password)
{
    size_t res = _dataBase->AddUser(name, login, password);
    QString message;
    switch (res)
    {
    case 0:
        message = QTime::currentTime().toString() + " User with login " + login
            + " not added in database! Registration failed...";
        _ui->informationFromClientTextEdit->append(message);
        socket->write("111");
        break;
    case 1:
        message = QTime::currentTime().toString() + " User with login " + login
            + " add in database! Registration success...";
        getClientBySocket(socket)->setState(true);
        getClientBySocket(socket)->setName(login);
        _usersOnline++;
        _ui->authUsersLineEdit->setText(QString::number(_usersOnline));
        _ui->allUsersLineEdit->setText(QString::number((_ui->allUsersLineEdit->
            text().toInt())+1));
        _ui->informationFromClientTextEdit->append(message);

        if(_ui->allUsersRadioButton->isChecked() || _ui->usersOnlineRadioButton
            ->isChecked())
            _ui->usersListWidget->addItem(login);
        _ui->user1ComboBox->addItem(login);
        _ui->user2ComboBox->addItem(login);
        message = "112;" + login + ";";
        for(auto& user : _users)
            if(user->Online())
                message += user->getName() + ",";
        message.resize(message.size()-1);
        socket->write(message.toUtf8());
        message = "100;" + login;
        SendMessageToAllButOne(message.toUtf8(), socket);
        break;
    case 2:
        message = QTime::currentTime().toString() + " User with login " + login
            + " is already exists";
        _ui->informationFromClientTextEdit->append(message);
        socket->write("113");
        break;
    default:
        break;
    }
}

void MainWindowServer::LogIn(QTcpSocket *socket, QString login, QString
    password)
{
    bool res = _dataBase->CheckUserByLoginAndPassword(login, password);
    bool isAlreadyOnline = false;
    for(auto& user : _users)
        if((user->getName() == login) && user->Online())
            isAlreadyOnline = true;
    QString message;
    if(res && !isAlreadyOnline)
    {
        message = QTime::currentTime().toString() + " The user with login " +
            login + " was authenticated";
        getClientBySocket(socket)->setState(true);
        getClientBySocket(socket)->setName(login);
        ++_usersOnline;
        _ui->authUsersLineEdit->setText(QString::number(_usersOnline));
        _ui->informationFromClientTextEdit->append(message);
        if(_ui->usersOnlineRadioButton->isChecked())
            _ui->usersListWidget->addItem(login);
        message = "121;" + login + ";";
        for(auto& user : _users)
            if(user->Online())
                message += user->getName() + ",";
        message.resize(message.size()-1);
        socket->write(message.toUtf8());
        message = "100;" + login;
        SendMessageToAllButOne(message.toUtf8(), socket);
    }
    else if(isAlreadyOnline)
    {
        message = QTime::currentTime().toString() + " The user with login " +
            login + " is already online";
        _ui->informationFromClientTextEdit->append(message);
        socket->write("123;");
    }
    else
    {
        message = QTime::currentTime().toString() + " The user with login " +
            login + " was not authenticated";
        _ui->informationFromClientTextEdit->append(message);
        socket->write("122");
    }
}

void MainWindowServer::LogOut(QTcpSocket *socket)
{
    _ui->informationFromClientTextEdit->append(tr("<font color=darkRed>%1 The "
        "user with login %2 logged out the chat</font>").arg(QTime::
        currentTime().toString()).arg(getClientBySocket(socket)->getName()));
    _ui->authUsersLineEdit->setText(QString::number(--_usersOnline));
    if(_ui->usersOnlineRadioButton->isChecked())
    {
        QListWidgetItem* currentItem = _ui->usersListWidget->findItems
            (getClientBySocket(socket)->getName(), Qt::MatchExactly)[0];
        delete _ui->usersListWidget->takeItem(_ui->usersListWidget->
            row(currentItem));
    }
    QString message = "200;" + getClientBySocket(socket)->getName();
    SendMessageToAllButOne(message.toUtf8(), socket);
    socket->write("500;3");
    getClientBySocket(socket)->setName("");
    getClientBySocket(socket)->setState(false);
}

void MainWindowServer::SendMessage(QTcpSocket *socket, QString text)
{
    QString message;
    bool res = _dataBase->AddMessage(getClientBySocket(socket)->getName(),
        text);
    if(res)
    {
        for (auto i = 0; i < _users.size(); ++i)
        {
            QTcpSocket* onlineClientSocket = _users.at(i)->getSocket();
            if(socket != onlineClientSocket && _users.at(i)->Online())
            {
                message = "131;" + getClientBySocket(socket)->getName() + ";" +
                    text;
                onlineClientSocket->write(message.toUtf8());
            }
        }
        socket->write("500;1");
    }
    else
        socket->write("500;4");
}

void MainWindowServer::SendPrivateMessage(QTcpSocket *socket, QString to,
    QString text)
{
    QString message;
    bool res = _dataBase->AddPrivateMessage(getClientBySocket(socket)->
        getName(), to, text);
    if(res)
    {
        for (auto& user : _users)
            if(user->getName() == to)
            {
                message = "141;" + getClientBySocket(socket)->getName() + ";" +
                    text;
                user->getSocket()->write(message.toUtf8());
            }
        socket->write("500;2");
    }
    else
        socket->write("500;4");
}

void MainWindowServer::SendListOfUsers(QTcpSocket *socket)
{
    QVector<QString> users = _dataBase->getAllUsers();
    QString str;
    for (auto& user : users)
        if(user != getClientBySocket(socket)->getName())
            str += user + ",";
    str.resize(str.size()-1);
    QString message = "146;" + str;
    socket->write(message.toUtf8());
}

void MainWindowServer::GetChatBetweenTwoUsers(QTcpSocket *socket,
    QString user2)
{
    QVector<QString> messages = _dataBase->getMessagesBetweenTwoUsers
        (getClientBySocket(socket)->getName(),user2);
    QString str;
    if(messages.size()>0)
    {
        for(auto& mes : messages)
            str += mes + "***";
        str.resize(str.size()-3);
        QString message = "142;" + str;
        socket->write(message.toUtf8());
    }
    else
        socket->write("142;");
}

void MainWindowServer::on_connectButton_clicked()
{
    if(_ui->connectButton->text() == QString("Connect"))
    {
        qint16 port = _ui->portLineEdit->text().toInt();
        if(!_server->listen(QHostAddress::Any, port))
        {
            QMessageBox(QMessageBox::Critical, QObject::tr("Error"),
                _server->errorString(), QMessageBox::Ok).exec();
            return;
        }
        _ui->connectButton->setText("Disconnect");
        _ui->portLineEdit->setEnabled(false);
        _ui->allUsersRadioButton->setEnabled(true);
        _ui->usersOnlineRadioButton->setEnabled(true);
        _ui->inBanRadioButton->setEnabled(true);
        _ui->user1ComboBox->setEnabled(true);
        _ui->user2ComboBox->setEnabled(true);
        _ui->showChatPushButton->setEnabled(true);
        _ui->allUsersLineEdit->setText(QString::number(_dataBase->getAllUsers()
            .size()));
        QVector<QString> users = _dataBase->getAllUsers();
        for(auto& user : users)
        {
            _ui->usersListWidget->addItem(user);
            _ui->user1ComboBox->addItem(user);
            _ui->user2ComboBox->addItem(user);
        }
        _ui->user1ComboBox->setCurrentIndex(-1);
        _ui->user2ComboBox->setCurrentIndex(-1);
        QVector<QString> messages = _dataBase->get20MessagesToAll();
        QString user1, text;
        for(const auto& message : messages)
        {
            user1 = message.section(';',0,0);
            text = message.section(';',1);
            _ui->allMessagesTextEdit->append(tr("%1 write:").arg(user1));
            _ui->allMessagesTextEdit->append(QString("<font color=gray>%1"
                "</font>").arg(text));
        }
    }
    else
    {
        for(auto& user : _users)
            if(user->getSocket()->state() == QAbstractSocket::ConnectedState)
                user->getSocket()->disconnectFromHost();
        _server->close();
        _ui->connectButton->setText("Connect");
        _ui->portLineEdit->setEnabled(true);
        _ui->allUsersRadioButton->setEnabled(false);
        _ui->usersOnlineRadioButton->setEnabled(false);
        _ui->user1ComboBox->setEnabled(false);
        _ui->user2ComboBox->setEnabled(false);
        _ui->showChatPushButton->setEnabled(false);
        _ui->allUsersRadioButton->setChecked(true);
        _ui->banUserPushButton->setEnabled(false);
        _ui->disconnectUserPushButton->setEnabled(false);
        _ui->user1ComboBox->clear();
        _ui->user2ComboBox->clear();
        _ui->usersListWidget->clear();
        _ui->allMessagesTextEdit->clear();
        _ui->privateMessagesTextEdit->clear();
        _ui->informationFromClientTextEdit->clear();
    }
}

void MainWindowServer::on_showChatPushButton_clicked()
{
    _ui->privateMessagesTextEdit->clear();
    QString sender, reciever, text;
    QString user1 = _ui->user1ComboBox->currentText();
    QString user2 = _ui->user2ComboBox->currentText();
    QVector<QString> messages = _dataBase->getMessagesBetweenTwoUsers(user1,
        user2);
    if(messages.isEmpty())
        _ui->privateMessagesTextEdit->append("No messages between users!");
    else
        for(const auto& message : messages)
        {
            sender = message.section(';',0,0);
            reciever = message.section(';',1,1);
            text = message.section(';',2);
            _ui->privateMessagesTextEdit->append(tr("%1 write to %2:").
                arg(sender).arg(reciever));
            _ui->privateMessagesTextEdit->append(QString("<font color=gray>%1"
                "</font>").arg(text));
        }
}

void MainWindowServer::on_disconnectUserPushButton_clicked()
{
    QListWidgetItem* item = _ui->usersListWidget->currentItem();
    QString client = item->text();
    for(auto& user : _users)
    {
        if((user->getName() == client) && user->Online())
        {
            QString message = "200;" + user->getName();
            SendMessageToAllButOne(message.toUtf8(), user->getSocket());
            user->getSocket()->deleteLater();
            RemoveClient(user->getSocket());
            QListWidgetItem* currentItem = _ui->usersListWidget->findItems
                (client, Qt::MatchExactly)[0];
            delete _ui->usersListWidget->takeItem(_ui->usersListWidget->row
                (currentItem));
            _ui->authUsersLineEdit->setText(QString::number(--_usersOnline));
            _ui->clientCountLineEdit->setText(QString::number(_users.size()));
            _ui->informationFromClientTextEdit->append(tr("<font color=darkRed"
                ">%1 Client from port number %2 disconnect</font>").arg(QTime::
                currentTime().toString()).arg(user->getPort()));
            break;
        }
    }
}

void MainWindowServer::on_banUserPushButton_clicked()
{
    QListWidgetItem* item = _ui->usersListWidget->currentItem();
    QString client = item->text();
    for(auto& user : _users)
        if(user->getName() == client)
        {
            if(_ui->banUserPushButton->text() == QString("Ban user"))
            {
                _ui->banUserPushButton->setText("Unban user");
                user->setBan(true);
                QString message = "300;";
                SendMessageToOne(message.toUtf8(), user->getSocket());
                message = "301;" + user->getName();
                SendMessageToAllButOne(message.toUtf8(), user->getSocket());
                _ui->informationFromClientTextEdit->append(tr("<font color="
                    "darkRed>%1 Client with login %2 sent to ban</font>").arg
                    (QTime::currentTime().toString()).arg(user->getName()));
                break;
            }
            else
            {
                _ui->banUserPushButton->setText("Ban user");
                user->setBan(false);
                QString message;
                _ui->informationFromClientTextEdit->append(tr("<font color="
                    "darkGreen>%1 Client with login %2 unban</font>").arg(QTime
                    ::currentTime().toString()).arg(user->getName()));
                message = "302;" + user->getName() + ";";
                for(auto& us : _users)
                {
                    if(us->Online())
                        message += us->getName() + ",";
                }
                message.resize(message.size()-1);
                user->getSocket()->write(message.toUtf8());
                message = "303;" + user->getName();
                SendMessageToAllButOne(message.toUtf8(), user->getSocket());
                if(_ui->inBanRadioButton->isChecked())
                {
                    QListWidgetItem* currentItem = _ui->usersListWidget->
                        findItems(client, Qt::MatchExactly)[0];
                    delete _ui->usersListWidget->takeItem(_ui->usersListWidget
                        ->row(currentItem));
                }
            }
        }
}
