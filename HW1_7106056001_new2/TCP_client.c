#include <stdio.h>
#include <stdlib.h>
#include <string.h>  //for bzero()
#include <unistd.h>  //for close()
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>

#define Server_PortNumber 5200
#define Server_Address "127.0.0.1"

int main(int argc, char *argv[]) {
    struct timeval start;
    struct timeval end;
    unsigned long diff;

    struct sockaddr_in server_addr;
    int sock, byte_sent,byte_recv, server_addr_length = sizeof(server_addr),recfd;
    char sendbuffer[50],rcvbuffer[19];

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock < 0)    printf("Error creating socket!\n");

    bzero(&server_addr, server_addr_length);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(Server_PortNumber);
    server_addr.sin_addr.s_addr = inet_addr(Server_Address);

    if (connect(sock, (struct sockaddr *)&server_addr,server_addr_length)==-1) {
        printf("connect failed!");
        close(sock);}

    printf("Packet content:");
    scanf("%[^\n]",sendbuffer);

    gettimeofday(&start,NULL);

    byte_sent = send(sock, sendbuffer, sizeof(sendbuffer),0);
    if (byte_sent < 0)    printf("Error sending packet!\n");

    byte_recv = recv(sock, rcvbuffer, sizeof(rcvbuffer),0); 
    if (byte_recv < 0)    printf("Error sending packet!\n");

    gettimeofday(&end,NULL);

    printf("Server response: %s\n",rcvbuffer);

    diff = 1000000 * (end.tv_sec-start.tv_sec)+ end.tv_usec-start.tv_usec;
    diff=diff/2;
    printf("the latency is %ld (ms)\n",diff);

    close(sock);
    return 0;
}
