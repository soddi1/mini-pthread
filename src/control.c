#include "control.h"
#include "thread.h"
#include "util.h"

#include <stdio.h>


void mutex_init(mutex *m)
{
    m->flag = 0;
    return;
}

void mutex_acquire(mutex *m)
{
    while (test_and_set(&m->flag, 1) == 1)
    {
        thread_yield();
    }
    return;
}

void mutex_release(mutex *m)
{
    m->flag = 0;
    return;
}

int test_and_set(int *old, int new)
{
    int old_num = *old;
    *old = new;
    return old_num;
}

void sem_init(semaphore *sem, int value)
{
    sem->mut = (mutex *)malloc(sizeof(mutex));
    if(sem->mut == NULL)
    {
        return;
    }
    mutex_init(sem->mut);
    sem->val = value;
    return;
}

void sem_wait(semaphore *sem)
{
    mutex_acquire(sem->mut);
    sem->val--;
    if(sem->val<0)
    {
        mutex_release(sem->mut);
        current_thread->state = WAITING;
        enqueue(&waiting_head, current_thread);
        thread_yield();
        mutex_release(sem->mut);
    }

    mutex_release(sem->mut);

    return;
}

void sem_post(semaphore *sem)
{
    mutex_acquire(sem->mut);
    sem->val++;
    
    if(!is_empty(waiting_head))
    {
        thread_t *temp = dequeue(&waiting_head);
        enqueue(&ready_head, temp);
    }
    mutex_release(sem->mut);

    return;
}