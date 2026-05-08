/* blocking_pool.h — blocking thread pool for Zeta Tokio AOT runtime */
#include <pthread.h>
#include <stdlib.h>
#include <stdint.h>

#define POOL_SIZE 4

typedef void (*thunk_fn)(void*);

typedef struct {
    thunk_fn func;
    void *arg;
    int64_t result;
    pthread_mutex_t lock;
    pthread_cond_t done;
    int finished;
} task_t;

static pthread_t g_pool[POOL_SIZE];
static task_t g_queue[256];
static int g_queue_head = 0, g_queue_tail = 0, g_queue_count = 0;
static pthread_mutex_t g_queue_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t g_queue_cond = PTHREAD_COND_INITIALIZER;
static int g_pool_initialized = 0;

static void* worker_loop(void *arg) {
    (void)arg;
    while (1) {
        pthread_mutex_lock(&g_queue_lock);
        while (g_queue_count == 0)
            pthread_cond_wait(&g_queue_cond, &g_queue_lock);
        
        task_t t = g_queue[g_queue_head];
        g_queue_head = (g_queue_head + 1) % 256;
        g_queue_count--;
        pthread_mutex_unlock(&g_queue_lock);
        
        if (t.func) {
            t.func(t.arg);
        }
        pthread_mutex_lock(&t.lock);
        t.finished = 1;
        pthread_cond_signal(&t.done);
        pthread_mutex_unlock(&t.lock);
    }
    return NULL;
}

static void ensure_pool(void) {
    if (!g_pool_initialized) {
        g_pool_initialized = 1;
        for (int i = 0; i < POOL_SIZE; i++)
            pthread_create(&g_pool[i], NULL, worker_loop, NULL);
    }
}

int64_t blocking_spawn(int64_t fn_ptr) {
    ensure_pool();
    
    task_t t;
    t.func = (thunk_fn)fn_ptr;
    t.arg = (void*)fn_ptr;  /* Pass fn ptr as arg for now */
    t.result = 0;
    t.finished = 0;
    pthread_mutex_init(&t.lock, NULL);
    pthread_cond_init(&t.done, NULL);
    
    pthread_mutex_lock(&g_queue_lock);
    g_queue[g_queue_tail] = t;
    g_queue_tail = (g_queue_tail + 1) % 256;
    g_queue_count++;
    pthread_cond_signal(&g_queue_cond);
    pthread_mutex_unlock(&g_queue_lock);
    
    return 1; /* Simplified handle */
}
