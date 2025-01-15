// tests/test_employee_pool.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include "object_pool.h"
#include "cli_logger.h"

typedef struct Employee
{
    int id;
    char name[50];
} Employee;

int main()
{
    srand(time(NULL)); // Seed for randomness

    ObjectPool pool;
    size_t initial_size = 7;
    size_t object_size = sizeof(Employee);

    // Initialize the object pool
    if (!object_pool_init(&pool, initial_size, object_size))
    {
        log_error("Failed to initialize the Employee object pool.");
        return EXIT_FAILURE;
    }
    log_info("Employee object pool initialized with size %zu.", initial_size);

    // Array to hold pointers to acquired Employees
    Employee *employees[10] = {0}; // Allow up to 10 for testing

    // Acquire 7 Employees from the pool
    for (int i = 0; i < 7; i++)
    {
        employees[i] = (Employee *)object_pool_acquire(&pool);
        if (employees[i] == NULL)
        {
            log_warning("Failed to acquire Employee %d from the pool.", i + 1);
            continue;
        }
        employees[i]->id = i + 1;
        snprintf(employees[i]->name, sizeof(employees[i]->name), "Employee_%d", i + 1);
        log_info("Acquired Employee ID: %d, Name: %s", employees[i]->id, employees[i]->name);
    }

    // Attempt to acquire the 8th Employee, which may trigger resizing
    employees[7] = (Employee *)object_pool_acquire(&pool);
    if (employees[7] != NULL)
    {
        employees[7]->id = 8;
        snprintf(employees[7]->name, sizeof(employees[7]->name), "Employee_%d", 8);
        log_info("Acquired Employee ID: %d, Name: %s", employees[7]->id, employees[7]->name);
    }
    else
    {
        log_warning("Failed to acquire the 8th Employee from the pool.");
    }

    // Randomly release and acquire Employees
    for (int i = 0; i < 10; i++)
    {
        int action = rand() % 2; // 0 for release, 1 for acquire

        if (action == 0)
        { // Release
            // Find a non-NULL employee to release
            int release_index = -1;
            for (int j = 0; j < 8; j++)
            {
                if (employees[j] != NULL)
                {
                    release_index = j;
                    break;
                }
            }
            if (release_index != -1)
            {
                object_pool_release(&pool, employees[release_index]);
                log_info("Released Employee ID: %d, Name: %s", employees[release_index]->id, employees[release_index]->name);
                employees[release_index] = NULL;
            }
            else
            {
                log_warning("No Employees available to release.");
            }
        }
        else
        { // Acquire
            // Find the first NULL slot to acquire into
            int acquire_index = -1;
            for (int j = 0; j < 8; j++)
            {
                if (employees[j] == NULL)
                {
                    acquire_index = j;
                    break;
                }
            }
            if (acquire_index != -1)
            {
                employees[acquire_index] = (Employee *)object_pool_acquire(&pool);
                if (employees[acquire_index] != NULL)
                {
                    employees[acquire_index]->id = 100 + acquire_index;
                    snprintf(employees[acquire_index]->name, sizeof(employees[acquire_index]->name), "Employee_%d", 100 + acquire_index);
                    log_info("Acquired Employee ID: %d, Name: %s", employees[acquire_index]->id, employees[acquire_index]->name);
                }
                else
                {
                    log_warning("Failed to acquire Employee %d from the pool.", acquire_index + 1);
                }
            }
            else
            {
                log_warning("No available slots to acquire new Employees.");
            }
        }
    }

    // Cleanup: Release any remaining Employees
    for (int i = 0; i < 8; i++)
    {
        if (employees[i] != NULL)
        {
            object_pool_release(&pool, employees[i]);
            log_info("Released Employee ID: %d, Name: %s", employees[i]->id, employees[i]->name);
            employees[i] = NULL;
        }
    }

    // Destroy the object pool
    object_pool_destroy(&pool);
    log_info("Employee object pool destroyed successfully.");

    printf("[INFO]: All Employee object pool tests passed successfully.\n");
    return EXIT_SUCCESS;
}
