//#define INCLUDE_WINDOWS // Berkeley doesn't use windows
#include <m_eng.h>
#include <l_misclibm.h>

#include "l_socker_berk.h"

// Berkeley sockets
#ifdef BERK // see 'l_misclibm.h'

// turn both on for max distance from wsock api
#define NIX // were close to a 'NIX machine, probably have include files, (keep some wsock stuff around)
//#define RNIX // really 'NIX, no wsock whatsoever

//#ifdef NIX // devstud and mingw both don't have these files, Berkeley socket includes
#if 0 // windows side 'nix test, devstud and mingw both don't have these files, Berkeley socket includes
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <netdb.h>

#else // wing it, don't have, define includes above locally
// compare this with 'nix include files
extern "C" {

// Berkeley
// defines
#define AF_INET         2               /* internetwork: UDP, TCP, etc. */
#define PF_INET         AF_INET
#define SOCK_STREAM     1               /* stream socket */
#define SOCK_DGRAM      2               /* datagram socket */
#define IPPROTO_TCP             6               /* tcp */
#define IPPROTO_UDP             17              /* user datagram protocol */
// structures
typedef U32 SOCKET; // Berkely likes 'typedef S32 socket;'
struct in_addr {
        union {
                struct { U8 s_b1,s_b2,s_b3,s_b4; } S_un_b;
                struct { U16 s_w1,s_w2; } S_un_w;
                U32 S_addr;
        } S_un;
};
#define s_addr  S_un.S_addr        /* can be used for most tcp & ip code */
#define s_host  S_un.S_un_b.s_b2   /* host on imp */
#define s_net   S_un.S_un_b.s_b1   /* network */
#define s_imp   S_un.S_un_w.s_w2   /* imp */
#define s_impno S_un.S_un_b.s_b4   /* imp # */
#define s_lh    S_un.S_un_b.s_b3   /* logical host */
struct  hostent {
        char    * h_name;           /* official name of host */
        char    * * h_aliases;  /* alias list */
        short   h_addrtype;             /* host address type */
        short   h_length;               /* length of address */
        char    * * h_addr_list; /* list of addresses */
};
#define h_addr  h_addr_list[0]          /* address, for backward compat */
struct sockaddr_in {
        short   sin_family;
        U16 sin_port;
        struct  in_addr sin_addr;
        char    sin_zero[8];
};
typedef struct sockaddr_in SOCKADDR_IN;
struct sockaddr {
        U16 sa_family;              /* address family */
        char    sa_data[14];            /* up to 14 bytes of direct address */
};
typedef struct sockaddr *LPSOCKADDR;
// calls
U32 _stdcall ntohl (U32 netlong);
U32 _stdcall htonl (U32 hostlong);
U16 _stdcall ntohs (U16 netshort);
U16 _stdcall htons (U16 hostshort);
int _stdcall gethostname (char * name,int namelen);
int _stdcall getsockname (SOCKET s,struct sockaddr *name,int * namelen);
int _stdcall getpeername (SOCKET s,struct sockaddr* name,int* namelen);
SOCKET _stdcall socket (int af,int type,int protocol);
int _stdcall bind (SOCKET s,const struct sockaddr *addr,int namelen);
int _stdcall listen (SOCKET s,int backlog);
SOCKET _stdcall accept (SOCKET s,struct sockaddr *addr,int *addrlen);
int _stdcall connect (SOCKET s,const struct sockaddr *name,int namelen);
int _stdcall send (SOCKET s,const char * buf,int len,int flags);
int _stdcall sendto (SOCKET s,const char * buf,int len,int flags,const struct sockaddr *to,int tolen);
int _stdcall recv (SOCKET s,char * buf,int len,int flags);
int _stdcall recvfrom (SOCKET s,char * buf,int len,int flags,struct sockaddr *from,int * fromlen);
int _stdcall closesocket (SOCKET s);
// errors, Berkeley likes errno,h_errno etc.
// windows and not winsock, or need temporarily
U32 _stdcall  GetTickCount();
#define INVALID_SOCKET					(SOCKET)(~0) // Berkeley likes // #define INVALID_SOCKET -1
#define SOCKET_ERROR					 (-1)
// new
struct hostent* _stdcall gethostbyname(const char *name); // DEPRECATED!
struct hostent* _stdcall gethostbyaddr(const char *addr, int len, int type);
#ifndef RNIX
// structures
#define WSADESCRIPTION_LEN      256
#define WSASYS_STATUS_LEN       128
typedef struct WSAData {
        U16                    wVersion;
        U16                    wHighVersion;
        char                    szDescription[WSADESCRIPTION_LEN+1];
        char                    szSystemStatus[WSASYS_STATUS_LEN+1];
        unsigned short          iMaxSockets;
        unsigned short          iMaxUdpDg;
        char  *              lpVendorInfo;
} WSADATA;
int _stdcall WSAStartup(U16 wVersionRequired,WSADATA* lpWSAData);
int _stdcall WSACleanup();
#define LOBYTE(w)           ((U8)((U32)(w) & 0xff))
#define HIBYTE(w)           ((U8)((U32)(w) >> 8))
#define MAKEWORD(a, b)      ((U16)(((U8)((U32)(a) & 0xff)) | ((U16)((U8)((U32)(b) & 0xff))) << 8))
// all this for nonblocking
int _stdcall ioctlsocket (SOCKET s,U32 cmd,U32 *argp);
#define IOCPARM_MASK    0x7f            /* parameters must be < 128 bytes */
#define IOC_IN          0x80000000      /* copy in parameters */
#define _IOW(x,y,t)     (IOC_IN|(((long)sizeof(t)&IOCPARM_MASK)<<16)|((x)<<8)|(y))
#define FIONBIO     _IOW('f', 126, U32) /* set/clear non-blocking i/o */
// end all this for nonblocking
int _stdcall WSAGetLastError(void);
// errors
#define WSAEWOULDBLOCK                   10035L
#define WSAECONNRESET                    10054L
#define WSAENETDOWN                      10050L
#define WSAECONNABORTED                  10053L
#define WSAENOTCONN                      10057L
#define WSAESHUTDOWN                     10058L
#endif

#ifndef NIX
// windows extensions
// defines
#define FD_READ         0x01
#define FD_WRITE        0x02
#define FD_ACCEPT       0x08
#define FD_CONNECT      0x10
#define FD_CLOSE        0x20
#define WM_USER 0x0400
typedef U32 HWND;
// macros
#define HIWORD(l)           ((U16)((U32)(l) >> 16))
#define LOWORD(l)           ((U16)((U32)(l) & 0xffff))
#define WSAGETASYNCERROR(lParam)            HIWORD(lParam)
#define WSAGETSELECTEVENT(lParam)           LOWORD(lParam)
// calls
U32 _stdcall WSAAsyncGetHostByName(HWND hWnd,U32 wMsg,const char* name,char* buf,int buflen);
U32 _stdcall WSAAsyncGetHostByAddr(HWND hWnd,U32 wMsg,const char* addr,int len,int type,char* buf,int buflen);
int _stdcall WSAAsyncSelect(SOCKET s,HWND hWnd,U32 wMsg,U32 lEvent);
#endif
} // extern "C"
#endif // end wing it

