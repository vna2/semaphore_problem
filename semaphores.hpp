#ifndef SEMAPHORE_SHARED_MEM
#define SEMAPHORE_SHARED_MEM
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <iostream>
#include "child.hpp"
#include "shared_memory_class.hpp"

#define shared_mem_resp_size sizeof(shared_mem_resp)
#define shared_mem_req_size sizeof(shared_mem_req)
 
void die(char er[1000]);


int generate_memory_segment(key_t mem_key, size_t mem_sz, char* file_name){
    int mem_seg_id,mem_fd;
    /* Allocate a shared memory id. */
    mem_seg_id = shmget (mem_key, mem_sz, 0666 | IPC_CREAT); //IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    if(mem_seg_id < 0)die("can not allocate memory segment");

    /*creating file for memory key*/
    mem_fd = open(file_name, O_WRONLY | O_TRUNC | O_EXCL | O_CREAT, 0644);
    if (mem_fd < 0)die("Could not open mem.id");
    /*writing memory id*/
    if (write(mem_fd, &mem_key, sizeof(key_t)) < 0) die("Could not write key to file");
    close(mem_fd);
    return mem_seg_id;
}

int get_memory_id_from_file(char* file_name, int mem_sz){
    int mem_fd,mem_seg_id;
    key_t mem_key;
    /*Recover mem_key from file*/
    mem_fd = open(file_name, O_RDONLY);
    if (mem_fd < 0) die("Could not open memory file for reading");
    if (read(mem_fd, &mem_key, sizeof(int)) != sizeof(int)) die("Error reading the memory key");
    close(mem_fd);
    /*optain mem id*/
    mem_seg_id=shmget(mem_key, mem_sz, 0666);

    return mem_seg_id;
}

void clear_mem(char* mem_file, int mem_sz){
    /* Deallocate the shared memory segment. */
    if(shmctl (get_memory_id_from_file(mem_file,mem_sz), IPC_RMID, 0) < 0)die("can not dealocate memory");

    //removing memory file
    if (unlink(mem_file) < 0) die("Could not unlink memory file ");

}

void die(char er[1000]){//for handling errors
    int fd = open("error.txt", O_WRONLY | O_TRUNC | O_EXCL | O_CREAT, 0644);
    write(fd, er, strlen(er));
    close(fd);
    perror(er);
    exit(1);
}


#endif
