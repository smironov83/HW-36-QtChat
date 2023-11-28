#include "client.h"

Client::Client(QTcpSocket* socket, QObject *parent) : QObject{parent},
    _socket(socket)
{  
}

Client::~Client()
{
     _socket->deleteLater();
}

auto Client::getSocket()->QTcpSocket* const
{
    return _socket;
}

auto Client::getPort()->int const
{
    return _socket->peerPort();
}

void Client::setName(QString name)
{
    _clientName = name;
}

auto Client::getName()->QString const
{
    return _clientName;
}

void Client::setState(bool state)
{
    _online = state;
}

auto Client::isOnline()->bool
{
    return _online;
}

auto Client::InBan()->bool
{
    return _inBan;
}

void Client::setBan(bool ban)
{
    _inBan = ban;
}