#define MAXMYIPS 32
#define MAXMYNAMES 32
#define MAXOTHERIPS 32
#define MAXOTHERNAMES 32

#define MESSTIMEOUT 10000 // in millisecs
#define MAXMESS 2500 // this is the max number of connections

#if 1
#define TCPWTBUFFSIZE 16384
#define TCPRDBUFFSIZE 16384
#else
#define TCPWTBUFFSIZE 40
#define TCPRDBUFFSIZE 40
#endif


static U32 nmyips;
static U32 myips[MAXMYIPS];
static U32 nmynames;
static string mynames[MAXMYNAMES];

static U32 notherips;
static U32 otherips[MAXOTHERIPS];
static U32 nothernames;
static string othernames[MAXOTHERNAMES];

#define MAXGETHOSTSTRUCT 1024
static C8 hostbuff[MAXGETHOSTSTRUCT];

static C8* dum;

static U64 globalseqnum;

// user message handler
#ifdef NIX

U32 messesused()
{
	return 0;
}

#else

// async message handler
enum {
	MESS_NONE,
	MESS_GETMY,
	MESS_GETOTHER,
	MESS_ACCEPT,
	MESS_RWC}; // kinds
struct mess { // uses WM_USER+0 to WM_USER+MAXMESS-1
	U32 val;	// could be a pointer or a socket
	S32 timestamp;	// timestamp of last freemess or message
	U32 timing; // is it timing out or not
	U32 used;
	U32 kind;
	U64 seqnum; // a method of keeping messages separate, oldest unused message slot, could break after 2 - 100 years, not anymore
};
static struct mess messes[MAXMESS];

static S32 allocmess(U32 kind,U32 val,S32 time) // -1 on error, WM_USER to WM_USER+MAXMESS-1 on success
{
	U32 i;
	S32 ts=GetTickCount();
	logger("allocmess: called with kind %d, val %d and time %d\n",kind,val,time);
	for (i=0;i<MAXMESS;i++) {
		if (messes[i].timing && ts>messes[i].timestamp) {
			logger("allocmess: expired message %d\n",i+WM_USER);
			messes[i].timing=0;
			messes[i].used=0;
			messes[i].kind=0;
			messes[i].seqnum=globalseqnum++;
		}
	}
	if (kind==MESS_GETMY || kind==MESS_GETOTHER)
		for (i=0;i<MAXMESS;i++)
			if (messes[i].kind==MESS_GETMY || messes[i].kind==MESS_GETOTHER) {
				logger("allocmess: already have a getmy or getother message in progress..\n");
				return -1;
			}
	U32 besti=~0;	// best unused slot ~0 means none, find oldest unused message slot, use that one
					// before it was 'first' unused message slot
	U64 bestseqnum=~0ULL;
	for (i=0;i<MAXMESS;i++) {
		if (!messes[i].used && messes[i].seqnum<bestseqnum) {
			besti=i;
			bestseqnum=messes[i].seqnum;
		}
	}
	if (besti!=~0U) {
		i=besti;
		messes[i].used=1;
		messes[i].kind=kind;
		messes[i].val=val;
		if (time) { // message handler will expire soon
			messes[i].timing=1;
			messes[i].timestamp=ts+time;
		} else { // persistant message, ouch, bet you need this too, maybe
			messes[i].timing=0;
			messes[i].timestamp=0;
		}
		logger("allocmess: returning messnum %d\n",i+WM_USER);
		return i+WM_USER;
	}
	errorexit("allocmess: too many messages");
	return -1;
}

U32 messesused()
{
	S32 i,r=0,ts=GetTickCount();
	for (i=0;i<MAXMESS;i++) {
		if (messes[i].timing && ts>messes[i].timestamp) {
			logger("expired message %d\n",i+WM_USER);
			messes[i].timing=0;
			messes[i].used=0;
			messes[i].kind=0;
			messes[i].val=0;
			messes[i].seqnum=globalseqnum++;
		}
	}
	for (i=0;i<MAXMESS;i++)
		r+=messes[i].used;
	return r;
}

static void freemess(U32 messnum,U32 when)
{
	logger("in freemess with messnum %d and when %d\n",messnum,when);
	messnum-=WM_USER;
	if (when) {
		messes[messnum].timestamp=GetTickCount()+when; // free it later
		messes[messnum].timing=1;
		messes[messnum].kind=0; // but it won't process message anymore
		messes[messnum].val=0;
	} else {
		messes[messnum].timestamp=0;
		messes[messnum].timing=0;
		messes[messnum].used=0;
		messes[messnum].kind=0;
		messes[messnum].val=0;
		messes[messnum].seqnum=globalseqnum++;
	}
}

// general
///// message functions
static void getxbyyfunc(U32 messnum,U32 wParam,S32 lParam)
{
	U32 /*i,*/kind,ip;
	struct in_addr myip;
	struct hostent *hep=(struct hostent*)hostbuff;
	C8** strptr;
	logger("<<<<<<<<< processing getxbyyfunc messsage %d, with wParam %d, lParam %d >>>>>>>>>\n",
		messnum,wParam,lParam);
	kind=messes[messnum-WM_USER].kind;
	freemess(messnum,0);
/*	if (kind==MESS_GETMY) {
		for (i=0;i<nmynames;i++)
//			memfree(mynames[i]);
			delete[] mynames[i];
	} else if (kind==MESS_GETOTHER) {
		for (i=0;i<nothernames;i++)
//			memfree(othernames[i]);
			delete[] othernames[i];
	} */
	if (WSAGETASYNCERROR(lParam)) {
		logger("WSAGETASYNCERROR() %d\n",WSAGETASYNCERROR(lParam));
		if (kind==MESS_GETMY) {
//			mynames[0]=strclone("Err");
			mynames[0]="Err";
			nmynames=1;
			myips[0]=~0;
			nmyips=1;
		} else if (kind==MESS_GETOTHER) {
//			othernames[0]=strclone("Err");
			othernames[0]="Err";			
			nothernames=1;
			otherips[0]=~0;
			notherips=1;
		}
		return;
	}
// dump hostent structure
	if (kind==MESS_GETMY) {
//		mynames[0]=strclone(hep->h_name);
		mynames[0]=hep->h_name;
		nmynames=1;
	} else if (kind==MESS_GETOTHER) {
//		othernames[0]=strclone(hep->h_name);
		othernames[0]=hep->h_name;
		nothernames=1;
	}
//	logger("offical name '%s'\n",hep->h_name);
	strptr=(C8**)hep->h_aliases;
	while(*strptr) {
		logger("alias '%s'\n",*strptr);
		if (kind==MESS_GETMY) {
			if (nmynames<MAXMYNAMES)
//				mynames[nmynames++]=strclone(*strptr);
				mynames[nmynames++]=*strptr;
		} else if (kind==MESS_GETOTHER) {
			if (nothernames<MAXOTHERNAMES)
//				othernames[nothernames++]=strclone(*strptr);
				othernames[nothernames++]=*strptr;
		}
		strptr++;
	}
	strptr=(C8**)hep->h_addr_list;
	while(*strptr) {
		if (nmyips<MAXMYIPS) {
			memcpy(&myip,*strptr,4);
			ip=ntohl(myip.s_addr);
			if (kind==MESS_GETMY) {
				if (nmyips<MAXMYIPS)
					myips[nmyips++]=ip;
			} else if (kind==MESS_GETOTHER) {
				if (notherips<MAXOTHERIPS)
					otherips[notherips++]=ip;
			} 
//			logger("address '%s'\n",ip2str(ip));
//			logger("%08x\n",ip);
		}
		strptr++;
	}
}

