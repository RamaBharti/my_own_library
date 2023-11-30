#ifndef MMU_H
#define MMU_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#define MMAP_THRESHOLD 1024 

// Include your Headers below

// You are not allowed to use the function malloc and calloc directly .

//defining a struct to represent memory block
struct MemBlock{
    size_t size; //size of block
    struct MemBlock* next; //pointer to the next block
};

//defining a pointer to keep track of the free memory block list
static struct MemBlock* free_list=NULL;


// Function to allocate memory using mmap
void* my_mmap(size_t size){
    void* ptr=mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);

    if(ptr==MAP_FAILED){
        perror("mmap");
        return NULL;
    }
    return ptr;
}

//function to allocate using heap (brk & sbrk)
void* my_heap_alloc(size_t size){
    //for better performance size is aligned to a multiple of the system's page size
    size_t pageSize=(size_t)sysconf(_SC_PAGESIZE);
    size_t alignedSize=(size+pageSize-1)/pageSize*pageSize;

    //allocating memory using brk and sbrk
    void* ptr=sbrk(0); //get the current program break
    if(ptr==(void*)-1){
        perror("sbrk");
        return NULL; //allocation failed
    }
    //check if the current program break is page aligned
    size_t offset=(size_t)ptr%pageSize;
    if(offset!=0){
        //adjust the program break to the next page boundary
        offset=pageSize-offset;
        if(sbrk(offset)==(void*)-1){
            perror("sbrk");
            return NULL; //allocation failed
        }
    }
    //update the program break to allocate memory
    if(sbrk(alignedSize)==(void*)-1){
        perror("sbrk");
        return NULL;//allocation failed
    }
    return ptr; 
}

void* my_malloc(size_t size) {
    if (size >= MMAP_THRESHOLD) {
        void* ptr=my_mmap(size);
        if (ptr!=NULL) {
            struct MemBlock* block= (struct MemBlock*)ptr;
            block->size = size;
            return (void*)(block + 1); //Adjust the pointer to the user data
        }
    } else {
        void* ptr=my_heap_alloc(size);
        if (ptr!=NULL) {
            struct MemBlock* block= (struct MemBlock*)ptr;
            block->size= size;
            return (void*)(block + 1); //Adjust the pointer to the user data
        }
    }
    return NULL;
}

// Function to allocate and initialize memory to zero using mmap
void* my_calloc(size_t nelem, size_t size) {
    // Your implementation of my_calloc goes here
    size_t total_size=nelem*size;
    void* ptr=my_malloc(total_size);

    if(ptr!=NULL) memset(ptr, 0, total_size);

    return ptr;
}

// Function to release memory allocated using my_malloc and my_calloc
void my_free(void* ptr){
    // Your implementation of my_free goes here
    if(ptr!=NULL){
        struct MemBlock* block=(struct MemBlock*)ptr-1;
        //if the block was allocated using map
        if(block->size >= MMAP_THRESHOLD){
            if(munmap(block, block->size)!=0) perror("munmap");
        }else{
            //otherwise the block was allocated using brk/sbrk
            block->next=free_list;
            free_list=block;
        }
    }
}

void debug(){
    struct MemBlock* current=free_list;
    size_t total_free_memory=0;
    int block_count=0;

    while(current!=NULL){
        total_free_memory+=current->size;
        block_count++;
        current=current->next;
    }
    fprintf(stderr, "Heap Information:\n");
    fprintf(stderr, "Total Free Block: %d\n", block_count);
    fprintf(stderr, "Total Free Memory: %lu bytes\n", total_free_memory);

    fflush(stderr);
}

#endif

