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

#define shared_mem_size sizeof(shared_mem*)
 
void die(char er[1000]);

class semaphore_name {
    char sem_name[10];
    int id;

    semaphore_name(int id_){
        id = id_;
        sprintf(sem_name ,"sem_%d", id );
    }
};

int generate_memory_segment(key_t mem_key, size_t mem_sz, char* file_name){
    int mem_seg_id,mem_fd;
    /* Allocate a shared memory id. */
    mem_seg_id = shmget (mem_key, mem_sz, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
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


////semaphore////
int generate_semaphore(key_t sem_key, char* file_name, unsigned int num=0){
    char final_file_name[50]="\0";
    if(num!=0)
        sprintf(final_file_name,"%u",num);
    strcat(final_file_name,file_name);

    int sem_fd,sem_id;
    /*make file for semaphore key*/
    sem_fd = open(final_file_name, O_WRONLY | O_TRUNC | O_EXCL | O_CREAT, 0644);
    if (sem_fd < 0)die("Could not open sem.key");
    /*write the semaphore key*/
    if (write(sem_fd, &sem_key, sizeof(key_t)) < 0) die("Could not write key to file");
    close(sem_fd);
    /*creating shemapgore*/
    sem_id = semget(sem_key, 1, IPC_CREAT | IPC_EXCL | 0600);
    if (sem_id < 0) die("Could not create sem");


    return sem_id;
}

int get_semaphore_id_from_file(char* file_name, unsigned int num=0){
    char final_file_name[50]="\0";
    if(num!=0)
        sprintf(final_file_name,"%u",num);
    strcat(final_file_name,file_name);

    int sem_fd, sem_id;
    key_t sem_key;
    /*Recover sem_key from file*/
    sem_fd = open(final_file_name, O_RDONLY);
    if (sem_fd < 0) die("Could not open sem key for reading");
    if (read(sem_fd, &sem_key, sizeof(key_t)) != sizeof(key_t)) die("Error reading the semaphore key");
    close(sem_fd);
    // Now obtain the (hopefully) existing sem
    sem_id = semget(sem_key, 0, 0);
    if (sem_id < 0) die("Could not obtain semaphore");
    return sem_id;
}

void initialise_semaphore(int sem_id){
    /*seting shemaphore to 0*/
    if(semctl(sem_id, 0 , SETVAL, 0) < 0) die("Could not set value of semaphore");
}

static int semaphore_wait(int sem_id){
    struct sembuf sem_b;
    sem_b.sem_num = 0;
    sem_b.sem_op = -1; /* P() */
    sem_b.sem_flg = 0;
    if (semop(sem_id, &sem_b, 1) == -1) {
        fprintf(stderr, "semaphore_p failed\n");
        return(0);
    }
    return(1);
}

static int semaphore_signal(int sem_id){
    #if DEBUG >= 1
        cout<<"!! semaphore "<<sem_id<<"releasing !! \n";
    #endif
    struct sembuf sem_b;
    sem_b.sem_num = 0;
    sem_b.sem_op = 1; /* V() */
    sem_b.sem_flg = 0;
    if (semop(sem_id, &sem_b, 1) == -1) {
        fprintf(stderr, "semaphore_v failed\n");
        return(0);
    }
    return(1);
}

void clear_sem(char* sem_file, unsigned int num=0){
    char final_file_name[50]="\0";
    if(num!=0)
        sprintf(final_file_name,"%u",num);
    strcat(final_file_name,sem_file);

    /*deleting semaphore*/
    if (semctl(get_semaphore_id_from_file(final_file_name), 0, IPC_RMID) < 0)die("Could not delete semaphore");
    //removing semaphore file
    if (unlink(final_file_name) < 0) die("Could not unlink key file");

}

void die(char er[1000]){//for handling errors
    int fd = open("error.txt", O_WRONLY | O_TRUNC | O_EXCL | O_CREAT, 0644);
    write(fd, er, strlen(er));
    close(fd);
    perror(er);
    exit(1);
}



#endif