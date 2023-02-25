//
// Created by ASUS on 2023/2/25.
//

#include <mysql.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// #define HOST "192.168.142.128"
#define HOST "192.168.19.128"
#define USER "root"
#define PASSWORD "jianv4as"
#define DATABASE "YOSHIKI_DB"
#define PORT 3306
#define CHECK_RET(db, ret, funName, retNum) {                    \
    if (ret) {                                                   \
        printf(#funName" error : %s\n", mysql_error(db));        \
        return retNum;                                           \
    }                                                            \
}
#define BUFFER_LENGTH (64 * 1024)
#define WRITE 0
#define READ 1
#define OPERATOR READ
int imgRead (const char *filename, char *buffer)
{
    if (filename == NULL || buffer == NULL)
        return 1;

    FILE *file = fopen(filename, "rb");
    if (file == NULL)
    {
        printf("open file error\n");
        return -1;
    }

    // 文件指针，偏移量，从哪里开始偏移
    fseek(file, 0, SEEK_END);
    int length = (int)ftell(file);
    fseek(file, 0, SEEK_SET);

    int readSize;
    if ((readSize = (int)fread(buffer, 1, length, file)) != length)
    {
        printf("read file length error : %d\n", readSize);
        return -2;
    }

    return readSize;
}

int imgWrite (const char *filename, const char *buffer, int length)
{
    if (filename == NULL || buffer == NULL || length < 1)
        return 1;

    FILE *file = fopen(filename, "wb");
    if (file == NULL)
    {
        printf("open file error\n");
        return -1;
    }

    int writeSize;
    if ((writeSize = (int)fwrite(buffer, 1, length, file)) != length)
    {
        printf("write file length error : %d\n", writeSize);
        return -2;
    }

    return 0;
}

int mysqlWriteImg (MYSQL *db, const char *buffer, int length)
{
    if (db == NULL || buffer == NULL || length < 1)
        return 1;

    MYSQL_STMT *stmt = mysql_stmt_init(db);
    if (stmt == NULL)
    {
        printf("mysql_stmt_init error\n");
        return -1;
    }

    const char *sql = "insert into Y_user(`y_username`, `y_sex`, `y_img`) values (\"李奇缘\", \"woman\", ?)";
    int ret = mysql_stmt_prepare(stmt, sql, strlen(sql));
    CHECK_RET(db, ret, mysql_stmt_prepare, -2);

    MYSQL_BIND params;
    memset(&params, 0, sizeof(MYSQL_BIND));
    params.buffer_type = MYSQL_TYPE_LONG_BLOB;

    ret = (int)mysql_stmt_bind_param(stmt, &params);
    CHECK_RET(db, ret, mysql_stmt_bind_param, -3);

    ret = mysql_stmt_send_long_data(stmt, 0, buffer, length);
    CHECK_RET(db, ret, mysql_stmt_send_long_data, -4);

    ret = mysql_stmt_execute(stmt);
    CHECK_RET(db, ret, mysql_stmt_execute, -5);

    ret = mysql_stmt_close(stmt);
    CHECK_RET(db, ret, mysql_stmt_close, -6);

    return 0;
}

int mysqlReadImg (MYSQL *db, char *buffer, const char *username)
{
    if (db == NULL || buffer == NULL)
        return 1;

    MYSQL_STMT *stmt = mysql_stmt_init(db);
    if (stmt == NULL)
    {
        printf("mysql_stmt_init error\n");
        return -1;
    }

    const char *sql = "select y_img from Y_user where y_username=?";
    int ret = mysql_stmt_prepare(stmt, sql, strlen(sql));
    CHECK_RET(db, ret, mysql_stmt_prepare, -6);

    MYSQL_BIND params;
    memset(&params, 0, sizeof(MYSQL_BIND));
    params.buffer_type = MYSQL_TYPE_VARCHAR;
    params.buffer = (void *)username;
    params.buffer_length = strlen(username);

    ret = (int)mysql_stmt_bind_param(stmt, &params);
    CHECK_RET(db, ret, mysql_stmt_bind_param, -3);

    MYSQL_BIND result;
    memset(&result, 0, sizeof(MYSQL_BIND));
    result.buffer_type = MYSQL_TYPE_LONG_BLOB;
    unsigned long totalLength = 0;
    result.length = &totalLength;

    ret = mysql_stmt_bind_result(stmt, &result);
    CHECK_RET(db, ret, mysql_stmt_bind_result, -3);

    ret = mysql_stmt_execute(stmt);
    CHECK_RET(db, ret, mysql_stmt_execute, -4);

    ret = mysql_stmt_store_result(stmt);
    CHECK_RET(db, ret, mysql_stmt_store_result, -5);

    int column = 0;
    while (1)
    {
        ret = mysql_stmt_fetch(stmt);
        // MYSQL_DATA_TRUNCATED表示数据没读完  继续读就好
        if (ret != 0 && ret != MYSQL_DATA_TRUNCATED)
            break;

        int start = 0;
        while (start < totalLength)
        {
            result.buffer = buffer + start;
            // 每次读取的字节数
            result.buffer_length = 1;
            mysql_stmt_fetch_column(stmt, &result, column, start);
            start += (int)result.buffer_length;
        }
        column++;
    }

    mysql_stmt_close(stmt);
    return (int)totalLength;
}
int selectUserTable (MYSQL *db)
{
    const char *sql = "select * from Y_user";
    if (mysql_real_query(db, sql, strlen(sql)))
    {
        printf("mysql_real_query error : %s", mysql_error(db));
        return -1;
    }

    struct st_mysql_res *res = mysql_store_result(db);

    int rows = (int)mysql_num_rows(res);
    int fields = (int)mysql_field_count(db);
    MYSQL_ROW result;
    printf("rows count : %d\n", rows);
    while ((result = mysql_fetch_row(res)))
    {
        for (int i = 0; i < fields; ++i)
            printf("%s\t", result[i]);
        printf("\n");
    }

    return 0;
}
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

    selectUserTable(&db);

    // const char *insert = "insert into Y_user(`y_username`, `y_sex`) values (\"狗蛋\", \"woman\")";
    // if (mysql_real_query(&db, insert, strlen(insert)))
    // {
    //     printf("mysql_real_query error : %s", mysql_error(&db));
    //     exit(EXIT_FAILURE);
    // }

    char buffer[BUFFER_LENGTH];
#if OPERATOR == WRITE
    int length = imgRead("assets/5f3fc08a3a35a307e8d99e0045b26091.png", buffer);
    if (length < 0)
        exit(EXIT_FAILURE);

    mysqlWriteImg(&db, buffer, length);
#elif OPERATOR == READ
    int length = mysqlReadImg(&db, buffer, "李奇缘");
    if (length < 0)
        exit(EXIT_FAILURE);

    imgWrite("assets/copy.png", buffer, length);
#endif

    mysql_close(&db);
    return 0;
}