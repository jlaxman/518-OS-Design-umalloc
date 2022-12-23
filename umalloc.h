// File:	umalloc.h
// List all group members' names: Vennela Chava(vc494) Sai Laxman Jagarlamudi(sj1018)
// iLab machine tested on:ilab3

#ifndef UMALLOC_H
#define UMALLOC_H
#define _GNU_SOURCE

#include<unistd.h>
#include<stdio.h>
#include<time.h>
#include<ucontext.h>
#include<sys/syscall.h>
#include<sys/types.h>
#include<stdlib.h>
#include<signal.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<sys/time.h>
#include<stddef.h>


#define malloc(X) umalloc(X, __FILE__, __LINE__)
#define free(Y) ufree(Y, __FILE__, __LINE__)
//static char array mem's size that is 10MB
#define memory_size 10485760
//char named init to check if the memory or user data is initialized or not 
extern char init;
//user defined functions for malloc and free
void* umalloc(size_t, char*, int);
void ufree(void*, char*, int);
//other functions used in the implementation of umalloc and ufree
//to track the usage of current memory block (pointed by metadata_ptr)
unsigned short currently_using(unsigned char*);
//function to calculate size of metadata (based on number of bytes requested by the user)
unsigned short metadata_size(unsigned char*);
//function to calculate size of payload (taking pointer to metadata as parameter)
unsigned int payload_chunk(unsigned char*);
//function to calculate size of payload (taking overall size as parameter)
unsigned int payload_size(unsigned int);
//function to set the metadata information 
void set_metadata(unsigned char* metadata_ptr, unsigned short using, unsigned int size);
//function to unset current metadata information
void unset_metadata(unsigned char* metadata_ptr);

#endif