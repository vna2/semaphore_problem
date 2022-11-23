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

int generate_segments(char file_name_[50], int lines_seg,int segments);

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
        
    int child_num      = atoi(argv[1]);
    int max_lines      = atoi(argv[2]);
    int segments_line  = atoi(argv[3]);
    char file_name[25];
    strcpy(file_name ,argv[4]);

    int segments = max_lines / segments_line;
    cout << max_lines << " " << segments_line << " " << file_name << " " << segments << endl;
    
    
    //shared_mem_info mem_info_arr[segments];     
    
    //Shared_memory
    ////multiple shared memory
    //for (int i = 0; i < segments; i++) {
//
    //    mem_info_arr[i].segment = i;
//
    //    char key_char[50];
    //    key_t shared_mem_key = ftok("main.o" ,i);
    //    
    //    if (shared_mem_key == -1){
    //        printf("ftok not working\n");
    //        cout << strerror(errno) << endl;
    //    }
    //    mem_info_arr[i].mem_key = shared_mem_key;
//
    //    char file_name_[50];
    //    sprintf(file_name_,"keys/segment_%d.key",mem_info_arr[i].segment);
    //    strcpy(mem_info_arr[i].name_file,file_name_);
//
    //    generate_memory_segment(mem_info_arr[i].mem_key, shared_mem_size, mem_info_arr[i].name_file);
    
    //}
    //

    //one shared memory 
    key_t shared_mem_key = ftok("main.o" ,1);
    if (shared_mem_key == -1){
        printf("ftok not working\n");
        cout << strerror(errno) << endl;
    }
    
    char file_sha_mem[50];
    sprintf(file_sha_mem,"keys/shared_mem.key");
    generate_memory_segment(shared_mem_key, shared_mem_size,file_sha_mem);
   

    time_t t;
    srand((unsigned) time(&t));
    char file_name_[50];
    sprintf(file_name_,"program.txt");
    generate_segments(file_name_,segments_line,segments);
    

    for (int i = 0; i < child_num; i++) {
        int rand_segnent = rand() % segments;
        int rand_line    = rand() % segments;

        //print-details
        cout << endl<< endl<< endl << endl <<"for_loop: " <<file_sha_mem << endl;

        // Attach the shared memory segment.
        int I_mem_id=get_memory_id_from_file(file_sha_mem, shared_mem_size);
        shared_mem* shared_memory = (shared_mem*) shmat(I_mem_id, NULL, 0);
        if(shared_memory==(void*)-1)die("shared memory atached problem");
        //shared_memory = new shared_mem;
        
        shared_memory->line    = rand_line;
        shared_memory->segment = rand_segnent;

        //read line from specific segment
        char file_seg_name[50];
        sprintf( file_seg_name, "files/segment_%d.txt",rand_segnent);
        
        FILE *file_inp = fopen( file_seg_name, "r");

        if (!file_inp) {
            return -1;
        }

        //print-details
        cout << endl << "file seg name: " << file_seg_name << " line rand: " << rand_line<< endl;

        for (int i = 0; i <= rand_line; i++){
            char line_detail_[200];
            fgets(line_detail_, MAX_LINE_LENGTH, file_inp);
            //print-details
            cout << "line that read: " << line_detail_ << endl;
            if (i == rand_line){
                strcpy(shared_memory->line_detail,line_detail_);
                
                //print-details
                cout << "line that copyed on shared memory: "  << shared_memory->line_detail << endl;
                
            }
        }

        /* Close file */
        fclose(file_inp);

        //Detach the shared memory segment
        shmdt(shared_memory);

        if(fork() == 0) {
            child_procces();

            exit(0);
        }
    }

    //clear_mem(file_sha_mem,shared_mem_size);
    //Detached all shared memory
    //for (int i = 0; i < segments; i++){ 
    //    clear_mem(mem_info_arr[i].name_file,shared_mem_size);
    //}
}


int generate_segments(char file_name_[50], int lines_seg,int segments){

    char line_detail[MAX_LINE_LENGTH] = {0};
    int  line_count = 0;
    int  segm_count = 0;
    char file_seg_name[50];   
     
    /* Open file */
    FILE *file_inp = fopen(file_name_, "r");
    FILE *file_segm ;
    
    if (!file_inp) {
        return -1;
    }

    for (int i = 0; i < segments; i++){
        for (int y = 0; y < lines_seg; y++)  {
            fgets(line_detail, MAX_LINE_LENGTH, file_inp);
            //cout << line_detail <<endl;
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






