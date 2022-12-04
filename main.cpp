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

  
#define MAX_LINE_LENGTH 200

int DEBUG = 2;

using namespace std;

int generate_segments_and_file(char file_name_[50], int lines_seg,int segments);
 

int main(int argc, char const *argv[]) {
    cout << argv[0] << endl;
    cout << "Child Number:      " << argv[1] << endl;
    cout << "Max lines of file: " << argv[2] << endl;
    cout << "Sevments Per Line: " << argv[3] << endl;
    cout << "Request Per Child: " << argv[4] << endl;

    mode_t sem_modes = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP;

    //cout << argv[1] << ' ' << argv[2] << ' ' <<argv[3] << ' '<<argv[4] << endl;
    if(argc==1){
        cout << "Exit Program Wrong Parameter\n";
        return -1;
    }
        
    int child_num         = atoi(argv[1]);
    int max_lines         = atoi(argv[2]);
    int segments_line     = atoi(argv[3]);
    int request_per_child = atoi(argv[4]);
    char file_name[25];
    strcpy(file_name ,argv[5]);
    int segments          = max_lines / segments_line;
    int max_requests      = segments * request_per_child;
    
    semaphore_segm  samaphore_names_segm[segments];
    //semaphore_child samaphore_names_child[child_num];

    if(generate_segments_and_file(file_name,segments_line,segments) == -1 ){
        cout <<"error on Segment " <<endl;
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
    generate_memory_segment(shared_mem_req_key, shared_mem_req_size,file_sha_mem_req);
    int I_mem_id1=get_memory_id_from_file(file_sha_mem_req , shared_mem_req_size);
    shared_mem_req* sha_mem_req = (shared_mem_req*) shmat(I_mem_id1, NULL, 0);
    if(sha_mem_req ==(void*)-1)die("shared memory atached problem");
    


    //shared memory for response
    key_t shared_mem_resp_key = ftok("main.o" ,2);
    if (shared_mem_resp_key == -1){
        printf("ftok not working\n");
        cout << strerror(errno) << endl;
    }  
    char file_sha_mem_resp[50];
    sprintf(file_sha_mem_resp,"keys/shared_mem_resp.key");
    generate_memory_segment(shared_mem_resp_key, shared_mem_resp_size,file_sha_mem_resp);
    int I_mem_id2 = get_memory_id_from_file(file_sha_mem_resp , shared_mem_resp_size);
    shared_mem_resp* sha_mem_resp = (shared_mem_resp*) shmat(I_mem_id2, NULL, 0);
    if(sha_mem_resp ==(void*)-1)die("shared memory atached problem");

    sha_mem_resp->segment = -2; 

    //shared memory for data log
    key_t shared_mem_data_log_key = ftok("main.o" ,3);
    if (shared_mem_data_log_key == -1){
        printf("ftok not working\n");
        cout << strerror(errno) << endl;
    }  
    char file_shared_mem_data_log[50];
    sprintf(file_shared_mem_data_log,"keys/shared_mem_data_log.key");
    generate_memory_segment(shared_mem_data_log_key, shared_mem_data_log_size,file_shared_mem_data_log);
    int I_mem_id3 = get_memory_id_from_file(file_shared_mem_data_log , shared_mem_data_log_size);
    shared_mem_data_log* sha_mem_data_log = (shared_mem_data_log*) shmat(I_mem_id3, NULL, 0);
    if(sha_mem_data_log ==(void*)-1)die("shared memory atached problem");
   
    //to-print-delete
    //for(int i=0; i<100; i++){
    //    cout << "aaaaaaaaa"<<endl;
    //    cout << sha_mem_data_log->log[i].pid <<endl;
    //}
    //return 0;

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
    //~~~~~~~~~~~~~~~~~~~Sheared memory~~~~~~~~~~~~~~~~~~~//
    //~~~~~~~~~~~~~~~~~~~~~~~end~~~~~~~~~~~~~~~~~~~~~~~~~~//


    //~~~~~~~~~~~~~~~~~~~~~Begin~~~~~~~~~~~~~~~~~~~~~~~~~~//
    //~~~~~~~~~~~~~~~~~~~Semaphore~~~~~~~~~~~~~~~~~~~~~~~~//
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
    //for(int i = 0; i < segments; i++ ){
    //    char sem_name_file_req[50];
    //    sprintf(sem_name_file_req,"semaphore_req%d",i);
    //    //to-print-delete
    //    //cout << "Semaphores details:~~~~~~~~~ name: " << sem_name_file_req << endl;
    //    samaphore_names_segm[i].req_sem = generate_semaphore(sem_name_file_req ,sem_modes,0);
    //    strcpy(samaphore_names_segm[i].req_name,sem_name_file_req);
//
    //    char sem_name_file_resp[50];
    //    sprintf(sem_name_file_resp,"semaphore_resp%d",i);
    //    samaphore_names_segm[i].resp_sem = generate_semaphore(sem_name_file_resp,sem_modes,0);
    //    strcpy(samaphore_names_segm[i].resp_name,sem_name_file_resp);
    //}
//
    //for(int i = 0; i < child_num; i++ ){
    //    char sem_name_file_child[50];
    //    sprintf(sem_name_file_child,"semaphore_child%d",i);
    //    //to-print-delete
    //    //cout << "Semaphores details:~~~~~~~~~ name: " << sem_name_file_req << endl;
    //    samaphore_names_child[i].chi_sem = generate_semaphore(sem_name_file_child,sem_modes,1);       
    //    strcpy(samaphore_names_child[i].chi_name,sem_name_file_child);
    //}
    //char sem_mutex[50];
    //sprintf(sem_mutex,"mutex-1");
    //sem_t * mutex = generate_semaphore(sem_mutex , sem_modes,1);
//
    //char sem_mutex2[50];
    //sprintf(sem_mutex2,"mutex-2");
    //sem_t * mutex2 = generate_semaphore(sem_mutex , sem_modes,1);
//

    key_t mutex_key = ftok("main.o" ,4);
    if (mutex_key == -1){
            printf("ftok not working\n");
            cout << strerror(errno) << endl;
    }  
    char sem_mutex_file[50];
    sprintf(sem_mutex_file,"keys/mutex");
    int mutex     = generate_semaphore(mutex_key, sem_mutex_file);
    int mutex_sem = get_semaphore_id_from_file(sem_mutex_file);
    semaphore_signal(mutex_sem);
    

    for(int i = 0; i < segments; i++ ){
        key_t semaphore_mem_req_key = ftok("main.o" ,i+5);
        if (semaphore_mem_req_key == -1){
            printf("ftok not working\n");
            cout << strerror(errno) << endl;
        }  
        char sem_name_file_req[50];
        sprintf(sem_name_file_req,"keys/semaphore_req%d.key",i);
        //to-print-delete
        //cout << "Semaphores details:~~~~~~~~~ name: " << sem_name_file_req << endl;
        int sem_req=generate_semaphore(semaphore_mem_req_key, sem_name_file_req);
        samaphore_names_segm[i].req_sem = get_semaphore_id_from_file(sem_name_file_req);
        initialise_semaphore(sem_req);


        key_t shared_mem_resp_key = ftok("main.o" ,i+segments+6);
        if (semaphore_mem_req_key == -1){
            printf("ftok not working\n");
            cout << strerror(errno) << endl;
        }  
        char sem_name_file_resp[50];
        sprintf(sem_name_file_resp,"keys/semaphore_resp%d.key",i);
        //to-print-delete
        //cout << "Semaphores details:~~~~~~~~~ name: " << sem_name_file_req << endl;
        int sem_resp=generate_semaphore(shared_mem_resp_key, sem_name_file_resp);
        samaphore_names_segm[i].resp_sem = get_semaphore_id_from_file(sem_name_file_resp);
        initialise_semaphore(sem_resp);
        //semaphore_wait(sem_resp);
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
    //~~~~~~~~~~~~~~~~~~~Semaphore~~~~~~~~~~~~~~~~~~~~~~~~//
    //~~~~~~~~~~~~~~~~~~~~~~end~~~~~~~~~~~~~~~~~~~~~~~~~~~//
    //to-print-delete
    //for(int i = 0; i < segments; i++ ){
    //    cout << "Shared memory array ~~~~~~ resp name : " << samaphore_names_segm[i].resp_name <<
    //    "req name : " << samaphore_names_segm[i].req_name <<endl;
    //}
    
    int pids[child_num];

    for (int i = 0; i < child_num; i++) {

        if( (pids[i] = fork()) == 0) {
            int child_pid = getpid();
            time_t t;
            srand((unsigned) time(&t));     
            int child_number= i;
            char outname[50];
            sprintf(outname,"outputs/child%d.%d",child_number,child_pid);
            FILE *file_output = fopen(outname, "w+");
            
            for (int y = 0; y < request_per_child; y++){
                cout << "child_number: "<< child_number << endl<< endl<< endl<< endl << endl;

                clock_t time_start = clock();
                int line_rand      =  0;//rand() %  segments_line;
                int segments_rand  =  0;//rand() %  segments;
                //if( child_number == 3){
                //    cout << "aaaaa3 pedi" << endl;
                //    int line_rand      =  1;//rand() %  segments_line;
                //    int segments_rand  =  1;//
                //}
                                    
                semaphore_wait(mutex_sem);
                sha_mem_data_log->log[child_number].pid          = child_pid;
                sha_mem_data_log->log[child_number].segment_req  = segments_rand;
                sha_mem_data_log->log[child_number].start_time   = time_start;
                semaphore_signal(mutex_sem);
                
                
                
                //for starting the process 
                if(child_number == 0 && y == 0){
                    sha_mem_req->line_child    = line_rand;
                    sha_mem_req->segment_child = segments_rand;
                    sha_mem_req->pid           = child_pid;
                    
                }
                
                //if we have different segment load in shared memmory we wait wait
                if (sha_mem_req->segment_child != segments_rand){
                   
                    sha_mem_req->line_child    = line_rand;
                    sha_mem_req->segment_child = segments_rand;
                    sha_mem_req->pid           = child_pid;
                    
                }
                
                clock_t time_end = clock();
                char line_to_print[500];
                sprintf(line_to_print ,"Child%d with Pid %d Reads Segment: %d Line:%d time_start: %ld time_stop: %ld Line Detail: %s\n",
                child_number,child_pid,segments_rand,line_rand,time_start, time_end,sha_mem_resp->segment_detail[line_rand].info);
                ////to-print-delete
                cout << "Ta kataferame we did it : " << line_to_print << endl; 
                fputs(line_to_print,file_output);
                cout << "mpike sto arxeio toutoutou : " << line_to_print << endl;
                
            }
            fclose(file_output);
            return 1;
            //exit(0);
        }

    }
    //read request parent

    int cnt = 0;
    int request_table[child_num];
    int current_segm = -3;
    
    for (int i = 0; i < max_requests; i++) {
       
                
        if(current_segm != sha_mem_req->segment_child){
            int segm_to_load = sha_mem_req->segment_child;
            FILE *file_inp = fopen(file_name, "r");
            for (int y = 0; y <= segments-1; y++){
                for (int z = 0; z <= segments_line-1; z++){
                    char line_detail[MAX_LINE_LENGTH] = {0};
                    fgets(line_detail, MAX_LINE_LENGTH, file_inp);   
                    if (segm_to_load == y){
                        sha_mem_resp->segment_detail[z].line = z;
                        strcpy(sha_mem_resp->segment_detail[z].info,line_detail); 
                    }
                }
            }
            fclose(file_inp);
            current_segm = segm_to_load;
            
        }

    
        

    }
            
    for(int a = 0; a < child_num; a++) {
        if (waitpid(pids[a],NULL,0)<0){
            perror("waitpid(2) faild");
        }
    
    }
}

    
    
    //clear_mem(file_sha_mem,shared_mem_size);
    //Detached all shared memory
    //for (int i = 0; i < segments; i++){ 
    //    clear_mem(mem_info_arr[i].name_file,shared_mem_size);
    //}



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


