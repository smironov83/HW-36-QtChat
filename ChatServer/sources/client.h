#pragma once
#include <QObject>
#include <QTcpSocket>
#include <QString>

class Client : public QObject
{
    Q_OBJECT
    QTcpSocket* _socket;
    QString _clientName;
    bool _online = false;
    bool _inBan = false;
public:
    explicit Client(QTcpSocket* socket, QObject *parent = nullptr);
    ~Client();
    auto getSocket()->QTcpSocket* const;
    auto getPort()->int const;
    void setName(QString name);
    auto getName()->QString const;
    void setState(bool state);
    auto Online()->bool;
    auto InBan()->bool;
    void setBan(bool ban);
};
