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

#define MAX_LINE_LENGTH 200

using namespace std;


int child_procces(){

    int child_pid = getpid();
    char file_sha_mem[50];
    sprintf(file_sha_mem,"keys/shared_mem_info.key");
 

    // Attach the shared memory segment.
    int I_mem_id=get_memory_id_from_file(file_sha_mem , shared_mem_info_size);
    shared_mem_info* shared_memory_inf = (shared_mem_info*) shmat(I_mem_id, NULL, 0);
    if(shared_memory_inf==(void*)-1)die("shared memory atached problem");

    //to-print-delete
    cout << "sheared_memory_detail_child:!~~~~  lines: " << shared_memory_inf->lines_per_segm <<
    "request_per_child: " << shared_memory_inf->request_per_child << endl;

    
   
    
    return 0;

}


#endif
