#ifndef SEM_SHARED_MEM
#define SEM_SHARED_MEM

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/sem.h>
//#include <semaphore.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <iostream>



#include <iostream>
//#include "shared_memory_class.hpp"

#define shared_mem_resp_size sizeof(shared_mem_resp)
#define shared_mem_req_size sizeof(shared_mem_req)
#define shared_mem_data_log_size sizeof(shared_mem_data_log)
 
//sem_t* generate_semaphore(char* name , mode_t modes, int val);
void die(char er[1000]);
int generate_memory_segment(key_t mem_key, size_t mem_sz, char* file_name);
int get_memory_id_from_file(char* file_name, int mem_sz);
void clear_mem(char* mem_file, int mem_sz);

int generate_semaphore(key_t sem_key, char* file_name, unsigned int num=0);
int get_semaphore_id_from_file(char* file_name, unsigned int num=0);
void initialise_semaphore(int sem_id);
int semaphore_wait(int sem_id, char name[50]);
int semaphore_signal(int sem_id, char name[50]);
void clear_sem(char* sem_file, unsigned int num=0);


#endif
