#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <syslog.h>
#include <netinet/in.h>

#include "logutil.h"

#include "threadsafe_list.h"

#define NUM_WORKER 3
#define DEFAULT_SERVER_PORT 10000
#ifdef SOMAXCONN
#define LISTEN_BACKLOG SOMAXCONN
#else
#define LISTEN_BACKLOG 5
#endif

char *program_name = "server";

int open_accepting_socket(int port) {
    struct sockaddr_in self_addr;
    socklen_t self_addr_size;
    int sock, sockopt;

    memset(&self_addr, 0, sizeof(self_addr));
    self_addr.sin_family = AF_INET;
    self_addr.sin_addr.s_addr = INADDR_ANY;
    self_addr.sin_port = htons(port);
    self_addr_size = sizeof(self_addr);
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock < 0) logutil_fatal("accepting socket: %d", errno);
    sockopt = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &sockopt, sizeof(sockopt)) == -1)
        logutil_warning("SO_REUSEADDR: %d", errno);
    if (bind(sock, (struct sockaddr *)&self_addr, self_addr_size) < 0)
        logutil_fatal("bind accepting socket: %d", errno);
    if (listen(sock, LISTEN_BACKLOG) < 0) logutil_fatal("listen: %d", errno);
    return (sock);
}

void usage(void) {
    fprintf(stderr, "Usage: %s [option]\n", program_name);
    fprintf(stderr, "option:\n");
    fprintf(stderr, "\t-d\t\t\t\t... debug mode\n");
    fprintf(stderr, "\t-p <port>\n");
    exit(1);
}

void *wait_eof(void *arg) {
    int sock = *((int *)arg);
    logutil_info("   connected: %d", sock);
    char buf[128];
    int buf_len;
    while ((buf_len = read(sock, buf, 128)) > 0)
        for (int i = 0; i < buf_len; i++)
            if (buf[i] == EOF) goto end;

end:
    close(sock);
    logutil_info("disconnected: %d", sock);
}

sigset_t sigset;
void *signal_handler(void *arg) {
    int signal, error;

    error = sigwait(&sigset, &signal);
    if (error || (signal != SIGINT && signal != SIGTERM)) {
        if (error)
            logutil_error("ERROR: invalid signal number");
        else
            logutil_error("ERROR: signal_handler caught an unexpected signal");
        abort();
    }

    logutil_notice("bye");

    /* terminate all thread */
    _exit(0);

    return NULL;
}

void cleanup(void *arg) {
    struct entry *entry = (struct entry *)arg;
    close(*((int *)entry->arg));
    logutil_info("       close: %d", *((int *)entry->arg));
    free(entry->arg);
    free(entry);
}

void *worker(void *arg) {
    struct list *list = (struct list *)arg;
    struct entry *entry;
    pthread_t tid;

    for (;;) {
        entry = list_dequeue(list);
        pthread_cleanup_push(cleanup, entry);
        (*entry->start_routine)(entry->arg);
        pthread_cleanup_pop(1);
    }
}

int main(int argc, char **argv) {
    char *port_number = NULL;
    int ch, sock, server_port = DEFAULT_SERVER_PORT;
    int debug_mode = 0;

    while ((ch = getopt(argc, argv, "dp:")) != -1) {
        switch (ch) {
            case 'd':
                debug_mode = 1;
                break;
            case 'p':
                port_number = optarg;
                break;
            case '?':
            default:
                usage();
        }
    }
    argc -= optind;
    argv += optind;

    if (port_number != NULL) server_port = strtol(port_number, NULL, 0);

    /* server_portでlistenし，socket descriptorをsockに代入 */
    sock = open_accepting_socket(server_port);

    /* block SIGINT and SIGTERM */
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGINT);
    sigaddset(&sigset, SIGTERM);
    pthread_sigmask(SIG_BLOCK, &sigset, NULL);

    if (!debug_mode) {
        logutil_syslog_open(program_name, LOG_PID, LOG_LOCAL0);
        daemon(0, 0);
    }

    /* create signal handler thread */
    pthread_t tid = pthread_create(&tid, NULL, signal_handler, NULL);

    /* create worker thread */
    pthread_t tid_worker[NUM_WORKER];
    struct list *list = list_init();

    for (int i = 0; i < NUM_WORKER; i++)
        pthread_create(&tid_worker[i], NULL, worker, list);

    /* main loop */
    for (int i = 0; i < NUM_WORKER * 2; i++) {
        int *chldsock = (int *)malloc(sizeof(int));
        struct sockaddr_in addr;
        socklen_t addr_len = sizeof(addr);
        if ((*chldsock = accept(sock, (struct sockaddr *)&addr, &addr_len)) < 0) {
            perror("accept");
            break;
        } else {
            list_enqueue(list, wait_eof, chldsock);
            logutil_info("      accept: %d", *chldsock);
        }
    }

    close(sock);

    for (int i = 0; i < NUM_WORKER; i++)
        pthread_cancel(tid_worker[i]);

    for (int i = 0; i < NUM_WORKER; i++)
        pthread_join(tid_worker[i], NULL);
    free(list);

    return (0);
}