static void dotcpmess(U32 messnum,U32 wParam,S32 lParam)
{
	U32 res;
	SOCKADDR_IN sa;
	S32 sas=sizeof(sa);
	U32 mo=messnum-WM_USER;
	socker* s=(socker *)messes[mo].val;
#if 0
	U32 kind=messes[mo].kind;
	U32 used=messes[mo].used;
	U32 timing=messes[mo].timing;
	S32 timestamp=messes[mo].timestamp;
	logger("processing dotcpmess messsage %d, with wParam %d, lParam %d, used %d, kind %d, timing %d, timestamp %d\n",
		messnum,wParam,lParam,used,kind,timing,timestamp);
	if (s) {
		logger("socket %d, sockmess\n",
			s->s,s->messnum);
	}
#endif
	if (!s) {
		logger("null s\n");
		return;
	}
	res=WSAGETASYNCERROR(lParam);
	if (res) {
		logger("user message %d error, dotcpmess %d\n",messnum,res); // hmmm ?? should we close ??
		s->flags|=SF_CLOSED;
		return;
	}
	U32 event=WSAGETSELECTEVENT(lParam);
	if (!messes[messnum-WM_USER].used) {
		logger("unused message %d\n",messnum);
		return;
	}
//	s->messnum=0; // remove later..
//	freemess(messnum,0);
	switch (event) {
	case FD_ACCEPT:
		if (s && s->kind==SOCK_MASTER)
			s->flags|=SF_ACCEPTREADY;
		else
			logger("not a master for accept message %d\n",messnum);
		break;
	case FD_CONNECT:
		if (s && s->kind==SOCK_CLIENT) {
//			res=getsockname(s->s,(LPSOCKADDR)&sa,&sas);
//			if (res)
//				logger("error on getsockname\n");
//			s->ip=ntohl(sa.sin_addr.s_addr);
//			s->port=ntohs(sa.sin_port);
			res=getpeername(s->s,(LPSOCKADDR)&sa,&sas);
			if (res)
				logger("error on getpeername\n");
			s->remoteip=ntohl(sa.sin_addr.s_addr);
			s->remoteport=ntohs(sa.sin_port);
//			logger("got a connection local '%s':%d ",ip2str(s->ip),s->port);
//			logger("remote '%s':%d\n",ip2str(s->remoteip),s->remoteport);
			s->flags|=SF_CONNECTED;
		} else
			logger("not a client for connect message %d\n",messnum);
		break;
	case FD_READ:
		s->flags|=SF_READREADY;
//		logger("read ready\n");
		break;
	case FD_WRITE:
		s->flags|=SF_WRITEREADY;
//		logger("write ready\n");
		break;
	case FD_CLOSE:
		s->flags|=SF_CLOSED;
//		logger("close ready\n");
		break;
	default:
		logger("wrong message type %d\n",messnum);
		break;
	}
}

#endif

// database functions
U32 getnmyname() // how many names do we have, return 0 for error
{
	return nmynames;
}

const C8* getmyname(U32 idx)
{
	if (idx<nmynames)
		return mynames[idx].c_str();
	return 0;
}

U32 getnmyip()	// returns 0 until we get one
{
	return nmyips;
}

U32 getmyip(U32 idx)	// returns 1 of my ips
{
	if (idx<nmyips)
		return myips[idx];
	return 0;
}

U32 getinfofromname(const C8* name) // initiate getinfo
{
	U32 ip;
	ip=str2ip(name);
	if (ip!=~0U) {
//		for (i=0;i<nothernames;i++)
//			memfree(othernames[i]);
//			delete[] othernames[i];
		nothernames=notherips=1;
		otherips[0]=ip;
//		othernames[0]=strclone(name);
		othernames[0]=name;
		return 1;
	}
#ifdef NIX
	nothernames=notherips=0;
	struct hostent* hep = gethostbyname(name); // get the host info
	if (!hep) {
//		errorexit("can't get hostbyname 2");
		othernames[0]="Err";			
		nothernames=1;
		otherips[0]=~0;
		notherips=1;
		return 1;
	}
	struct in_addr myip;
	C8** strptr;
// dump hostent structure
	othernames[0]=hep->h_name;
	nothernames=1;
	logger("offical other name '%s'\n",hep->h_name);
	strptr=(C8**)hep->h_aliases;
	while(*strptr) {
		logger("alias other '%s'\n",*strptr);
		if (nothernames<MAXOTHERNAMES)
			othernames[nothernames++]=*strptr;
		strptr++;
	}
	strptr=(C8**)hep->h_addr_list;
	while(*strptr) {
		if (notherips<MAXOTHERIPS) {
			memcpy(&myip,*strptr,4);
			ip=ntohl(myip.s_addr);
			if (notherips<MAXOTHERIPS)
				otherips[notherips++]=ip;
			logger("other address '%s'\n",ip2str(ip));
//			logger("%08x\n",ip);
		}
		strptr++;
	}
#else
	U32 messnum=allocmess(MESS_GETOTHER,0,MESSTIMEOUT);
	if (messnum==~0U)
		return 0;
//	for (i=0;i<nothernames;i++)
//		memfree(othernames[i]);
//		delete[] othernames[i];
	nothernames=notherips=0;
	setusermessage(messnum,getxbyyfunc);
	U32 res=(U32)WSAAsyncGetHostByName((HWND)wininfo.MainWindow,messnum,name,hostbuff,MAXGETHOSTSTRUCT);
	if (!res)
		errorexit("WSAAsyncGetHostByName");
#endif
	return 1;
}

U32 getinfofromip(U32 ip)
{
	ip=htonl(ip);
#ifdef NIX
	nothernames=notherips=0;
	struct hostent* hep=gethostbyaddr((C8*)&ip,4,PF_INET);
	if (!hep) { 
//		errorexit("can't get hostbyaddr");
		othernames[0]="Err";			
		nothernames=1;
		otherips[0]=~0;
		notherips=1;
		return 1;
	}
	struct in_addr myip;
	C8** strptr;
// dump hostent structure
	othernames[0]=hep->h_name;
	nothernames=1;
	logger("offical other name '%s'\n",hep->h_name);
	strptr=(C8**)hep->h_aliases;
	while(*strptr) {
		logger("alias other '%s'\n",*strptr);
		if (nothernames<MAXOTHERNAMES)
			othernames[nothernames++]=*strptr;
		strptr++;
	}
	strptr=(C8**)hep->h_addr_list;
	while(*strptr) {
		if (notherips<MAXOTHERIPS) {
			memcpy(&myip,*strptr,4);
			ip=ntohl(myip.s_addr);
			if (notherips<MAXOTHERIPS)
				otherips[notherips++]=ip;
			logger("other address '%s'\n",ip2str(ip));
//			logger("%08x\n",ip);
		}
		strptr++;
	}
#else
	U32 messnum=allocmess(MESS_GETOTHER,0,MESSTIMEOUT);
	if (messnum==~0U)
		return 0;
//	for (i=0;i<nothernames;i++)
//		memfree(othernames[i]);
//		delete[] othernames[i];
	nothernames=notherips=0;
	setusermessage(messnum,getxbyyfunc);
	U32 res=(U32)WSAAsyncGetHostByAddr ((HWND)wininfo.MainWindow,messnum,(C8*)&ip,4,PF_INET,hostbuff,MAXGETHOSTSTRUCT);
	if (!res)
		errorexit("WSAAsyncGetHostByName");
#endif
	return 1;
}

