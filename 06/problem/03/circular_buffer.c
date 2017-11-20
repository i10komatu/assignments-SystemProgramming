#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>

#define QSIZE 10 // キューの長さ
typedef struct{
    pthread_mutex_t buf_lock;   // 構造体のロック
    int start;                  // バッファの開始
    int num_full;               // データの数
    pthread_cond_t notfull;     // notfullの条件変数
    pthread_cond_t notempty;    // notemptyの条件変数
    void *data[QSIZE];          // 巡回バッファ
} circ_buf_t;

pthread_mutex_t lock_enq = PTHREAD_MUTEX_INITIALIZER;
int data[60];
int count = 0;

/*** circular_buffer ***/
void circ_buf_init(circ_buf_t *cbp) {
    pthread_mutex_init(&cbp->buf_lock, NULL);
    pthread_cond_init(&cbp->notfull, NULL);
    pthread_cond_init(&cbp->notempty, NULL);
}

void put_cb_data (circ_buf_t *cbp, void *data) {
    pthread_mutex_lock(&cbp->buf_lock);

    /* wait while the buffer is full */
    while (cbp->num_full == QSIZE)
        pthread_cond_wait(&cbp->notfull, &cbp->buf_lock);
    
    cbp->data[(cbp->start + cbp->num_full) % QSIZE] = data;
    cbp->num_full++;

    /* let a waiting reader know there's data */
    pthread_cond_signal(&cbp->notempty);
    pthread_mutex_unlock(&cbp->buf_lock);
}

void *get_cb_data(circ_buf_t *cbp) {
    void *data;
    
    pthread_mutex_lock(&cbp->buf_lock);

    /* wait while there's nothing in the buffer */
    while (cbp->num_full == 0)
        pthread_cond_wait(&cbp->notempty, &cbp->buf_lock);

    data = cbp->data[cbp->start];
    cbp->start = (cbp->start + 1) % QSIZE;
    cbp->num_full--;

    /* let a waiting writer know there's room */
    pthread_cond_signal(&cbp->notfull);
    pthread_mutex_unlock(&cbp->buf_lock);

    return data;
}

/*** test ***/
pid_t gettid() {
    return syscall(SYS_gettid);
}

void *enqueue(void *arg) {
    circ_buf_t *cbp = (circ_buf_t*)arg;
    
    for (int i = 0; i < 30; i++) {
        pthread_mutex_lock(&lock_enq);
        data[count] = count;
        put_cb_data(cbp, &data[count]);
        printf("Put(%d):%d\n", gettid(), count);
        count++;
        pthread_mutex_unlock(&lock_enq);
    }
}

void *dequeue(void *arg) {
    circ_buf_t *cbp = (circ_buf_t*)arg;

    int *cnt;
    
    for (int i = 0; i < 10; i++) {
        cnt = (int*)get_cb_data(cbp);
        printf("Get(%d):%d\n", gettid(), *cnt);
    }
}

int main() {
    pthread_t tid_enq[2], tid_deq[6];
    circ_buf_t cb;
    circ_buf_init(&cb);

    for (int i = 0; i < 2; i++) {
        pthread_create(&tid_enq[i], NULL, enqueue, &cb);
    }
    for (int i = 0; i < 6; i++) {
        pthread_create(&tid_deq[i], NULL, dequeue, &cb);
    }
    for (int i = 0; i < 2; i++) {
        pthread_join(tid_enq[i], NULL);
    }
    for (int i = 0; i < 6; i++) {
        pthread_join(tid_deq[i], NULL);
    }

    return 0;
}
