#ifndef __THREADSAFE_LIST__
#define __THREADSAFE_LIST__

#include <pthread.h>

struct entry {
    struct entry *next;
    void *(*start_routine)(void *);
    void *arg;
};

struct list {
    struct entry *head;
    struct entry **tail;
    pthread_mutex_t list_lock;
    pthread_cond_t notempty;
};

struct list *list_init(void);
int list_enqueue(struct list *list, void *(*start_routine)(void *), void *arg);
struct entry *list_dequeue(struct list *list);

#endif