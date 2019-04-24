#include <linux/if_ether.h>	//for ethernet header
#include <net/if.h>
#include <arpa/inet.h>

#include <sys/types.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/file.h>
#include <sys/time.h>
#include <sys/signal.h>
#include <sys/ioctl.h>

#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>

#define	MAXPACKET	(65536 - 60 - 8)/* max packet size */

struct timeval	tvorig, tvrecv;	/* originate & receive timeval structs */
long tsorig, tsrecv;	/* originate & receive timestamps */
			/* timestamp = #millisec since midnight UTC */
			/* both host byte ordered */
long tsdiff;		/* adjustment must also be signed */
unsigned char send_buf[MAXPACKET],recv_buf[MAXPACKET];

in_cksum(u_short *addr, int len)
{
	register int nleft = len;
	register u_short *w = addr;
	register int sum = 0;
	u_short answer = 0;

	while (nleft > 1)  {
		sum += *w++;
		nleft -= 2;
	}

	if (nleft == 1) {
		*(u_char *)(&answer) = *(u_char *)w ;
		sum += answer;
	}

	sum = (sum >> 16) + (sum & 0xffff);	/* add hi 16 to low 16 */
	sum += (sum >> 16);			/* add carry */
	answer = ~sum;				/* truncate to 16 bits */
	return(answer);
}

int main(int argc, char *argv[])
{
	struct sockaddr_in server_addr;	/* who to ping */
	char *target;

	unsigned char buf[ETH_FRAME_LEN];
    int sock,r,server_addr_length=sizeof(server_addr),i,packsize;
    int T1,T2,T3,T4,RTT;

   ///////////////////////////////////////////////////////////////////
	
	sock = socket(PF_INET,SOCK_RAW,1);
    if (sock < 0)    printf("Error creating socket!\n");

    target = argv[1];
    bzero(&server_addr, server_addr_length);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(ETH_P_ALL);
    server_addr.sin_addr.s_addr = inet_addr(target);
    

    struct icmp *icmp_hdr = (struct icmp *) send_buf;
  	//Define ICMP timestamp
  	icmp_hdr->icmp_type = ICMP_TSTAMP;
  	icmp_hdr->icmp_code = 0;
  	icmp_hdr->icmp_cksum = 0;
  	gettimeofday(&tvorig, (struct timezone *)NULL);
  	tsorig = (tvorig.tv_sec % (24*60*60)) * 1000 + tvorig.tv_usec / 1000;
  	icmp_hdr->icmp_otime = htonl(tsorig);
  	icmp_hdr->icmp_id = 123;
  	icmp_hdr->icmp_rtime = 0;		/* filled in by receiver */
	icmp_hdr->icmp_ttime = 0;		/* filled in by receiver */

	printf("icmp_type = 13, icmp_code = 0\n");
	printf("Original timestamp = %ld\n",tsorig);
	printf("Receive timestamp = 0\n");
	printf("Transmit timestamp = 0\n");
	printf("Final timestamp = 0\n");
	printf("--------------------------\n");
                  
  	

  	packsize = 4 + 8;	/* 8 bytes of header, 4 bytes of data */
  	icmp_hdr->icmp_cksum=in_cksum((u_short *)icmp_hdr, packsize);

  	i=sendto(sock, send_buf, packsize, 0, (struct sockaddr*)&server_addr, server_addr_length);

	if (i < 0 || i != packsize)  {
		if (i < 0)
			perror("sendto error\n");
		printf("wrote %s %d chars, ret=%d\n", target, packsize, i);
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////

	int hlen;
	for(;;)
	{
		i=recvfrom(sock, recv_buf, sizeof(recv_buf), 0, NULL,NULL);
		//printf("recv done\n");
		if (i < 0)

			perror("recvfrom error\n");

		struct ip *ip;// = (struct ip *)recv_buf;
		struct icmp *icmp_hdr_recv ;//= recv_buf + ip->ip_hl<<2;  //Skip IP header
		ip = (struct ip *)recv_buf;
		hlen = ip->ip_hl << 2;
		
		icmp_hdr_recv = (struct icmp *)(recv_buf + hlen);
		if(icmp_hdr_recv->icmp_type == ICMP_TSTAMPREPLY && icmp_hdr_recv->icmp_id==123)
		{
			
			printf("id=%d\n", icmp_hdr_recv->icmp_id);
			//icmp_hdr_recv->icmp_code = 0;
		  	gettimeofday(&tvrecv, (struct timezone *)NULL);
		  	T4= (tvrecv.tv_sec % (24*60*60)) * 1000 + tvrecv.tv_usec / 1000;

		  	//icmp_hdr_recv->icmp_otime = htonl(tsorig);
		  	T1=ntohl(icmp_hdr_recv->icmp_otime);
		  	T2=ntohl(icmp_hdr_recv->icmp_rtime);		/* filled in by receiver */
			T3=ntohl(icmp_hdr_recv->icmp_ttime);		/* filled in by receiver */
			RTT=(T4-T3)+(T2-T1);
			printf("icmp_type = 14, icmp_code = 0 \n");
			printf("Originate timestamp=%d\n", T1);
			printf("Receive timestamp=%d\n", T2);
			printf("Transmit timestamp=%d\n", T3);
			printf("Final timestamp=%d\n", T4);
			printf("RTT = %d\n", RTT);
		}
	}
	
  	

}

