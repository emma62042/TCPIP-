#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
//#include <arpa/inet.h>
//#include <netdb.h>

#define PortNumber 4400

int new_sock[10];

void send_to_all(char *sendbuf,int recfd,int i,fd_set *readfds)
{
    int byte_sent;
    char msg[50];
    //printf("send to all : %d\n",recfd);
    for(int j=0;j<10;j++)
    {
        if (FD_ISSET(new_sock[j], readfds)){
	        if(new_sock[j]!=recfd)
	        {
	            if(i==0)
	                {strcpy(msg,"Client1:");}
	            else if(i==1)
	                {strcpy(msg,"Client2:");}
	            else if(i==2)
	                {strcpy(msg,"Client3:");}
	            else if(i==3)
	                {strcpy(msg,"Client4:");}
	            else if(i==4)
	                {strcpy(msg,"Client5:");}
	            else if(i==5)
	                {strcpy(msg,"Client6:");}
                else if(i==6)
                    {strcpy(msg,"Client7:");}
                else if(i==7)
                    {strcpy(msg,"Client8:");}
                else if(i==8)
                    {strcpy(msg,"Client9:");}
                else if(i==9)
                    {strcpy(msg,"Client10:");}
	            strcat(msg,sendbuf);
	            byte_sent = send((int)new_sock[j], msg, sizeof(msg),0);
	            if (byte_sent < 0)    printf("Error sending packet\n");
	        }
	    }
    }
}

void Send_Receive (int i,fd_set *readfds)
{
    int recfd= new_sock[i];
    int byte_recv; 
    char buffer[50];

    if(byte_recv = recv(recfd, buffer, sizeof(buffer),0)>0)
    {
        if(strlen(buffer)!=0)
        {
        	printf("Client%d: %s\n",i+1,buffer);
        	send_to_all(buffer,recfd,i,readfds);
        }
    }
    if (byte_recv < 0){    
    	printf("Error recving packet\n");
    	close(recfd);
		FD_CLR(recfd, readfds);
	}
}

int main(int argc, char *argv[]) {
    struct sockaddr_in server_addr,client_addr;
    int sock=0, byte_recv,byte_sent, server_addr_length = sizeof(server_addr), client_addr_length=sizeof(client_addr), recfd; 
    char buffer[50];
    fd_set readfds;
    fd_set master;

    int fdmax;
    FD_ZERO(&readfds);
    FD_ZERO(&master);
    
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock < 0)    printf("Error creating socket\n");
    
    bzero(&server_addr, server_addr_length);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PortNumber);
    server_addr.sin_addr.s_addr = INADDR_ANY;  

    if (bind(sock,(struct sockaddr *)&server_addr, server_addr_length) == -1) {
        printf("error binding!\n");
        close(sock);}

    if (listen(sock, 10) == -1) {
        printf("listen failed!\n");
        close(sock);}
    printf("sock=%d\n",sock);

    FD_SET(sock, &master);

    fdmax = sock;

    int i=0;
    while(1)
    {
    	readfds=master;
        if(select(fdmax+1, &readfds, NULL, NULL, NULL) == -1){
			perror("select");
			exit(4);
		}
        
        for (int j = 0; j <= fdmax; j++){
			if (FD_ISSET(j, &readfds)){
				if (j == sock)
				{
					recfd = accept(sock,(struct sockaddr *)&client_addr,&client_addr_length);
			        if(recfd<0)
			        {
			            printf("recfd error!\n");
			        }
			        else
			        {
			        	printf("accept!\n");
        				new_sock[i] = recfd;
        				printf("Client%d connect\n",i+1);
        				i++;
        				FD_SET(recfd,&master);
        				if(recfd > fdmax){
							fdmax = recfd;
						}
        			}
        		}
				else
				{
					for(int k=0;k<10;k++)
					{
						if(j==new_sock[k])
						{
							Send_Receive(k,&master);
						}
					}
				}
			}
		}
    }
    
    printf("Out to close\n");
    close(sock);
    return 0;


}


