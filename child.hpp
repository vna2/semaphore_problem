#ifndef CHILD_MOD
#define CHILD_MOD

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <iostream>
#include <cstring>
#include <string.h>
#include "shared_memory_class.hpp"
#include "semaphores.hpp"

#define MAX_LINE_LENGTH 80

using namespace std;


int child_procces(){

    int child_pid = getpid();
    char file_sha_mem[50];
    sprintf(file_sha_mem,"keys/shared_mem.key");
 

    // Attach the shared memory segment.
    int I_mem_id=get_memory_id_from_file(file_sha_mem , shared_mem_size);
    shared_mem* shared_memory = (shared_mem*) shmat(I_mem_id, NULL, 0);
    if(shared_memory==(void*)-1)die("shared memory atached problem");

    //print-details
    cout << child_pid << ": " << shared_memory->file_seg_name << endl;

    // Detach the shared memory segment
    shmdt(shared_memory);


    
    return 0;

}


#endif
