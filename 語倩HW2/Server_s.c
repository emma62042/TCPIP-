#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<pthread.h>

#define PortNumber 5555

#include<sys/time.h>
#define SecPerHour 3600
#define SecPerMin 60


int client = 0;
pthread_t thread_s, thread_r, thread;

int rec_fd[10] = {0,0,0,0,0,0,0,0,0,0};
int i;
char buffer[50];
char title[50];

void *Send(void *fd)
{
	int byte_sent;

	byte_sent = send(fd, title, sizeof(title), 0);		
	if(byte_sent < 0) 
		printf("Error sending packet,%d\n",byte_sent);
	else
		printf("message is sending.\n");
	//pthread_exit(NULL);
}

void *Receive(void *fd)
{	
	int byte_recv;
	int i;
	
	struct timeval timeout;
	fd_set readfds;

	timeout.tv_sec = 0;
	timeout.tv_usec = 100000; //0.1sec

	while(1)
	{
		FD_ZERO(&readfds);
		FD_SET((int)fd, &readfds);
		
		select((int)fd+1, &readfds, NULL, NULL, &timeout);

		if (FD_ISSET((int)fd, &readfds))
		{
			byte_recv = recv(fd, buffer, sizeof(buffer),0);
			if(byte_recv < 0) printf("Error recving packet\n");
		
			printf("Reveived : %s(recfd = %d)\n", buffer, fd);
			
			for(i = 0; i < 10; i++)
			{
				if(rec_fd[i] != 0 && rec_fd[i] != fd)
				{
					if(fd == rec_fd[1])
				{
					strcpy(title, "Client1:");
					strcat(title, buffer);
				}
				if(fd == rec_fd[2])
				{
					strcpy(title, "Client2:");
					strcat(title, buffer);
				}
				if(fd == rec_fd[3])
				{
					strcpy(title, "Client3:");
					strcat(title, buffer);
				}
				if(fd == rec_fd[4])
				{
					strcpy(title, "Client4:");
					strcat(title, buffer);
				}
				if(fd == rec_fd[5])
				{
					strcpy(title, "Client5:");
					strcat(title, buffer);
				}
				if(fd == rec_fd[6])
				{
					strcpy(title, "Client6:");
					strcat(title, buffer);
				}
				if(fd == rec_fd[7])
				{
					strcpy(title, "Client7:");
					strcat(title, buffer);
				}
				if(fd == rec_fd[8])
				{
					strcpy(title, "Client8:");
					strcat(title, buffer);
				}
				if(fd == rec_fd[9])
				{
					strcpy(title, "Client9:");
					strcat(title, buffer);
				}
							
				Send(rec_fd[i]);
				}
				
			}
			if(strcmp(buffer,"exit") == 0)
			{
				client--;			
				break;
			}
		}
	}
	//pthread_join(thread, NULL);
	pthread_exit(NULL);
	
}


int main(int argc, char *argv[])
{
	struct sockaddr_in server_addr, client_addr;
	int sock;
	int server_addr_length = sizeof(server_addr), client_addr_length = sizeof(client_addr), recfd;
	
	int rc;

	//Establish socket
	sock = socket(PF_INET, SOCK_STREAM, 0);
	if(sock < 0)
		printf("Error creating socket\n");

	bzero(&server_addr, server_addr_length);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PortNumber);
	server_addr.sin_addr.s_addr = INADDR_ANY;
	
	//Establish bind
	if(bind(sock,(struct sockaddr *)&server_addr, server_addr_length) == -1){
		printf("error binding\n");
		close(sock);
	}
	//Estable listen
	if(listen(sock, 100) == -1){
		printf("listen failed\n");
		close(sock);
	}

	for(;;){
		//Establish accept
		if((recfd = accept(sock, (struct sockaddr*)&client_addr, &client_addr_length)) == -1){
			printf("accept failed\n");
			close(sock);
		}
		else
		{
				
			client++;//count the number of client
			rec_fd[client] = recfd;
			printf("client%d connect!(recfd:%d)(rec_fd[%d]:%d)\n", client, recfd,client, rec_fd[client]);
			
			//pass the "recfd" to pthread_create()
			pthread_create(&thread, NULL, Receive, (void *)recfd);
			
		}
	}//for end
	

}

