// File:	memgrind.c
// List all group members' names: Vennela Chava(vc494) Sai Laxman Jagarlamudi(sj1018)
// iLab machine tested on:ilab3
#include <stdlib.h>
#include <stdio.h>
#include <time.h> 
#include "umalloc.h"

            //allocate a small block (1 to 10B), cast it to a type, write to it, free it
            //allocate a block of the same size, cast it to the same type, then check to see if the address of the pointers are the same
static void Consistency(){
		void * n = malloc(4);
        printf("This is address n %p\n", n);
        // printf("%p\n", n);
        *(int*)n=3;
        // printf("%d\n", *(int*)n);
        free(n);
        void * n1 = malloc(4);
        *(int*)n1=5;
        if(n==n1)
        {
           
            printf("This is address n1 %p\n", n1);
            printf("Addresses are same!\n\n" );
            free(n1);
        }

}

//allocate 1B, if the result is not NULL, free it, double the size and try again
  //          on NULL, halve the size and try again
    //        on success after a NULL, stop
      //      free it
static unsigned int Maximization(){

    unsigned int currmemory = 1;
    unsigned int maxmemory=0;
    void* ptr = malloc(currmemory);
    while(NULL != ptr){
        free(ptr);
        currmemory = currmemory<<1;
        ptr = malloc(currmemory);
    }

    if(NULL == ptr){
        maxmemory = currmemory/2;  // maxmemory Obtained;
        //printf("max %d\n",maxmemory);
        //one = malloc(memory/2);
        while(NULL == ptr){
            currmemory = currmemory/2;
            ptr = malloc(currmemory);
        }
        if(NULL != ptr){
            free(ptr);
        }
    }
    return maxmemory;
}

        /*    allocate one half of your maximal allocation (see test 1)
            allocate one quarter of your maximal allocation
            free the first pointer
            free the second pointer
            attempt to allocate your maximal allocation - it should work
            free it
        */
static void BasicCoalescence(unsigned int maxmemory){
    void* half = malloc(maxmemory/2);
    //printf("%p\n",half );
    void* quarter = malloc(maxmemory/4);
    //printf("%p\n",quarter );
    free(half);
    free(quarter);
    void* max = malloc(maxmemory);
    if(max!=NULL){
        printf("Maxmemory is allocated!\n\n");
    }

    //printf("%p\n",max );
    free(max);
    //printf("after free\n" );
    //printf("%p\n",max );
}


/*
Saturation:
            do 9K 1KB allocations (i.e. do 9216 1024 byte allocations)
            switch to 1B allocations until malloc responds with NULL, that is your saturation of space
*/
static void Saturation(void** pt){
    struct timeval tv_start, tv_end;
   
    unsigned int j=0;
    for(int i=0; i<9216; i++){
        void* ptr=malloc(1024);
        pt[j]=ptr;
        j++;
    }
    unsigned int count=0;
    gettimeofday(&tv_start, NULL);
    void* ptr=malloc(1);
    void* prev;
    while(ptr !=NULL){
        prev=ptr;
        pt[j]=ptr;
        count++;
        ptr=malloc(1);
        j++;
    }
    gettimeofday(&tv_end, NULL);
    printf("the time taken for the saturation in secs: ");
    printf("%ld", (tv_end.tv_sec)-(tv_start.tv_sec));
    printf("\n");
    printf("Number of bytes allocated after 9K 1KB allocations %d\n\n", count);
    return ;
}
/*saturate your memory (i.e. immediately after test 3)free the last 1B block
get the current time allocate 1B get the current time, compute the elapsed time, 
that is your max time overhead 
Input: Takes memory last block pointer input as n
Output: Max time overhead

*/
static void timeOverhead(void* ptr){
    if(ptr==NULL){
        printf("Time Overhead Not Working!");
        return ;
    }
    free(ptr);
    struct timeval tv_start, tv_end;
    gettimeofday(&tv_start, NULL);
    void* new_ptr=malloc(1);
    gettimeofday(&tv_end, NULL);
    printf("Maximum timeOverhead in micro secs: ");
    printf("%ld\n\n", (tv_end.tv_usec)-(tv_start.tv_usec));
}

/* saturate your memory (i.e. immediately after test 3) free the last 1B block
   get the current time allocate 1B get the current time, compute the elapsed time, 
   that is your max time overhead.
   Input: array of pointers that need to be freed, max memmory to allocate
   Output: Check if Maxmemory can be allocated
   */
static void intermediateCoalescence(void** pt, unsigned int maxmemory){

    unsigned int i=0;
    while(i<524288){
        free(pt[i]);
        i++;
    }
    void* max = malloc(maxmemory);
    if(max!=NULL){
        printf("Maxmemory is allocated in IntermediateCoalescence!\n\n");
    }
    free(max);

}



int main() {
    
    printf("Consistency Check running! \n");
    Consistency();
    printf("Maximisation Compute running! \n");
    unsigned int maxmemory = Maximization();
    printf("Maximum memory: %d\n\n",maxmemory);
    printf("Basic Coalescence running! \n");
    BasicCoalescence(maxmemory);
    //This number 524288 is precomputed based on saturation ptrs we get to store all pointers in single array of pointers
    void *pt[524288];
    printf("Saturation running! \n");
    Saturation(pt);
    printf("TimeOverhead running! \n");
    timeOverhead(pt[524287]);
    printf("IntermediateCoalescence running! \n");
    intermediateCoalescence(pt, maxmemory);
    return 0;
}
