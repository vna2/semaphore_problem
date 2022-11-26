#ifndef CHILD_MOD
#define CHILD_MOD

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <iostream>
#include <cstring>
#include <string.h>
#include <semaphore.h>
#include "shared_memory_class.hpp"
#include "semaphores.hpp"

#define MAX_LINE_LENGTH 200
#define DEBUG 2
using namespace std;


int child_procces(){

    int child_pid = getpid();
    time_t t;
    srand((unsigned) time(&t));

    char file_sha_mem_inf[50];
    sprintf(file_sha_mem_inf,"keys/shared_mem_info.key");
     // Attach the shared memory segment info.
    int I_mem_id=get_memory_id_from_file(file_sha_mem_inf , shared_mem_info_size);
    shared_mem_info* shared_memory_inf = (shared_mem_info*) shmat(I_mem_id, NULL, 0);
    if(shared_memory_inf==(void*)-1)die("shared memory atached problem");

    //to-print-delete
    //cout << "sheared_memory_detail_child:!~~~~  lines: " << shared_memory_inf->lines_per_segm <<
    //"request_per_child: " << shared_memory_inf->request_per_child << endl;
    int lines_rand     =  rand() % shared_memory_inf->lines_per_segm;
    int segments_rand  =  rand() % shared_memory_inf->segments ;
    //to-print-delete
    cout << "sheared_memory_info_child:!~~~~  segments rand: " << shared_memory_inf->segments <<
    "lines rand " << shared_memory_inf->request_per_child << endl;
    char semaphore_name1[50];
    sprintf(semaphore_name1,"keys/semaphore_req%d.key",segments_rand);
    int sepaphore_id1 = get_semaphore_id_from_file(semaphore_name1);
    
    #if DEBUG >= 1
        printf("Child: %d Wait_semaphore: %d\n", getpid(),semaphore_name1);
    #endif

    //to-print-delete
    cout << "pernaaaaa1" << endl;
    char file_sha_mem_req[50];
    sprintf(file_sha_mem_req,"keys/shared_mem_info.key");
    // Attach the shared memory segment.
    int I_mem_id1 = get_memory_id_from_file(file_sha_mem_req , shared_mem_req_size);
    shared_mem_child_re* shared_memory_req = (shared_mem_child_re*) shmat(I_mem_id, NULL, 0);
    if(shared_memory_req ==(void*)-1)die("shared memory atached problem");
    shared_memory_req->line_child    = lines_rand;
    shared_memory_req->segment_child = segments_rand;
    
    cout << "sheared_memory_detail_req:!~~~~  segments rand: " << shared_memory_req->segment_child <<
    "lines rand " << shared_memory_req->line_child << endl;

    //Detach the shared memory segment
    shmdt(shared_memory_req);
    sleep(7);

    char semaphore_name2[50];
    sprintf(semaphore_name2,"keys/semaphore_resp%d.key",segments_rand);
    int sepaphore_id2 = get_semaphore_id_from_file(semaphore_name2);
    #if DEBUG > 1
        printf("Chaild: %d Wait_semaphore: %d\n", getpid(),semaphore_name2);
    #endif
    
     // Attach the shared memory segment.
    int I_mem_id2 = get_memory_id_from_file(file_sha_mem_req , shared_mem_req_size);
    shared_mem_child_re* shared_memory_resp = (shared_mem_child_re*) shmat(I_mem_id, NULL, 0);
    if(shared_memory_resp ==(void*)-1)die("shared memory atached problem");
    cout << "Cild line detailed : " << shared_memory_resp->segment_detail[1].info;
    


}


#endif