U32 getninfoname()	// returns 0 until we get names
{
	return nothernames;
}

const C8* getinfoname(U32 idx)
{
	if (idx<nothernames)
		return othernames[idx].c_str();
	return 0;
}

U32 getninfoip()	// returns 0 until we get ips
{
	return notherips;
}

U32 getinfoip(U32 idx)
{
	if (idx<notherips)
		return otherips[idx];
	return 0;
}

// handy routines
const C8* ip2str(U32 ip)
{
	static C8 ipstr[16];
	sprintf(ipstr,"%d.%d.%d.%d",ip>>24,0xff&(ip>>16),0xff&(ip>>8),0xff&ip);
	return ipstr;
}

static const C8* getoctet(const C8* str,S32 c,U32* oct)
{
	if (!str)
		return 0;
	if (*str==c)
		return 0;
	*oct=0;
	while(*oct<256) {
		if (*str==c)
			break;
		if (*str=='\0')
			return 0;
		if (*str<'0' || *str>'9')
			return 0;
		*oct = 10 * *oct + *str++ - '0';
	}
	str++;
	return str;
}

U32 str2ip(const C8* str)
{
	U32 a,b,c,d;
	str=getoctet(str,'.',&a);
	str=getoctet(str,'.',&b);
	str=getoctet(str,'.',&c);
	str=getoctet(str,'\0',&d);
	if (!str)
		return ~0;
	return (a<<24)+(b<<16)+(c<<8)+d;
}

#ifdef NIX
void setnonblocking(SOCKET s)
{
	U32 arg=1; // nonblocking
	ioctlsocket(s,FIONBIO,&arg); // this might be ioctl on 'NIX Berkeley machines
}
#else
void setnonblocking(SOCKET s) // windows, do nothing, using async method
{
}
#endif

// init uninit
U32 initsocker() // returns 0 on fail, also starts off get myip's and name async
{
	logger("initsocker\n");
/*
// test 64bit
	U64 u64=~0ULL;
	S32 so=sizeof(u64);
	logger("u64 %llu sizeof %d\n",u64,so);
	S64 s64=-123456789123456789LL;
	so=sizeof(s64);
	logger("s64 %lld sizeof %d\n",s64,so);
// end test 64bit
*/
	C8 hostname[200];
	U32 res;
#ifndef RNIX
	U16 versionwanted=MAKEWORD(1,1);
//	U8 myipstr[16];
	WSADATA wsadata;
//	struct in_addr myip;
// open winsock
//	nsockmess=0;
#endif
	if (dum) {
//		errorexit("initsocker: already inited");
		logger("initsocker: already inited\n");
		return 0;
	}
#ifndef NIX
	memset(messes,0,sizeof(messes));
//	S32 i;
//	for (i=0;i<MAXMESS;++i)
//		messes[i].seqnum=globalseqnum++;
#endif
//	dum=(C8*)memalloc(1);
	dum=new C8[1];
	nmyips=nmynames=0;
#ifndef RNIX
	res=WSAStartup(versionwanted,&wsadata);
	if (res)
		errorexit("Error WSAStartup");
	if ( (LOBYTE(wsadata.wVersion)!=LOBYTE(versionwanted)) ||
		(HIBYTE(wsadata.wVersion)!=HIBYTE(versionwanted)) )
		errorexit("wrong version");
#endif
// getting hostname
//	nmynames=1;
//	mynames[0]=memalloc(200);
	res=gethostname(hostname,200);
	if (res)
		errorexit("can't get gethostname");
	logger("got my hostname '%s'\n",hostname);
//	myip.s_addr=inet_addr(mynames[0]);
//	if (myip.s_addr==INADDR_NONE) {
//		logger("host name not an ip address\n");
// get ip address from hostname, gethostbyname will dial up, get my ip address, or anybody else
#ifdef NIX
	struct hostent* hep = gethostbyname(hostname); // get the host info
	if (!hep)  
		errorexit("can't get hostbyname");
	U32 /*i,*/ip;
	struct in_addr myip;
	C8** strptr;
// dump hostent structure
	mynames[0]=hep->h_name;
	nmynames=1;
	logger("offical name '%s'\n",hep->h_name);
	strptr=(C8**)hep->h_aliases;
	while(*strptr) {
		logger("alias '%s'\n",*strptr);
		if (nmynames<MAXMYNAMES)
			mynames[nmynames++]=*strptr;
		strptr++;
	}
	strptr=(C8**)hep->h_addr_list;
	while(*strptr) {
		if (nmyips<MAXMYIPS) {
			memcpy(&myip,*strptr,4);
			ip=ntohl(myip.s_addr);
			if (nmyips<MAXMYIPS)
				myips[nmyips++]=ip;
			logger("address '%s'\n",ip2str(ip));
//			logger("%08x\n",ip);
		}
		strptr++;
	}
#else
// get ip address from hostname, gethostbyname will dial up, get my ip address, or anybody else
		U32 messnum=allocmess(MESS_GETMY,0,MESSTIMEOUT);
		if (messnum==~0U)
			errorexit("can't alloc critical mess");
		setusermessage(messnum,getxbyyfunc);
		res=(U32)WSAAsyncGetHostByName((HWND)wininfo.MainWindow,messnum,hostname,hostbuff,MAXGETHOSTSTRUCT);
		if (!res)
			errorexit("WSAAsyncGetHostByName");
		logger("gethostbyname messnum %d\n",messnum);
//	} else {
//		myips[0]=ntohl(myip.s_addr);
//		strcpy(myipstr,inet_ntoa(myip));
//		logger("my ip address from gethostname '%s'\n",myipstr);
//		nmyips=1;
//	}
#endif
	defaultfpucontrol();
	return 1;
}


U32 uninitsocker() // returns 0 on fail
{
// uninit winsock
	logger("uninitsocker\n");
//	U32 i;
	if (!dum) {
//		errorexit("uninitsocker: not inited");
		logger("uninitsocker: not inited\n");
		return 0;
	}
//	memfree(dum);
	delete dum;
	dum=0;
//	for (i=0;i<nmynames;i++)
//		memfree(mynames[i]);
//		delete[] mynames[i];
//	for (i=0;i<nothernames;i++)
//		memfree(othernames[i]);
//		delete[] othernames[i];
	nmynames=nothernames=nmyips=notherips=0;
#ifndef RNIX
	U32 res=WSACleanup();
	if (res)
		errorexit("Socket Cleanup");
#endif
	defaultfpucontrol();
	return 1;
}

