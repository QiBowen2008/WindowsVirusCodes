/*
 * Filename: wsa_xtra.h
 *
 * Description:
 *  - macros WinSock v1.1 forgot
 *  - fixes one broken macro in v1.1
 *  - structures for optional features 
 *
 * Edit History:
 *  10/10/95 changed ip_mreq declaration to avoid redefinition if already defined,
 *           and forced BSD macro values for multicast socket options.
 */
#ifndef _WSA_XTRA_
#define _WSA_XTRA_ 
 
#include <winsock.h>

/* fix of broken macro in v1.1 WinSock.H */
#undef  IPROTO_GGP
#define IPROTO_GGP  3
#ifndef IPROTO_IGMP
#define IPROTO_IGMP 2
#endif

#define MAXHOSTNAME   128   /* Maximum length of a DNS hostname */
#define MAXADDRSTR    16    /* Maximum length of an IP Address string */

/* handy in many places */ 
#define SOCKADDR_LEN sizeof(struct sockaddr)

/* WinSock version information (use WSA_VERSION for WSAStartup()) */
#ifndef MAKEWORD
#define MAKEWORD(l,h) ((WORD)(((BYTE)(l))|(((WORD)(BYTE)(h))<<8)))
#endif
#define WSA_MAJOR_VERSION 1
#define WSA_MINOR_VERSION 1
#define WSA_VERSION MAKEWORD(WSA_MAJOR_VERSION, WSA_MINOR_VERSION)

/* for unix compatibility */
#define bcopy(s,d,n)   _fmemcpy((d),(s),(n))
#define bcmp(s1,s2,n)  _fmemcmp((s1),(s2),(n))
#define bzero(s,n)     _fmemset(((s),0,(n))

#define IP_TTL	 4	/* level=IPPROTO_IP option, Time To Live */
#define MAX_TTL	255	/* maximum IP "Time To Live" value */

/* ICMP types */
#define ICMP_ECHOREPLY	0	/* ICMP type: echo reply */
#define ICMP_ECHOREQ	8	/* ICMP type: echo request */

/* definition of ICMP header as per RFC 792 */
typedef struct icmp_hdr {
	u_char	icmp_type;		/* type of message */
	u_char	icmp_code;		/* type sub code */
	u_short icmp_cksum;		/* ones complement cksum */
	u_short	icmp_id;		/* identifier */
	u_short	icmp_seq;		/* sequence number */
	char	icmp_data[1];	/* data */
} ICMP_HDR, *PICMPHDR, FAR *LPICMPHDR;
#define ICMP_HDR_LEN	sizeof(ICMP_HDR)

/* definition of IP header version 4 as per RFC 791 */
#define	IPVERSION	4
typedef struct ip_hdr {
	u_char	ip_hl;		/* header length */
	u_char	ip_v;		/* version */
	u_char	ip_tos;		/* type of service */
	short	ip_len;		/* total length */
	u_short	ip_id;		/* identification */
	short	ip_off;		/* fragment offset field */
	u_char	ip_ttl;		/* time to live */
	u_char	ip_p;		/* protocol */
	u_short	ip_cksum;		/* checksum */
	struct	in_addr ip_src;	/* source address */
	struct	in_addr ip_dst;	/* destination address */
} IP_HDR, *PIP_HDR, *LPIP_HDR;
#define IP_HDR_LEN sizeof(IP_HDR)

/* 
 * The following constants are from BSD /include/netinet/in.h
 *  header file.  They DO NOT match the values defined by Steve Deering 
 *  of Stanford University <deering@pescadero.Stanford.EDU> (WinNT v3.5
 *  did use Deering's values, however, which are 2-6 ...offset 7).
 *
 *  The use of these multicast options is described in "IP Multicast 
 *  Extensions for 4.3BSD UNIX related systems (MULTICAST 1.2 Release)".  
 *  It describes the extensions to BSD, SunOS and Ultrix to support 
 *  multicasting, as specified by RFC-1054 (since superceded by RFC-1112)
 *
 *  See chapter 16, "Optional Features," in _Windows Sockets Network
 *  Programming_ by Bob Quinn & Dave Shute, for more information 
 *  (published by Addison-Wesley, ISBN: 0-201-63372-8).
 */
#define DEERING_OFFSET      7   /* subtract this bias for MS WSOCK32 options */

#ifndef  IP_MULTICAST_IF
/* The structure used to add and drop multicast addresses  (the
 *  imr_interface field is ignored since we only support one interface */
typedef struct ip_mreq {
	struct in_addr imr_multiaddr;	/* multicast group to join */
	struct in_addr imr_interface;	/* interface to join on */
}IP_MREQ;
#endif

#undef  IP_MULTICAST_IF
#define IP_MULTICAST_IF	    9   /* set/get IP mulicast interface */
#undef  IP_MULTICAST_TTL
#define	IP_MULTICAST_TTL    10  /* set/get IP multicast timetolive */
#undef  IP_MULTICAST_LOOP
#define IP_MULTICAST_LOOP   11  /* set/get IP multicast loopback */
#undef  IP_ADD_MEMBERSHIP
#define	IP_ADD_MEMBERSHIP   12  /* add  (set) IP group membership */
#undef  IP_DROP_MEMBERSHIP
#define	IP_DROP_MEMBERSHIP  13  /* drop (set) IP group membership */

#undef  IP_DEFAULT_MULTICAST_TTL
#define IP_DEFAULT_MULTICAST_TTL	1
#undef  IP_DEFAULT_MULTICAST_LOOP
#define IP_DEFAULT_MULTICAST_LOOP	1

#define IP_MAX_MEMBERSHIPS		20

#endif /* _WS_XTRA_ */
