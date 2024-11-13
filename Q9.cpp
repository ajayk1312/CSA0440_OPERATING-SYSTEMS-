#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#define SHM_SIZE 1024  

int main() {
    key_t key = ftok("shmfile", 65);
    if (key == -1) {
        perror("ftok");
        exit(1);
    }

    int shmid = shmget(key, SHM_SIZE, 0666 | IPC_CREAT);
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }

    
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        
        char *shared_memory = (char *)shmat(shmid, NULL, 0);
        if (shared_memory == (char *)(-1)) {
            perror("shmat");
            exit(1);
        }

        sleep(1); 
        printf("Child Process: Data read from shared memory: '%s'\n", shared_memory);

        shmdt(shared_memory);  
    } else {
        char *shared_memory = (char *)shmat(shmid, NULL, 0);
        if (shared_memory == (char *)(-1)) {
            perror("shmat");
            exit(1);
        }

        char message[] = "Hello from parent process!";
        strncpy(shared_memory, message, SHM_SIZE);

        wait(NULL);  
        shmdt(shared_memory);  
        shmctl(shmid, IPC_RMID, NULL); 
    }

    return 0;
}
