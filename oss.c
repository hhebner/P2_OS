#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>


void help(){
        printf("Please use this template for running the program: oss [-h] [-n proc] [-s simul] [-t iter]")
}

int main(int argc, char *argv[]) {

    int children = 0, iterations = 0, time_limit = 0, opt;

    while ((opt = getopt(argc, argv, "hn:s:t:")) != -1){
        switch(opt) {
            case 'h':
               help();
               exit(0);
            case 'n':
                children = atoi(optarg);
                break;
            case 's':
                simulations = atoi(optarg);
                break;
            case 't':
                time_limit = atoi(optarg);
                break;
            default:
                help();
                exit(1);
        }
    }




    const int seconds_key = 5396211;
    const int nano_key = 2798414;

    int shmid_seconds = shmget(seconds_key, sizeof(int), IPC_CREAT|0666);
    if (shmid_seconds <= 0){
        printf("Shared memory failed for seconds");
    }
    int shmid_nano = shmget(nano_key, sizeof(int), IPC_CREAT|0666);
    if (shmid_nano <= 0){
        printf("Shared memory failed for nano");
    }
    int *seconds_ptr = shmat(shmid_seconds, 0, 0);
    if (seconds_ptr <= 0){
        printf("Failed to attach to seconds shared memory");
    }
    int *nano_ptr = shmat(shmid_nano, 0, 0);
    if (nano_ptr <= 0){
        printf("Failed to attach to nano shared memory");
    }

    *seconds_ptr = 0;
    *nano_ptr = 0;

    for (int i = 0; i < 41; i++){
        (*nano_ptr) += 100000000;
        if(*nano_ptr == 1000000000){
            (*seconds_ptr) += 1;
            *nano_ptr = 0;
        }
        printf("Seconds: %d Nanoseconds: %d\n", *seconds_ptr, *nano_ptr);
    }

    shmdt(seconds_ptr);
    shmdt(nano_ptr);
    shmctl(shmid_seconds, IPC_RMID, NULL);
    shmctl(shmid_nano, IPC_RMID, NULL);

    return 0;

}
