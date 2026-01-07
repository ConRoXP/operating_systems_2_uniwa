/*
 Εργαστήριο ΛΣ 2 / Άσκηση 2 / Ερώτημα 1 / 2024-25
 Ονοματεπώνυμο: ΡΟΜΠΕΡΤ ΠΟΛΟΒΙΝΑ
 ΑΜ: 23390338
 Τρόπος μεταγλώττισης: gcc LS2-23390338-Δ3-Α-1.c -o ls2_1
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int* X;
int total_max= -1;
pthread_mutex_t mutex= PTHREAD_MUTEX_INITIALIZER;

typedef struct{
    int start;
    int end;
} threadArgs;

void* localMax(void* arg);

int main(){

    int n, t;
    pthread_t* threads;
    int chunk;
    threadArgs* args;
    
    printf("Enter number of digits (n):\n");
    scanf("%d", &n);

    printf("Enter number of threads (t)\n");
    scanf("%d", &t);
    threads= (pthread_t*)malloc(t*sizeof(pthread_t));

    printf("Enter each number of the sequence (x(i))\n");
    X= (int*)malloc(n*sizeof(int));
    for(int i= 0; i< n; i++){
        scanf("%d", &X[i]);
    }
    printf("\n");
    chunk= n/t;

    /*
     args= (threadArgs*)malloc(t*sizeof(threadArgs)); <- My first idea
     Didn't work because (void*)args[i] is not allowed by the compiler.
    */
    for(int i= 0; i< t; i++){
        threadArgs* args= (threadArgs*)malloc(sizeof(threadArgs));
        args->start= i*chunk;
        args->end= (i+1)*chunk;
        // I'm not using (i+1)*chunk-1 - I deal with it inside the function.
        if(pthread_create(&threads[i], NULL, localMax, (void*)args)!= 0){
            perror("Error creating thread");
            return -1;
        }
    }

    for(int i= 0; i< t; i++){
        pthread_join(threads[i], NULL);
    }

    printf("Max: %d\n", total_max);

    free(X);
    free(threads);
    return 0;
}

void *localMax(void* arg){
    threadArgs* args= (threadArgs*)arg; //typecasting from void*
    int local_max= X[args->start];

    // If I had (i+1)*chunk-1, I'd use i<= args->end instead.
    for(int i= args->start+1; i< args->end; i++){
        if(X[i]> local_max) local_max= X[i];
    }

    pthread_mutex_lock(&mutex);
    if(local_max> total_max) total_max= local_max;
    pthread_mutex_unlock(&mutex);

    free(arg);
    pthread_exit(NULL);
}

/*
 To test the runtimes I temporarily modified the program
 to input a number of random numbers through the use of rand().
 For each test I assigned a value of n= 1,000,000 positions in the X matrix
 where each index received 1 to 3 (random) digits.

 For the measurements I modified the program as follows:

 struct timespec start, end;
 clock_gettime(CLOCK_MONOTONIC, &start);
 /thread code/
 clock_gettime(CLOCK_MONOTONIC, &end);
 double elapsed= (end.tv_sec- start.tv_sec) + (end.tv_nsec- start.tv_nsec) /1e9;
 printf("Time: %.6f sec\n", elapsed);

 In the section of code between clock_gettime() I made sure to include only the
 threads' code so that I/O would not affect the measurements.

 1 thread: 0.001162 sec
 2 threads: 0.000744 sec
 4 threads: 0.000512 sec
 8 threads: 0.000952 sec
 12 threads: 0.001733 sec
 16 threads: 0.003146 sec

 It seems that our program benefits from the increase to 4 threads.
 More than 4 and the various delays that accompany the creation
 of threads end up working negatively.

 Also, from 4 threads and above I also noticed variation in execution times which
 was not present for threads 1 & 2. The times I write above are the averages of
 6 measurements for threads >= 4.
*/
