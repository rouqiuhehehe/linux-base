//
// Created by ASUS on 2023/2/26.
//

#ifndef LINUX_BASE_MYSQL_CONNECTION_POLL_CONNECTION_H_
#define LINUX_BASE_MYSQL_CONNECTION_POLL_CONNECTION_H_
#include <string>
#include <mysql.h>
#include <ctime>
class Connection
{
public:
    typedef unsigned short port_t;
    Connection ();
    ~Connection ();

    bool connect (
        const std::string &ip,
        port_t port,
        const std::string &username,
        const std::string &password,
        const std::string &dbname
    );

    MYSQL *db();

private:
    // 刷新连接的起始空闲时间
    void refreshAliveTime();
    // 获取连接空闲时间
    clock_t getAliveTime ();

private:
    class ConnectionPrivate;
    ConnectionPrivate *d;
};
#endif //LINUX_BASE_MYSQL_CONNECTION_POLL_CONNECTION_H_