// TCP make socket
struct socker* tcpmakemaster(U32 myip,U32 myport)
{
	SOCKET sk;
	SOCKADDR_IN sa;
	U32 res;
	struct socker* s;
//	U32 sas=sizeof(sa);
// make socket
	sk=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);	
	defaultfpucontrol();
	if (sk==INVALID_SOCKET) {
		logger("inv makemaster socket\n");
		defaultfpucontrol();
		return 0;
	}
	setnonblocking(sk);
//	logger("master socket handle %d\n",sk);
// bind socket
	sa.sin_family=AF_INET;
	sa.sin_port=htons((U16)myport);
	sa.sin_addr.s_addr=htonl(myip);
	res=bind(sk,(LPSOCKADDR)&sa,sizeof(sa));
	defaultfpucontrol();
	if (res) {
		logger("bad bind mastersocket %08x,%d\n",myip,myport);
		res=closesocket(sk);
		if (res)
			logger("can't close bad bind mastersocket\n");
		defaultfpucontrol();
		return 0;
	}
	res=listen(sk,5); // just found out the '5' might be special 'SOMAXCONN'
	if (res)
		logger("can't set master socket to listen\n");
// set for async
//	s=(struct socker *)memzalloc(sizeof(struct socker));
	s=new socker;
	memset(s,0,sizeof(socker));
#ifndef NIX
	U32 messnum=allocmess(MESS_ACCEPT,(U32)s,0); // -1 on error, WM_USER to WM_USER+MAXMESS-1 on success
	if (messnum<0)
		errorexit("sockmess makemaster");
	setusermessage(messnum,dotcpmess);
#if 0
	s->messnum=messnum;
	s->ip=myip;
	s->port=myport;
	s->kind=SOCK_MASTER;
	s->s=sk;
#endif
	res=WSAAsyncSelect(sk,(HWND)wininfo.MainWindow,messnum,FD_ACCEPT);
	if (res) {
		logger("bad WSAAsyncSelect sockmaster\n");
		freesocker(s);
		defaultfpucontrol();
		return 0;
	}
	s->messnum=messnum;
#endif
	s->ip=myip;
	s->port=myport;
	s->kind=SOCK_MASTER;
	s->s=sk;
	defaultfpucontrol();
#ifdef NIX
	logger("tcpmakemaster: socket %d\n",sk);
#else
	logger("tcpmakemaster: socket %d, messnum %d\n",sk,messnum);
#endif
	return s;
}

struct socker* tcpgetaccept(struct socker* s)
{
	struct socker* r;
	SOCKADDR_IN sa;
	SOCKET sk;
	S32 sas=sizeof(sa);
	if (!s)
		return 0;
	if (s->kind!=SOCK_MASTER)
		return 0;
//	if (!(s->flags&SF_ACCEPTREADY))
//		return 0;
	sk=accept(s->s,(LPSOCKADDR)&sa,&sas);
//	s->flags&=~SF_ACCEPTREADY;
	if (sk==INVALID_SOCKET) {
		defaultfpucontrol();
		return 0;
	}
	setnonblocking(sk);
//	logger("accept socket handle %d\n",sk);
//	r=(struct socker *)memzalloc(sizeof(struct socker));
	r=new socker;
	memset(r,0,sizeof(socker));
//	r->rdbuff=(C8*)memalloc(TCPRDBUFFSIZE);
//	r->wtbuff=(C8*)memalloc(TCPWTBUFFSIZE);
	r->rdbuff=new C8[TCPRDBUFFSIZE];
	r->wtbuff=new C8[TCPWTBUFFSIZE];
#ifndef NIX
	U32 messnum=allocmess(MESS_RWC,(U32)r,0);
	if (messnum==-1)
		errorexit("sockmess getaccept");
	setusermessage(messnum,dotcpmess);
	r->messnum=messnum;
#if 1
	S32 res=WSAAsyncSelect(sk,(HWND)wininfo.MainWindow,messnum,FD_READ|FD_WRITE|FD_CLOSE);
	if (res)
		logger("error on getaccept WSAAsyncSelect\n");
#endif
#endif
	S32 res=getsockname(sk,(LPSOCKADDR)&sa,&sas);
	if (res)
		logger("error on getsockname\n");
	r->ip=ntohl(sa.sin_addr.s_addr);
	r->port=ntohs(sa.sin_port);
	res=getpeername(sk,(LPSOCKADDR)&sa,&sas);
	if (res)
		logger("error on getpeername\n");
	r->remoteip=ntohl(sa.sin_addr.s_addr);
	r->remoteport=ntohs(sa.sin_port);
	r->kind=SOCK_SERVER;
	r->s=sk;
	r->flags|=SF_CONNECTED;
#if 0
	res=WSAAsyncSelect(sk,(HWND)wininfo.MainWindow,messnum,FD_READ|FD_WRITE|FD_CLOSE);
	if (res)
		logger("error on getaccept WSAAsyncSelect\n");
#endif
//	logger("accept new connection local '%s':%d ",ip2str(r->ip),r->port);
//	logger("remote '%s':%d\n",ip2str(r->remoteip),r->remoteport);
	defaultfpucontrol();
#ifdef NIX
	logger("tcpgetaccept: socket %d\n",sk);
#else
	logger("tcpgetaccept: socket %d, messnum %d\n",sk,messnum);
#endif
	return r;
}

#ifdef NIX
void makeconnected(socker* s)
{
	SOCKADDR_IN sa;
	S32 sas=sizeof(sa);
	s->flags|=SF_CONNECTED;
	S32 res=getpeername(s->s,(LPSOCKADDR)&sa,&sas);
	if (res)
		logger("error on getpeername\n");
	s->remoteip=ntohl(sa.sin_addr.s_addr);
	s->remoteport=ntohs(sa.sin_port);
}
#endif

struct socker* tcpgetconnect(U32 serverip,U32 serverport)
{
	int res,res2;
	SOCKET sk;
	SOCKADDR_IN sa;
	struct socker* s;
	S32 sas=sizeof(sa);
// make socket
	sk=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);	
	if (sk==INVALID_SOCKET) {
		logger("inv makeconnect socket\n");
		defaultfpucontrol();
		return 0;
	}
	setnonblocking(sk);
//	logger("connect socket handle %d\n",sk);
// set for async
//	s=(struct socker *)memzalloc(sizeof(struct socker));
	s=new socker;
	memset(s,0,sizeof(socker));
#ifdef NIX
	sa.sin_family=AF_INET;
	sa.sin_port=htons((U16)serverport);
	sa.sin_addr.s_addr=htonl(serverip);
	res=connect(sk,(LPSOCKADDR)&sa,sas);
	if (res) {
		res=WSAGetLastError(); // errno
		if (res!=WSAEWOULDBLOCK) {
			logger("bad connect error\n");
			freesocker(s);
			defaultfpucontrol();
			return 0;
		}
	} 
#if 1
//	s->messnum=messnum;
	s->kind=SOCK_CLIENT;
	s->s=sk;
#endif
	res2=getsockname(sk,(LPSOCKADDR)&sa,&sas);
	if (res2)
		logger("error on getsockname\n");
	s->ip=ntohl(sa.sin_addr.s_addr);
	s->port=ntohs(sa.sin_port);
	s->flags|=SF_CONNECTED;
	if (!res) {
		makeconnected(s);
//		logger("connect finished right away, no waiting for message\n");
//		logger("got a connection local '%s':%d ",ip2str(s->ip),s->port);
//		logger("remote '%s':%d\n",ip2str(s->remoteip),s->remoteport);
	}
