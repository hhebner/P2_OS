#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main() {
    const int seconds_key = 5396211;
    const int nano_key = 2798414;

    int shmid_seconds = shmget(seconds_key, sizeof(int), IPC_CREAT|0666);
    if (shmid_seconds <= 0){
        printf("Shared memory failed for seconds");
    }
    int smid_nano = shmget(nano_key, sizeof(int), IPC_CREAT|0666);
    if (smid_nano <= 0){
        printf("Shared memoryt failed for nano");
    }
    int *seconds_ptr = shmat(shmid_seconds, 0, 0);
    if (seconds_ptr <= 0){
        printf("Failed to attach to seconds shared memory");
    }
    int *nano_ptr = shmat(smid_nano, 0, 0);
    if (nano_ptr <= 0){
        printf("Failed to attach to nano shared memory");
    }

    shmdt(seconds_ptr);
    shmdt(nano_ptr);
    shmctl(shmid_seconds, IPC_RMID, NULL);
    shmctl(smid_nano, IPC_RMID, NULL);


}
