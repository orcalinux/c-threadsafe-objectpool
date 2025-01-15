#include <stdio.h>
#include <assert.h>
#include "object_pool.h"
#include "cli_logger.h"

int main()
{
    ObjectPool *pool = NULL;

    // Initialize the object pool
    if (!object_pool_init(&pool, 5, sizeof(double)))
    {
        log_error("Failed to initialize object pool.");
        return 1;
    }

    // Acquire an object
    double *val = (double *)object_pool_acquire(pool);
    assert(val != NULL);
    *val = 3.14;
    assert(*val == 3.14);
    log_info("Acquired and set value to %.2f.", *val);

    // Release the object
    object_pool_release(pool, val);
    log_info("Released object with value %.2f.", *val);

    // Attempt to acquire the same object again
    double *val2 = (double *)object_pool_acquire(pool);
    assert(val2 != NULL);
    *val2 = 6.28;
    assert(*val2 == 6.28);
    log_info("Re-acquired and set value to %.2f.", *val2);

    // Release the object again
    object_pool_release(pool, val2);
    log_info("Released object with value %.2f.", *val2);

    // Destroy the pool
    object_pool_destroy(pool);
    log_info("Object pool destroyed successfully.");

    printf("[INFO]: All simple tests passed successfully.\n");
    return 0;
}
