#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <time.h>

struct PCB {
    int occupied; 
    pid_t pid; 
    int start_seconds;
    int start_nano; 
};
struct PCB process_table[20];

void update_PCB(pid_t pid) {
    for (int i = 0; i < 20; i++) {
        if (!process_table[i].occupied) {
            process_table[i].occupied = 1;
            process_table[i].pid = pid;
            process_table[i].start_seconds = *seconds_ptr;
            process_table[i].start_nano = *nano_ptr;
            break;
        }
    }
}

void updatePCBOnTermination(pid_t pid) {
    for (int i = 0; i < 20; i++) {
        if (process_table[i].pid == pid) {
            process_table[i].occupied = 0;
            process_table[i].pid = 0;
            process_table[i].start_seconds = 0;
            process_table[i].start_nano = 0;
        }
    }
}

void printprocess_table() {
    printf("OSS PID:%d SysClockS: %d SysclockNano: %d\n", getpid(), *seconds_ptr, *nano_ptr);
    printf("Process Table:\nEntry Occupied PID StartS StartN\n");
    for (int i = 0; i < 20; i++) {
        printf("%d %d %d %d %d\n", i, process_table[i].occupied, process_table[i].pid, process_table[i].start_seconds, process_table[i].start_nano);
    }
}


void help(){
        printf("Please use this template for running the program: oss [-h] [-n proc] [-s simul] [-t iter]");
}

int main(int argc, char *argv[]) {

    int children = 0, simulations = 0, time_limit = 0, opt;
    srand(time(NULL));

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
    int total_workers_launched = 0, active_workers = 0;

    while (total_workers_launched < children || active_workers > 0) {
            (*nano_ptr) += 200000;
            if((*nano_ptr) >= 1000000000){
                    (*seconds_ptr) += 1;
                    (*nano_ptr) -= 1000000000;
                }

            if(*nano_ptr % 500000000 == 0){
                   print_PCB();

           if (active_workers < simulations && total_workers_launched < children){
                int rand_seconds = (rand() % time_limit) + 1;
                int rand_nano = rand() % 1000000000;
                char second_string[10];
                char nano_string[15];
                sprintf(second_string, "%d", rand_seconds);
                sprintf(nano_string, "%d", rand_nano);

                pid_t fork_pid = fork();
                if (fork_pid == 0) { //Then we are working with child process
                        execlp("./worker", "worker", second_string, nano_string, NULL);//This is sending our number of iterations to worker.c
                        perror("execlp has failed");//if execlp fails for some reason perror will give us a descriptive error message
                        update_PCB(fork_pid);
                        exit(1);
                } else if (fork_pid > 0) {//In parent process
                       //We increment active_workers and total_workers_launched to keep track of our processes
                       total_workers_launched++;
                       active_workers++;
                } else {
                       perror("fork has failed");
                       exit(1);
                }
           }
           pid_t wait_pid = waitpid(-1, NULL, WNOHANG);
           if (wait_pid > 0) {
                   update_PCB_on_termination(wait_pid);
                   active_workers--;
           }


        }
    shmdt(seconds_ptr);
    shmdt(nano_ptr);
    shmctl(shmid_seconds, IPC_RMID, NULL);
    shmctl(shmid_nano, IPC_RMID, NULL);

    return 0;

}

