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

class shared_mem{
 public:
   int  child_id;
   int  segment;
   int  line;
   int  time_start;
   int  time_end;
   char line_detail[200];
   char file_seg_name[50];
   char sem_file_name[5];
};



#endif
