#include <stdio.h>
#include <stdlib.h>
#include <string.h>  //for bzero()
#include <unistd.h>  //for close()
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>

#define Server_PortNumber 5400
#define Server_Address "127.0.0.1"

int main(int argc, char *argv[]) 
{
    struct timeval start;
    struct timeval end;
    unsigned long diff;
    struct sockaddr_in address;
    int sock, byte_sent,byte_recv;
    char sendbuffer[50],rcvbuffer[50];

	sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (sock < 0)    printf("Error creating socket\n");

    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    //The "PF_" prefix stands for "protocol family and the "AF_" prefix stands for "address family"
    address.sin_port = htons(Server_PortNumber);
    address.sin_addr.s_addr = inet_addr(Server_Address);
    
    int address_length = sizeof(address);
    
    printf("Packet content:");
    scanf("%[^\n]",sendbuffer);
    
    gettimeofday(&start,NULL);

    byte_sent = sendto(sock, sendbuffer, sizeof(sendbuffer), 0, (struct sockaddr *)&address, address_length);
    
    gettimeofday(&end,NULL);
    if (byte_sent < 0)    printf("Error sending packet\n");

    byte_recv = recvfrom(sock, rcvbuffer, sizeof(rcvbuffer), 0, (struct sockaddr *)&address, &address_length);
    
    if (byte_recv < 0)    printf("Error recving packet\n");
    gettimeofday(&end,NULL);
	printf("Server response: %s\n",rcvbuffer);

    diff = 1000000 * (end.tv_sec-start.tv_sec)+ end.tv_usec-start.tv_usec;
    diff=diff/2;
    printf("the latency is %ld (ms)\n", diff);

    close(sock);
    return 0;
}