//	s->rdbuff=(C8*)memalloc(TCPRDBUFFSIZE);
//	s->wtbuff=(C8*)memalloc(TCPWTBUFFSIZE);
	s->rdbuff=new C8[TCPRDBUFFSIZE];
	s->wtbuff=new C8[TCPWTBUFFSIZE];
#else
	U32 messnum=allocmess(MESS_RWC,(U32)s,0); // -1 on error, WM_USER to WM_USER+MAXMESS-1 on success
	if (messnum<0)
		errorexit("sockmess connect");
	setusermessage(messnum,dotcpmess);
#if 0
	s->messnum=messnum;
	s->kind=SOCK_CLIENT;
	s->s=sk;
#endif
	res=WSAAsyncSelect(sk,(HWND)wininfo.MainWindow,messnum,FD_CONNECT|FD_READ|FD_WRITE|FD_CLOSE);
	if (res) {
		logger("bad WSAAsyncSelect sockconnect\n");
		freesocker(s);
		defaultfpucontrol();
		return 0;
	}
	sa.sin_family=AF_INET;
	sa.sin_port=htons((U16)serverport);
	sa.sin_addr.s_addr=htonl(serverip);
	res=connect(sk,(LPSOCKADDR)&sa,sas);
	if (res) {
		res=WSAGetLastError();
		if (res!=WSAEWOULDBLOCK) {
			logger("bad connect error\n");
			freesocker(s);
			defaultfpucontrol();
			return 0;
		}
	} 
#if 1
	s->messnum=messnum;
	s->kind=SOCK_CLIENT;
	s->s=sk;
#endif
	res2=getsockname(sk,(LPSOCKADDR)&sa,&sas);
	if (res2)
		logger("error on getsockname\n");
	s->ip=ntohl(sa.sin_addr.s_addr);
	s->port=ntohs(sa.sin_port);
	if (!res) {
//		logger("connect finished right away, no waiting for message\n");
		s->flags|=SF_CONNECTED;
		res=getpeername(sk,(LPSOCKADDR)&sa,&sas);
		if (res)
			logger("error on getpeername\n");
		s->remoteip=ntohl(sa.sin_addr.s_addr);
		s->remoteport=ntohs(sa.sin_port);
//		logger("got a connection local '%s':%d ",ip2str(s->ip),s->port);
//		logger("remote '%s':%d\n",ip2str(s->remoteip),s->remoteport);
	}
//	s->rdbuff=(C8*)memalloc(TCPRDBUFFSIZE);
//	s->wtbuff=(C8*)memalloc(TCPWTBUFFSIZE);
	s->rdbuff=new C8[TCPRDBUFFSIZE];
	s->wtbuff=new C8[TCPWTBUFFSIZE];
#endif
	defaultfpucontrol();
#ifdef NIX
	logger("tcpgetconnect: socket %d\n",sk);
#else
	logger("tcpgetconnect: socket %d, messnum %d\n",sk,messnum);
#endif
	return s;
}

// free socket
U32 freesocker(struct socker* sk) // returns 0 on fail
{
	logger("in freesocker\n");
	U32 res;
	if (!sk) { 
		logger("freeing a null socket\n");
		return 0;
	}
#ifndef NIX
	if (sk->messnum>0) {
		freemess(sk->messnum,0);
		logger("freesocker: sock %d, messnum %d\n",sk->s,sk->messnum);
//		freemess(sk->messnum,MESSTIMEOUT);
	} else
		logger("freesocker: sock %d, nomess\n",sk->s);
	// try a graceful close here for tcp sockets
#endif
	switch(sk->kind) {
	case SOCK_MASTER:
	case SOCK_SERVER:
	case SOCK_CLIENT:
	case SOCK_UDP:
		break;
	default:
		logger("unknown socket type to free\n");
	}
	if (sk->rdbuff)
//		memfree(sk->rdbuff);
		delete[] sk->rdbuff;
	if (sk->wtbuff)
//		memfree(sk->wtbuff);
		delete[] sk->wtbuff;
	res=closesocket(sk->s);
//	memfree(sk);
	delete sk;
	if (res) {
		logger("can't close socket\n");
		return 0;
	}
	defaultfpucontrol();
	return 1;
}

// tcp read and write
static S32 tcpreaddata(struct socker* s,C8* buff,S32 len)
{
#ifdef NIX
	S32 e;
	if (!s)
		return 0;
	if (s->kind!=SOCK_CLIENT && s->kind!=SOCK_SERVER)
		return 0;
//	if (!(s->flags & SF_CONNECTED)) {
//		logger("checkrbuff: not connected\n");
//		return XFER_ERROR;
//		return 0;
//	}
//	if (s->flags & SF_CLOSED)
//		if (!(s->flags & SF_READREADY))
//			return XFER_EOF;
//	if (!(s->flags & SF_READREADY))
//		return 0;
	len=recv(s->s,buff,len,0);
//	s->flags&=~SF_READREADY;
	if (len==SOCKET_ERROR) {
		e=WSAGetLastError(); // use 'errno'
		if (e==WSAENETDOWN || /* e==WSAENOTCONN || */ e==WSAESHUTDOWN || e==WSAECONNABORTED || e==WSAECONNRESET) {
			logger("error on recv\n");
			s->flags |= SF_CLOSED;
			return XFER_EOF;
		}
	}
	if (len==0) { // graceful
//		logger("readdata close (len 0)\n");
		s->flags |= SF_CLOSED;
		return XFER_EOF;
	} 
	if (len==SOCKET_ERROR) {
		len=e;
		if (len==WSAESHUTDOWN) { // WSAEWOULDBLOCK
//			logger("readdata close (by shutdown)\n");
			s->flags |= SF_CLOSED;
			return XFER_EOF;
		} else if (len==WSAEWOULDBLOCK || len==WSAENOTCONN) {
//			logger("would block wait for a read message\n");
//			s->readready=0;
			return 0;
		} else
			return XFER_ERROR;
	}
	return len;
#else
	S32 e;
	if (!s)
		return 0;
	if (s->kind!=SOCK_CLIENT && s->kind!=SOCK_SERVER)
		return 0;
	if (!(s->flags & SF_CONNECTED)) {
//		logger("checkrbuff: not connected\n");
		return XFER_ERROR;
	}
	if (s->flags & SF_CLOSED)
		if (!(s->flags & SF_READREADY))
			return XFER_EOF;
	if (!(s->flags & SF_READREADY))
		return 0;
	len=recv(s->s,buff,len,0);
	s->flags&=~SF_READREADY;
	if (len==SOCKET_ERROR) {
		e=WSAGetLastError();
		if (e==WSAENETDOWN || /*e==WSAENOTCONN || */e==WSAESHUTDOWN || e==WSAECONNABORTED || e==WSAECONNRESET) {
			logger("error on recv\n");
			s->flags |= SF_CLOSED;
			return XFER_EOF;
		}
	}
	if (len==0) {
//		logger("readdata close (len 0)\n");
		s->flags |= SF_CLOSED;
		return XFER_EOF;
	}
	if (len==SOCKET_ERROR) {
		len=e;
		if (len==WSAESHUTDOWN) { // WSAEWOULDBLOCK
//			logger("readdata close (by shutdown)\n");
			s->flags |= SF_CLOSED;
			return XFER_EOF;
		} else if (len==WSAEWOULDBLOCK  || len==WSAENOTCONN) {
//			logger("would block wait for a read message\n");
//			s->readready=0;
			return 0;
		} else
			return XFER_ERROR;
	}
	return len;
#endif
}

