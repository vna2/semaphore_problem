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
#include <time.h>

using namespace std;

class segment_info{
  public:
      int line;
      char info[50];
};

class data_log{
  public: 
    int pid;
    int segment_req;
    clock_t start_time;

};

class shared_mem_req{
 public:
   int segment_child;
   int line_child;
   int pid;
};

class shared_mem_resp{
 public:
   segment_info segment_detail[1000];
   int segment;
};

class shared_mem_data_log{
 public:
   data_log log[1000];
   
   //shared_mem_data_log(int segments){
   //   log = new data_log;
   //   for (int i = 0; i < segments; i++){
   //      log[i].pid=0;
   //      log[i].segment_req = -1;
   //      log[i].start_time  = 0;
   //   }
   //   
   //}
   //~shared_mem_data_log()
};

class semaphore_segm{
  public: 
    char req_name[50];
    key_t req_sem;
    char resp_name[50];
    key_t resp_sem;
};

class semaphore_child{
  public: 
    char chi_name[50];
    key_t chi_sem;
};

//class info_for_queue{
//  public:
//   
//};

#endif
