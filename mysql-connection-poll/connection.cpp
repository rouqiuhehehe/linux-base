//
// Created by ASUS on 2023/2/26.
//

#include "connection.h"
class Connection::ConnectionPrivate
{
public:
    ConnectionPrivate () : db(new MYSQL), aliveTime(0) {}
    ~ConnectionPrivate ()
    {
        mysql_close(db);
        delete db;
    }
    MYSQL *db;
    // 进入空闲状态的起始时间
    clock_t aliveTime;
};
Connection::Connection () : d(new ConnectionPrivate) { mysql_init(d->db); }
Connection::~Connection () { delete d; }
bool Connection::connect (
    const std::string &ip,
    Connection::port_t port,
    const std::string &username,
    const std::string &password,
    const std::string &dbname
)
{
    if (mysql_real_connect(d->db,
        ip.c_str(),
        username.c_str(),
        password.c_str(),
        dbname.c_str(),
        port,
        nullptr,
        0) == nullptr)
        return false;

    return true;
}
MYSQL *Connection::db ()
{
    return d->db;
}
void Connection::refreshAliveTime ()
{
    d->aliveTime = clock();
}
clock_t Connection::getAliveTime ()
{
    return d->aliveTime;
}
