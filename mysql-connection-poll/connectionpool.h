//
// Created by ASUS on 2023/2/26.
//

#ifndef LINUX_BASE_MYSQL_CONNECTION_POLL_CONNECTIONPOOL_H_
#define LINUX_BASE_MYSQL_CONNECTION_POLL_CONNECTIONPOOL_H_

#include <memory>
#include "connection.h"
#include <mutex>
#include <condition_variable>

class ConnectionPool
{
public:
    using port_t = Connection::port_t;
    // 单例模式
    static ConnectionPool *createConnectionPool ();
    // 外部接口，提供一个空闲连接
    // 用智能指针，否则还需定义一个归还方法
    std::shared_ptr<Connection> getConnection ();

    void setIp (std::string ip);
    void setPort (port_t port);
    void setUsername (std::string username);
    void setPassword (std::string password);
    void setDbname (std::string dbname);
    std::string ip () const;
    port_t port () const;
    std::string username () const;
    std::string password () const;
    std::string dbname () const;
private:
    ConnectionPool ();

    //
    void produceConnectionTask ();
};

#endif //LINUX_BASE_MYSQL_CONNECTION_POLL_CONNECTIONPOOL_H_
