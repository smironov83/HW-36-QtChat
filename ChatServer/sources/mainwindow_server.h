#pragma once
#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QVector>
#include <QMap>
#include "client.h"
#include "sql_db.h"

namespace Ui
{
    class MainWindowServer;
}

class MainWindowServer : public QMainWindow

{
    Q_OBJECT
    Ui::MainWindowServer* _ui;
    QTcpServer* _server = new QTcpServer;
    QVector<Client*> _users;
    size_t _usersCount = 0;
    size_t _usersOnline = 0;
    size_t _usersAll = 0;
    SQLDB* _dataBase = new SQLDB;

public:
    MainWindowServer(QWidget *parent = nullptr);
    ~MainWindowServer();
    auto getClientBySocket(QTcpSocket* socket)->Client* const;
    void RemoveClient(QTcpSocket* socket);
    void UserIsOnline(QTcpSocket* socket, QString login);
    void SendMessageToAllButOne(QByteArray message, QTcpSocket* socket);
    void SendMessageToOne(QByteArray message, QTcpSocket* socket);
    void Registration(QTcpSocket* socket, QString name, QString login,
        QString password);
    void LogIn(QTcpSocket* socket, QString login, QString password);
    void LogOut(QTcpSocket* socket);
    void SendMessage(QTcpSocket* socket, QString text);
    void SendPrivateMessage(QTcpSocket* socket, QString to, QString text);
    void SendListOfUsers(QTcpSocket* socket);
    void GetChatBetweenTwoUsers(QTcpSocket* socket, QString user2);

private slots:
    void newConnect();
    void disconnect();
    void readData();
    void on_connectButton_clicked();
    void on_showChatPushButton_clicked();
    void on_disconnectUserPushButton_clicked();
    void on_banUserPushButton_clicked();
};