static S32 tcpwritedata(struct socker* s,C8* buff,S32 len)
{
#ifdef NIX
	if (!s)
		return 0;
	if (s->kind!=SOCK_CLIENT && s->kind!=SOCK_SERVER)
		return 0;
/*	if (!(s->flags & SF_CONNECTED)) {
//		logger("checkwbuff: not connected\n");
		return XFER_ERROR;
	} */
	if (s->flags & SF_CLOSED)
		return XFER_EOF;
//	if (!(s->flags & SF_WRITEREADY))
//		len=0;
//	else {
		len=send(s->s,buff,len,0);
//		s->writeready=0;
//	}
	if (len==SOCKET_ERROR) {
		len=WSAGetLastError(); // use 'errno'
		if (len==WSAESHUTDOWN) { // WSAEWOULDBLOCK
//			logger("writedata close len 0\n");
			s->flags |=SF_CLOSED;
			return XFER_EOF;
		} else if (len==WSAEWOULDBLOCK || len==WSAENOTCONN || len==WSAENOTCONN) {
//			logger("would block wait for a write message\n");
//			s->flags &=~SF_WRITEREADY;
			return 0;
		}
		return XFER_ERROR;
	}
	return len;
#else
	if (!s)
		return 0;
	if (s->kind!=SOCK_CLIENT && s->kind!=SOCK_SERVER)
		return 0;
	if (!(s->flags & SF_CONNECTED)) {
//		logger("checkwbuff: not connected\n");
		return XFER_ERROR;
	}
	if (s->flags & SF_CLOSED)
		return XFER_EOF;
	if (!(s->flags & SF_WRITEREADY))
		len=0;
	else {
		len=send(s->s,buff,len,0);
//		s->writeready=0;
	}
	if (len==SOCKET_ERROR) {
		len=WSAGetLastError();
		if (len==WSAESHUTDOWN) { // WSAEWOULDBLOCK
//			logger("writedata close len 0\n");
			s->flags |=SF_CLOSED;
			return XFER_EOF;
		} else if (len==WSAEWOULDBLOCK || len==WSAENOTCONN) {
//			logger("would block wait for a write message\n");
			s->flags &=~SF_WRITEREADY;
			return 0;
		}
		return XFER_ERROR;
	}
	return len;
#endif
}

// read functions
void tcpfillreadbuff(struct socker* s)
{
	S32 len;
	if (!s)
		return;
	if (s->kind!=SOCK_CLIENT && s->kind!=SOCK_SERVER)
		return;
// compact buffer
	if (s->rdstpos>0) {
		if (s->rdstpos!=s->rdendpos)
			memcpy(s->rdbuff,s->rdbuff+s->rdstpos,s->rdendpos-s->rdstpos);
		s->rdendpos-=s->rdstpos;
		s->rdstpos=0;
	}
// fill buffer
	if (TCPRDBUFFSIZE!=s->rdendpos) {
		len=tcpreaddata(s,s->rdbuff+s->rdendpos,TCPRDBUFFSIZE-s->rdendpos);
		if (len>0)
			s->rdendpos+=len;
	}
}

S32 tcpgetrbuffsize(struct socker* s)
{
	if (!s)
		return 0; 
	if (s->kind!=SOCK_CLIENT && s->kind!=SOCK_SERVER)
		return 0;
	return s->rdendpos-s->rdstpos;
}

S32 tcppeekbuffdata(struct socker *s,C8* littlebuff,S32 minlen,S32 maxlen)
{
	if (!s)
		return 0;
	if (s->kind!=SOCK_CLIENT && s->kind!=SOCK_SERVER)
		return 0;
	if (minlen>s->rdendpos-s->rdstpos)
		return 0;
	if (maxlen>s->rdendpos-s->rdstpos)
		maxlen=s->rdendpos-s->rdstpos;
	if (maxlen)
		memcpy(littlebuff,s->rdbuff+s->rdstpos,maxlen);
	return maxlen;
}

S32 tcpgetbuffdata(struct socker* s,C8* littlebuff,S32 minlen,S32 maxlen)
{
	if (!s)
		return 0;
	if (s->kind!=SOCK_CLIENT && s->kind!=SOCK_SERVER)
		return 0;
	if (minlen>s->rdendpos-s->rdstpos)
		return 0;
	if (maxlen>s->rdendpos-s->rdstpos)
		maxlen=s->rdendpos-s->rdstpos;
	if (maxlen)
		memcpy(littlebuff,s->rdbuff+s->rdstpos,maxlen);
	s->rdstpos+=maxlen;
	return maxlen;
}

S32 tcpgetwbuffleft(struct socker* s)
{
	if (!s)
		return 0;
	if (s->kind!=SOCK_CLIENT && s->kind!=SOCK_SERVER)
		return 0;
	return TCPWTBUFFSIZE-s->wtendpos;
}

S32 tcpputbuffdata(struct socker* s,const C8* littlebuff,S32 minlen,S32 maxlen)
{
	if (!s)
		return 0;
	if (s->kind!=SOCK_CLIENT && s->kind!=SOCK_SERVER)
		return 0;
	if (TCPWTBUFFSIZE-s->wtendpos<minlen)
		return 0;
	if (TCPWTBUFFSIZE-s->wtendpos<maxlen)
		maxlen=TCPWTBUFFSIZE-s->wtendpos;
	memcpy(s->wtbuff+s->wtendpos,littlebuff,maxlen);
	s->wtendpos+=maxlen;
	return maxlen;
}

S32 tcpsendwritebuff(struct socker* s)
{
	S32 len;
	if (!s)
		return 1;
	if (s->kind!=SOCK_CLIENT && s->kind!=SOCK_SERVER)
		return 0;
	if (!s->wtendpos)
		return 1;
	len=tcpwritedata(s,s->wtbuff,s->wtendpos);
	if (len>0) {
		if (len==s->wtendpos) {
			s->wtendpos=0;
			return 1;
		} else {
			s->wtendpos-=len;
			memcpy(s->wtbuff,s->wtbuff+len,s->wtendpos);
			return 0;
		}
	} else if (len<0)
		return 1;
	return 0;
}

// a token will be packed like this, LEN,ID,TOKDATA, returns 0 if cannot do, -1 if error
int tcpreadtoken8(struct socker *s,int *id,char *tok,int len)
{
	int rb;
	int plen;
	U8 plen8;
	U8 id8;
	if (!s)
		return -1;
	if (s->kind!=SOCK_CLIENT && s->kind!=SOCK_SERVER)
		return -1;
	rb=tcpgetrbuffsize(s);
	if (rb<1+1)
		return 0; // no header yet
	tcppeekbuffdata(s,(C8*)&plen8,1,1);
	plen=plen8;
	if (plen>len)
		errorexit("tcpreadtoken8: can't fit"); // the only error (so far)
//		return -1;
	if (rb<1+1+plen)
		return 0; // not enough data yet
	tcpgetbuffdata(s,(C8*)&plen8,1,1);
	plen=plen8;
	tcpgetbuffdata(s,(char *)&id8,1,1);
	S32 did=id8;
	if (id)
		*id=did;
	tcpgetbuffdata(s,tok,plen,plen);
	if (plen>20)
		logger("tcpreadtoken8 len %d, id %d\n",plen,did);
	return plen;
}

