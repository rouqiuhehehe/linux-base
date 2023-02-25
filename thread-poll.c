//
// Created by ASUS on 2023/2/24.
//
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define INSERT_LIST(item, list) {    \
            (item)->next = list;     \
            if ((list) != NULL)      \
                (list)->prev = item; \
            (list) = item;           \
}
#define DELETE_LIST(item, list) {                                        \
            if ((item)->prev != NULL) (item)->prev->next = (item)->next; \
            if ((item)->next != NULL) (item)->next->prev = (item)->prev; \
            if ((item) == (list)) (list) = (item)->next;                 \
            (item)->prev = (item)->next = NULL;                          \
}
#define ERROR perror

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
    // 线程池
    struct ThreadPool *threadPool;

    struct Worker *prev;
    struct Worker *next;
};

struct ThreadPool
{
    // 任务链表
    struct Task *task;
    // 线程链表
    struct Worker *worker;
    // 是否终止线程池
    int terminate;

    pthread_mutex_t mutex;
    // 线程变量，通过pthread_cond_wait让线程进入等待状态， 
    pthread_cond_t cond;
};

// static 类似于private修饰符，只在当前源文件可用
static void *threadCallback (void *arg)
{
    struct Worker *worker = (struct Worker *)arg;

    while (1)
    {
        pthread_mutex_lock(&worker->threadPool->mutex);

        while (worker->threadPool->task == NULL)
        {
            if (worker->threadPool->terminate)
                break;
            // 需要mutex参数，此处wait会先解锁，然后阻塞线程，直到被唤醒，重新加锁
            // 此处代码约等于一下代码的原子操作
            // pthread_mutex_unlock(&worker->threadPool->mutex);
            // pthread_cond_wait(&worker->threadPool->cond);
            // pthread_mutex_lock(&worker->threadPool->mutex);
            pthread_cond_wait(&worker->threadPool->cond, &worker->threadPool->mutex);
        }

        if (worker->threadPool->terminate)
        {
            pthread_mutex_unlock(&worker->threadPool->mutex);
            break;
        }

        struct Task *task = worker->threadPool->task;
        DELETE_LIST(task, worker->threadPool->task);
        // {
        //     if ((task)->prev != ((void *)0))(task)->prev->next = (task)->next;
        //     if ((task)->next != ((void *)0))(task)->next->prev = (task)->prev;
        //     if ((task) == (worker->threadPool->task))(worker->threadPool->task) = (task)->next;
        //     (task)->prev = (task)->next = ((void *)0);
        // }

        pthread_mutex_unlock(&worker->threadPool->mutex);

        task->taskCallback(task->argData);
        free(task);
    }

    free(worker);
    return NULL;
}

struct ThreadPool *createThreadPoll (int threadNum)
{
    struct ThreadPool *threadPool = malloc(sizeof(struct ThreadPool));
    if (threadPool == NULL)
        return NULL;
    if (threadNum < 1)
        threadNum = 1;

    memset(threadPool, 0, sizeof(struct ThreadPool));
    pthread_mutex_init(&threadPool->mutex, NULL);
    pthread_cond_init(&threadPool->cond, NULL);

    for (int i = 0; i < threadNum; ++i)
    {
        struct Worker *worker = malloc(sizeof(struct Worker));
        if (worker == NULL)
        {
            ERROR("worker 创建错误");
            free(threadPool);
            return NULL;
        }

        memset(worker, 0, sizeof(struct Worker));
        worker->threadPool = threadPool;

        int ret = pthread_create(&worker->threadId, NULL, threadCallback, worker);
        if (ret)
        {
            perror("线程创建错误");
            free(threadPool);
            free(worker);
            return NULL;
        }
        INSERT_LIST(worker, threadPool->worker);
    }

    return threadPool;
};

int destroyThreadPool (struct ThreadPool *threadPool)
{
    threadPool->terminate = 1;
    pthread_mutex_lock(&threadPool->mutex);
    // 广播  唤醒所有线程
    pthread_cond_broadcast(&threadPool->cond);
    pthread_mutex_unlock(&threadPool->mutex);

    threadPool->task = NULL;
    threadPool->worker = NULL;
    return 0;
}

int pushTaskThreadPool (struct ThreadPool *threadPool, struct Task *task)
{
    pthread_mutex_lock(&threadPool->mutex);
    INSERT_LIST(task, threadPool->task);
    // 唤醒一个线程处理任务
    pthread_cond_signal(&threadPool->cond);
    pthread_mutex_unlock(&threadPool->mutex);
}

#if 1
#define THREAD_POOL_INIT_COUNT 20
#define TASK_INIT_SIZE 1000

void taskEntry (void *arg)
{
    int index = *(int *)arg;
    printf("%d\n", index);

    free(arg);
}
int main ()
{
    setbuf(stdout, NULL);
    struct ThreadPool *threadPool = createThreadPoll(THREAD_POOL_INIT_COUNT);

    for (int i = 0; i < TASK_INIT_SIZE; ++i)
    {
        struct Task *task = malloc(sizeof(struct Task));
        memset(task, 0, sizeof(struct Task));
        task->taskCallback = taskEntry;
        task->argData = malloc(sizeof(int));
        *(int *)task->argData = i;

        pushTaskThreadPool(threadPool, task);
    }

    getchar();

    return 0;
}

#endif