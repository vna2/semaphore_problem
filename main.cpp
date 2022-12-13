#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <iostream>
#include <errno.h>
#include "shared_mem_sem.hpp"

#define NUM_CHILDREN      10 
#define REQUEST_PER_CHILD 2
#define SEGMENTS          4
#define LINES_PER_SEGMENT 20
#define MAX_LINE_FILE     200
#define MAX_LINE_LENGTH   200


typedef struct {
  char line_info[MAX_LINE_LENGTH];  // line info
  int  line;  // line number
} segment_detail;


// struct for request and response shared memory
typedef struct {
  int segment;  // segment to load
  segment_detail segment_info[LINES_PER_SEGMENT];  // data for the segment
} shm_data;

// struct for FIFO priority queue
typedef struct {
  int requests[NUM_CHILDREN * REQUEST_PER_CHILD];  // array of requests
  int front;  // front of the queue
  int rear;  // rear of the queue
} request_queue;

//  struct to truck semaphores
typedef struct {
  char semaphore_name[50];  // semaphore name
  key_t  sem_key;  // semaphore key
} semaphore_name;


int generate_segments_and_file(char file_name_[50], int lines_seg,int segments);

using namespace std;

int main(int argc, char const *argv[]) {
    cout << argv[0] << endl;
    cout << "Child Number:      " << argv[1] << endl;
    cout << "Max lines of file: " << argv[2] << endl;
    cout << "Sevments Per Line: " << argv[3] << endl;
    cout << "Request Per Child: " << argv[4] << endl;
    int child_num;         
    int max_lines;         
    int segments_lines;
    int request_per_child;
    char file_name[25];
   
    if(argc==1){
        cout << "Using default values\n";
        child_num         = NUM_CHILDREN;
        max_lines         = MAX_LINE_FILE;
        segments_lines    = LINES_PER_SEGMENT;
        request_per_child = REQUEST_PER_CHILD;
        sprintf(file_name,"input_file.txt");
    }else{
        child_num         = atoi(argv[1]);
        max_lines         = atoi(argv[2]);
        segments_lines    = atoi(argv[3]);
        request_per_child = atoi(argv[4]);
        strcpy(file_name ,argv[5]);
    }
        
   
    int segments          = max_lines / segments_lines;
    int max_requests      = child_num * request_per_child;
    

    if(generate_segments_and_file(file_name,max_lines,segments_lines) == -1 ){
        cout <<"error on CREATE FILE " <<endl;
        exit(EXIT_FAILURE);
    }
   
    //~~~~~~~~~~~~~~~~~~~~~~~begin~~~~~~~~~~~~~~~~~~~~~~~~//
    //~~~~~~~~~~~~~~~~~~~Sheared memory~~~~~~~~~~~~~~~~~~~//
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
    //shared memory for request
    key_t shared_mem_req_key = ftok("main.o" ,1);
    if (shared_mem_req_key == -1){
        printf("ftok not working\n");
        cout << strerror(errno) << endl;
    }  
    char file_sha_mem_req[50];
    sprintf(file_sha_mem_req,"keys/shared_mem_req.key");
    generate_memory_segment(shared_mem_req_key, sizeof(shm_data),file_sha_mem_req);
    int I_mem_id1=get_memory_id_from_file(file_sha_mem_req , sizeof(shm_data));
    shm_data* sha_mem_req = (shm_data*) shmat(I_mem_id1, NULL, 0);
    if(sha_mem_req ==(void*)-1)die("shared memory atached problem");
     

    //shared memory for response
    key_t shared_mem_resp_key = ftok("main.o" ,2);
    if (shared_mem_resp_key == -1){
        printf("ftok not working\n");
        cout << strerror(errno) << endl;
    }  
    char file_sha_mem_resp[50];
    sprintf(file_sha_mem_resp,"keys/shared_mem_resp.key");
    generate_memory_segment(shared_mem_resp_key, sizeof(shm_data),file_sha_mem_resp);
    int I_mem_id2 = get_memory_id_from_file(file_sha_mem_resp , sizeof(shm_data));
    shm_data* sha_mem_resp = (shm_data*) shmat(I_mem_id2, NULL, 0);
    if(sha_mem_resp ==(void*)-1)die("shared memory atached problem");
    
     

    //shared memory for data log fifo requests
    key_t shared_mem_data_log_key = ftok("main.o" ,3);
    if (shared_mem_data_log_key == -1){
        printf("ftok not working\n");
        cout << strerror(errno) << endl;
    }  
    char file_shared_mem_data_log[50];
    sprintf(file_shared_mem_data_log,"keys/shared_mem_data_log.key");
    generate_memory_segment(shared_mem_data_log_key, sizeof(request_queue),file_shared_mem_data_log);
    int I_mem_id3 = get_memory_id_from_file(file_shared_mem_data_log , sizeof(request_queue));
    request_queue* sha_mem_data_log = (request_queue*) shmat(I_mem_id3, NULL, 0);
    if(sha_mem_data_log ==(void*)-1)die("shared memory atached problem");
    
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
    //~~~~~~~~~~~~~~~~~~~Sheared memory~~~~~~~~~~~~~~~~~~~//
    //~~~~~~~~~~~~~~~~~~~~~~~end~~~~~~~~~~~~~~~~~~~~~~~~~~//


    //~~~~~~~~~~~~~~~~~~~~~Begin~~~~~~~~~~~~~~~~~~~~~~~~~~//
    //~~~~~~~~~~~~~~~~~~~Semaphore~~~~~~~~~~~~~~~~~~~~~~~~//
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
    semaphore_name samaphore_names_segm[segments];

    key_t response_key = ftok("main.o" ,4);
    if (response_key == -1){
            printf("ftok not working\n");
            cout << strerror(errno) << endl;
    }  
    char sem_response_file[50];
    sprintf(sem_response_file,"keys/response.key");
    int response_id  = generate_semaphore(response_key, sem_response_file);
    int response_sem = get_semaphore_id_from_file(sem_response_file);
    initialise_semaphore(response_sem);
    semaphore_signal(response_sem,sem_response_file);
    cout << "ID : " << response_sem << "Name : " << sem_response_file << endl;
  

    key_t sem_request_key = ftok("main.o" ,5);
    if (sem_request_key == -1){
            printf("ftok not working\n");
            cout << strerror(errno) << endl;
    }  
    char sem_request_file[50];
    sprintf(sem_request_file,"keys/request.key");
    int request_id     = generate_semaphore(sem_request_key, sem_request_file);
    int request_sem = get_semaphore_id_from_file(sem_request_file);
    initialise_semaphore(request_sem);
    cout << "ID : " << request_sem << "Name : " << sem_request_file << endl;

    
    for(int i = 0; i < segments; i++ ){
        key_t semaphore_seg_key = ftok("main.o" ,i+9);
        if (semaphore_seg_key == -1){
            printf("ftok not working\n");
            cout << strerror(errno) << endl;
        }  
        char seg_file[50];
        sprintf(seg_file,"keys/sem_seg%d.key",i);

        int sem_req=generate_semaphore(semaphore_seg_key, seg_file);
        samaphore_names_segm[i].sem_key = get_semaphore_id_from_file(seg_file);
        strcpy(samaphore_names_segm[i].semaphore_name,seg_file);
        initialise_semaphore(samaphore_names_segm[i].sem_key );
        cout << "ID : " << samaphore_names_segm[i].sem_key << "Name : " << seg_file << endl;
        semaphore_signal(samaphore_names_segm[i].sem_key ,seg_file);
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
    //~~~~~~~~~~~~~~~~~~~Semaphore~~~~~~~~~~~~~~~~~~~~~~~~//
    //~~~~~~~~~~~~~~~~~~~~~~end~~~~~~~~~~~~~~~~~~~~~~~~~~~//
    
 

    pid_t pid;
 
    for (int c = 0; c < child_num; c++) {
        pid = fork();
        if(pid == 0){
            //child process
            break;
        }
    }  
    
    if (pid == 0) {
        // child process
        char outname[50];
        sprintf(outname,"outputs/child.%d",getpid());
        FILE *file_output = fopen(outname, "a+");
        time_t t;
        srand((int) time(&t) % getpid());  
        int segments_log[REQUEST_PER_CHILD];
        for (int i = 0; i < REQUEST_PER_CHILD; i++) {
            clock_t time_start = clock();
            // rand() segment to request
            int segment_rand = rand() % segments + 1;
            int rand_line    = rand() % segments_lines;
            //if (i == 0){
            //    segments_log[i] = segment_rand;
            //}
            //if(i > 0){
//
            //}
            

            // wait for the segment semaphore to be available
            semaphore_wait(samaphore_names_segm[segment_rand - 1].sem_key,samaphore_names_segm[segment_rand - 1].semaphore_name);

            // wait for the response semaphore to be available
            semaphore_wait(response_sem ,sem_response_file);

            // set the request shared memory
            sha_mem_req->segment = segment_rand;

            // add the request to the queue
            sha_mem_data_log->requests[sha_mem_data_log->rear] = segment_rand;
            sha_mem_data_log->rear = (sha_mem_data_log->rear + 1) % (NUM_CHILDREN * REQUEST_PER_CHILD);

            // signal the request semaphore to let the parent know there is a request
            semaphore_signal(request_sem ,sem_request_file);

            // wait for the response semaphore to be available
            semaphore_wait(response_sem ,sem_response_file);

            // write the data to file
            printf("Child %d received response for segment %d: %s\n", getpid(), segment_rand, sha_mem_resp->segment_info[rand_line].line_info);
            clock_t time_end = clock();
            char line_to_print[500];
            sprintf(line_to_print ,"Child with Pid %d Reads Segment: %d Line:%d time_start: %ld time_stop: %ld Line Detail: %s\n",
             getpid(),segment_rand,rand_line,time_start, time_end,sha_mem_resp->segment_info[rand_line].line_info);
            fputs(line_to_print,file_output);
            // signal the response semaphore to indicate the response has been handled
            semaphore_signal(response_sem ,sem_response_file);

        }
        exit(0);
    } else {
        // parent process
        for (int i = 0; i < NUM_CHILDREN * REQUEST_PER_CHILD; i++) {
            // wait for the request semaphore to be signaled
            semaphore_wait(request_sem ,sem_request_file);

            // get the next request from the queue
            int segment_req = sha_mem_data_log->requests[sha_mem_data_log->front];
            sha_mem_data_log->front = (sha_mem_data_log->front + 1) % (NUM_CHILDREN * REQUEST_PER_CHILD);

            // check if the segment is already loaded
            int found = 0;
            for (int j = 0; j < i; j++) {
              if (sha_mem_resp[j].segment == sha_mem_resp[i].segment) {
                found = 1;
                break;
              }
            }

            if (!found) {
                // read the requested segment from the file
                usleep(30);
                FILE *file_inp = fopen(file_name, "r");
                for (int y = 0; y <= segments; y++){
                    for (int z = 0; z < segments_lines; z++){
                        char line_detail[MAX_LINE_LENGTH] = {0};
                        fgets(line_detail, MAX_LINE_LENGTH, file_inp);   
                        if (segment_req == y){
                            strcpy(sha_mem_resp->segment_info[z].line_info,line_detail); 
                            sha_mem_resp->segment = segment_req;
                        }
                    }
                }
                fclose(file_inp);

            }

            // signal the segment semaphore to indicate the segment has been loaded
            semaphore_signal(samaphore_names_segm[segment_req - 1].sem_key, samaphore_names_segm[segment_req - 1].semaphore_name);

            // signal the response semaphore to let the child know the response is ready
            semaphore_signal(response_sem ,sem_response_file);
        }
    }
         
    while (wait(NULL) > 0);

    clear_mem(file_shared_mem_data_log,sizeof(request_queue));
    clear_mem(file_sha_mem_req,sizeof(shm_data));
    clear_mem(file_sha_mem_resp,sizeof(shm_data)); 
    for (size_t i = 0; i < segments; i++){
        clear_sem(samaphore_names_segm[i].semaphore_name);
    }
    clear_sem(sem_response_file);
    clear_sem(sem_request_file);
 

}
 
 
int generate_segments_and_file(char file_name_[50], int lines_seg,int segments){
    int  line_count = 0;
    int  segm_count = 0;
    
    /* Open file */
    FILE *file_inp = fopen(file_name_, "a+");
    
    if (!file_inp) {
        return -1;
    }


    int lines_new = segments * lines_seg;

    for (int i = 0; i < lines_new; i++){
        char generate_new_line[50];
        sprintf(generate_new_line,"%d\n",i);    
        fputs(generate_new_line,file_inp);
    }

    fclose(file_inp);

}
 


 

 
 