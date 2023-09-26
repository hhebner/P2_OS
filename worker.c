#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
    //This holds the number of iterations to be performed in our loop below
    int num_loop_iter = atoi(argv[1]);
    //variables for the getpid() and getppid() functions 
    pid_t pid = getpid();
    pid_t ppid = getppid();
    //Our loop that prints out pid, ppid and what iteration the loop is currently on
    for (int i = 1; i < num_loop_iter + 1; i++){
        printf("WORKER PID:%d PPID:%d Iteration:%d before sleeping\n", pid, ppid, i);
        sleep(1);
        printf("WORKER PID:%d PPID:%d Iteration:%d after sleeping\n", pid, ppid, i);
    }

    return 0;
}