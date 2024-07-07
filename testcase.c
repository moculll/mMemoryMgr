#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <mMemoryMgr.h>

void test_memory_manager_init() {
    size_t partsMode = 0;
    MMGR_INIT_PARTS_MODE_BIT(partsMode, 0, 0);
    MMGR_INIT_PARTS_MODE_BIT(partsMode, 1, 1);
    mMemoryMgrInit(20480000, 2, partsMode);
   
    printf("Memory manager initialization test passed.\n");
}

void test_memory_allocation_free() {
    char *test_array = (char *)mMemoryMalloc(1024);
    assert(test_array != NULL);
    strcpy(test_array, "[test][test][test][test][test][test][test][test][test][test][test][test][test][test][test][test]");
    printf("Allocated memory contains: %s\n", test_array);
    mMemoryFree(test_array);
    printf("Freed memory, content should be invalid or garbage now: %s\n", test_array);
    printf("Memory allocation and free test passed.\n");
}

void test_multiple_allocations() {
    char *test_array1 = (char *)mMemoryMalloc(512);
    char *test_array2 = (char *)mMemoryMalloc(256);
    char *test_array3 = (char *)mMemoryMalloc(128);

    assert(test_array1 != NULL && test_array2 != NULL && test_array3 != NULL);

    strcpy(test_array1, "Array1");
    strcpy(test_array2, "Array2");
    strcpy(test_array3, "Array3");

    printf("Test array1: %s\n", test_array1);
    printf("Test array2: %s\n", test_array2);
    printf("Test array3: %s\n", test_array3);

    mMemoryFree(test_array1);
    mMemoryFree(test_array2);
    mMemoryFree(test_array3);

    printf("Multiple memory allocations and frees test passed.\n");
}

void test_performance(size_t num_allocations, size_t allocation_size) {
    clock_t start = clock();
    for (size_t i = 0; i < num_allocations; ++i) {
        char *ptr = (char *)mMemoryMalloc(allocation_size);
        assert(ptr != NULL);
        mMemoryFree(ptr);
    }
    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Performance test: %zu allocations of size %zu took %f seconds.\n", num_allocations, allocation_size, time_spent);
}

void test_random_allocations(size_t num_allocations, size_t max_size) {
    srand(time(NULL));
    for (size_t i = 0; i < num_allocations; ++i) {
        size_t size = rand() % max_size + 1;
        char *ptr = (char *)mMemoryMalloc(size);
        assert(ptr != NULL);
        mMemoryFree(ptr);
    }
    printf("Random size allocations test passed.\n");
}

int main() {
    test_memory_manager_init();
    test_memory_allocation_free();
    test_multiple_allocations();
    test_performance(10000, 256);
    test_random_allocations(10000, 1024);
    return 0;
}
