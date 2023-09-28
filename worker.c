#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/shm.h>


int main(int argc, char *argv[]){

    int time_limit_s = atoi(argv[1]);
    int time_limit_n = atoi(argv[2]);
    
    
    const int seconds_key = 5396211;
    const int nano_key = 2798414;

    int shmid_seconds = shmget(seconds_key, sizeof(int), 0666);
    if (shmid_seconds <= 0){
        printf("Shared memory failed for seconds");
    }
    int shmid_nano = shmget(nano_key, sizeof(int), 0666);
    if (shmid_nano <= 0){
        printf("Shared memory failed for nano");
    }
    int *seconds_ptr_worker = shmat(shmid_seconds, 0, 0);
    if (seconds_ptr_worker <= 0){
        printf("Failed to attach to seconds shared memory");
    }
    int *nano_ptr_worker = shmat(shmid_nano, 0, 0);
    if (nano_ptr_worker <= 0){
        printf("Failed to attach to nano shared memory");
    }


     int start_time_s = *seconds_ptr_worker;
     int prev_second = *seconds_ptr_worker;
     int term_time_s = ((*seconds_ptr_worker) + time_limit_s);
     int term_time_n = ((*nano_ptr_worker) + time_limit_n);

      while (term_time_n >= 1000000000) {
        term_time_n -= 1000000000;
        term_time_s++;
    }


    printf("WORKER PID: %d PPID: %d SysClockS: %d SysclockNano: %d TermTimeS: %d TermTimeNano: %d\n --Just Starting\n",
     getpid(), getppid(), *seconds_ptr_worker, *nano_ptr_worker, term_time_s, term_time_n);

    while ((*seconds_ptr_worker < term_time_s) || (*seconds_ptr_worker) == term_time_s && (*nano_ptr_worker) < term_time_n){
        if (prev_second < *seconds_ptr_worker){
            int time_elapsed = *seconds_ptr_worker - start_time_s;
            printf("WORKER PID: %d PPID: %d SysClockS: %d SysclockNano: %d TermTimeS: %d TermTimeNano: %d\n --%d seconds have passed since starting\n", getpid(), getppid(), *seconds_ptr_worker, *nano_ptr_worker, term_time_s, term_time_n, time_elapsed);
            prev_second = *seconds_ptr_worker;
        }

    }
    printf("WORKER PID: %d PPID: %d SysClockS: %d SysclockNano: %d TermTimeS: %d TermTimeNano: %d\n --Terminating\n", getpid(), getppid(), *seconds_ptr_worker, *nano_ptr_worker, term_time_s, term_time_n);

    shmdt(seconds_ptr_worker);
    shmdt(nano_ptr_worker);

    return 0;
}


    