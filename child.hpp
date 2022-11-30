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


using namespace std;


int child_procces(int lines_per_segm,int segments, int request_per_child){
//todo orismata
    int child_pid = getpid();
    time_t t;
    srand((unsigned) time(&t));
    
        //to-print-deletefor (int i = 0; i < request_per_child; i++){
        //cout << "sheared_memory_detail_child:!~~~~  lines: " << shared_memory_inf->lines_per_segm <<
        //"request_per_child: " << shared_memory_inf->request_per_child << endl;
        int lines_rand     =  rand() %  lines_per_segm;
        int segments_rand  =  rand() %  segments ;
        //to-print-delete
        cout << "sheared_memory_info_child:!~~~~  segments rand: " <<  segments <<
        "lines rand " <<  request_per_child << endl;
        char semaphore_name1[50];

        sprintf(semaphore_name1,"keys/semaphore_req%d.key",segments_rand);
        //int sepaphore_id1 = get_semaphore_id_from_file(semaphore_name1);
 
}


#endif
