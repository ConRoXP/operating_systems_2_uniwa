/*
 Εργαστήριο ΛΣ 2 / Άσκηση 1 / Ερώτημα 2 / 2024-25
 Ονοματεπώνυμο: ΡΟΜΠΕΡΤ ΠΟΛΟΒΙΝΑ
 ΑΜ: 23390338
 Τρόπος μεταγλώττισης: gcc LS2-23390338-Δ3-Α-2.c -o ls2
*/

/*
 It is possible for orphan or zombie processes to be created.
 P1 will become a zombie process as it finishes but P0 does not collect
 its exit status.

 Also P2 only collects the exit status of the child process
 that will finish first. Therefore the rest will become zombie processes as
 they finish while P2 is still in the for() loop creating children.

 The last child process of P2 is interesting as it prints its data after P2 has printed
 that it is terminating (for N>= 3). However the last child has time to print that
 it has ppid the pid of P2 so it too becomes a zombie and not an orphan.

 The exit statuses of P2, P3 & P4 are collected by their parents.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define SIZE 30

void forkErr(int pid){
    if(pid< 0){
        perror("Fork error!");
        exit(-1);
    }
}

int main(){
    //P0
    int P1, P2, P3, P4;
    int status, status2, status3, status4;
    int fd[2];
    char msg[SIZE]; //P3 message
    int i, chID, N;
    int* PIDs;
    char* args[]= {"ps", NULL}; //ps cmd arguments

    if(pipe(fd)< 0){
        perror("Pipe Error");
        exit(-1);
    }

    printf("Enter number of P2 child processes to create\n");
    scanf("%d", &N);
    PIDs= (int*)malloc(N*sizeof(int));
    //Initializing the array to make the validity check reliable.
    for(i= 0; i< N; i++){
        PIDs[i]= -1;
    }

    printf("\n    PID    PPID\n");
    printf("P0: %d, %d\n", getpid(), getppid());

    P1= fork();
    forkErr(P1);
    if(P1== 0){
        //Ρ1
        printf("P1: %d, %d\n", getpid(), getppid());
        P3= fork();
        forkErr(P3);
        if(P3!= 0){
            //Ρ1
            P4= fork();
            forkErr(P4);
            if(P4== 0){
                //Ρ4
                printf("P4: %d, %d\n", getpid(), getppid());
                exit(0);
            }
            /*
             Ρ1
             
             Because P4 is terminated by exit(),
             there's no need to put the following code in an else block
             to prevent it from running twice.
            */
            waitpid(P3, &status3, 0);
            close(fd[1]);
            read(fd[0], msg, SIZE);
            printf("P1 received message: %s\n", msg);
            close(fd[0]);

            waitpid(P4, &status4, 0);
            printf("P4 exited with code %d\n", WEXITSTATUS(status4));
        }
        else{
            //Ρ3
            printf("P3: %d, %d\n", getpid(), getppid());

            close(fd[0]);
            write(fd[1], "Hello from your child.", 22);
            close(fd[1]);
        }
    }

    else{
        //Ρ0
        P2= fork();
        forkErr(P2);
        if(P2== 0){
            //Ρ2
            printf("P2: %d, %d\n", getpid(), getppid());
            for(i= 0; i< N; i++){
                PIDs[i]= fork();
                forkErr(PIDs[i]);
                if(PIDs[i]== 0){
                    printf("P2_%d: %d, %d\n", i+1, getpid(), getppid());
                    exit(0);
                }
            }
            chID= wait(&status);
            printf("Child of P2 that finished first (PID): %d\nP2 exiting...\n", chID);
            exit(0);
        }
        else{
            //Ρ0
            waitpid(P2, &status2, 0);
            free(PIDs);
            printf("P0 terminating & executing ps...\n\n");
            execv("/bin/ps", args);
        }
    }

    return 0;
}
