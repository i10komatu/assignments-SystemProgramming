#include <pthread.h>
#include <signal.h>
#include <stdlib.h> /* abort() */
#include <unistd.h> /* sleep() */
#include <stdio.h>

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
int hup = 0;
sigset_t hupset;

/* signal handler thread. We're not restricted to async-safe functions since
 * we're in a thread context */

void *handle_hup(void *arg) {
    int sig, err;

    err = sigwait(&hupset, &sig);
    if (err || sig != SIGHUP) abort();

    pthread_mutex_lock(&m);
    hup = 1;
    pthread_mutex_unlock(&m);

    return NULL;
}

int check_hup() {
    int h;

    pthread_mutex_lock(&m);
    h = hup;
    pthread_mutex_unlock(&m);

    return h;
}

int main() {
    pthread_t t;

    /* initialize set to empty */
    sigemptyset(&hupset);

    /* add SIGHUP */
    sigaddset(&hupset, SIGHUP);

    /* block signals in initial thread. New threads will inherit this signal
     * mask */
    pthread_sigmask(SIG_BLOCK, &hupset, NULL);
    pthread_create(&t, NULL, handle_hup, NULL);

    for (;;) {
        /* do stuff */
        printf("do stuff\n");
        sleep(1);

        if (check_hup()) {
            /* cleanup */
            printf("cleanup\n");
            break;
            /* got SIGHUP. We're done. */
        }
    }

    return 0;
}
