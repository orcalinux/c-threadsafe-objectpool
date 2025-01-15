// tests/test_simple.c
#include <stdio.h>
#include <assert.h>
#include "object_pool.h"
#include "cli_logger.h"

int main()
{
    ObjectPool pool;
    assert(object_pool_init(&pool, 5, sizeof(double)));

    double *val = (double *)object_pool_acquire(&pool);
    assert(val != NULL);
    *val = 3.14;
    assert(*val == 3.14);
    object_pool_release(&pool, val);

    object_pool_destroy(&pool);
    printf("[INFO]: All simple tests passed successfully.\n");
    return 0;
}
