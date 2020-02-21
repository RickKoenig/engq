#define INCLUDE_WINDOWS
#include <m_eng.h>
#include <l_misclibm.h>

#include "l_socker_berk.h"
// windows sockets
#ifndef BERK

#define MAXMYIPS 32
#define MAXMYNAMES 32
#define MAXOTHERIPS 32
#define MAXOTHERNAMES 32

#define MESSTIMEOUT 10000 // in millisecs
#define MAXMESS 2500 // this is the max number of connections

#if 1 // fast
#define TCPWTBUFFSIZE 32768
#define TCPRDBUFFSIZE 32768
#elif 1 // 12000 KB/sec
#define TCPWTBUFFSIZE 400
#define TCPRDBUFFSIZE 400
#else // 1200 KB/sec
#define TCPWTBUFFSIZE 40
#define TCPRDBUFFSIZE 40
#endif

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

static U32 nmyips;
static U32 myips[MAXMYIPS];
static U32 nmynames;
static string mynames[MAXMYNAMES];

static U32 notherips;
static U32 otherips[MAXOTHERIPS];
static U32 nothernames;
static string othernames[MAXOTHERNAMES];
static struct mess messes[MAXMESS];

static C8 hostbuff[MAXGETHOSTSTRUCT];

static C8* dum;

static U64 globalseqnum=1ULL;

// user message handler
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
	U32 messnum,res;//,i;
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
	messnum=allocmess(MESS_GETOTHER,0,MESSTIMEOUT);
	if (messnum==~0U)
		return 0;
//	for (i=0;i<nothernames;i++)
//		memfree(othernames[i]);
//		delete[] othernames[i];
	nothernames=notherips=0;
	setusermessage(messnum,getxbyyfunc);
	res=(U32)WSAAsyncGetHostByName((HWND)wininfo.MainWindow,messnum,name,hostbuff,MAXGETHOSTSTRUCT);
	if (!res)
		errorexit("WSAAsyncGetHostByName");
	return 1;
}

U32 getinfofromip(U32 ip)
{
	U32 messnum,res;//,i;
	ip=htonl(ip);
	messnum=allocmess(MESS_GETOTHER,0,MESSTIMEOUT);
	if (messnum==~0U)
		return 0;
//	for (i=0;i<nothernames;i++)
//		memfree(othernames[i]);
//		delete[] othernames[i];
	nothernames=notherips=0;
	setusermessage(messnum,getxbyyfunc);
	res=(U32)WSAAsyncGetHostByAddr ((HWND)wininfo.MainWindow,messnum,(C8*)&ip,4,PF_INET,hostbuff,MAXGETHOSTSTRUCT);
	if (!res)
		errorexit("WSAAsyncGetHostByName");
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
	U32 messnum;
	WORD versionwanted=MAKEWORD(1,1);
//	U8 myipstr[16];
	WSADATA wsadata;
//	struct in_addr myip;
// open winsock
//	nsockmess=0;
	if (dum) {
//		errorexit("initsocker: already inited");
		logger("initsocker: already inited\n");
		return 0;
	}
	memset(messes,0,sizeof(messes));
//	S32 i;
//	for (i=0;i<MAXMESS;++i)
//		messes[i].seqnum=globalseqnum++;
//	dum=(C8*)memalloc(1);
	dum=new C8[1];
	nmyips=nmynames=0;
	res=WSAStartup(versionwanted,&wsadata);
	if (res)
		errorexit("Error WSAStartup");
	if ( (LOBYTE(wsadata.wVersion)!=LOBYTE(versionwanted)) ||
		(HIBYTE(wsadata.wVersion)!=HIBYTE(versionwanted)) )
		errorexit("wrong version");
// getting hostname
//	nmynames=1;
//	mynames[0]=memalloc(200);
	res=gethostname(hostname,200);
	if (res)
		errorexit("gethostname");
	logger("got my hostname '%s'\n",hostname);
//	myip.s_addr=inet_addr(mynames[0]);
//	if (myip.s_addr==INADDR_NONE) {
//		logger("host name not an ip address\n");
// get ip address from hostname, gethostbyname will dial up, get my ip address, or anybody else
		messnum=allocmess(MESS_GETMY,0,MESSTIMEOUT);
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
	defaultfpucontrol();
	return 1;
}

U32 uninitsocker() // returns 0 on fail
{
// uninit winsock
	logger("uninitsocker\n");
//	U32 i;
	U32 res;
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
	memset(messes,0,sizeof(messes));
	res=WSACleanup();
	if (res)
		errorexit("Socket Cleanup");
	defaultfpucontrol();
	return 1;
}

// TCP make socket
struct socker* tcpmakemaster(U32 myip,U32 myport)
{
	SOCKET sk;
	S32 messnum;
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
	messnum=allocmess(MESS_ACCEPT,(U32)s,0); // -1 on error, WM_USER to WM_USER+MAXMESS-1 on success
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
#if 1
	s->messnum=messnum;
	s->ip=myip;
	s->port=myport;
	s->kind=SOCK_MASTER;
	s->s=sk;
#endif
	defaultfpucontrol();
	logger("tcpmakemaster: socket %d, messnum %d\n",sk,messnum);
	return s;
}

