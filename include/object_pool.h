#ifndef OBJECT_POOL_H
#define OBJECT_POOL_H

#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

/**
 * @file object_pool.h
 * @brief Thread-safe Object Pool for efficient memory management.
 */

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @struct AcquiredNode
     * @brief Structure representing a node in the acquired objects list.
     */
    typedef struct AcquiredNode
    {
        void *object;
        struct AcquiredNode *next;
    } AcquiredNode;

    /**
     * @struct ObjectPool
     * @brief Structure representing the Object Pool.
     */
    typedef struct ObjectPool
    {
        void **free_list;            /**< Array of pointers to free objects */
        size_t object_size;          /**< Size of each object */
        size_t pool_size;            /**< Current pool size */
        size_t available;            /**< Number of free objects */
        void *memory_block;          /**< Pointer to the memory block */
        pthread_mutex_t lock;        /**< Mutex for thread safety */
        AcquiredNode *acquired_head; /**< Head of the acquired objects list */
    } ObjectPool;

    // Callback function type for iterating over acquired objects
    typedef void (*object_callback)(void *object, void *user_data);

    /**
     * @brief Initialize the object pool with a dynamic memory block.
     *
     * @param pool Pointer to the ObjectPool structure.
     * @param initial_size Initial number of objects in the pool.
     * @param object_size Size of each object in bytes.
     * @return true on success, false on failure.
     */
    bool object_pool_init(ObjectPool **pool, size_t initial_size, size_t object_size);

    /**
     * @brief Acquire an object from the pool.
     *
     * @param pool Pointer to the ObjectPool structure.
     * @return Pointer to the acquired object, or NULL if the pool is empty.
     */
    void *object_pool_acquire(ObjectPool *pool);

    /**
     * @brief Release an object back to the pool.
     *
     * @param pool Pointer to the ObjectPool structure.
     * @param object Pointer to the object to be released.
     */
    void object_pool_release(ObjectPool *pool, void *object);

    /**
     * @brief Resize the pool to add more objects.
     *
     * @param pool Pointer to the ObjectPool structure.
     * @param new_size The new size of the pool.
     * @return true on success, false on failure.
     */
    bool object_pool_resize(ObjectPool *pool, size_t new_size);

    /**
     * @brief Destroy the object pool and free its memory.
     *
     * @param pool Pointer to the ObjectPool structure.
     */
    void object_pool_destroy(ObjectPool *pool);

    /**
     * @brief Iterate over all acquired objects in the pool.
     *
     * @param pool Pointer to the ObjectPool structure.
     * @param callback Callback function to be called for each object.
     * @param user_data User data to be passed to the callback function.
     */
    void object_pool_iterate_acquired(ObjectPool *pool, object_callback callback, void *user_data);

#ifdef __cplusplus
}
#endif

#endif // OBJECT_POOL_H
