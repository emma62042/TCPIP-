#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define Server_PortNumber 5555
#define Server_Address "127.0.0.1"


pthread_t thread_s, thread_r, thread;
char buffer[50];

void *Send(void *fd)
{	
	int byte_sent;

	while(1)
	{
		printf("Enter :");
		scanf("%s", buffer);
		
		byte_sent = send(fd, buffer, sizeof(buffer), 0); 
		if(byte_sent < 0) printf("Error sending packet\n");
		
		if(strcmp(buffer,"exit")==0)
			break;
	}
}

void *Receive(void *fd)
{	
	int byte_recv;
	
	while(1)
	{
		byte_recv = recv(fd, buffer, sizeof(buffer),0);
		
		if(byte_recv < 0) printf("Error recving packet(%d)\n",byte_recv);
		printf("%s\n", buffer);
	}
}


int main(int argc, char *argv[])
{
	struct sockaddr_in server_addr;
	int sock, server_addr_length = sizeof(server_addr);
	//int byte_sent, byte_recv, recfd;

	int rc, i;
	
	struct timeval start;
	struct timeval end;
	long int diff, throuput;

	//for compute latency
	gettimeofday(&start, NULL);

	sock = socket(PF_INET, SOCK_STREAM, 0);
	if(sock < 0)
		printf("Error creating socket\n");

	bzero(&server_addr, server_addr_length);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(Server_PortNumber);
	server_addr.sin_addr.s_addr =inet_addr(Server_Address);

	if(connect(sock,(struct sockaddr *)&server_addr, server_addr_length) == -1){
		printf("connent failed\n");
		close(sock);
	}
	

		
	pthread_create(&thread_s, NULL, Send, (void *)sock);
	pthread_create(&thread_r, NULL, Receive, (void *)sock);
	
	pthread_join(thread_s, NULL);
	//pthread_join(thread_r, NULL);

	
	
		
		

	close(sock);

	return 0;

}
