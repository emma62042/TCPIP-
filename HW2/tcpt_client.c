#include <stdio.h>
#include <stdlib.h>
#include <string.h>  //for bzero()
#include <unistd.h>  //for close()
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <pthread.h>

#define Server_PortNumber 4400
#define Server_Address "127.0.0.1"

int down=0;

void *Send (void *sock)
{ 
    char sendbuffer[1024];
    int byte_sent;
    while(1)
    {
        //printf("Packet content:");
        gets(sendbuffer);
        byte_sent = send((int)sock, sendbuffer, sizeof(sendbuffer),0);
        if (byte_sent < 0)    printf("Error sending packet!\n");
    }
    pthread_exit(NULL);
}

void *Receive(void *sock)
{
    char rcvbuffer[1024];
    int byte_recv;
    while(byte_recv = recv((int)sock, rcvbuffer, sizeof(rcvbuffer),0)>=0)
    {
        //byte_recv = recv((int)sock, rcvbuffer, sizeof(rcvbuffer),0);
        if(strlen(rcvbuffer)==0)
        {
            continue;
        } 
        if (byte_recv < 0)    printf("Error sending packet!\n");
        printf("%s\n",rcvbuffer);

    }
    pthread_exit(NULL);
}


int main(int argc, char *argv[]) {
    struct timeval start;
    struct timeval end;
    unsigned long diff;

    struct sockaddr_in server_addr;
    int sock, byte_sent,byte_recv, server_addr_length = sizeof(server_addr),recfd;
    char sendbuffer[50],rcvbuffer[10];

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock < 0)    printf("Error creating socket!\n");

    bzero(&server_addr, server_addr_length);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(Server_PortNumber);
    server_addr.sin_addr.s_addr = inet_addr(Server_Address);

    if (connect(sock, (struct sockaddr *)&server_addr,server_addr_length)==-1) {
        printf("connect failed!");
        close(sock);}

    pthread_t thread1,thread2;
    pthread_create(&thread1,NULL,Send,(void *)sock);
    pthread_create(&thread2,NULL,Receive,(void *)sock);
    if(down==1)
        {close(sock);return 0;}
    pthread_join(thread1,NULL);
    pthread_join(thread2,NULL);

    close(sock);
    return 0;
}
