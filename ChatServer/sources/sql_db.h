#pragma once
#include <QObject>
#include <QtSql>
#include <QVector>
#include <QString>

constexpr auto DATA_BASE = "QODBC";
constexpr auto HOST_NAME = "localhost";
constexpr auto DATA_BASE_NAME = "PostgreSQL35W";
constexpr auto USER_NAME = "postgres";
constexpr auto PASSWORD = "123456";

class SQLDB : public QObject
{
    Q_OBJECT
    QSqlDatabase _db;
public:
    explicit SQLDB(QObject *parent = nullptr);
    ~SQLDB();
    auto OpenDB()->std::pair<bool, QString>;
    auto CreateTable1()->std::pair<bool, QString>;
    auto CreateTable2()->std::pair<bool, QString>;
    auto CreateTable3()->std::pair<bool, QString>;
    auto CheckUserByLogin(QString login)->bool;
    auto getUserId(QString login)->size_t const;
    auto getAllUsers()->QVector<QString> const;
    auto AddUser(QString name, QString login, QString password)->size_t;
    auto CheckUserByLoginAndPassword(QString login, QString password)->bool;
    auto AddMessage(QString from, QString text)->bool;
    auto AddPrivateMessage(QString from, QString to, QString text)->bool;
    auto getMessagesBetweenTwoUsers(QString user1, QString user2)
        ->QVector<QString> const;
    auto getUserById(QString id)->QString const;
    auto get20MessagesToAll()->QVector<QString> const;
};
