#include <pthread.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "object_pool.h"
#include "cli_logger.h"

#define THREAD_COUNT 10
#define OBJECT_COUNT 20
#define OBJECT_SIZE sizeof(int)

typedef struct
{
    ObjectPool *pool;
    int thread_num;
} ThreadArg;

// Worker function for threads
void *test_worker(void *arg)
{
    ThreadArg *thread_arg = (ThreadArg *)arg;
    ObjectPool *pool = thread_arg->pool;
    int thread_num = thread_arg->thread_num;

    for (int i = 0; i < 100; ++i)
    {
        int *obj = (int *)object_pool_acquire(pool);
        if (obj)
        {
            *obj = thread_num;
            // Simulate work (optional)
            // log_info("Thread %d acquired object with value %d", thread_num, *obj);
            object_pool_release(pool, obj);
            // log_info("Thread %d released object with value %d", thread_num, *obj);
        }
        else
        {
            // Pool exhausted
            log_warning("Thread %d failed to acquire object.", thread_num);
        }
    }

    return NULL;
}

// Callback function to print object details
void print_object(void *object, void *user_data)
{
    (void)user_data;

    if (object == NULL)
    {
        printf("Encountered a NULL object.\n");
        return;
    }

    int *val = (int *)object;
    printf("Active Object Value: %d\n", *val);
}

int main()
{
    srand(time(NULL)); // Seed for randomness

    ObjectPool *pool = NULL;

    // Initialize pool
    if (!object_pool_init(&pool, OBJECT_COUNT, OBJECT_SIZE))
    {
        log_error("Failed to initialize object pool.");
        return 1;
    }

    // Create threads
    pthread_t threads[THREAD_COUNT];
    ThreadArg args[THREAD_COUNT];
    for (int i = 0; i < THREAD_COUNT; ++i)
    {
        args[i].pool = pool;
        args[i].thread_num = i;
        if (pthread_create(&threads[i], NULL, test_worker, &args[i]) != 0)
        {
            log_error("Failed to create thread %d.", i);
            object_pool_destroy(pool);
            return 1;
        }
    }

    // Join threads
    for (int i = 0; i < THREAD_COUNT; ++i)
    {
        pthread_join(threads[i], NULL);
    }

    // Test resizing
    if (!object_pool_resize(pool, OBJECT_COUNT * 2))
    {
        log_error("Failed to resize object pool.");
        object_pool_destroy(pool);
        return 1;
    }

    log_info("Object pool resized successfully.");

    // Test acquiring after resizing
    int *test_obj = (int *)object_pool_acquire(pool);
    if (test_obj)
    {
        *test_obj = 999;
        assert(*test_obj == 999);
        log_info("Acquired and set test object to %d.", *test_obj);
        object_pool_release(pool, test_obj);
        log_info("Released test object.");
    }
    else
    {
        log_error("Failed to acquire test object after resizing.");
        object_pool_destroy(pool);
        return 1;
    }

    // Display all active objects (should be 0 at this point)
    printf("-------------------------------------------------");
    printf("\n--- Active Objects After Threads and Resizing ---\n");
    object_pool_iterate_acquired(pool, print_object, NULL);
    printf("-------------------------------------------------\n");

    // Destroy pool
    object_pool_destroy(pool);
    log_info("All tests passed successfully.");

    return 0;
}
