// win socker api
enum {SOCK_NONE,SOCK_MASTER,SOCK_SERVER,SOCK_CLIENT,SOCK_UDP};

#define SF_READREADY 1
#define SF_WRITEREADY 2
#define SF_ACCEPTREADY 4
#define SF_CONNECTED 8
#define SF_CLOSED 16
#define SF_ERR 32

struct socker {
	U32 s;	// actual socket
	U32 kind;
	U32 ip;
	U32 port;
	U32 remoteip,remoteport;	// for tcp only
	U32 flags;
	S32 messnum;
	C8* rdbuff,*wtbuff;
	S32 rdstpos,rdendpos,wtendpos;
};

U32 messesused();

#define XFER_ERROR -1 // for all reads and writes
#define XFER_EOF -2 

// database functions
U32 getnmyname();	// how many names do we have, return 0 for error
const C8* getmyname(U32); 
U32 getnmyip();	// returns 0 until we get one
U32 getmyip(U32);	// returns 1 of my ips

U32 getinfofromname(const C8*); // initiate getinfo, returns 1 if can do, 0 if try again
U32 getinfofromip(U32);

U32 getninfoname();	// returns 0 until we get names
const C8* getinfoname(U32 n);
U32 getninfoip();	// returns 0 until we get ips
U32 getinfoip(U32 n);

// handy routines
const C8* ip2str(U32 ip);
U32 str2ip(const C8* str);

// init and uninit
U32 initsocker(); // returns 0 on fail, also starts off get myip's and name async
U32 uninitsocker(); // returns 0 on fail

// TCP make socket
struct socker* tcpmakemaster(U32 myip,U32 myport);
struct socker* tcpgetaccept(struct socker*);
struct socker* tcpgetconnect(U32 serverip,U32 serverport);
// tcp read and write
// read functions
void tcpfillreadbuff(struct socker*); // winsock ----> buff
S32 tcpgetrbuffsize(struct socker*);
S32 tcppeekbuffdata(struct socker*,C8* littlebuff,S32 minlen,S32 maxlen);
S32 tcpgetbuffdata(struct socker*,C8* littlebuff,S32 minlen,S32 maxlen); // buff ----> app 
// write functions
S32 tcpgetwbuffused(struct socker*);
S32 tcpgetwbufffree(struct socker*);
S32 tcpgetwbuffsize(struct socker*);
S32 tcpputbuffdata(struct socker*,const C8* littlebuff,S32 minlen,S32 maxlen); // app ----> buff
S32 tcpsendwritebuff(struct socker*); // buff ----> winsock , 1 all data sent to winsock

int tcpreadtoken8(struct socker *,int *id,char *tok,int len); // ret >0 for valid tok, 0 not yet, -1 err
int tcpwritetoken8(struct socker *,int id,const char *tok,int len); // ret >0 for valid write, 0 full, -1 err
int tcpreadtoken32(struct socker *,int *id,char *tok,int len); // ret >0 for valid tok, 0 not yet, -1 err
int tcpwritetoken32(struct socker *,int id,const char *tok,int len); // ret >0 for valid write, 0 full, -1 err

// UDP make socket
struct socker* udpmake(U32 myip,U32 myport);	// port of 0 is any, -1 none (unbound)
// read/write functions
S32 udpgetbuffdata(struct socker*,C8* buff,S32 len,U32* fromip,U32* fromport); // app ----> buff
S32 udpputbuffdata(struct socker*,const C8* buff,S32 len,U32 toip,U32 toport); // buff ----> app 

// free all socket types
U32 freesocker(struct socker*);

