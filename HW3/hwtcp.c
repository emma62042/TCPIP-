#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <linux/ip.h>	//for IP header
#include <linux/tcp.h>
#include <linux/if_ether.h>	//for ethernet header
#include <net/if.h>
#include<arpa/inet.h>

//#include <if.h>
#define	Server_Address "127.0.0.1"


//#define ETH_FRAME_LEN 1500

int main(int argc, char *argv[])
{
	struct ethhdr *peth;
	struct iphdr *pip;
	struct iphdr *iph;
	struct ifreq ifr;
	char *ptemp;
	
	struct timeval start;
    struct timeval end;
    unsigned long diff;

    struct sockaddr_in addr,source,dest;
	unsigned char buf[ETH_FRAME_LEN];
    int sock,r,len= sizeof(addr);

   ///////////////////////////////////////////////////////////////////
	
	sock = socket(PF_PACKET,SOCK_RAW,htons(ETH_P_ALL));
    if (sock < 0)    printf("Error creating socket!\n");

    /*bzero(&server_addr, server_addr_length);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(ETH_P_ALL);
    server_addr.sin_addr.s_addr = inet_addr(Server_Address);

    if (connect(sock, (struct sockaddr *)&server_addr,server_addr_length)==-1) {
        printf("connect failed!");
        close(sock);}*/

	
	strncpy(ifr.ifr_name, "enp0s3", 7);
	if(ioctl(sock,SIOCGIFFLAGS,&ifr) == -1)
	{
		perror("ioctl");exit(1);
	}
	ifr.ifr_flags |= IFF_PROMISC;
	if(ioctl(sock, SIOCSIFFLAGS, &ifr) == -1)
	{
		perror("ioctl");exit(3);
	}

//////////////////////////////////////////////////////////////////////////////////////////////////

	struct tcphdr *ptcp;
	int ip_counter=0,arp_counter=0,rarp_counter=0,tcp_counter=0,udp_counter=0,icmp_counter=0,igmp_counter=0;
	int i,j;

	while(tcp_counter<10)
	{	
		//rintf("recvfrom!!\n");
		r = recvfrom(sock, (char *)buf, sizeof(buf), 0,NULL,NULL);
		ptemp = buf;

		peth = (struct ethhdr *)ptemp; //ethernet header
		
		switch(ntohs(peth->h_proto))
		{
			case 0x0800:
				ip_counter++;
				break;
			case 0x0806:
				arp_counter++;
				break;
			case 0x8035:
				rarp_counter++;
				break;
		}

		///////////////////////////////////////////////////////////////////////////////////////////////////
		ptemp += sizeof(struct ethhdr);
		pip = (struct iphdr *)ptemp;

		memset(&source,0,sizeof(source));
        source.sin_addr.s_addr = pip -> saddr;         
        memset(&dest,0,sizeof(dest));
        dest.sin_addr.s_addr = pip -> daddr;

		if(pip->protocol==IPPROTO_TCP)
		{
			if(strcmp("10.0.2.15",inet_ntoa(dest.sin_addr))!=0) 
			{
				//if(strcmp("10.0.2.15",inet_ntoa(dest.sin_addr))!=0)
				//{
					//ptemp += sizeof(struct iphdr);
					printf("IP->protocol = TCP\n");
					printf("IP->src_ip = %s\n",inet_ntoa(source.sin_addr));
					printf("IP->dst_ip = %s\n",inet_ntoa(dest.sin_addr));
					ptcp = (struct tcphdr *)ptemp;
					memset(&source,0,sizeof(source));
			        source.sin_port = ptcp -> source;
			        memset(&dest,0,sizeof(dest));
			        dest.sin_port = ptcp -> dest;

					tcp_counter++;
					

					
					printf("src port = %d\n",source.sin_port);
					printf("dst port = %d\n",dest.sin_port);
					printf("////////////////////////////////////////\n");
				//}

			}
		}
	}
	printf("ip=%d\n",ip_counter);
	printf("arp=%d\n",arp_counter);
	printf("rarp=%d\n",rarp_counter);
	printf("tcp=%d\n",tcp_counter);
	printf("udp=%d\n",udp_counter);
	printf("icmp=%d\n",icmp_counter);
	printf("igmp=%d\n",igmp_counter);
}

