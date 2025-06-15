#include "thread.h"
#include "jmpbuf-offsets.h"
#include "util.h"
#include "time_control.h"

extern struct itimerval timer;

thread_t *current_thread = NULL;
int number_of_threads;
int thread_num;
Node *waiting_head = NULL;
Node *ready_head = NULL;

void enqueue(Node **queue, thread_t *thread)
{
    Node *new_node = (Node *)malloc(sizeof(Node));
    if (new_node == NULL)
    {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    new_node->thread = thread;
    new_node->next = NULL;

    if (*queue == NULL)
    {
        *queue = new_node;
    }
    else
    {
        Node *current = *queue;
        while (current->next != NULL)
        {
            current = current->next;
        }
        current->next = new_node;
    }
}

thread_t *dequeue(Node **queue)
{
    if (*queue == NULL)
    {
        return NULL;
    }
    Node *node = *queue;
    *queue = node->next;
    thread_t *thread = node->thread;
    free(node);
    return thread;
}

int is_empty(Node *queue)
{
    return queue == NULL;
}

void init_lib()
{
    ready_head = NULL;
    waiting_head = NULL;

    number_of_threads = 0;
    thread_num = 0;

    return;
}

int thread_create(thread_t *thread, void(*start_routine))
{
    thread->stack = malloc(STACK_SIZE);
    thread->pc = start_routine;
    thread->rsp = thread->stack+STACK_SIZE-sizeof(void *);
    sigsetjmp(thread->context, 1);
    
    thread->thread_id = thread_num;
    thread_num++;
    number_of_threads++;
    thread->state = READY;
    enqueue(&ready_head, thread);

    if (thread->thread_id == 0)
    {
        dequeue(&ready_head);
        thread->state = RUNNING;
        current_thread = thread;
    }
    else
    {
        thread->context->__jmpbuf[JB_RSP] = mangle((addr_t)thread->rsp);
        thread->context->__jmpbuf[JB_PC] = mangle((addr_t)thread->pc);
        sigemptyset(&thread->context->__saved_mask);
    }

    return thread->thread_id;
}

void print_queue()
{
    Node *temp = ready_head;
    while(temp->next!=NULL)
    {
        temp = temp->next;
    }
}

void thread_exit()
{
    if(current_thread != NULL)
    {
        current_thread->state = COMPLETED;
        number_of_threads--;

        free((void*)(current_thread->rsp - STACK_SIZE + sizeof(void *)));

        thread_t *next_thread = dequeue(&ready_head);
        if(next_thread != NULL)
        {
            context_switch(current_thread, next_thread);
        }
        else
        {
            exit(0);
        }
    }
}

void thread_join(thread_t *thread)
{
    if(thread==NULL || thread->state==COMPLETED)
    {
        return;
    }

    current_thread->state = BLOCKED;

    while(thread->state!=COMPLETED)
    {
        thread_yield();
    }

    current_thread->state = READY;

    return;
}

void thread_sleep(unsigned int milliseconds)
{
    unsigned int start_time = get_time();
    unsigned int end_time = start_time + milliseconds;
    while ((unsigned int)get_time() < end_time) 
    {
        continue;
    }

    return;
}

void thread_yield(void)
{
    if(!is_empty(ready_head))
    {
        thread_t *new = dequeue(&ready_head);

        if(new->state!=COMPLETED)
        {
            context_switch(current_thread, new);
        }
    }

    return;
}

void context_switch(thread_t *prev, thread_t *next)
{
    setitimer(ITIMER_REAL, &timer, NULL);
    int ret_val = sigsetjmp(prev->context, 1);

    if(ret_val==0)
    {
        if(prev->state!=COMPLETED && prev->state!=WAITING)
        {
            enqueue(&ready_head, prev);
        }
        next->state = RUNNING;
        current_thread = next;
        siglongjmp(current_thread->context, 1);
    }
    else
    {
        prev->state = RUNNING;
        current_thread = prev;
    }
    return;
}

void context()
{
    thread_t *old = current_thread;
    if(!is_empty(ready_head))
    {
        thread_t *new = dequeue(&ready_head);
        if(old->state!=COMPLETED)
        {
            context_switch(old, new);
        }
    }
}