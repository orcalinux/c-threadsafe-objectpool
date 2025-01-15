#include "object_pool.h"
#include "cli_logger.h"
#include <string.h>
#include <stdio.h>

bool object_pool_init(ObjectPool *pool, size_t initial_size, size_t object_size)
{
    if (!pool)
    {
        log_error("object_pool_init received NULL pool pointer.");
        return false;
    }
    if (initial_size == 0)
    {
        log_error("object_pool_init received initial_size of 0.");
        return false;
    }
    if (object_size == 0)
    {
        log_error("object_pool_init received object_size of 0.");
        return false;
    }

    pool->object_size = object_size;
    pool->pool_size = initial_size;
    pool->available = initial_size;

    // Allocate memory for objects
    pool->memory_block = malloc(initial_size * object_size);
    if (!pool->memory_block)
    {
        log_error("Failed to allocate memory_block in object_pool_init.");
        return false;
    }

    // Allocate memory for the free list
    pool->free_list = malloc(initial_size * sizeof(void *));
    if (!pool->free_list)
    {
        log_error("Failed to allocate free_list in object_pool_init.");
        free(pool->memory_block);
        pool->memory_block = NULL;
        return false;
    }

    // Initialize the free list
    for (size_t i = 0; i < initial_size; ++i)
    {
        pool->free_list[i] = (char *)pool->memory_block + i * object_size;
    }

    // Initialize the mutex
    if (pthread_mutex_init(&pool->lock, NULL) != 0)
    {
        log_error("Failed to initialize mutex in object_pool_init.");
        free(pool->memory_block);
        free(pool->free_list);
        pool->memory_block = NULL;
        pool->free_list = NULL;
        return false;
    }

    log_info("Object pool initialized with size %zu and object size %zu.", initial_size, object_size);
    return true;
}

void *object_pool_acquire(ObjectPool *pool)
{
    if (!pool)
    {
        log_error("object_pool_acquire received NULL pool pointer.");
        return NULL;
    }

    pthread_mutex_lock(&pool->lock);

    void *object = NULL;
    if (pool->available > 0)
    {
        object = pool->free_list[--pool->available];
        // Optionally, clear the memory for safety
        memset(object, 0, pool->object_size);
        log_info("Acquired object. Available objects: %zu.", pool->available);
    }
    else
    {
        log_warning("Object pool exhausted in object_pool_acquire.");
    }

    pthread_mutex_unlock(&pool->lock);

    return object;
}

void object_pool_release(ObjectPool *pool, void *object)
{
    if (!pool)
    {
        log_error("object_pool_release received NULL pool pointer.");
        return;
    }
    if (!object)
    {
        log_error("object_pool_release received NULL object pointer.");
        return;
    }

    pthread_mutex_lock(&pool->lock);

    if (pool->available < pool->pool_size)
    {
        pool->free_list[pool->available++] = object;
        log_info("Released object. Available objects: %zu.", pool->available);
    }
    else
    {
        log_warning("Attempt to release more objects than the pool size in object_pool_release.");
    }

    pthread_mutex_unlock(&pool->lock);
}

bool object_pool_resize(ObjectPool *pool, size_t new_size)
{
    if (!pool)
    {
        log_error("object_pool_resize received NULL pool pointer.");
        return false;
    }
    if (new_size <= pool->pool_size)
    {
        log_error("object_pool_resize received new_size <= current pool_size.");
        return false;
    }

    pthread_mutex_lock(&pool->lock);

    // Reallocate memory for the objects
    void *new_memory_block = realloc(pool->memory_block, new_size * pool->object_size);
    if (!new_memory_block)
    {
        log_error("Failed to reallocate memory_block in object_pool_resize.");
        pthread_mutex_unlock(&pool->lock);
        return false; // Reallocation failed
    }
    pool->memory_block = new_memory_block;

    // Reallocate memory for the free list
    void **new_free_list = realloc(pool->free_list, new_size * sizeof(void *));
    if (!new_free_list)
    {
        log_error("Failed to reallocate free_list in object_pool_resize.");
        pthread_mutex_unlock(&pool->lock);
        return false; // Reallocation failed
    }
    pool->free_list = new_free_list;

    // Initialize new objects in the free list
    for (size_t i = pool->pool_size; i < new_size; ++i)
    {
        pool->free_list[i] = (char *)pool->memory_block + i * pool->object_size;
    }

    pool->available += (new_size - pool->pool_size);
    log_info("Resized object pool from %zu to %zu. Available objects: %zu.", pool->pool_size, new_size, pool->available);
    pool->pool_size = new_size;

    pthread_mutex_unlock(&pool->lock);

    return true;
}

void object_pool_destroy(ObjectPool *pool)
{
    if (!pool)
    {
        log_error("object_pool_destroy received NULL pool pointer.");
        return;
    }

    // Lock the mutex to ensure no other thread is accessing the pool
    pthread_mutex_lock(&pool->lock);

    // Perform cleanup while holding the lock
    free(pool->memory_block);
    free(pool->free_list);
    pool->memory_block = NULL;
    pool->free_list = NULL;
    pool->pool_size = 0;
    pool->available = 0;

    // Unlock the mutex before destroying it
    pthread_mutex_unlock(&pool->lock);

    // Now destroy the mutex
    if (pthread_mutex_destroy(&pool->lock) != 0)
    {
        log_warning("Failed to destroy mutex in object_pool_destroy.");
    }
    else
    {
        log_info("Mutex destroyed successfully.");
    }

    log_info("Object pool destroyed.");
}
