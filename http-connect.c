//
// Created by ASUS on 2023/3/1.
//
#include <stdio.h>
#include <netdb.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#define HTTP_VERSION "HTTP/1.1"
#define CONNECTION_TYPE "close"
int hostnameToIp (const char *hostname, char **ips)
{
    if (hostname == NULL)
        return -1;

    char *ipsPtr = (char *)ips;
    char ip[INET_ADDRSTRLEN];
    int i = 0;
    struct hostent *hostEntry = gethostbyname(hostname);
    for (char **ptr = hostEntry->h_addr_list; *ptr != NULL; ++ptr)
    {
        memset(ip, 0, INET_ADDRSTRLEN);
        inet_ntop(hostEntry->h_addrtype,
            *ptr,
            ip,
            INET_ADDRSTRLEN);
        memcpy(ipsPtr, ip, INET_ADDRSTRLEN);
        ipsPtr += INET_ADDRSTRLEN;
        i++;
    }
    if (i)
        return i;

    return 0;
}

int httpCreateSocket (const char *ip, in_port_t port)
{
    if (ip == NULL)
        return -1;

    int socketFd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in sockaddrIn = {};
    sockaddrIn.sin_port = htons(port);
    sockaddrIn.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &sockaddrIn.sin_addr);

    int ret = connect(socketFd,
        (const struct sockaddr *)&sockaddrIn,
        sizeof(struct sockaddr_in));
    if (ret != 0)
        return -2;

    // 设置非阻塞io
    fcntl(socketFd, F_GETFL, O_NONBLOCK);

    return socketFd;
}

char *httpSendRequest (const char *hostname, const char *path, in_port_t port)
{
    printf("-----------------\n");
    if (hostname == NULL || path == NULL)
        return NULL;

    char ips[5][INET_ADDRSTRLEN];
    memset(ips, 0, 5 * INET_ADDRSTRLEN);
    int len = hostnameToIp(hostname, (char **)ips);

    if (len < 1)
        return NULL;
    // 取一个能用的ip就行
    char *ip = ips[0];
    int socketFd = httpCreateSocket(ip, port);

    char buffer[1024] = {};
    char *httpHeader = "GET %s %s\r\n"
                       "HOST:%s\r\n"
                       "Connection:%s\r\n"
                       "\r\n";
    sprintf(
        buffer,
        httpHeader,
        path,
        HTTP_VERSION,
        hostname,
        CONNECTION_TYPE
    );

    send(socketFd, buffer, strlen(buffer), 0);

    // select
    fd_set fdSet;
    FD_ZERO(&fdSet);
    FD_SET(socketFd, &fdSet);

    struct timeval tv;
    // 5s轮询一次
    tv.tv_sec = 5;
    tv.tv_usec = 0;

    char *result = malloc(sizeof(int));
    memset(result, 0, sizeof(int));

    while (1)
    {
        // 轮询的最大socketid + 1，可读set，可写set，错误set，时间（传NULL设置为阻塞，传0s0ms设置为纯粹的非阻塞函数）
        int selection = select(socketFd + 1, &fdSet, NULL, NULL, &tv);

        // 返回值>0表示有文件描述符发生状态改变，=0超时，-1错误
        // FD_ISSET 检测集合中指定文件描述符是否可读写
        if (!selection || !FD_ISSET(socketFd, &fdSet))
            break;

        memset(buffer, 0, 1024);
        int length = (int)recv(socketFd, buffer, 1024, 0);
        // 断开连接
        if (length == 0)
            break;

        result = realloc(result, (strlen(result) + length + 1) * sizeof(char));
        if (result != NULL)
            strncat(result, buffer, length);
    }
    return result;
}

int saveHttp (const char *filename, const char *result)
{
    char *p = strstr(result, "\r\n\r\n");
    // 不保留\r\n\r\n
    p += 4;
    int len = strlen(result);
    char *string = malloc(len - (p - result));
    strncpy(string, p, len - (p - result));
    FILE *file = fopen(filename, "w");
    fputs(string, file);
    fclose(file);

    return 0;
}

int main (int argv, char **argc)
{
    if (argv < 4)
    {
        errno = EINVAL;
        perror("argument too few, please enter [url] [port] [path]");
        exit(EXIT_FAILURE);
    }

    char *hostname = argc[1];
    int port = atoi(argc[2]);
    char *path = argc[3];

    if (port < 0)
    {
        errno = EINVAL;
        perror("argument invalid, [port] must be a valid number");
        exit(EXIT_FAILURE);
    }

    char *result = httpSendRequest(hostname, path, port);
    saveHttp("assets/http-request.html", result);
    free(result);
    return 0;
}