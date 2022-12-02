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
#include "child.hpp"
#include "semaphores.hpp"
#include "child.hpp"

  
#define MAX_LINE_LENGTH 200
#define DEBUG 2

using namespace std;

int generate_segments_and_file(char file_name_[50], int lines_seg,int segments);

int main(int argc, char const *argv[]) {
    cout << argv[0] << endl;
    cout << argv[1] << endl;
    cout << argv[2] << endl;
    cout << argv[3] << endl;
    cout << argv[4] << endl;

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
    //to-print-delete
    cout << max_lines << " " << segments_line << " " << file_name << " " << segments << endl;
    semaphore_segm  samaphore_names_segm[segments];
    semaphore_child samaphore_names_child[child_num];

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

    sha_mem_resp->segment = -2;// edo skaei


    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
    //~~~~~~~~~~~~~~~~~~~Sheared memory~~~~~~~~~~~~~~~~~~~//
    //~~~~~~~~~~~~~~~~~~~~~~~end~~~~~~~~~~~~~~~~~~~~~~~~~~//


    //~~~~~~~~~~~~~~~~~~~~~Begin~~~~~~~~~~~~~~~~~~~~~~~~~~//
    //~~~~~~~~~~~~~~~~~~~Semaphore~~~~~~~~~~~~~~~~~~~~~~~~//
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
    for(int i = 0; i < segments; i++ ){
        char sem_name_file_req[50];
        sprintf(sem_name_file_req,"semaphore_req%d",i);
        //to-print-delete
        //cout << "Semaphores details:~~~~~~~~~ name: " << sem_name_file_req << endl;
        samaphore_names_segm[i].req_sem = generate_semaphore(sem_name_file_req ,sem_modes,0);
        strcpy(samaphore_names_segm[i].req_name,sem_name_file_req);

        char sem_name_file_resp[50];
        sprintf(sem_name_file_resp,"semaphore_resp%d",i);
        samaphore_names_segm[i].resp_sem = generate_semaphore(sem_name_file_resp,sem_modes,0);
        strcpy(samaphore_names_segm[i].resp_name,sem_name_file_resp);
    }

    //for(int i = 0; i < child_num; i++ ){
    //    char sem_name_file_child[50];
    //    sprintf(sem_name_file_child,"semaphore_child%d",i);
    //    //to-print-delete
    //    //cout << "Semaphores details:~~~~~~~~~ name: " << sem_name_file_req << endl;
//
    //    if(i>0)
    //        samaphore_names_child[i].chi_sem = sem_open(sem_name_file_child, O_CREAT | O_EXCL, SEM_PERMS, 0);
    //    else
    //        samaphore_names_child[i].chi_sem = sem_open(sem_name_file_child, O_CREAT | O_EXCL, SEM_PERMS, 1);
    //    
    //    strcpy(samaphore_names_child[i].chi_name,sem_name_file_child);
    //}
    char sem_mutex[50];
    sprintf(sem_mutex,"mutex-1");
    sem_t * mutex = generate_semaphore(sem_mutex , sem_modes,0);
    

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
    //~~~~~~~~~~~~~~~~~~~Semaphore~~~~~~~~~~~~~~~~~~~~~~~~//
    //~~~~~~~~~~~~~~~~~~~~~~end~~~~~~~~~~~~~~~~~~~~~~~~~~~//
    //to-print-delete
    //for(int i = 0; i < segments; i++ ){
    //    cout << "Shared memory array ~~~~~~ resp name : " << samaphore_names_segm[i].resp_name <<
    //    "req name : " << samaphore_names_segm[i].req_name <<endl;
    //}

    for (int i = 0; i < child_num; i++) {

        if(fork() == 0) {
            int child_pid = getpid();
            time_t t;
            srand((unsigned) time(&t));     
            int child_num_= i;
            for (int i = 0; i < request_per_child; i++){
                int line_rand      =  rand() %  segments_line;
                int segments_rand  =  rand() %  segments;

                //to-print-delete
                cout << "sheared_memory_info_child:!1~~~~  segments rand: " <<  segments_rand <<
                "lines rand " <<  line_rand << endl;
                
                if (sha_mem_resp->segment == -2 ){
                    //to-print-delete
                    cout << "child - aaaaaaaaaaaaaaaa"<<endl;

                    sha_mem_req->line_child    = line_rand;
                    sha_mem_req->segment_child = segments_rand;
                    sha_mem_req->child_num     = child_num_;
                    //to-print-delete
                    cout << "child - aaaaaaaaaaaaaaaa"<<endl;

                    if(sem_post(mutex)< 0){
                    perror("sem_post(3) error on child");
                    exit(EXIT_FAILURE);
                    //to-print-delete
                    cout << "sheared_memory_info_child:!2~~~~  segments rand: " <<  sha_mem_req->segment_child <<
                    endl;
                }
            }

                if(sha_mem_resp->segment != segments_rand) {

                    if(sem_wait(samaphore_names_segm[segments_rand].req_sem )< 0){
                        perror("sem_wait(3) error on child");
                        exit(EXIT_FAILURE);
                    }
                    //write request                
                    sha_mem_req->line_child    = line_rand;
                    sha_mem_req->segment_child = segments_rand;
                    sha_mem_req->child_num     = child_num_;
                    if(sem_post(mutex)< 0){
                        perror("sem_post(3) error on child");
                        exit(EXIT_FAILURE);
                    }
                } 

                if(sem_wait(samaphore_names_segm[segments_rand].resp_sem )< 0){
                    perror("sem_wait(3) error on child");
                    exit(EXIT_FAILURE);
                }
                //read response
                //to-print-delete
                cout << "Ta kataferame we did it : " << sha_mem_resp->segment_detail[line_rand].info << endl; 
            }
            exit(0);
        }

    }
    //read request parent

    int cnt = 0;
    int request_table[child_num];
    int current_segm = -3;
    for (int i = 0; i < max_requests; i++) {
        if(sem_wait(mutex)< 0){
            perror("sem_wait(3) error on child");
            exit(EXIT_FAILURE);
        }


        if(sem_post(samaphore_names_segm[sha_mem_req->segment_child].req_sem )< 0){
            perror("sem_post(3) error on child");
            exit(EXIT_FAILURE);
        }
        
        if(sem_wait(mutex)< 0){
            perror("sem_wait(3) error on child");
            exit(EXIT_FAILURE);
        }
        
        if(current_segm != sha_mem_req->segment_child){
            int segm_to_load = sha_mem_req->segment_child;
            FILE *file_inp = fopen(file_name, "r");
            for (int y = 0; y < segments; y++){
                for (int z = 0; z < segments_line; z++){
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

        if(sem_post((samaphore_names_segm[sha_mem_req->segment_child].resp_sem ))< 0){
            perror("sem_post(3) error on child");
            exit(EXIT_FAILURE);
        }      

        if(sem_post((samaphore_names_segm[sha_mem_req->segment_child].req_sem ))< 0){
            perror("sem_post(3) error on child");
            exit(EXIT_FAILURE);
        }      
    }
            
    wait(NULL);
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

    //for (int i = 0; i < segments; i++){
    //    for (int y = 0; y < lines_seg; y++)  {
    //        char line_detail[MAX_LINE_LENGTH] = {0};
    //        fgets(line_detail, MAX_LINE_LENGTH, file_inp);
    //        if (y == 0){
    //            sprintf(file_seg_name,"files/segment_%d.txt",segm_count);
    //            file_segm = fopen(file_seg_name, "a+");
    //            if (!file_segm) {
    //                cout << "error on file on segment creation\n";
    //                return -1;
    //            }
    //            segm_count++;
    //        }
    //        fputs(line_detail,file_segm);
    //    }
    //    fclose(file_segm);
    //}
    //
    ///* Close file */
    //fclose(file_inp);

}