int tcpwritetoken8(struct socker *s,int id,char *tok,int len)
{
	if (!s)
		return -1;
	if (s->kind!=SOCK_CLIENT && s->kind!=SOCK_SERVER)
		return -1;
	if (len<0 || len>=256)
		errorexit("tcpwritetoken8 too big len %d",len);
	if (id<0 || id>=256)
		errorexit("tcpwritetoken8 bad id %d",id);
	if (len>20)
		logger("tcpwritetoken8 len %d, id %d\n",len,id);
	if (tcpgetwbuffleft(s)<len+1+1) {
		error("tcpwritetoken8: write buffer overflow, some data lost, toklen %d",len);
		return 0; // no room yet
	}
	tcpputbuffdata(s,(char *)&len,1,1);
	tcpputbuffdata(s,(char *)&id,1,1);
	tcpputbuffdata(s,tok,len,len);
	return len;
}

// a token will be packed like this, LEN,ID,TOKDATA, returns 0 if cannot do, -1 if error
int tcpreadtoken32(struct socker *s,int *id,char *tok,int len)
{
	int rb;
	int plen;
	if (!s)
		return -1;
	if (s->kind!=SOCK_CLIENT && s->kind!=SOCK_SERVER)
		return -1;
	rb=tcpgetrbuffsize(s);
	if (rb<(int)(sizeof(int)+sizeof(int)))
		return 0; // no header yet
	tcppeekbuffdata(s,(char *)&plen,sizeof(int),sizeof(int));
	if (plen>len)
		errorexit("tcpreadtoken32: can't fit"); // the only error (so far)
//		return -1;
	if (rb<(int)(sizeof(int)+sizeof(int))+plen)
		return 0; // not enough data yet
	tcpgetbuffdata(s,(char *)&plen,sizeof(int),sizeof(int));
	if (id)
		tcpgetbuffdata(s,(char *)id,sizeof(int),sizeof(int));
	else {
		S32 did;
		tcpgetbuffdata(s,(char *)&did,sizeof(int),sizeof(int));
	}
	tcpgetbuffdata(s,tok,plen,plen);
	return plen;
}

int tcpwritetoken32(struct socker *s,int id,char *tok,int len)
{
	if (!s)
		return -1;
	if (s->kind!=SOCK_CLIENT && s->kind!=SOCK_SERVER)
		return -1;
	if (tcpgetwbuffleft(s)<len+(int)(sizeof(len)+sizeof(id))) {
		error("tcpwritetoken32: write buffer overflow, some data lost, toklen %d",len);
		return 0; // no room yet
	}
	tcpputbuffdata(s,(char *)&len,sizeof(int),sizeof(int));
	tcpputbuffdata(s,(char *)&id,sizeof(int),sizeof(int));
	tcpputbuffdata(s,tok,len,len);
	return len;
}


// UDP make socket
struct socker* udpmake(U32 myip,U32 myport)	// port of 0 is any, -1 none (unbound)
{
#ifdef NIX
	return 0;
#else
	SOCKET sk;
	S32 messnum;
	SOCKADDR_IN sa;
	U32 res;
	struct socker* s;
	S32 sas=sizeof(sa);
// make socket
	sk=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);	
	if (sk==INVALID_SOCKET) {
		logger("inv udpmake socket\n");
		return 0;
	}
//	logger("udp socket handle %d\n",sk);
// bind socket
	sa.sin_family=AF_INET;
	sa.sin_port=htons((U16)myport);
	sa.sin_addr.s_addr=htonl(myip);
	res=bind(sk,(LPSOCKADDR)&sa,sizeof(sa));
	if (res) {
		logger("bad bind udp socket %08x,%d\n",myip,myport);
		res=closesocket(sk);
		if (res)
			logger("can't close bad bind udp make socket\n");
		return 0;
	}
// set for async
//	s=(struct socker *)memzalloc(sizeof(struct socker));
	s=new socker;
	memset(s,0,sizeof(socker));
	messnum=allocmess(MESS_RWC,(U32)s,0);
	setusermessage(messnum,dotcpmess);
	s->messnum=messnum;
	res=WSAAsyncSelect(sk,(HWND)wininfo.MainWindow,messnum,FD_READ|FD_WRITE|FD_CLOSE);
	if (res)
		logger("error on getaccept WSAAsyncSelect\n");
	res=getsockname(sk,(LPSOCKADDR)&sa,&sas);
	if (res)
		logger("error on getsockname\n");
	else {
		s->ip=ntohl(sa.sin_addr.s_addr);
		s->port=ntohs(sa.sin_port);
//		logger("udp socket setup for ip '%s' and port %d\n",ip2str(s->ip),s->port);
	}
//	s->ip=myip;
//	s->port=myport;
	s->kind=SOCK_UDP;
	s->s=sk;
	defaultfpucontrol();
	return s;
#endif
}

S32 udpputbuffdata(struct socker* s,C8* buff,S32 len,U32 toip,U32 toport)
{
#ifdef NIX
	return 0;
#else
	SOCKADDR_IN sa;
	U32 sas=sizeof(sa);
	if (!s)
		return 0;
	if (s->kind!=SOCK_UDP)
		return 0;
	sa.sin_addr.s_addr=htonl(toip);
	sa.sin_port=htons((U16)toport);
	sa.sin_family=AF_INET;
	if (!(s->flags & SF_WRITEREADY))
		len=0;
	else
		len=sendto(s->s,buff,len,0,(LPSOCKADDR)&sa,sas);
	if (len==SOCKET_ERROR) {
		len=WSAGetLastError();
		if (len==WSAEWOULDBLOCK) {
//			logger("would block wait for a write message\n");
			s->flags &=~SF_WRITEREADY;
			return 0;
		}
		return 0;
	}
	return len;
#endif
}

S32 udpgetbuffdata(struct socker* s,C8* buff,S32 len,U32* fromip,U32* fromport)
{
#ifdef NIX
	return 0;
#else
	SOCKADDR_IN sa;
	S32 sas=sizeof(sa);
	if (!s)
		return 0;
	if (s->kind!=SOCK_UDP)
		return 0;
	sa.sin_family=AF_INET;
	*fromport=0;
	*fromip=~0U;
//	if (!(s->flags & SF_READREADY))
//		len=0;
//	else {
		len=recvfrom(s->s,buff,len,0,(LPSOCKADDR)&sa,&sas);
		*fromip=ntohl(sa.sin_addr.s_addr);
		*fromport=ntohs(sa.sin_port);
//	}
	s->flags&=~SF_READREADY;
	if (len==SOCKET_ERROR) {
		len=WSAGetLastError();
		if (len==WSAEWOULDBLOCK) {
//			logger("would block wait for a read message\n");
//			s->readready=0;
			return 0;
		} else
			return 0;
	}
	return len;
#endif
}

#endif // #ifdef BERK
