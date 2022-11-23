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

class shared_mem_info{
 public:
   int segments;
   int lines_per_segm;
   int request_per_child; 

};

class shared_mem_child{
 public:
   int segment_child;
   int line_child;
   int time_start;
   int time_end; 
   segment_info segment_detail[1000];
};



#endif
