sem-reader[segme] = 0;
sem-writer[segme] = 0;
mutex = 0



void readers(){
    while(1){
        segm = rand();
        if(child== 0 ){
            sh_mem_count_per_seg[segm]++;
            shm-req-segm = segm;
            up(sem-writer[shm-req-segm]);
        }

    

        read and write on file
    }



}

void writers(){
    requests[segments];
    while (1){
        
        read-request

        if(shm-req-segm <> shm-req-segm ){
            down(sem-writer[shm-req-segm])
            write segm;
        }
    }
    
    
    

}