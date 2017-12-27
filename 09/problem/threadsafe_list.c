#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "threadsafe_list.h"

struct list *list_init(void) {
    struct list *list;

    list = malloc(sizeof *list);
    if (list == NULL) return (NULL);
    list->head = NULL;
    list->tail = &list->head;
    pthread_mutex_init(&list->list_lock, NULL);
    pthread_cond_init(&list->notempty, NULL);
    return (list);
}

int list_enqueue(struct list *list, void *(*start_routine)(void *), void *arg) {
    struct entry *e;

    e = malloc(sizeof *e);
    if (e == NULL) return (1);
    e->next = NULL;
    e->start_routine = start_routine;
    e->arg = arg;

    pthread_mutex_lock(&list->list_lock);
    *list->tail = e;
    list->tail = &e->next;
    pthread_cond_signal(&list->notempty);
    pthread_mutex_unlock(&list->list_lock);
    return (0);
}

struct entry *list_dequeue(struct list *list) {
    struct entry *e;

    pthread_mutex_lock(&list->list_lock);

    while (list->tail == &list->head)
        pthread_cond_wait(&list->notempty, &list->list_lock);

    if (list->head == NULL) {
        pthread_mutex_unlock(&list->list_lock);
        return (NULL);
    }

    e = list->head;
    list->head = e->next;
    if (list->head == NULL) list->tail = &list->head;
    pthread_mutex_unlock(&list->list_lock);
    return (e);
}
