#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <if.h>

#define ETH_FRAME_LEN 1500

struct ifreq ethreq;	
//ifreq in <net/if.h>

unsigned char buffer[ETH_FRAME_LEN];
//frame could be placed at this buffer

int main(int argc, char *argv[])
{
	struct ether_header *peth;
	struct iphdr *pip;
	char *ptemp;
	
	struct timeval start;
    struct timeval end;
    unsigned long diff;

    struct sockaddr_in server_addr;
	unsigned char buffer[ETH_FRAME_LEN];
    int sock, byte_sent,byte_recv, server_addr_length = sizeof(server_addr),recfd;
	
	sock = socket(PF_PACKET,SOCK_RAW,htons(ETH_P_ALL));
    if (sock < 0)    printf("Error creating socket!\n");

    /*bzero(&server_addr, server_addr_length);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(Server_PortNumber);
    server_addr.sin_addr.s_addr = inet_addr(Server_Address);

    if (connect(sock, (struct sockaddr *)&server_addr,server_addr_length)==-1) {
        printf("connect failed!");
        close(sock);}*/

	struct ifreq ifr;
	strncpy(ifr.ifr_name, "eth0", 5);
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
	int ip_counter,arp_counter,rarp_counter,tcp_counter,udp_counter,icmp_counter,igmp_counter;
	for(;;)
	{	
		r = recvfrom(sock, (char *)buf, sizeof(buf), 0, (struct sockaddr *)&addr, &len);
		ptemp = buf;
		peth = (struct ether_header *)ptemp; //ethernet header
		switch(ntohs(peth->ether_type))
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
		ptemp += sizeof(struct ether_header);
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
	ethreq.ifr_flags&=~IFF_PROMISC;
}

