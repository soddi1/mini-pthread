#ifndef CONTROL_H
#define CONTROL_H

typedef struct mutex_t
{
    int flag;
} mutex;

typedef struct semaphore_t
{
    int val;
    mutex *mut;

} semaphore;

void mutex_init(mutex *m);
void mutex_acquire(mutex *m);
void mutex_release(mutex *m);
int test_and_set(int *old, int new);

void sem_init(semaphore *sem, int value);
void sem_wait(semaphore *sem);
void sem_post(semaphore *sem);

#endif // CONTROL_H