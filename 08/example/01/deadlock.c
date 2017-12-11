#include <pthread.h>

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

main() {
    pthread_mutex_lock(&m);
    pthread_mutex_lock(&m);
}