#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mMemoryMgr.h>

void test_custom_memory_manager_allocation(size_t num_allocations, size_t allocation_size) {
    char *ptrs[num_allocations];
    clock_t start = clock();
    for (size_t i = 0; i < num_allocations; ++i) {
        ptrs[i] = (char *)mMemoryMalloc(allocation_size);
    }
    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Custom memory manager allocation: %zu allocations of size %zu took %f seconds.\n", num_allocations, allocation_size, time_spent);

    for (size_t i = 0; i < num_allocations; ++i) {
        
        mMemoryFree(ptrs[i]);
    }
}

void test_custom_memory_manager_free(size_t num_allocations, size_t allocation_size) {
    char *ptrs[num_allocations];
    for (size_t i = 0; i < num_allocations; ++i) {
        ptrs[i] = (char *)mMemoryMalloc(allocation_size);
    }

    clock_t start = clock();
    for (size_t i = 0; i < num_allocations; ++i) {
        mMemoryFree(ptrs[i]);
    }
    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Custom memory manager free: %zu allocations of size %zu took %f seconds.\n", num_allocations, allocation_size, time_spent);
}

void test_standard_malloc_allocation(size_t num_allocations, size_t allocation_size) {
    char *ptrs[num_allocations];
    clock_t start = clock();
    for (size_t i = 0; i < num_allocations; ++i) {
        ptrs[i] = (char *)malloc(allocation_size);
    }
    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Standard malloc allocation: %zu allocations of size %zu took %f seconds.\n", num_allocations, allocation_size, time_spent);

    for (size_t i = 0; i < num_allocations; ++i) {
        free(ptrs[i]);
    }
}

void test_standard_free(size_t num_allocations, size_t allocation_size) {
    char *ptrs[num_allocations];
    for (size_t i = 0; i < num_allocations; ++i) {
        ptrs[i] = (char *)malloc(allocation_size);
    }

    clock_t start = clock();
    for (size_t i = 0; i < num_allocations; ++i) {
        free(ptrs[i]);
    }
    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Standard free: %zu allocations of size %zu took %f seconds.\n", num_allocations, allocation_size, time_spent);
}

void test_custom_memory_manager(size_t num_allocations, size_t allocation_size) {
    clock_t start = clock();
    for (size_t i = 0; i < num_allocations; ++i) {
        char *ptr = (char *)mMemoryMalloc(allocation_size);
        mMemoryFree(ptr);
    }
    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Custom memory manager: %zu allocations of size %zu took %f seconds.\n", num_allocations, allocation_size, time_spent);
}

void test_standard_malloc(size_t num_allocations, size_t allocation_size) {
    clock_t start = clock();
    for (size_t i = 0; i < num_allocations; ++i) {
        char *ptr = (char *)malloc(allocation_size);
        free(ptr);
    }
    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Standard malloc/free: %zu allocations of size %zu took %f seconds.\n", num_allocations, allocation_size, time_spent);
}

int main() {
    size_t num_allocations = 400000;
    size_t allocation_size = 256;
    
    size_t partsMode = 0;
    MMGR_INIT_PARTS_MODE_BIT(partsMode, 0, 1);
    MMGR_INIT_PARTS_MODE_BIT(partsMode, 1, 0);
    mMemoryMgrInit(2048000, 2, partsMode);
    test_custom_memory_manager_allocation(num_allocations, allocation_size);
    test_standard_malloc_allocation(num_allocations, allocation_size);

    test_custom_memory_manager_free(num_allocations, allocation_size);
    test_standard_free(num_allocations, allocation_size);

    test_custom_memory_manager(num_allocations, allocation_size);
    test_standard_malloc(num_allocations, allocation_size);

    return 0;
}
