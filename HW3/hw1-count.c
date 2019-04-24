#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <linux/ip.h>	//for IP header
#include <linux/if_ether.h>	//for ethernet header
#include <net/if.h>
//#include <if.h>
#define	Server_Address "127.0.0.1"


//#define ETH_FRAME_LEN 1500

int main(int argc, char *argv[])
{
	struct ethhdr *peth;
	struct iphdr *pip;
	struct ifreq ifr;
	char *ptemp;
	
	struct timeval start;
    struct timeval end;
    unsigned long diff;

    struct sockaddr_in addr;
	unsigned char buf[ETH_FRAME_LEN];
    int sock,r,len= sizeof(addr);
	
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
	int ptcp,pudp,picmp,pigmp;
	int ip_counter=0,arp_counter=0,rarp_counter=0,tcp_counter=0,udp_counter=0,icmp_counter=0,igmp_counter=0;
	int i;
	for(i=0;i<100;i++)
	{	
		//rintf("recvfrom!!\n");
		r = recvfrom(sock, (char *)buf, sizeof(buf), 0, (struct sockaddr *)&addr,&len);
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
		ptemp += sizeof(struct ethhdr);
		pip = (struct ip *)ptemp;
		switch(pip->protocol)
		{
			case IPPROTO_TCP:
				ptcp = (struct tcphdr *)ptemp;					
				tcp_counter++;
				break;
			case IPPROTO_UDP:
				pudp = (struct udphdr *)ptemp;
				udp_counter++;
				break;
			case IPPROTO_ICMP:
				picmp = (struct icmphdr *)ptemp;
				icmp_counter++;
				break;
			case IPPROTO_IGMP:
				pigmp = (struct igmphdr *)ptemp;
				igmp_counter++;
				break;
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

