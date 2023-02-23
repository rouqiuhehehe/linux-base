//
// Created by ASUS on 2023/2/24.
//
#include <pthread.h>
#define INSERT_LIST (item, list) { \
            item->next = list;     \
            if (item != NULL)      \
                list->prev = item; \
            list = item;           \
}
#define DELETE_LIST (item, list) {                                 \
            if (item->prev != NULL) item->prev->next = item->next; \
            if (item->next != NULL) item->next->prev = item->prev; \
            if (item == list) list = item->next;                   \
            item->prev = item->next = NULL;                        \
}

struct Task
{
    void (*taskCallback) (void *arg);
    void *argData;

    struct Task *prev;
    struct Task *next;
};

struct Worker
{
    pthread_t threadId;
    // 线程链表
    struct ThreadPool *threadPoll;

    struct Worker *prev;
    struct Worker *next;
};

struct ThreadPool
{
    // 任务链表
    struct Task *task;
    // 线程链表
    struct Worker *worker;

    pthread_mutex_t mutex;
    // 线程变量，通过pthread_cond_wait让线程进入等待状态， 
    pthread_cond_t cond;
};