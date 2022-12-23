// File:	umalloc.c
// List all group members' names: Vennela Chava(vc494) Sai Laxman Jagarlamudi(sj1018)
// iLab machine tested on:ilab3
#include <stdio.h>
#include <stdlib.h>
#include "umalloc.h" // Reference our defined header file
// Define our memory block location
static char mem[memory_size];
char init='n';
// Utility functions for malloc, free functions

unsigned short currently_using(unsigned char* metadata_ptr) {
    return *metadata_ptr & 1;
}
unsigned short metadata_size(unsigned char* metadata_ptr) {
    return (*metadata_ptr >> 1) & 3;
}
unsigned int payload_chunk(unsigned char* metadata_ptr) {
    unsigned short meta_size=metadata_size(metadata_ptr);
    if(meta_size==0){
        return (*metadata_ptr>>3);
    }
    else if(meta_size==1){
        return (*(unsigned short*)metadata_ptr)>>3;
    }
    else{
        return (*(unsigned int*)metadata_ptr)>>3;
    }
}
unsigned int payload_size(unsigned int size){
    if(size<32){
        return size-1;
    }
    else if(size<8192){
        return size-2;
    }
    else{
        return size-4;
    }
}
void set_metadata(unsigned char* metadata_ptr, unsigned short using, unsigned int size) {
    if(size<32){
        *(metadata_ptr)=(size<<3)+using;
    }
    else if(size<8192){
        *((short*)metadata_ptr)=(size<<3)+using+2;
    }
    else{
        *((int*)metadata_ptr)=(size<<3)+using+6;
    }
}
void unset_metadata(unsigned char* metadata_ptr) {
    set_metadata(metadata_ptr, 0, payload_chunk(metadata_ptr));
}

void* umalloc(size_t size, char* file, int line) {
    if(size<=0){
        printf("The amount of bytes requested makes no sense! Possible Bytes that can be requested: 1B-10MB in line: %d, %s\n", line, file);
        return NULL;
    }
    //the size needed for the metadata and for the given size
    unsigned int required_size=size+1;//adding 1B for the metadata
    if (required_size>=8192) {
        required_size=required_size+3;//adding 3B for metadata so total metadata to be of 4B size
    }else if (required_size>=32) {
        required_size=required_size+1;//adding 1B for metadata so total metadata to be of 1B size
    }
    if(init=='n'){
        if(required_size<=10485760){
            unsigned char* metadata_ptr = (unsigned char*)&mem[0];
            set_metadata(metadata_ptr, 1, required_size); // Set the metadata with information of the data and size
            if (10485760-required_size>0) { //checking the remaining memory
                set_metadata(metadata_ptr+required_size, 0, 10485760-required_size);
            }
            init='y';//memory is initialized
            return metadata_ptr+metadata_size(metadata_ptr)+1;
        }
        else{
            printf("Error on malloc(): OutOfMemory: Memory limit exceeded when trying to allocate in line %d, file %s\n", line, file);
            return NULL;
        }
    }
    // freeCheck to check if free space available at all, 
    // freeMem counts total free space available including discontinuos blocks
    unsigned short freeCheck=0;
    unsigned int freeMem=0;
    for(unsigned int i=0; i<memory_size; i++){
        unsigned char* metadata_ptr=(unsigned char*)&mem[i];
        if(currently_using(metadata_ptr)==0){//checking if current memory is not in use
            freeCheck=1;
            unsigned int curr_chunkSize=payload_chunk(metadata_ptr);

            
            if(curr_chunkSize==0) { // That means we are at the end of memory and no allocated memory is available after this point
                 freeMem+= memory_size-i; // To count total amount of free space available
                if (i+required_size<=memory_size){
                    set_metadata(metadata_ptr, 1, required_size);
                    set_metadata(metadata_ptr + required_size, 0, 0);
                    return metadata_ptr+metadata_size(metadata_ptr)+1;
                } 
                else {
                    break; // break to print out related error, here requested memory is greater than available memory
                }
            }
            else{
                 freeMem+= curr_chunkSize+((*metadata_ptr>>1)&3)+1; // Compute free mem by adding total size of memory available
                if(required_size<=curr_chunkSize && required_size>0){
                    set_metadata(metadata_ptr, 1, required_size); // Set the current memory address
                    if(curr_chunkSize-required_size>0){
                        set_metadata(metadata_ptr+required_size, 0, curr_chunkSize-required_size);
                    }
                    return metadata_ptr+metadata_size(metadata_ptr)+1; // Return the memory chunk
                }
            }
        }
        i=i+payload_chunk(metadata_ptr)-1;
    }

    if(freeCheck==0){
        printf("Error on malloc(): There is no free memory available at all in line %d, file: %s\n", line, file);
    }else if(freeMem<required_size){
        printf("Error on malloc(): Not enough free memory available for allocation in line %d, file: %s\n", line, file);
    }else{
        printf("Error on malloc(): Though there is enough free memory, there is no continguos block available for allocation in line: %d, file: %s\n", line, file);
    }
    return NULL;
}


void ufree(void* ptr, char* file, int line){
    if (ptr == NULL) {
        printf("Error on free(): Attempted to deallocate a NULL pointer in line %d, file: %s\n", line, file);
        return;
    }
    unsigned short freed=0;
    unsigned char* prev=NULL;
    for(unsigned int i=0; i<memory_size; i++){
        unsigned char* metadata_ptr = (unsigned char*)&mem[i];
        if(currently_using(metadata_ptr)==0){
            unsigned int curr_chunkSize=payload_chunk(metadata_ptr);
            if(curr_chunkSize==0){
                if(prev!=NULL){
                    set_metadata(prev, 0, 0); // Since these conditions mean that we are at the end of our allocated memory spaces, we can simply blanketly set these to blank
                }
                break;
            }
            else{
                if(metadata_ptr+1+metadata_size(metadata_ptr)==ptr && freed==0) {
                    printf("Error on free(): RedundantFree: Trying to free memory which already deallocated in line %d, %s\n", line, file);
                    freed=1;
                    break;
                }
                if(prev!=NULL){
                    int curr_size=payload_chunk(prev)+payload_chunk(metadata_ptr);
                    set_metadata(prev, 0, curr_size);
                }
                i=i+payload_chunk(metadata_ptr)-1;
            }
            if (prev==NULL){
                prev=metadata_ptr;
            }
        }
        else{
            if(metadata_ptr+1+metadata_size(metadata_ptr)==ptr) {
                if(payload_size(payload_chunk(metadata_ptr))!=0){
                    unset_metadata(metadata_ptr);
                }
                freed=1;
                i=i-1;
            }
            else{
                prev=NULL;
                i=i+payload_chunk(metadata_ptr)-1;
            }
        }
    }
    if (freed==0) {
        printf("Error on free(): Deallocation: Attempted to deallocate an invalid pointer in line %d, %s\n", line, file);
    }
}

