#define	IFF_PROMISC  0x100  /*receive all packets */
#include <linux/if_ether.h>	//for ethernet header
struct ether_header
{
      unsigned char ether_dhost[ETH_ALEN];
      unsigned char ether_shost[ETH_ALEN];
      unsigned short  ether_type;
}

#include <linux/ip.h>	//for IP header
		struct iphdr {
　unsigned int version:4;
　unsigned int h_len:4;
　unsigned char tos;
　unsigned short total_len;
　unsigned short ident;
　unsigned short frag_and_flags;
　unsigned char ttl;
　unsigned char proto;
　unsigned short checksum; 
　unsigned int sourceIP; 
　unsigned int destIP;
}

//You need to set NIC’s flag to IFF_PROMISC
ethreq.ifr_flags|=IFF_PROMISC;
ioctl(fd,SIOCSIFFLAGS,&ethreq);
//P.S. before program terminated, remember to restore NIC’s flag  --  ethreq.ifr_flags&=~IFF_PROMISC; 	