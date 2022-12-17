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

#define NUM_CHILDREN      5 //Proccess number(readers)
#define REQUEST_PER_CHILD 100 //Request Per Procces
#define SEGMENTS          10 
#define LINES_PER_SEGMENT 50 // line that file haD
#define MAX_LINES_FILE    1000   // How many lines the file had
#define FILE_NAME    " "
#define MAX_LINE_LENGTH   20


typedef struct {
  char line_info[MAX_LINE_LENGTH];  // Line info
  int  line;  // Line number
} segment_detail;


// Struct for request and response shared memory
typedef struct {
  int segment;  // Segment to load
  segment_detail segment_info[LINES_PER_SEGMENT];  // Data for the segment
} shm_data;

// Struct for FIFO priority queue
typedef struct {
  int requests[NUM_CHILDREN * REQUEST_PER_CHILD+1];  // Array of requests
  int front;  // Front of the queue
  int rear;  // Rear of the queue
} request_queue;

//  Struct to truck semaphores
typedef struct {
  char semaphore_name[50];  // Semaphore name
  key_t  sem_key;  // Semaphore key
} semaphore_name;


int generate_segments_and_file(char file_name_[50], int lines_seg,int segments);
int rand50();
bool rand75();
void resuffle( request_queue* sha_mem_data_log ,int value);

using namespace std;

