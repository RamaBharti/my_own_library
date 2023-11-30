#include "my_mmu.h"

int main() {
    // Test case 1: Allocate and free an integer
    int* int_ptr = (int*)my_malloc(sizeof(int));
    if (int_ptr != NULL) {
        *int_ptr = 42;
        printf("Test 1: value: %d\n", *int_ptr);
        my_free(int_ptr);
    }

    // Test case 2: Allocate an array of integers and initialize to zero
    int* arr = (int*)my_calloc(5, sizeof(int));
    if (arr != NULL) {
        printf("Test 2:\n");
        for (int i = 0; i < 5; i++) {
            printf("arr[%d]: %d\n", i, arr[i]);
        }
        my_free(arr);
    }

    // Test case 3: Allocate a large memory block using mmap
    char* large_block = (char*)my_malloc(2048);
    if (large_block != NULL) {
        strcpy(large_block, "This is a large block allocated using mmap.");
        printf("Test 3: large_block: %s\n", large_block);
        my_free(large_block);
    }

    // Test case 4: Debugging information
    // Allocate and deallocate some memory
    int* ptr = (int*)my_malloc(sizeof(int));
    int* arr1 = (int*)my_calloc(5, sizeof(int));
    printf("Test 4:\n");
    // Print heap information
    debug();

    // Free memory
    my_free(ptr);
    my_free(arr1);

    // Print heap information again
    debug();

    return 0;
}




