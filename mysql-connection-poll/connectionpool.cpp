//
// Created by ASUS on 2023/2/26.
//

#include "connectionpool.h"
#include <queue>
#include <condition_variable>
#include <atomic>
#include <thread>
#include "error.h"
class ConnectionPool::ConnectionPoolPrivate
{
public:
    ConnectionPoolPrivate () = default;

    std::string ip;
    std::string username;
    std::string password;
    std::string dbname;
    port_t port = 3306;

    // 连接池初始数量
    int initSize = 10;
    // 连接池最大连接数量
    int maxSize = 1024;
    // 连接池最大等待空闲
    int maxIdleTime = 60 * 1000;
    // 获取连接的最大超时时间，如果超过当前时间还没有空闲连接，则getConnection返回空指针
    int maxConnectionTimeout = 100;
    // 记录所有的连接数
    std::atomic_int connectionCount { 0 };

    // 连接队列
    std::queue <Connection *> connectionQueue;
    std::mutex mutex;
    std::condition_variable cond;

    Error error;
};

ConnectionPool *ConnectionPool::createConnectionPool ()
{
    // 懒汉式单例，c++11后线程安全
    // static 是全局变量，所有线程共享
    static ConnectionPool connectionPool;
    return &connectionPool;
}
std::shared_ptr <Connection> ConnectionPool::getConnection ()
{
    std::unique_lock <std::mutex> lock(d->mutex);

    if (!testConnectionPoolHasConnection(lock))
        return nullptr;

    // 第二个参数为析构函数
    // 因为此处并不需调用原本Connection的析构函数释放，只需将其还给连接池，所以重写
    std::shared_ptr <Connection> connection(
        d->connectionQueue.front(), [this] (Connection *cont) {
            Lock lock(d->mutex);
            // 返还队列后，重置空闲起始时间
            cont->refreshAliveTime();
            d->connectionQueue.push(cont);
        }
    );

    d->connectionQueue.pop();
    // 通知生产者队列
    // 如果生产者队列为空，并且连接池数量小于maxSize，则创建一个新连接
    // 然后通知该线程继续接收新请求
    // 如果生产者队列还有空闲连接，则什么都不做，继续等待消费者
    d->cond.notify_all();
    return connection;
}
bool ConnectionPool::testConnectionPoolHasConnection (Lock &lock)
{
    // 当连接池为空时，等待maxConnectionTimeout
    // 如果超时后连接池依然无可用链接，返回超时
    while (d->connectionQueue.empty())
    {
        if (d->cond
             .wait_for(lock, std::chrono::milliseconds(d->maxConnectionTimeout))
            == std::cv_status::timeout)
        {
            if (d->connectionQueue.empty())
            {
                d->error.setError("获取连接超时，无可用连接");
                return false;
            }
        }
    }
    return true;
}
ConnectionPool::ConnectionPool ()
    : d(new ConnectionPoolPrivate)
{
    if (ip().empty() || username().empty() || password().empty()
        || dbname().empty())
    {
        d->error
         .setError(
             "init info error,please check ip,username,password,dbname,port"
         );
        return;
    }

    for (int i = 0; i < d->initSize; ++i)
        createConnection();

    std::thread produce(&ConnectionPool::produceConnectionTask, this);
    std::thread scanner(&ConnectionPool::scannerConnectionTask, this);

    produce.detach();
    scanner.detach();
}
[[noreturn]] void ConnectionPool::produceConnectionTask ()
{
    while (true)
    {
        Lock lock(d->mutex);
        while (!d->connectionQueue.empty())
            d->cond.wait(lock);

        if (d->connectionCount < d->maxSize)
            createConnection();

        // 如果创建成功，或者此时有连接还回连接池时，通知消费者可以消费了
        if (!d->connectionQueue.empty())
            d->cond.notify_all();
    }
}
[[noreturn]] void ConnectionPool::scannerConnectionTask ()
{
    while (true)
    {
        // 等待maxIdleTime，实现定时效果
        std::this_thread::sleep_for(std::chrono::milliseconds(d->maxIdleTime));

        Lock lock(d->mutex);
        // 如果因为任务量爆发，临时创建了多个连接
        while (d->connectionCount > d->initSize)
        {
            // 因为队列先进先出的特性，所以此处只需要循环检查队列头
            auto *connection = d->connectionQueue.front();
            // 如果此处连接等待的空闲时间大于最大空闲时间
            if (connection->getAliveTime() > d->maxIdleTime)
            {
                d->connectionQueue.pop();
                delete connection;
                d->connectionCount--;
            }
            else break; // 如果队列头无超时，其他的肯定也不会超时
        }
    }
}
Connection *ConnectionPool::createConnection ()
{
    auto *connection = new Connection;
    connection->connect(ip(), port(), username(), password(), dbname());
    // 记录创建的开始时间
    connection->refreshAliveTime();
    d->connectionQueue.push(connection);
    d->connectionCount++;

    return connection;
}
Error ConnectionPool::lastError () const
{
    return d->error;
}
void ConnectionPool::setIp (std::string ip)
{
    d->ip = std::move(ip);
}
void ConnectionPool::setPort (ConnectionPool::port_t port)
{
    d->port = port;
}
void ConnectionPool::setUsername (std::string username)
{
    d->username = std::move(username);
}
void ConnectionPool::setPassword (std::string password)
{
    d->password = std::move(password);
}
void ConnectionPool::setDbname (std::string dbname)
{
    d->dbname = std::move(dbname);
}
void ConnectionPool::setInitSize (int size)
{
    d->initSize = size;
}
void ConnectionPool::setMaxSize (int size)
{
    d->maxSize = size;
}
void ConnectionPool::setMaxIdleTime (int milliseconds)
{
    d->maxIdleTime = milliseconds;
}
void ConnectionPool::setMaxConnectionTimeout (int milliseconds)
{
    d->maxConnectionTimeout = milliseconds;
}
std::string ConnectionPool::ip () const
{
    return d->ip;
}
ConnectionPool::port_t ConnectionPool::port () const
{
    return d->port;
}
std::string ConnectionPool::username () const
{
    return d->username;
}
std::string ConnectionPool::password () const
{
    return d->password;
}
std::string ConnectionPool::dbname () const
{
    return d->dbname;
}
int ConnectionPool::initSize () const
{
    return d->initSize;
}
int ConnectionPool::maxSize () const
{
    return d->maxSize;
}
int ConnectionPool::maxIdleTime () const
{
    return d->maxIdleTime;
}
int ConnectionPool::maxConnectionTimeout () const
{
    return d->maxConnectionTimeout;
}