int main(int argc, char const *argv[]) {
    int child_num;         
    int max_lines;         
    int segments_lines;
    int request_per_child;
    int segments;   
    char file_name[25];
   
    if(argc==1){
        child_num         = NUM_CHILDREN;
        max_lines         = MAX_LINES_FILE;
        segments_lines    = LINES_PER_SEGMENT;
        request_per_child = REQUEST_PER_CHILD;
        segments          = SEGMENTS;
        if (strcmp(FILE_NAME, " ") == 0){
            sprintf(file_name,"input_file.txt");
        }else{
            strcpy(file_name,FILE_NAME);
        }
    }
    else{
        cout << "Values are passed through define variables\n";
    //    child_num         = atoi(argv[1]);
    //    max_lines         = atoi(argv[2]);
    //    segments_lines    = atoi(argv[3]);
    //    request_per_child = atoi(argv[4]);
    //    strcpy(file_name ,argv[5]);
    }
   
            
    
    if (strcmp(FILE_NAME, " ") == 0){
        if(generate_segments_and_file(file_name,max_lines,segments_lines) == -1 ){
            cout <<"error on CREATE FILE " <<endl;
            exit(EXIT_FAILURE);
        }
    }

    //~~~~~~~~~~~~~~~~~~~~~~~begin~~~~~~~~~~~~~~~~~~~~~~~~//
    //~~~~~~~~~~~~~~~~~~~Sheared memory~~~~~~~~~~~~~~~~~~~//
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
    //Shared memory for request
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
    //for (size_t i = 0; i < NUM_CHILDREN * REQUEST_PER_CHILD+1; i++){
    //    sha_mem_req.segment = -1;
    //}
     

    //Shared memory for response
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
    //for (size_t i = 0; i < NUM_CHILDREN * REQUEST_PER_CHILD+1; i++){
    //    sha_mem_resp[i].segment = -1;
    //}
    
     

    //Shared memory for data log fifo requests
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
    sha_mem_data_log->front = 0;
    sha_mem_data_log->rear  = 0;
    
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
    //cout << "ID : " << response_sem << "Name : " << sem_response_file << endl;
  

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
    //cout << "ID : " << request_sem << "Name : " << sem_request_file << endl;
       

    
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
        //cout << "ID : " << samaphore_names_segm[i].sem_key << "Name : " << seg_file << endl;
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
        int segments_log;
        for (int i = 0; i < REQUEST_PER_CHILD; i++) {
            clock_t time_start = clock();
           
            int segment_rand = rand() % segments + 1;
            int rand_line    = rand() % segments_lines;
            if (i == 0){
                segments_log = segment_rand;
            }else{
                if(rand75()==true){
                    segment_rand = segments_log;
                }else{
                    segments_log = segment_rand;
                }       
            }
            
            // Semaphore wait per segment based on segment that we need
            semaphore_wait(samaphore_names_segm[segment_rand - 1].sem_key,samaphore_names_segm[segment_rand - 1].semaphore_name);

            // Wait for the response semaphore to be available
            semaphore_wait(response_sem ,sem_response_file);

            // Load the request shared memory
            sha_mem_req->segment = segment_rand;

            // Add the request to the priority queue
            sha_mem_data_log->requests[sha_mem_data_log->rear ] = segment_rand;
            sha_mem_data_log->rear = (sha_mem_data_log->rear + 1) % (NUM_CHILDREN * REQUEST_PER_CHILD);
            resuffle(sha_mem_data_log,segment_rand);

            // Signal the request semaphore for parent understands that there is a request 
            semaphore_signal(request_sem ,sem_request_file);

            // Wait for the response  
            semaphore_wait(response_sem ,sem_response_file);

            // Write the data to file
            //printf("Child %d received response for segment %d: %s\n", getpid(), segment_rand, sha_mem_resp->segment_info[rand_line].line_info);
            clock_t time_end = clock();
            char line_to_print[500];
            sprintf(line_to_print ,"Child with Pid %d Reads Segment: %d Line:%d time_start: %ld time_stop: %ld Line Detail: %s",
            getpid(),segment_rand,rand_line,time_start, time_end,sha_mem_resp->segment_info[rand_line].line_info);
            fputs(line_to_print,file_output);

            // Signal the response semaphore to indicate the response has been handled
            semaphore_signal(response_sem ,sem_response_file);

        }
        exit(0);
        return 0 ;
    } else {
        // parent process 
        int count_segment_insert = 0;
        clock_t time_start;
        clock_t time_end ;
        for (int i = 0; i < NUM_CHILDREN * REQUEST_PER_CHILD; i++) {
            // Wait for until we get a response 
            semaphore_wait(request_sem ,sem_request_file);

            // get the next request from the queue
            int segment_req = sha_mem_data_log->requests[sha_mem_data_log->front];
            sha_mem_data_log->front = (sha_mem_data_log->front + 1) % (NUM_CHILDREN * REQUEST_PER_CHILD);

            // Check if the segment is already loaded 
            int found; 
            found = 0;
            if (sha_mem_resp->segment == segment_req) {
                if(count_segment_insert == 0 ){
                    time_start = clock();
                    cout << "if Segment : " << segment_req << " Starts : " << time_start ;
                    count_segment_insert++;
                }
                found = 1;
            }
            else {
                if (count_segment_insert == -1 ){
                    count_segment_insert = 0;
                    time_end = clock();
                    cout << " Ends : "<<time_end << endl;
                }
                time_start = clock();
                cout << "Segment : " << segment_req << " Starts : " << time_start ;
            }

            if (found != 1) {              
               
                if (count_segment_insert == 0) {
                    count_segment_insert = -1;
                }else{
                    count_segment_insert = 0; time_end = clock();
                    cout << " Ends : "<<time_end << endl;
                }
                
                
                // read the requested segment from the file
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
                usleep(30);

            }

            // signal the segment semaphore to indicate the segment has been loaded
            semaphore_signal(samaphore_names_segm[segment_req - 1].sem_key, samaphore_names_segm[segment_req - 1].semaphore_name);

            // signal the response semaphore to let the child know the response is ready
            semaphore_signal(response_sem ,sem_response_file);
        }
    }
    clock_t  time_end = clock();     
    cout << " Ends : "<<time_end << endl;     
    cout << endl << endl << endl;

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
 

// Random Function to that returns 0 or 1 with
// equal probability
int rand50() 
{
    // rand() function will generate odd or even
    // number with equal probability. If rand()
    // generates odd number, the function will
    // return 1 else it will return 0.
    return rand() & 1;
}

// Random Function to that returns 1 with 75%
// probability and 0 with 25% probability using
// Bitwise OR
bool rand75(){
    return rand50() | rand50();
}
 
void resuffle( request_queue* sha_mem_data_log ,int value){
    int count = 0;

    for (size_t i = sha_mem_data_log->front; i <= sha_mem_data_log->rear ; i++){
        if (sha_mem_data_log->requests[i] == value){
            count ++;
        }
    }

    for (size_t i = sha_mem_data_log->front + count; i <= sha_mem_data_log->rear; i++){
        sha_mem_data_log->requests[i] = sha_mem_data_log->requests[i - count];
    }
    
    for (size_t i = sha_mem_data_log->front ; i < sha_mem_data_log->front + count; i++){
        sha_mem_data_log->requests[i] = value;
    }    
}
 
 

 