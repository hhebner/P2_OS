#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

//Help message function
void help(){
        printf("Please use this template for running the program: oss [-h] [-n proc] [-s simul] [-t iter], Where -h is the help command, -n is the amount of children to be deployed, -s is the number of processes running at the same time and -t is the number of iterations worker.c will run. -n, -s, -t all take an integer\n");
}


int main(int argc, char *argv[]){
    int children = 0, iterations = 0, simulations = 0, opt;//keeping track of each command line argument and opt
    int  active_workers = 0, total_workers_launched = 0;//These are neccessary for -s command to work. 
    char iter_to_string[3];//execlp does not take in integers so this will be used to store iterations in string format

    //These are the command line arguments the user input is converted to integer becasue the command line takes them in as strings
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
                iterations = atoi(optarg);
                break;
            default:
                help();
                exit(1);
        }
    }
   //This is where we take the value from iterations and store it as a string in iter_to_string 
   sprintf(iter_to_string,"%d", iterations); 

  
   while (total_workers_launched < children || active_workers > 0) {
           if (active_workers < simulations && total_workers_launched < children){
                pid_t fork_pid = fork();
                if (fork_pid == 0) { //Then we are working with child process
                        execlp("./worker", "worker", iter_to_string, NULL);//This is sending our number of iterations to worker.c
                        perror("execlp has failed");//if execlp fails for some reason perror will give us a descriptive error message 
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
                   active_workers--;
           }
   }

   
   printf("All workers have finished\n");
   return 0;
}
