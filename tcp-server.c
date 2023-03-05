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

const int TRUE = 1;
#define MAX_PORT 100
#define CHECK_RET(ret, fnName, port, retn) {        \
    if (ret != 0)                                   \
    {                                               \
        char error[100] = {};                       \
        sprintf(error, #fnName" error on %d", port);\
        perror(error);                              \
        return retn;                                \
    }                                               \
}

int isListenFd (int fd, int *fds)
{
    for (int i = 0; i < MAX_PORT; ++i)
    {
        if (fd == *(fds + i))
            return fd;
    }

    return 0;
}
int tcpCreateServer (int port)
{
    if (port < 1)
        return -1;

    int epfd = epoll_create(1);
    struct epoll_event epollEvent[1024] = {};
    int sockfds[MAX_PORT] = {};
    struct epoll_event event = {};
    static int count = 0;

    for (int i = 0; i < MAX_PORT; ++i)
    {
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 1)
        {
            char error[100] = {};
            sprintf(error, "socket error on %d", port + i);
            perror(error);
            return -2;
        }

        // SO_REUSEADDR只有针对time-wait链接(linux系统time-wait连接持续时间为1min)，确保server重启成功的这一个作用
        // setsockopt(
        //     sockfd,
        //     SOL_SOCKET,
        //     SO_REUSEADDR,
        //     (const char *)&TRUE,
        //     sizeof(TRUE));
        struct sockaddr_in sockaddrIn = {};
        sockaddrIn.sin_port = htons(port + i);
        sockaddrIn.sin_family = AF_INET;
        sockaddrIn.sin_addr.s_addr = INADDR_ANY;
        int ret = bind(
            sockfd,
            (const struct sockaddr *)&sockaddrIn,
            sizeof(struct sockaddr));
        CHECK_RET(ret, bind, port + i, -3);

        ret = listen(sockfd, 5);
        CHECK_RET(ret, listen, port + i, -4);

        event.events = EPOLLIN;
        event.data.fd = sockfd;
        epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &event);

        sockfds[i] = sockfd;
        printf("tcp server running : %d\n", port + i);
    }
    int clientfd = 0;

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
    while (1)
    {
        int nready = epoll_wait(epfd, epollEvent, 1024, 5);
        if (nready == -1)
        {
            perror("");
            continue;
        }

        for (int i = 0; i < nready; ++i)
        {
            int sockfd = isListenFd(epollEvent[i].data.fd, sockfds);
            if (sockfd)
            {
                struct sockaddr_in clientAddr = {};
                socklen_t len = sizeof(struct sockaddr);
                clientfd = accept(sockfd, (struct sockaddr *)&clientAddr, &len);
                if (clientfd < 0)
                {
                    perror("accept error");
                    return -5;
                }
                fcntl(clientfd, F_SETFL, fcntl(clientfd, F_GETFL) | O_NONBLOCK);
                if (setsockopt(
                    clientfd,
                    SOL_SOCKET,
                    SO_REUSEADDR,
                    (const char *)&TRUE,
                    sizeof(TRUE)
                ) == -1)
                    perror("setsockopt error");
                // 设置边缘触发
                // 水平触发 监听buffer里是否有数据
                // 边缘触发 监听文件状态变化(从无数据到有数据)
                event.events = EPOLLIN | EPOLLET;
                event.data.fd = clientfd;
                epoll_ctl(epfd, EPOLL_CTL_ADD, clientfd, &event);
            }
            else
            {
                clientfd = epollEvent[i].data.fd;
                char buffer[1024] = {};
                char *message = malloc(sizeof(char));
                memset(message, 0, sizeof(char));
                int len = 0;
                int realLen = 0;
                while ((len = recv(clientfd, buffer, 1024, 0)) > 0)
                {
                    realLen += len;
                    message = realloc(message, realLen + 1);
                    strncat(message, buffer, len);
                }

                if (len == 0 || (len == -1 && errno != EAGAIN))
                {
                    close(clientfd);
                    event.data.fd = clientfd;
                    event.events = EPOLLIN | EPOLLET;
                    epoll_ctl(epfd, EPOLL_CTL_DEL, clientfd, &event);
                    count--;
                }
                else
                {
                    printf("第%d个连接,%s\n", count++, message);
                }
                free(message);
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

//
//
// #include <stdio.h>
// #include <string.h>
// #include <stdlib.h>
//
// #include <netinet/tcp.h>
// #include <arpa/inet.h>
// #include <fcntl.h>
//
// #include <sys/epoll.h>
// #include <unistd.h>
//
// #define BUFFER_LENGTH        1024
// #define EPOLL_SIZE            1024
//
// #define MAX_PORT            100
//
// void *client_routine (void *arg)
// {
//
//     int clientfd = *(int *)arg;
//
//     while (1)
//     {
//
//         char buffer[BUFFER_LENGTH] = { 0 };
//         int len = recv(clientfd, buffer, BUFFER_LENGTH, 0);
//         if (len < 0)
//         {
//             close(clientfd);
//             break;
//         }
//         else if (len == 0)
//         { // disconnect
//             close(clientfd);
//             break;
//         }
//         else
//         {
//             printf("Recv: %s, %d byte(s)\n", buffer, len);
//         }
//
//     }
//
// }
//
// int islistenfd (int fd, int *fds)
// {
//
//     int i = 0;
//     for (i = 0; i < MAX_PORT; i++)
//     {
//         if (fd == *(fds + i)) return fd;
//     }
//
//     return 0;
// }
//
// int main (int argc, char *argv[])
// {
//
//     int port = 3000; // start
//     int sockfds[MAX_PORT] = { 0 }; // listen fd
//     int epfd = epoll_create(1);
//
//     int i = 0;
//     for (i = 0; i < MAX_PORT; i++)
//     {
//
//         int sockfd = socket(AF_INET, SOCK_STREAM, 0);
//
//         struct sockaddr_in addr;
//         memset(&addr, 0, sizeof(struct sockaddr_in));
//         addr.sin_family = AF_INET;
//         addr.sin_port = htons(port + i); // 8888 8889 8890 8891 .... 8987
//         addr.sin_addr.s_addr = INADDR_ANY;
//
//         if (bind(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in))
//             < 0)
//         {
//             perror("bind");
//             return 2;
//         }
//
//         if (listen(sockfd, 5) < 0)
//         {
//             perror("listen");
//             return 3;
//         }
//         printf("tcp server listen on port : %d\n", port + i);
//
//         struct epoll_event ev;
//         ev.events = EPOLLIN;
//         ev.data.fd = sockfd;
//         epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev);
//
//         sockfds[i] = sockfd;
//     }
//     //
//
// #if 0
//
//     while (1) {
//
//         struct sockaddr_in client_addr;
//         memset(&client_addr, 0, sizeof(struct sockaddr_in));
//         socklen_t client_len = sizeof(client_addr);
//
//         int clientfd = accept(sockfd, (struct sockaddr*)&client_addr, &client_len);
//
//         pthread_t thread_id;
//         pthread_create(&thread_id, NULL, client_routine, &clientfd);
//
//     }
//
// #else
//
//     struct epoll_event events[EPOLL_SIZE] = { 0 };
//
//     while (1)
//     {
//
//         int nready = epoll_wait(epfd, events, EPOLL_SIZE, 5); // -1, 0, 5
//         if (nready == -1) continue;
//
//         int i = 0;
//         for (i = 0; i < nready; i++)
//         {
//
//             int sockfd = islistenfd(events[i].data.fd, sockfds);
//             if (sockfd)
//             { // listen 2
//
//                 struct sockaddr_in client_addr;
//                 memset(&client_addr, 0, sizeof(struct sockaddr_in));
//                 socklen_t client_len = sizeof(client_addr);
//
//                 int clientfd = accept(
//                     sockfd,
//                     (struct sockaddr *)&client_addr,
//                     &client_len
//                 );
//
//                 fcntl(clientfd, F_SETFL, O_NONBLOCK);
//
//                 int reuse = 1;
//                 setsockopt(
//                     clientfd,
//                     SOL_SOCKET,
//                     SO_REUSEADDR,
//                     (char *)&reuse,
//                     sizeof(reuse));
//
//                 struct epoll_event ev;
//                 ev.events = EPOLLIN | EPOLLET;
//                 ev.data.fd = clientfd;
//                 epoll_ctl(epfd, EPOLL_CTL_ADD, clientfd, &ev);
//
//             }
//             else
//             {
//
//                 int clientfd = events[i].data.fd;
//
//                 char buffer[BUFFER_LENGTH] = { 0 };
//                 int len = recv(clientfd, buffer, BUFFER_LENGTH, 0);
//                 if (len < 0)
//                 {
//                     close(clientfd);
//
//                     struct epoll_event ev;
//                     ev.events = EPOLLIN;
//                     ev.data.fd = clientfd;
//                     epoll_ctl(epfd, EPOLL_CTL_DEL, clientfd, &ev);
//
//                 }
//                 else if (len == 0)
//                 { // disconnect
//                     close(clientfd);
//
//                     struct epoll_event ev;
//                     ev.events = EPOLLIN;
//                     ev.data.fd = clientfd;
//                     epoll_ctl(epfd, EPOLL_CTL_DEL, clientfd, &ev);
//
//                 }
//                 else
//                 {
//                     printf(
//                         "Recv: %s, %d byte(s), clientfd: %d\n",
//                         buffer,
//                         len,
//                         clientfd
//                     );
//                 }
//
//             }
//
//         }
//
//     }
//
// #endif
//
//     return 0;
// }