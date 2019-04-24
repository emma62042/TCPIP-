#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define Server_PortNumber 5555
#define Server_Address "127.0.0.1"


pthread_t thread_s, thread_r, thread;
int isClosed = 0;


void *Send(void *fd)
{	
	int byte_sent;
	char buffer[50];
	while(1)
	{
		printf("Enter :");
		scanf("%s", buffer);
		
		byte_sent = send(fd, buffer, sizeof(buffer), 0); 
		if(byte_sent < 0) printf("Error sending packet\n");
		
		////
		if(strcmp(buffer,"exit") == 0)
		{
			isClosed = 1;
			printf("bye\n");
			break;
		}		
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
	struct timeval timeout;

	long int diff, throuput;
	fd_set readfds;

	//for compute latency
	gettimeofday(&start, NULL);

	sock = socket(PF_INET, SOCK_STREAM, 0);
	if(sock < 0)
	{
		printf("Error creating socket\n");
		exit(0);
	}		

	bzero(&server_addr, server_addr_length);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(Server_PortNumber);
	server_addr.sin_addr.s_addr =inet_addr(Server_Address);

	if(connect(sock,(struct sockaddr *)&server_addr, server_addr_length) == -1){
		printf("connent failed\n");
		exit(0);
		close(sock);
	}
	
	timeout.tv_sec = 0;
	timeout.tv_usec = 100000; //0.1sec


	pthread_create(&thread_s, NULL, Send, (void *)sock);

	while(1)
	{
		if (isClosed == 1)
			break;
		FD_ZERO(&readfds);
		FD_SET(sock, &readfds);
		
		select(sock+1, &readfds, NULL, NULL, &timeout);

		if (FD_ISSET(sock, &readfds))
		{
			int byte_recv;
			char buffer[50];

			byte_recv = recv(sock, buffer, sizeof(buffer),0);
			printf("\n %s\nEnter :", buffer);
		}
	}
	



	//
	//pthread_create(&thread_r, NULL, Receive, (void *)sock);
	
	//pthread_join(thread_s, NULL);
	//pthread_join(thread_r, NULL);

	
	
		
		

	close(sock);

	return 0;

}
