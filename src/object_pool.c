#include "object_pool.h"
#include <stdlib.h>
#include <string.h>
#include "cli_logger.h"

// Helper function to allocate memory and handle errors
static void *allocate_memory(size_t size, const char *error_msg, ObjectPool *pool, bool free_pool)
{
    void *ptr = malloc(size);
    if (!ptr)
    {
        log_error(error_msg);
        if (free_pool)
        {
            free(pool->memory_block);
            free(pool->free_list);
            free(pool);
        }
    }
    return ptr;
}

// Initializes the object pool
bool object_pool_init(ObjectPool **pool_ptr, size_t initial_size, size_t object_size)
{
    if (!pool_ptr || initial_size == 0 || object_size == 0)
    {
        log_error("Invalid parameters for object_pool_init.");
        return false;
    }

    ObjectPool *pool = (ObjectPool *)malloc(sizeof(ObjectPool));
    if (!pool)
    {
        log_error("Failed to allocate memory for ObjectPool.");
        return false;
    }

    pool->memory_block = allocate_memory(initial_size * object_size, "Failed to allocate memory for object pool.", pool, false);
    if (!pool->memory_block)
    {
        free(pool);
        return false;
    }

    pool->free_list = allocate_memory(initial_size * sizeof(void *), "Failed to allocate memory for free list.", pool, true);
    if (!pool->free_list)
    {
        // allocate_memory handles freeing pool->memory_block and pool
        return false;
    }

    // Initialize free list
    for (size_t i = 0; i < initial_size; i++)
    {
        pool->free_list[i] = (char *)pool->memory_block + i * object_size;
    }

    pool->pool_size = initial_size;
    pool->object_size = object_size;
    pool->available = initial_size;
    pool->acquired_head = NULL;

    if (pthread_mutex_init(&pool->lock, NULL) != 0)
    {
        log_error("Failed to initialize mutex.");
        free(pool->free_list);
        free(pool->memory_block);
        free(pool);
        return false;
    }

    log_info("Object pool initialized with %zu objects.", initial_size);
    *pool_ptr = pool;
    return true;
}

// Helper function to add a node to the acquired list
static void add_acquired_node(ObjectPool *pool, void *obj)
{
    AcquiredNode *node = (AcquiredNode *)malloc(sizeof(AcquiredNode));
    if (!node)
    {
        log_error("Failed to allocate memory for AcquiredNode.");
        return;
    }
    node->object = obj;
    node->next = pool->acquired_head;
    pool->acquired_head = node;
}

// Helper function to remove a node from the acquired list
static int remove_acquired_node(ObjectPool *pool, void *obj)
{
    AcquiredNode *current = pool->acquired_head;
    AcquiredNode *prev = NULL;

    while (current)
    {
        if (current->object == obj)
        {
            if (prev)
            {
                prev->next = current->next;
            }
            else
            {
                pool->acquired_head = current->next;
            }
            free(current);
            return 1; // Successfully removed
        }
        prev = current;
        current = current->next;
    }
    return 0; // Object not found
}

// Acquires an object from the pool
void *object_pool_acquire(ObjectPool *pool)
{
    if (!pool)
    {
        log_error("object_pool_acquire received NULL pool pointer.");
        return NULL;
    }

    pthread_mutex_lock(&pool->lock);
    if (pool->available == 0)
    {
        pthread_mutex_unlock(&pool->lock);
        log_warning("Object pool is empty. Cannot acquire object.");
        return NULL;
    }

    void *obj = pool->free_list[--pool->available];
    add_acquired_node(pool, obj);
    pthread_mutex_unlock(&pool->lock);
    log_info("Object acquired. %zu objects remaining.", pool->available);
    return obj;
}

// Releases an object back to the pool
void object_pool_release(ObjectPool *pool, void *obj)
{
    if (!pool)
    {
        log_error("object_pool_release received NULL pool pointer.");
        return;
    }

    if (!obj)
    {
        log_warning("Attempted to release a NULL object.");
        return;
    }

    pthread_mutex_lock(&pool->lock);
    if (!remove_acquired_node(pool, obj))
    {
        pthread_mutex_unlock(&pool->lock);
        log_warning("Attempted to release an object not acquired from the pool.");
        return;
    }

    pool->free_list[pool->available++] = obj;
    pthread_mutex_unlock(&pool->lock);
    log_info("Object released. %zu objects available.", pool->available);
}

// Iterates over all acquired objects and applies a callback function
void object_pool_iterate_acquired(ObjectPool *pool, object_callback callback, void *user_data)
{
    if (!pool || !callback)
    {
        log_error("object_pool_iterate_acquired received NULL pool or callback.");
        return;
    }

    pthread_mutex_lock(&pool->lock);
    AcquiredNode *current = pool->acquired_head;
    while (current)
    {
        callback(current->object, user_data);
        current = current->next;
    }
    pthread_mutex_unlock(&pool->lock);
}

// Resizes the object pool to add more objects
bool object_pool_resize(ObjectPool *pool, size_t new_size)
{
    if (!pool)
    {
        log_error("object_pool_resize received NULL pool pointer.");
        return false;
    }

    if (new_size <= pool->pool_size)
    {
        log_error("New size must be greater than the current pool size.");
        return false;
    }

    pthread_mutex_lock(&pool->lock);

    // Calculate the number of new objects to add
    size_t additional_objects = new_size - pool->pool_size;

    // Reallocate memory block
    void *new_memory_block = realloc(pool->memory_block, new_size * pool->object_size);
    if (!new_memory_block)
    {
        log_error("Failed to reallocate memory block for resizing.");
        pthread_mutex_unlock(&pool->lock);
        return false;
    }
    pool->memory_block = new_memory_block;

    // Reallocate free list
    void **new_free_list = realloc(pool->free_list, new_size * sizeof(void *));
    if (!new_free_list)
    {
        log_error("Failed to reallocate free list for resizing.");
        pthread_mutex_unlock(&pool->lock);
        return false;
    }
    pool->free_list = new_free_list;

    // Initialize new free objects
    for (size_t i = pool->pool_size; i < new_size; i++)
    {
        pool->free_list[i] = (char *)pool->memory_block + i * pool->object_size;
    }

    pool->available += additional_objects;
    pool->pool_size = new_size;

    pthread_mutex_unlock(&pool->lock);
    log_info("Object pool resized to %zu objects.", new_size);
    return true;
}

// Destroys the object pool
void object_pool_destroy(ObjectPool *pool)
{
    if (!pool)
    {
        log_error("object_pool_destroy received NULL pool pointer.");
        return;
    }

    pthread_mutex_lock(&pool->lock);

    // Check for memory leaks: if any objects are still acquired
    if (pool->acquired_head != NULL)
    {
        log_warning("Destroying pool with still-acquired objects.");
        AcquiredNode *current = pool->acquired_head;
        while (current)
        {
            log_warning("Leaked object at %p.", current->object);
            current = current->next;
        }
    }

    // Clean up the acquired list
    AcquiredNode *current = pool->acquired_head;
    while (current)
    {
        AcquiredNode *temp = current;
        current = current->next;
        free(temp);
    }
    pool->acquired_head = NULL;

    free(pool->memory_block);
    free(pool->free_list);
    pool->memory_block = NULL;
    pool->free_list = NULL;
    pool->pool_size = 0;
    pool->available = 0;

    pthread_mutex_unlock(&pool->lock);

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
