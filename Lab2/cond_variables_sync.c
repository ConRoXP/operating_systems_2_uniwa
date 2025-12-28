/*
 Εργαστήριο ΛΣ 2 / Άσκηση 2 / Ερώτημα 2 / 2024-25
 Ονοματεπώνυμο: ΡΟΜΠΕΡΤ ΠΟΛΟΒΙΝΑ
 ΑΜ: 23390338
 Τρόπος μεταγλώττισης: gcc LS2-23390338-Δ3-Α-2.c -o ls2_2
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define SIZE 3
#define REPEAT 6 //how many times the sequence will be printed

pthread_mutex_t mutex= PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond= PTHREAD_COND_INITIALIZER;
int flag= 0; //timing variable - 0: 1st thread | 1: 2nd thread | 2: 3rd thread

void* print_one(void*);
void* print_two(void*);
void* print_three(void*);

int main(){

    pthread_t threads[SIZE];

    if(pthread_create(&threads[0], NULL, print_one, NULL)!= 0){
        perror("Error creating thread 1");
        return -1;
    }
    if(pthread_create(&threads[1], NULL, print_two, NULL)!= 0){
        perror("Error creating thread 2");
        return -1;
    }
    if(pthread_create(&threads[2], NULL, print_three, NULL)!= 0){
        perror("Error creating thread 3");
        return -1;
    }

    for(int i= 0; i< SIZE; i++) pthread_join(threads[i], NULL);

    printf("\n");
    return 0;
}

void* print_one(void* arg){
    for(int i= 0; i< REPEAT; i++){
        pthread_mutex_lock(&mutex);
        while(flag!= 0) pthread_cond_wait(&cond, &mutex);
        printf("<one>");

        flag= 1; //2nd thread is signaled
        pthread_cond_broadcast(&cond);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void* print_two(void* arg){
    for(int i= 0; i< REPEAT; i++){
        pthread_mutex_lock(&mutex);
        while(flag!= 1) pthread_cond_wait(&cond, &mutex);
        printf("<two>");

        flag= 2; //3rd thread is signaled
        pthread_cond_broadcast(&cond);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void* print_three(void* arg){
    for(int i= 0; i< REPEAT; i++){
        pthread_mutex_lock(&mutex);
        while(flag!= 2) pthread_cond_wait(&cond, &mutex);
        printf("<three>");

        flag= 0; //1st thread is signaled
        pthread_cond_broadcast(&cond);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}
