//
// Created by ASUS on 2023/2/26.
//
#ifdef TEST

#include <iostream>
#include "connectionpool.h"
#include <functional>
#include <random>
#include "error.h"
#include <chrono>

#define HOST "192.168.19.128"
#define USER "root"
#define PASSWORD "jianv4as"
#define DATABASE "YOSHIKI_DB"
#define PORT 3306
template <class FN, class ...Arg>
inline void checkClock (const FN &fn, Arg &&...arg)
{
    auto begin = std::chrono::steady_clock::now();
    fn(arg...);
    auto end = std::chrono::steady_clock::now();
    std::cout << std::chrono::duration_cast <std::chrono::milliseconds>(
        end - begin
    ).count() << "ms" << std::endl;
}

inline int getRandomInt (int a, int b)
{
    std::random_device rd;
    std::default_random_engine eng(rd());
    return std::uniform_int_distribution <>(a, b)(eng);
}
inline void getRandomResult (std::string &name, std::string &sex)
{
    int len = getRandomInt(4, 10);
    sex = len % 2 == 0 ? "woman" : "man";
    for (int j = 0; j < len; ++j)
    {
        char s = getRandomInt(97, 122);
        name += s;
    }
}

void mysqlInsert (ConnectionPool *connectionPool)
{
    const char *initSql =
        R"(insert into Y_user (`y_username`, `y_sex`) values ("?", "?"))";

    std::string sql = initSql;
    std::string name;
    std::string sex;
    size_t index = sql.find('?');
    MYSQL *db;

    // 不使用连接池
    if (!connectionPool)
    {
        db = new MYSQL;
        mysql_init(db);
        mysql_real_connect(
            db,
            HOST,
            USER,
            PASSWORD,
            DATABASE,
            PORT,
            nullptr,
            0
        );
    }
    for (int i = 0; i < 10000; ++i)
    {
        getRandomResult(name, sex);
        sql.replace(index, 1, name);
        sql.replace(sql.find('?', index), 1, sex);

        // 使用连接池
        if (connectionPool)
        {
            auto connect = connectionPool->getConnection();
            db = connect->db();
        }

        if (mysql_real_query(db, sql.c_str(), sql.size()))
            std::cout << "mysql_real_query error : " << mysql_error(db)
                      << std::endl;

        name.clear();
        sex.clear();
        sql = initSql;
    }

    if (!connectionPool)
        mysql_close(db);
}
int main ()
{
    auto *connectionPool = ConnectionPool::createConnectionPool();
    connectionPool->setDbname(DATABASE);
    connectionPool->setIp(HOST);
    connectionPool->setUsername(USER);
    connectionPool->setPassword(PASSWORD);
    if (!connectionPool->run())
    {
        std::cout << connectionPool->lastError().what();
        exit(EXIT_FAILURE);
    }

    // 2971ms
    checkClock(
        [&connectionPool] () {
            mysqlInsert(connectionPool);
        }
    );

    // std::cin.get();
    // 2839ms
    // checkClock(
    //     [] () {
    //         mysqlInsert(nullptr);
    //     }
    // );

    return 0;
}

#endif