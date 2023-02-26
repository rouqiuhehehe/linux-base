//
// Created by ASUS on 2023/2/26.
//

#ifndef LINUX_BASE_MYSQL_CONNECTION_POLL_CONNECTIONPOOL_H_
#define LINUX_BASE_MYSQL_CONNECTION_POLL_CONNECTIONPOOL_H_

#include <memory>
#include "connection.h"
#include <chrono>
#include "noncopyable.h"
#include <mutex>

#define NODISCARD
#if __cplusplus >= 201703
#define NODISCARD [[nodiscard]]
#endif
class Error;
class ConnectionPool : public NonCopyAble
{
public:
    using port_t = Connection::port_t;
    using Lock = std::unique_lock <std::mutex>;
    // 单例模式
    static ConnectionPool *createConnectionPool ();
    // 外部接口，提供一个空闲连接
    // 用智能指针，否则还需定义一个归还方法
    std::shared_ptr <Connection> getConnection ();

    void setIp (std::string ip);
    void setPort (port_t port);
    void setUsername (std::string username);
    void setPassword (std::string password);
    void setDbname (std::string dbname);
    void setInitSize (int size);
    void setMaxSize (int size);
    void setMaxIdleTime (int milliseconds);
    void setMaxConnectionTimeout (int milliseconds);
    NODISCARD std::string ip () const;
    NODISCARD port_t port () const;
    NODISCARD std::string username () const;
    NODISCARD std::string password () const;
    NODISCARD std::string dbname () const;
    NODISCARD int initSize () const;
    NODISCARD int maxSize () const;
    NODISCARD int maxIdleTime () const;
    NODISCARD int maxConnectionTimeout () const;
    NODISCARD Error lastError () const;
private:
    ConnectionPool ();

    // 负责产生新连接
    [[noreturn]] void produceConnectionTask ();

    // 负责回收连接
    [[noreturn]] void scannerConnectionTask ();

    // 测试连接池是否有可用连接
    bool testConnectionPoolHasConnection (Lock &lock);
    Connection *createConnection ();
private:
    class ConnectionPoolPrivate;
    ConnectionPoolPrivate *d;
};

#endif //LINUX_BASE_MYSQL_CONNECTION_POLL_CONNECTIONPOOL_H_
