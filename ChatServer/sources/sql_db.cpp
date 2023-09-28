#include "sql_db.h"
#include <QMessageBox>

SQLDB::SQLDB(QObject *parent) : QObject{parent}
{
    _db = QSqlDatabase::addDatabase(DATA_BASE);
    _db.setHostName(HOST_NAME);
    _db.setDatabaseName(DATA_BASE_NAME);
    _db.setUserName(USER_NAME);
    _db.setPassword(PASSWORD);
}

SQLDB::~SQLDB()
{
    _db.close();
}

auto SQLDB::OpenDB()->std::pair<bool, QString>
{
    if(!_db.open())
    {
        QMessageBox(QMessageBox::Warning, QObject::tr("Error"),
            _db.lastError().text(), QMessageBox::Ok).exec();
        return std::make_pair(false, _db.lastError().text());
    }
    else
        return std::make_pair(true, "The Connection to the database was "
            "successful!");
}

auto SQLDB::CreateTable1()->std::pair<bool, QString>
{
    QSqlQuery query;
    if(!query.exec("create table if not exists users(id serial primary key, "
        "name varchar(255) not null, login varchar(255) not null, password "
        "varchar(255) not null);"))
        return std::make_pair(false, query.lastError().text());
    else
        return std::make_pair(true, "Table 'users' is created or finded!");
}

auto SQLDB::CreateTable2()->std::pair<bool, QString>
{
    QSqlQuery query;
    if(!query.exec("create table if not exists private_messages (id serial "
        "primary key, from_id integer references users(id), to_id integer "
        "references users(id), text varchar(1024));"))
        return std::make_pair(false, query.lastError().text());
    else
        return std::make_pair(true, "Table 'private_messages' is created or "
            "finded!");
}

auto SQLDB::CreateTable3()->std::pair<bool, QString>
{
    QSqlQuery query;
    if(!query.exec("create table if not exists to_all_messages (id serial "
                    "primary key, from_id integer references users(id), "
                    "text varchar(1024));"))
        return std::make_pair(false, query.lastError().text());
    else
        return std::make_pair(true, "Table 'to_all_messages' is created or "
                                    "finded!");
}

auto SQLDB::CheckUserByLogin(QString login)->bool
{
    QSqlQuery query;
    QString str = "select id from users where login='" + login + "'";
    if(query.exec(str))
        if(query.next())
            return true;
    return false;
}

auto SQLDB::getUserById(QString id)->QString const
{
    QSqlQuery query;
    QString str = "select login from users where id='" + id + "'";
    if(query.exec(str))
        if(query.next())
            return query.value(0).toString();
    return "";
}

auto SQLDB::get20MessagesToAll()->QVector<QString> const
{
    QVector<QString> messages;
    QString message;
    QSqlQuery query;
    QString str = "select id, from_id, text from (select id, from_id, text "
        "from to_all_messages order by id desc limit 20) order by id asc";
    if(query.exec(str))
        while(query.next())
        {
            message = getUserById(query.value(1).toString()) + ";" + query.
                value(2).toString();
            messages.push_back(message);
        }
    return messages;
}

auto SQLDB::getUserId(QString login)->size_t const
{
    QSqlQuery query;
    QString str = "select id from users where login='" + login + "'";
    if(query.exec(str))
        if(query.next())
            return query.value(0).toInt();
    return -1;
}

auto SQLDB::getAllUsers()->QVector<QString> const
{
    QSqlQuery query;
    QVector<QString> users;
    QString str = "select login from users";
    if(query.exec(str))
        while(query.next())
            users.push_back(query.value(0).toString());
    return users;
}

auto SQLDB::AddUser(QString name, QString login, QString password)->size_t
{
    if(!CheckUserByLogin(login))
    {
        QSqlQuery query;
        query.prepare("insert into users(name, login, password) "
                      "values(:name, :login, :password);");
        query.bindValue(":name", name);
        query.bindValue(":login", login);
        query.bindValue(":password", password);
        if(query.exec())
            return 1; //user added in database
        else
            return 0; //user not added in database
    }
    else
        return 2; //user is already exists
}

auto SQLDB::CheckUserByLoginAndPassword(QString login, QString password)->bool
{
    QSqlQuery query;
    QString str = "select login, password from users where login='" + login +
        "' and password='" + password + "'";
    if(query.exec(str))
        if(query.next())
            return true;
    return false;
}

auto SQLDB::AddMessage(QString sender, QString text)->bool
{
    QSqlQuery query;
    query.prepare("insert into to_all_messages (from_id, text) values"
        "(:from_id, :text);");
    query.bindValue(":from_id", getUserId(sender));
    query.bindValue(":text", text);
    if(query.exec())
        return true; //message to all added in database
    else
        return false; //message to all not added in database
}

auto SQLDB::AddPrivateMessage(QString from, QString to, QString text)->bool
{
    QSqlQuery query;
    query.prepare("insert into private_messages (from_id, to_id, text) "
                  "values(:from_id, :to_id, :text);");
    query.bindValue(":from_id", getUserId(from));
    query.bindValue(":to_id", getUserId(to));
    query.bindValue(":text", text);
    if(query.exec())
        return true; //private message added in database
    else
        return false; //private message not added in database
}

auto SQLDB::getMessagesBetweenTwoUsers(QString user1, QString user2)->
    QVector<QString> const
{
    QString userId1 = QString::number(getUserId(user1));
    QString userId2 = QString::number(getUserId(user2));
    QVector<QString> privateMessages;
    QString message;
    QSqlQuery query;
    QString str = "select from_id, to_id, text from private_messages where "
        "(from_id='" + userId1 + "' and to_id='" + userId2 + "') or (from_id"
        "'" + userId2 + "' and to_id='" + userId1 + "')";
    if(query.exec(str))
        while(query.next())
        {
            message = getUserById(query.value(0).toString()) + ";" +
                getUserById(query.value(1).toString()) + ";" + query.value(2).
                toString();
            privateMessages.push_back(message);
        }
    return privateMessages;
}
