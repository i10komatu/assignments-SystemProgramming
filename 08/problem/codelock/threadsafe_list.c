#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>

pthread_mutex_t list_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t notempty = PTHREAD_COND_INITIALIZER;

struct entry {
    struct entry *next;
    void *data;
};

struct list {
    struct entry *head;
    struct entry **tail;
};

struct list *list_init(void) {
    struct list *list;

    list = malloc(sizeof *list);
    if (list == NULL) return (NULL);
    list->head = NULL;
    list->tail = &list->head;
    return (list);
}

int list_enqueue(struct list *list, void *data) {
    struct entry *e;

    e = malloc(sizeof *e);
    if (e == NULL) return (1);
    e->next = NULL;
    e->data = data;

    /* critical section */
    pthread_mutex_lock(&list_lock);
    *list->tail = e;
    list->tail = &e->next;
    pthread_cond_signal(&notempty);
    pthread_mutex_unlock(&list_lock);
    /* critical section */
    return (0);
}

struct entry *list_dequeue(struct list *list) {
    struct entry *e;

    /* critical section */
    pthread_mutex_lock(&list_lock);

    while (list->tail == &list->head)
        pthread_cond_wait(&notempty, &list_lock);

    if (list->head == NULL) {
        pthread_mutex_unlock(&list_lock);
        return (NULL);
    }

    e = list->head;
    list->head = e->next;
    if (list->head == NULL) list->tail = &list->head;
    pthread_mutex_unlock(&list_lock);
    /* critical section */
    return (e);
}

struct entry *list_traverse(struct list *list, int (*func)(void *, void *), void *user) {
    struct entry **prev, *n, *next;

    if (list == NULL) return (NULL);

    /* critical section */
    pthread_mutex_lock(&list_lock);
    prev = &list->head;
    for (n = list->head; n != NULL; n = next) {
        next = n->next;
        switch (func(n->data, user)) {
            case 0:
                /* continues */
                prev = &n->next;
                break;
            case 1:
                /* delete the entry */
                *prev = next;
                if (next == NULL) list->tail = prev;
                pthread_mutex_unlock(&list_lock);
                return (n);
            case -1:
            default:
                /* traversal stops */
                pthread_mutex_unlock(&list_lock);
                return (NULL);
        }
    }
    /* critical section */
    pthread_mutex_unlock(&list_lock);
    return (NULL);
}

int print_entry(void *e, void *u) {
    printf("%s\n", (char *)e);
    return (0);
}

int delete_entry(void *e, void *u) {
    char *c1 = e, *c2 = u;

    return (!strcmp(c1, c2));
}

/*** Test ***/
pid_t gettid() { return syscall(SYS_gettid); }

pthread_mutex_t cnt_lock = PTHREAD_MUTEX_INITIALIZER;
int count = 0;

void *enqueue(void *arg) {
    struct list *list = (struct list *)arg;

    for (int i = 0; i < 30; i++) {
        char number[3];
        pthread_mutex_lock(&cnt_lock);
        sprintf(number, "%02d", ++count);
        pthread_mutex_unlock(&cnt_lock);

        printf("(I:%5d) %s\n", gettid(), number);
        list_enqueue(list, strdup(number));
        usleep(1000);
    }
}

void *dequeue(void *arg) {
    struct list *list = (struct list *)arg;
    struct entry *entry;

    for (int i = 0; i < 10; i++) {
        entry = list_dequeue(list);
        printf("(O:%5d) %s\n", gettid(), (char *)entry->data);
        free(entry->data);
        free(entry);
        usleep(5000);
    }
}

int main() {
    pthread_t tid_enq[2], tid_deq[6];
    struct list *list;
    struct entry *entry;

    list = list_init();

    for (int i = 0; i < 2; i++)
        pthread_create(&tid_enq[i], NULL, enqueue, (void *)list);

    for (int i = 0; i < 6; i++)
        pthread_create(&tid_deq[i], NULL, dequeue, (void *)list);

    for (int i = 0; i < 2; i++)
        pthread_join(tid_enq[i], NULL);

    for (int i = 0; i < 6; i++)
        pthread_join(tid_deq[i], NULL);

    free(list);
    return (0);
}