struct socker* tcpgetaccept(struct socker* s)
{
	S32 res,messnum;
	struct socker* r;
	SOCKADDR_IN sa;
	SOCKET sk;
	S32 sas=sizeof(sa);
	if (!s)
		return 0;
	if (s->kind!=SOCK_MASTER)
		return 0;
	if (!(s->flags&SF_ACCEPTREADY))
		return 0;
	sk=accept(s->s,(LPSOCKADDR)&sa,&sas);
	s->flags&=~SF_ACCEPTREADY;
	if (sk==INVALID_SOCKET) {
		defaultfpucontrol();
		return 0;
	}
//	logger("accept socket handle %d\n",sk);
//	r=(struct socker *)memzalloc(sizeof(struct socker));
	r=new socker;
	memset(r,0,sizeof(socker));
//	r->rdbuff=(C8*)memalloc(TCPRDBUFFSIZE);
//	r->wtbuff=(C8*)memalloc(TCPWTBUFFSIZE);
	r->rdbuff=new C8[TCPRDBUFFSIZE];
	r->wtbuff=new C8[TCPWTBUFFSIZE];
	messnum=allocmess(MESS_RWC,(U32)r,0);
	if (messnum==-1)
		errorexit("sockmess getaccept");
	setusermessage(messnum,dotcpmess);
	r->messnum=messnum;
#if 1
	res=WSAAsyncSelect(sk,(HWND)wininfo.MainWindow,messnum,FD_READ|FD_WRITE|FD_CLOSE);
	if (res)
		logger("error on getaccept WSAAsyncSelect\n");
#endif
	res=getsockname(sk,(LPSOCKADDR)&sa,&sas);
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
	logger("tcpgetaccept: socket %d, messnum %d\n",sk,messnum);
	return r;
}

struct socker* tcpgetconnect(U32 serverip,U32 serverport)
{
	SOCKET sk;
	S32 messnum;
	SOCKADDR_IN sa;
	U32 res,res2;
	struct socker* s;
	S32 sas=sizeof(sa);
// make socket
	sk=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);	
	if (sk==INVALID_SOCKET) {
		logger("inv makeconnect socket\n");
		defaultfpucontrol();
		return 0;
	}
//	logger("connect socket handle %d\n",sk);
// set for async
//	s=(struct socker *)memzalloc(sizeof(struct socker));
	s=new socker;
	memset(s,0,sizeof(socker));
	messnum=allocmess(MESS_RWC,(U32)s,0); // -1 on error, WM_USER to WM_USER+MAXMESS-1 on success
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
	defaultfpucontrol();
	logger("tcpgetconnect: socket %d, messnum %d\n",sk,messnum);
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
	if (sk->messnum>0) {
		freemess(sk->messnum,0);
		logger("freesocker: sock %d, messnum %d\n",sk->s,sk->messnum);
//		freemess(sk->messnum,MESSTIMEOUT);
	} else
		logger("freesocker: sock %d, nomess\n",sk->s);
	// try a graceful close here for tcp sockets
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
		if (e==WSAENETDOWN || e==WSAENOTCONN || e==WSAESHUTDOWN || e==WSAECONNABORTED || e==WSAECONNRESET) {
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
		len=WSAGetLastError();
		if (len==WSAESHUTDOWN) { // WSAEWOULDBLOCK
//			logger("readdata close (by shutdown)\n");
			s->flags |= SF_CLOSED;
			return XFER_EOF;
		} else if (len==WSAEWOULDBLOCK) {
//			logger("would block wait for a read message\n");
//			s->readready=0;
			return 0;
		} else
			return XFER_ERROR;
	}
	return len;
}

static S32 tcpwritedata(struct socker* s,C8* buff,S32 len)
{
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
		} else if (len==WSAEWOULDBLOCK) {
//			logger("would block wait for a write message\n");
			s->flags &=~SF_WRITEREADY;
			return 0;
		}
		return XFER_ERROR;
	}
	return len;
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

S32 tcpgetwbufffree(struct socker* s)
{
	if (!s)
		return 0;
	if (s->kind!=SOCK_CLIENT && s->kind!=SOCK_SERVER)
		return 0;
	return TCPWTBUFFSIZE-s->wtendpos;
}

S32 tcpgetwbuffused(struct socker* s)
{
	if (!s)
		return 0;
	if (s->kind!=SOCK_CLIENT && s->kind!=SOCK_SERVER)
		return 0;
	return s->wtendpos;
}

S32 tcpgetwbuffsize(struct socker* s)
{
	if (!s)
		return 0;
	if (s->kind!=SOCK_CLIENT && s->kind!=SOCK_SERVER)
		return 0;
	return TCPWTBUFFSIZE;
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

int tcpwritetoken8(struct socker *s,int id,const char *tok,int len)
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
	if (tcpgetwbufffree(s)<len+1+1) {
//		error("tcpwritetoken8: write buffer overflow, some data lost, toklen %d",len);
		logger("tcpwritetoken8, write buffer overflow!, some data lost, closing\n");
		s->flags |= SF_CLOSED;
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

int tcpwritetoken32(struct socker *s,int id,const char *tok,int len)
{
	if (!s)
		return -1;
	if (s->kind!=SOCK_CLIENT && s->kind!=SOCK_SERVER)
		return -1;
	if (tcpgetwbufffree(s)<len+(int)(sizeof(len)+sizeof(id))) {
		logger("tcpwritetoken32, write buffer overflow!, some data lost, closing\n");
		s->flags |= SF_CLOSED;
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
}

S32 udpputbuffdata(struct socker* s,const C8* buff,S32 len,U32 toip,U32 toport)
{
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
}

S32 udpgetbuffdata(struct socker* s,C8* buff,S32 len,U32* fromip,U32* fromport)
{
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
}

#endif // #ifndef BERK
