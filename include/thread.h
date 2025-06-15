#ifndef THREAD_H
#define THREAD_H

#include <signal.h>
#include <setjmp.h>
#include "util.h"

#define STACK_SIZE 4096 // in bytes
#define MAX_THREADS 64  // maximum number of threads

enum THREAD_STATES
{
    RUNNING,
    READY,
    BLOCKED,
    COMPLETED,
    WAITING
};

typedef struct thread
{
    int thread_id;
    enum THREAD_STATES state;

    void *rsp;
    void *pc;
    void *stack;

    sigjmp_buf context;

} thread_t; // struct for defining thread control block

typedef struct Node
{
    thread_t *thread;
    struct Node *next;
} Node;

extern thread_t *current_thread;
extern int number_of_threads;
extern int thread_num;
extern Node *waiting_head;
extern Node *ready_head;

void init_lib();
int thread_create(thread_t *thread, void(*start_routine));
void thread_exit();
void print_queue();
void thread_join(thread_t *thread);
void thread_sleep(unsigned int milliseconds);
void thread_yield(void);
void context_switch(thread_t *prev, thread_t *next);
void enqueue(Node **queue, thread_t *thread);
thread_t *dequeue(Node **queue);
int is_empty(Node *queue);
void context();

#endif // THREAD_H