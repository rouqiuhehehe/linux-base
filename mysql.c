//
// Created by ASUS on 2023/2/25.
//

#include <mysql.h>
#include <stdio.h>
#include <string.h>

#define HOST "192.168.142.128"
#define USER "root"
#define PASSWORD "jianv4as"
#define DATABASE "YOSHIKI_DB"
#define PORT 3306
int main ()
{
    MYSQL db;

    if (mysql_init(&db) == NULL)
    {
        printf("mysql init error : %s", mysql_error(&db));
        exit(EXIT_FAILURE);
    }

    if (!mysql_real_connect(&db, HOST, USER, PASSWORD, DATABASE, PORT, NULL, 0))
    {
        printf("mysql connect error : %s", mysql_error(&db));
        exit(EXIT_FAILURE);
    }

    mysql_query(&db, "set names utf8");

    const char *insert = "insert into Y_user(`y_username`, `y_sex`) values (\"狗蛋\", \"woman\")";
    if (mysql_real_query(&db, insert, strlen(insert)))
    {
        printf("mysql_real_query error : %s", mysql_error(&db));
        exit(EXIT_FAILURE);
    }

    return 0;
}