#include <stdio.h>
#include <stdlib.h>
#include <string.h>  //for bzero()
#include <unistd.h>  //for close()
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#define PortNumber 4400

int new_sock[10]={0,0,0,0,0,0,0,0,0,0};

void send_to_all(char *sendbuf,int recfd,int i)
{
    int byte_sent;
    char msg[50];
    //printf("send to all : %d\n",recfd);
    for(int j=0;j<10;j++)
    {
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
            if (byte_sent < 0 && new_sock[j]!=0)    printf("Error sending packet\n");
        }
    }
}

void *Send_Receive (void *i)
{
    int recfd= new_sock[(int)i];
    int byte_recv; 
    char buffer[50];

    while(byte_recv = recv(recfd, buffer, sizeof(buffer),0)>0)
    {
        if(strlen(buffer)==0)
        {
            continue;
        }
        printf("Client%d: %s\n",i+1,buffer);
        send_to_all(buffer,recfd,i);
    }
    if (byte_recv < 0 )    printf("Error recving packet\n");
    
    pthread_exit(NULL);
}


int main(int argc, char *argv[]) {
    struct sockaddr_in server_addr,client_addr;
    int sock, byte_recv,byte_sent, server_addr_length = sizeof(server_addr), client_addr_length=sizeof(client_addr), recfd; 
    char buffer[50];
    
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

    for(int i=0;i<11;i++)
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
            pthread_t th1;
            if(pthread_create(&th1,NULL,Send_Receive,(void *)i)<0)
            {
                printf("create error!\n");
                return 1;
            }
        }
    }
    
    printf("Out to close\n");
    close(sock);
    return 0;


}


