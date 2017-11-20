#include <pthread.h>
#include <stdio.h>

void *thread1(void *arg) {
    printf("thread1\n");
}

void *thread2(void *arg) {
    printf("thread2\n");
}

int main() {
    pthread_t tid1, tid2;
    pthread_create(&tid1, NULL, thread1, NULL);
    pthread_create(&tid2, NULL, thread2, NULL);
    printf("main thread\n");
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
}