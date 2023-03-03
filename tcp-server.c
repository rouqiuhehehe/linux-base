//
// Created by ASUS on 2023/3/3.
//

#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <unistd.h>

#define CHECK_RET(ret, fnName, retn) { \
    if (ret != 0)                      \
    {                                  \
        perror(#fnName" error");       \
        return retn;                   \
    }                                  \
}

int tcpCreateServer (int port)
{
    if (port < 1)
        return -1;

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 1)
    {
        perror("socket error");
        return -2;
    }

    struct sockaddr_in sockaddrIn = {};
    sockaddrIn.sin_port = htons(port);
    sockaddrIn.sin_family = AF_INET;
    sockaddrIn.sin_addr.s_addr = INADDR_ANY;
    int ret = bind(sockfd,
        (const struct sockaddr *)&sockaddrIn,
        sizeof(struct sockaddr));
    CHECK_RET(ret, bind, -3);

    ret = listen(sockfd, 5);
    CHECK_RET(ret, listen, -4);

    int epfd = epoll_create(1);
    struct epoll_event epollEvent[1024] = {};

    struct epoll_event event = {};
    event.events = EPOLLIN;
    event.data.fd = sockfd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &event);

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
    while (1)
    {
        int nready = epoll_wait(epfd, epollEvent, 1024, 5);
        if (nready == -1) continue;

        for (int i = 0; i < nready; ++i)
        {
            if (epollEvent[i].data.fd == sockfd)
            {
                struct sockaddr_in clientAddr = {};
                socklen_t len = sizeof(struct sockaddr);
                ret = accept(sockfd, (struct sockaddr *)&clientAddr, &len);
                if (ret < 0)
                {
                    perror("accept error");
                    return -5;
                }

                fcntl(ret, F_SETFL, fcntl(ret, F_GETFL) | O_NONBLOCK);
                // 设置边缘触发
                // 水平触发 监听buffer里是否有数据
                // 边缘触发 监听文件状态变化(从无数据到有数据)
                event.events = EPOLLIN | EPOLLET;
                event.data.fd = ret;
                epoll_ctl(epfd, EPOLL_CTL_ADD, ret, &event);
            }
            else
            {
                ret = epollEvent[i].data.fd;
                char buffer[1024] = {};
                char *message = malloc(sizeof(char));
                memset(message, 0, sizeof(char));
                int len = 0;
                int realLen = 0;
                while ((len = recv(ret, buffer, 1024, 0)) > 0)
                {
                    realLen += len;
                    message = realloc(message, realLen + 1);
                    strncat(message, buffer, len);
                }

                if (len == 0 || (len == -1 && errno != EAGAIN))
                {
                    close(ret);
                    event.data.fd = ret;
                    event.events = EPOLLIN;
                    epoll_ctl(epfd, EPOLL_CTL_DEL, sockfd, &event);
                }
                else
                {
                    printf("%s\n", message);
                    free(message);
                }
            }
        }
    }
#pragma clang diagnostic pop
}

int main ()
{
    setbuf(stdout, NULL);
    tcpCreateServer(3000);

    return 0;
}