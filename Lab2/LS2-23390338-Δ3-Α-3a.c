/*
 Εργαστήριο ΛΣ 2 / Άσκηση 2 / Ερώτημα 3 / 2024-25
 Ονοματεπώνυμο: ΡΟΜΠΕΡΤ ΠΟΛΟΒΙΝΑ
 ΑΜ: 23390338
 Τρόπος μεταγλώττισης: gcc LS2-23390338-Δ3-Α-3a.c -o ls2_server
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SOCK_PATH "socket"
#define SIZE 1024

void manage_client(int);

int main(){
    int server_sock, client_sock;
    struct sockaddr_un addr;
    socklen_t addrlength= sizeof(addr);

    unlink(SOCK_PATH);

    server_sock= socket(AF_UNIX, SOCK_STREAM, 0);
    if(server_sock< 0){
        perror("Error opening stream socket");
        exit(-1);
    }

    addr.sun_family= AF_UNIX;
    strcpy(addr.sun_path, SOCK_PATH);
    if(bind(server_sock, (struct sockaddr*)&addr, sizeof(struct sockaddr_un))){
        perror("Error binding stream socket");
        exit(-1);
    }

    if(listen(server_sock, 5)< 0){
        perror("Server listen error");
        exit(-1);
    }

    while(1){
        client_sock= accept(server_sock, (struct sockaddr *)&addr, &addrlength);
        if(client_sock< 0){
            perror("Accept error\n");
            continue;
        }

        //I create new processes so that the server can handle multiple clients.
        if(fork()== 0){
            close(server_sock);
            manage_client(client_sock);
        }

        close(client_sock);
    }

    close(server_sock);
    unlink(SOCK_PATH);
    return 0;
}

void manage_client(int client_sock){
    int N, sum= 0;
    int* seq;
    double avg;
    char str[SIZE];

    while(1){
        if(read(client_sock, &N, sizeof(int))<= 0){
            break;
        }
        seq= (int*)malloc(N*sizeof(int));
        if(read(client_sock, seq, sizeof(int)*N)<= 0){
            perror("Error reading from client\n");
            break;
        }

        for(int i= 0; i< N; i++){
            sum+= seq[i];
        }
        avg= (double)sum/N;

        //By using snprintf text is saved as a C string in the str array.
        if(avg> 20.0) snprintf(str, SIZE, "Avg: %.2f\tSequence Ok\n", avg);
        else snprintf(str, SIZE, "Check failed\n");

        write(client_sock, str, strlen(str));
        sum= 0;
    }

    close(client_sock);
    free(seq);
    exit(0);
}