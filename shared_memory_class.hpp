#ifndef SHARED_MEM
#define SHARED_MEM
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <iostream>

using namespace std;

class segment_info{
  public:
      int line;
      char info[50];
};

class shared_mem_req{
 public:
   int segment_child;
   int line_child;
   int time_start;
   int time_end; 
   int child_num;
};

class shared_mem_resp{
 public:
   segment_info segment_detail[1000];
   int segment;
};

class semaphore_segm{
  public: 
    char req_name[50];
    sem_t* req_sem;
    char resp_name[50];
    sem_t* resp_sem;
};

class semaphore_child{
  public: 
    char chi_name[50];
    sem_t* chi_sem;
};

//class info_for_queue{
//  public:
//   
//};

#endif
