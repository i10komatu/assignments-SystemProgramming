#include <pthread.h>
#include <stdio.h>

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
int count;

void *inc_count(void *arg) {
    pthread_mutex_lock(&lock);
    printf("%d->", count);
    ++count;
    printf("%d\n", count);
    pthread_mutex_unlock(&lock);
    pthread_exit(0);
}

int main() {
    pthread_t tid1, tid2;

    pthread_create(&tid1, NULL, inc_count, NULL);
    pthread_create(&tid2, NULL, inc_count, NULL);
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    return 0;
}