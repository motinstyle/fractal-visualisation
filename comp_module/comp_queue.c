#include "comp_queue.h"
#include "utils.h"

// why 100 ? 
#ifndef QUEUE_CAPACITY
#define QUEUE_CAPACITY 200
#endif

typedef struct
{
    message* queue[QUEUE_CAPACITY];
    int in;
    int out;

    pthread_mutex_t mtx;
    pthread_cond_t cond;
    bool quit;

} queue_msg;

static queue_msg qm = {.in = 0, .out = 0};

void queue_msg_init(void)
{
    pthread_mutex_init(&qm.mtx, NULL); // initialize mutex with default attributes
    pthread_cond_init(&qm.cond, NULL); // initialize condition variable with default attributes
}

void queue_push_msg(message* msg){
    pthread_mutex_lock(&(qm.mtx));
    while (((qm.in + 1) % QUEUE_CAPACITY) == qm.out) // queue is full
    {
        debug("queue is full");
        pthread_cond_wait(&(qm.cond), &(qm.mtx));
    }
    qm.queue[qm.in] = msg;
    qm.in = (qm.in + 1) % QUEUE_CAPACITY;
    pthread_cond_broadcast(&(qm.cond));

    pthread_mutex_unlock(&(qm.mtx));
}

message* queue_pop_msg(){
    message* msg;
    pthread_mutex_lock(&(qm.mtx));
    while (!qm.quit && qm.in == qm.out) // queue is empty
    {
        debug("queue is empty");
        return NULL;
        //pthread_cond_wait(&(qm.cond), &(qm.mtx));
    }
    if (qm.in != qm.out)
    {
        msg = qm.queue[qm.out];
        qm.out = (qm.out + 1) % QUEUE_CAPACITY;
        pthread_cond_broadcast(&(qm.cond));
    }

    pthread_mutex_unlock(&(qm.mtx));
    return msg;
} 

void queue_cleanup_msg(void)
{
    while (qm.in != qm.out)
    {
        message* msg = queue_pop_msg();
        if (msg)
        {
            free(msg);
        }
    }
}

bool is_quit_msg()
{
    bool quit;
    pthread_mutex_lock(&(qm.mtx));
    quit = qm.quit;
    pthread_mutex_unlock(&(qm.mtx));
    return quit;
}

// set quit status of queue
void set_quit_msg()
{
    pthread_mutex_lock(&(qm.mtx));
    qm.quit = true;
    pthread_mutex_unlock(&(qm.mtx));
}