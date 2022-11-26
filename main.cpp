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

using namespace std;

int generate_segments_and_file(char file_name_[50], int lines_seg,int segments);
void initilize_semaphores();

int main(int argc, char const *argv[]) {
    cout << argv[0] << endl;
    cout << argv[1] << endl;
    cout << argv[2] << endl;
    cout << argv[3] << endl;
    cout << argv[4] << endl;

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

    int segments = max_lines / segments_line;
    //to-print-delete
    cout << max_lines << " " << segments_line << " " << file_name << " " << segments << endl;
    
    generate_segments_and_file(file_name,segments_line,segments);
   
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
    sprintf(file_sha_mem_req ,"keys/shared_mem_req.key");
    generate_memory_segment(shared_mem_req_key, shared_mem_req_size,file_sha_mem_req);

    //shared memory for info
    key_t shared_mem_info_key = ftok("main.o" ,2);
    if (shared_mem_info_key == -1){
        printf("ftok not working\n");
        cout << strerror(errno) << endl;
    }  
    char file_sha_mem_info[50];
    sprintf(file_sha_mem_info,"keys/shared_mem_info.key");
    generate_memory_segment(shared_mem_info_key, shared_mem_info_size,file_sha_mem_info);
    
    // Attach the shared memory segment.
    int I_mem_id=get_memory_id_from_file(file_sha_mem_info , shared_mem_info_size);
    shared_mem_info* shared_memory_inf = (shared_mem_info*) shmat(I_mem_id, NULL, 0);
    if(shared_memory_inf==(void*)-1)die("shared memory atached problem");
    shared_memory_inf->lines_per_segm    =  segments_line;
    shared_memory_inf->request_per_child =  request_per_child;
    shared_memory_inf->segments          =  segments;
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
    //~~~~~~~~~~~~~~~~~~~Sheared memory~~~~~~~~~~~~~~~~~~~//
    //~~~~~~~~~~~~~~~~~~~~~~~end~~~~~~~~~~~~~~~~~~~~~~~~~~//

    //to-print-delete
    cout << "Calculate details:~~~~~~~~~ Segments: " << segments << endl;

    //~~~~~~~~~~~~~~~~~~~~~Begin~~~~~~~~~~~~~~~~~~~~~~~~~~//
    //~~~~~~~~~~~~~~~~~~~Semaphore~~~~~~~~~~~~~~~~~~~~~~~~//
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
    for(int i = 0; i < segments; i++ ){

        key_t semaphore_mem_req_key = ftok("main.o" ,i+4);

        char sem_name_file_req[50];
        sprintf(sem_name_file_req,"keys/semaphore_req%d.key",i);
        //to-print-delete
        //cout << "Semaphores details:~~~~~~~~~ name: " << sem_name_file_req << endl;
        int sem_req=generate_semaphore(semaphore_mem_req_key, sem_name_file_req);
        initialise_semaphore(sem_req);
        //semaphore_signal(sem_req);

        key_t shared_mem_resp_key = ftok("main.o" ,i+segments+5);
        char sem_name_file_resp[50];
        sprintf(sem_name_file_resp,"keys/semaphore_resp%d.key",i);
        //to-print-delete
        //cout << "Semaphores details:~~~~~~~~~ name: " << sem_name_file_req << endl;
        int sem_resp=generate_semaphore(shared_mem_resp_key, sem_name_file_resp);
        initialise_semaphore(sem_resp);
        //semaphore_signal(sem_resp);
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
    //~~~~~~~~~~~~~~~~~~~Semaphore~~~~~~~~~~~~~~~~~~~~~~~~//
    //~~~~~~~~~~~~~~~~~~~~~~end~~~~~~~~~~~~~~~~~~~~~~~~~~~//


    int request_log[segments][request_per_child * segments];

    for (int i = 0; i < child_num; i++) {

        if(fork() == 0) {
            child_procces();

            exit(0);
        }
        
 
        sleep(5);


        // Attach the shared memory segment.
        int I_mem_id1=get_memory_id_from_file(file_sha_mem_req , shared_mem_req_size);
        shared_mem_child_re* shared_memory_req = (shared_mem_child_re*) shmat(I_mem_id, NULL, 0);
        if(shared_memory_req ==(void*)-1)die("shared memory atached problem");

        //to-print-delete
        cout << "Parent shared mem :!~~~~~~~~ line_child: " <<  shared_memory_req->line_child
        << "Segment: " << shared_memory_req->segment_child << endl;
        
        char semaphore_name2[50];
        sprintf(semaphore_name2,"keys/semaphore_resp%d.key",shared_memory_req->segment_child);
        cout << "Parent semaphor response : " <<semaphore_name2<<endl;
        int sepaphore_id2 = get_semaphore_id_from_file(semaphore_name2);
        
        FILE *file_inp = fopen(file_name, "r");
        
        for (int i = 0; i < segments; i++){
            for (int y = 0; y < segments_line; y++)  {
                char line_detail[MAX_LINE_LENGTH] = {0};
                fgets(line_detail, MAX_LINE_LENGTH, file_inp);   
                //cout << "loop read : " <<  line_detail << endl;
                shared_memory_req->segment_detail[y].line = y;
                strcpy(shared_memory_req->segment_detail[y].info,line_detail); 
            }
        }
        fclose(file_inp);
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
    char file_seg_name[50];  
    
    /* Open file */
    FILE *file_inp = fopen(file_name_, "a+");
    FILE *file_segm ;
    
    if (!file_inp) {
        return -1;
    }

    int lines_new = segments * lines_seg;

    for (int i = 0; i < lines_new; i++){
        char generate_new_line[50];
        sprintf(generate_new_line,"%d\n",i);    
        fputs(generate_new_line,file_inp);
    }
    

    for (int i = 0; i < segments; i++){
        for (int y = 0; y < lines_seg; y++)  {
            char line_detail[MAX_LINE_LENGTH] = {0};
            fgets(line_detail, MAX_LINE_LENGTH, file_inp);
            if (y == 0){
                sprintf(file_seg_name,"files/segment_%d.txt",segm_count);
                file_segm = fopen(file_seg_name, "a+");
                if (!file_segm) {
                    cout << "error on file on segment creation\n";
                    return -1;
                }
                segm_count++;
            }
            fputs(line_detail,file_segm);
        }
        fclose(file_segm);
    }
    
    /* Close file */
    fclose(file_inp);

}