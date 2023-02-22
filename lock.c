//
// Created by Administrator on 2023/2/22.
//
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

#define PTHREAD_LENGTH 2

// 互斥锁，会引起线程切换
pthread_mutex_t mutex;
// 自旋锁，相当于在外层套了一个while(1)循环，
// 自旋锁用1来表示没有加锁
// static inline void __raw_spin_lock(raw_spinlock_t *lock)
// {
//     while(1){
//         if(--lock->slock == 0) // 此处--lock->slock 如果等于0，说明刚才是1，也就是加锁成功，直接return，为原子操作，不存在抢锁
//             return;
//         while((int)lock->slock <= 0){} // 一直循环知道其他线程修改lock->slock等于1后跳出循环
//     }
// }
pthread_spinlock_t spinlock;

int compare_and_swap (int *rest, int oldV, int newV)
{
    if (*rest == oldV)
    {
        *rest = newV;
        return 0;
    }
    return -1;
}
void *thread_callback (void *arg)
{
    int *count = (int *)arg;
    int i = 0;

    int old = *count;
    while (i++ < 100000)
    {
        // pthread_mutex_lock(&mutex);
        // pthread_spin_lock(&spinlock);
        // (*count)++;
        // pthread_mutex_unlock(&mutex);
        // pthread_spin_unlock(&spinlock);
        while (compare_and_swap(count, old, old + 1))
            old = *count;
        usleep(1);
    }

    return NULL;
}

int main ()
{
    setbuf(stdout, NULL);

    pthread_t pthreadId[PTHREAD_LENGTH];
    int count = 0;
    // pthread_mutex_init(&mutex, NULL);
    pthread_spin_init(&spinlock, PTHREAD_PROCESS_SHARED);

    for (int i = 0; i < PTHREAD_LENGTH; ++i)
        pthread_create(&pthreadId[i], NULL, thread_callback, &count);

    for (int i = 0; i < 10; ++i)
    {
        printf("count : %d\n", count);
        sleep(1);
    }

    return 0;
}