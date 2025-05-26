/*
 Εργαστήριο ΛΣ 2 / Άσκηση 2 / Ερώτημα 3 / 2024-25
 Ονοματεπώνυμο: ΡΟΜΠΕΡΤ ΠΟΛΟΒΙΝΑ
 ΑΜ: 23390338
 Τρόπος μεταγλώττισης: gcc LS2-23390338-Δ3-Α-3b.c -o ls2_client
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SOCK_PATH "socket"
#define SIZE 1024

int main(){
    int sock, N, bytes, flag= 0;
    int* seq;
    struct sockaddr_un addr;
    char str[SIZE];

    sock= socket(AF_UNIX, SOCK_STREAM, 0);
    if(sock< 0){
        perror("Error opening stream socket");
        exit(-1);
    }

    addr.sun_family= AF_UNIX;
    strcpy(addr.sun_path, SOCK_PATH);

    printf("Connecting...\n");
    if(connect(sock, (struct sockaddr*)&addr, sizeof(struct sockaddr_un))< 0){
        perror("Error connecting to stream socket");
        close(sock);
        exit(-1);
    }
    printf("Connection successful\n");

    while(1){
        printf("Enter the size of the sequence or 0 to exit\n");
        scanf("%d", &N);
        if(N<= 0) break;
        seq= (int*)malloc(N*sizeof(int));
        flag= 1;
        /*
         If the user inputs 0 as the first value, we get a free() error.
         By using the flag variable we avoid that case.
        */

        printf("Enter each number of the sequence (N(i))\n");
        for(int i= 0; i< N; i++){
            scanf("%d", &seq[i]);
        }

        //Sending the size of the sequence to the server.
        if(write(sock, &N, sizeof(int))< 0){
            perror("Error writing in the socket");
            close(sock);
            exit(-1);
        }

        //Sending the sequence to the server.
        if(write(sock, seq, sizeof(int)*N)< 0){
            perror("Error writing in the socket");
            close(sock);
            exit(-1);
        }

        //Saving how many bytes the server's message is.
        bytes= read(sock, str, SIZE-1);
        if(bytes<= 0){
            printf("No reply from the server\n");
            break;
        }
        //Converting to C string with the right size.
        str[bytes]= '\0';
        printf("The server replied: %s\n", str);
    }

    close(sock);
    if(flag== 1) free(seq);
    return 0;
}