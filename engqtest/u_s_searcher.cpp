#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <m_eng.h>
#include <time.h>
#include <l_misclibm.h>

#include "u_states.h"
#include "timelogger.h"
#include "u_searcher_mandle_thread.h"

int sport = 80; // master listen port for searcher web server

namespace u_s_searcher {


/*
//#define MINUTELOG
#define HOURLOG
#define DAYLOG
*/
timelogger* tl;
#define MAXSERVE 1000 // number of simultaneous connections
//#define TIMEOUT (1*60*30) // # of frames before timeout, about 1 minutes
#define TIMEOUT (45*60*30) // # of frames before timeout, about 45 minutes
#define MAXGETBUFF 4000 // buffer for both gets
#define MAXHTMLBUFF2 400000 // buffer for search results
#define GETTIMEOUT2 (4*30) // about 4 seconds
enum parseresult {parse_file,parse_search,parse_mand};

S32 filtermode=0; // 0 none, 1 exclude, 2 include
C8 *filtertext[]={
	"0 None",
	"1 Exclude",
	"2 Include",
};

script* includelist;
script* excludelist;

//tm curtime;
// gui
shape* rl,*focus,*oldfocus,*pbutquit,*pbutreload,*pbutfilt;

struct place {
	string url;
	string name;
	string prefix;
	bool used;
	bool operator<(const place& rhs) const;
};

bool place::operator<(const place& rhs) const
{
	return prefix.size()>rhs.prefix.size();
}

void parse_gog(vector<place>& vp,string instr,const C8* pf);
string buildget_gog(string instr);
void parse_yah(vector<place>& vp,string instr,const C8* pf);
string buildget_yah(string instr);
void parse_ask(vector<place>& vp,string instr,const C8* pf);
string buildget_ask(string instr);

/*FILE* dailylog;
FILE* hourlylog;
FILE* minutelog;
*/
struct us {
	C8* name;
	string (*getf)(string);
	void (*parsef)(vector<place>& vp,string htmlresultsfromrealsearchengine,const C8* pf);
	C8* prefix;
	U32 ip;
};
struct us usearch[] = {
	{"search.yahoo.com",buildget_yah,parse_yah,"Y",0}, // getting there
	{"www.ask.com",buildget_ask,parse_ask,"A",0},
	{"www.google.com",buildget_gog,parse_gog,"G",0}, // test google search
// OBSOLETE
//	{"www.google.com","/search?num=5&start=0&q=hi+ho","GT",0}, // test google search
//	{"www.google.com","/","GT2",0}, // default google page
//	{"1.2.3.4","/","NOSERV",0},	// no socket on yport
//	{"rickiters","/","NODNS",0}, // no dns name2ip, ip used is 255.255.255.255
	{0,0},
};

// state global
int frame;  // 1/30 sec frame number
int ndone;	// number of transactions finished
struct con32 *acon; // console output
int nus;

// for master
socker *ms;
int yport=80; // 80 or 8080 // port of the search engines 80
//int sport=80; // port of the custom web server 80 or 8080
enum msstate {
	MS_WAITIP,	// wait for valid net info , myip, myname
	MS_WAITUS,	// wait for ip's of search engines
	MS_READY	// master socket created and ready to listen, serve
};
msstate msmode;

script* redirstrs;

string buildget_gog(string inget)
{
	string outget;
	con32_printf(acon,"in gog get command '%s'\n",inget.c_str());
	outget="/search?num=100&start=0&";
	outget+=inget.substr(inget.find("?")+1,~0);
	con32_printf(acon,"out '%s'\n",outget.c_str());
	return outget;
}

string buildget_yah(string inget)
{
	string outget;
	con32_printf(acon,"in yah get command '%s'\n",inget.c_str());
	outget="/search";
	outget+="?p=";
	outget+=inget.substr(inget.find("?q=")+strlen("?q="),~0);
	outget+="&num=50";
	con32_printf(acon,"out '%s'\n",outget.c_str());
	return outget;
}

string buildget_ask(string inget)
{
	string outget;
	con32_printf(acon,"in ask get command '%s'\n",inget.c_str());
	outget="/web";
	outget+="?q=";
	outget+=inget.substr(inget.find("?q=")+strlen("?q="),~0);
	outget+="&num=50";
	con32_printf(acon,"out '%s'\n",outget.c_str());
	return outget;
}

string getbetween(string s,S32 pos,string header,string footer,S32* newpos)
{
	string ret;
	S32 startpos=s.find(header,pos);
	if (startpos==-1) {
		*newpos=-1;
		return ret;
	}
	startpos+=header.length();
	S32 endpos=s.find(footer,startpos+1);
	if (endpos==-1) {
		*newpos=-1;
		return ret;
	}
	ret=s.substr(startpos,endpos-startpos);
	*newpos=endpos+footer.length();
	return ret;
}

string getbetweeninclude(string s,S32 pos,string header,string footer,S32* newpos)
{
	string ret=getbetween(s,pos,header,footer,newpos);
	if (*newpos!=-1) {
		ret = header + ret + footer;
	}
	return ret;
}

void parse_gog(vector<place>& outstr,string instr,const C8* pf)
{
	S32 lipos=0;
	S32 cnt=0;
	while(lipos!=-1) {
		string li="<li"; // find next <li but not if <link
		lipos=instr.find(li,lipos);
		if (lipos!=-1) { // found <li
			string skiplink="<link";
			S32 linkpos=instr.find(skiplink,lipos);
			if (linkpos==lipos) { // it's a <link
				lipos+=skiplink.length(); // skip that
				continue;
			}
		}
		if (lipos!=-1) {
			string urlstr=getbetween(instr,lipos,"<a href=\"","\"",&lipos);
			string namestr=getbetween(instr,lipos,">","</a>",&lipos);
			size_t p;
			string urlcut="/url?q="; // remove this from a url...
			p=urlstr.find(urlcut);
			if (p!=string::npos) {
				urlstr=urlstr.substr(0,p)+urlstr.substr(p+urlcut.size(),~0);
			}
			p=urlstr.find("&"); // something new
			if (p != string::npos) {
				urlstr = urlstr.substr(0,p);
			}
			if (urlstr.find("http://")==~0U)
				continue;
//			logger("\turl = '%s'\n",urlstr.c_str());
//			logger("\tname = '%s'\n\n",namestr.c_str());
			place e;
			e.used=false;
			e.prefix=pf;
			e.url=urlstr;
			e.name=namestr;
			outstr.push_back(e);
			++cnt;
		}
	}
//	logger("cnt = %d\n",cnt);
}

void parse_yah(vector<place>& outstr,string instr,const C8* pf)
{
	S32 lipos=0;
	S32 cnt=0;
	while(lipos!=-1) {
		string li="<li"; // find next <li but not if <link
		lipos=instr.find(li,lipos);
		if (lipos!=-1) { // found <li
			string skiplink="<link";
			S32 linkpos=instr.find(skiplink,lipos);
			if (linkpos==lipos) { // it's a <link
				lipos+=skiplink.length(); // skip that
				continue;
			}
		}
		if (lipos!=-1) {
			string urlstr=getbetween(instr,lipos,"<a class=\"yschttl\" href=\"","\"",&lipos);
			if (lipos==-1)
				break;
			string namestr=getbetween(instr,lipos,">","</a>",&lipos);
			if (lipos==-1)
				break;
			S32 p;
			string urlcut="/url?q="; // remove this from a url...
			p=urlstr.find(urlcut);
			if (p!=-1) {
				urlstr=urlstr.substr(0,p)+urlstr.substr(p+urlcut.size(),~0);
			}
			if (urlstr.find("http://")==~0U)
				continue;
//			logger("\turl = '%s'\n",urlstr.c_str());
//			logger("\tname = '%s'\n\n",namestr.c_str());
			place e;
			e.used=false;
			e.prefix=pf;
			e.url=urlstr;
			e.name=namestr;
			outstr.push_back(e);
			++cnt;
		}
	}
//	logger("cnt = %d\n",cnt);
}
#if 0
    <a id="r5_t" href="http://www.hihosilver.nl/"
		onmousedown="return fp(this,{en:'te',io:'0',b:'a006',tp:'d',ec:'1'
		,ex:'tsrc%3Dlxlx'},'false',0)"
	class="L4" >
		<b>Hi Ho</b> Silver Coverbands: fifties sixties seventies rock n roll disco</a>
#endif
void parse_ask(vector<place>& outstr,string instr,const C8* pf)
{
	string href="href=";
	S32 lipos=0;
	S32 cnt=0;
	while(1) {
		string lo=getbetweeninclude(instr,lipos,"<a","</a>",&lipos);
		if (lipos==-1)
			break;
		S32 inner=0;
		string li=getbetween(lo,0,"<a id=\"","class=\"L4\"",&inner); // find <a id=" and class="L4"
		if (inner==-1)
			continue;
		string urlstr=getbetween(lo,0,"href=\"","\"",&inner); // find <a href=" and " in substr get get url
		if (inner==-1)
			continue;
		string namestr=getbetween(lo,inner,">","</a>",&inner);
		if (inner==-1)
			continue;
		if (urlstr.find("http://")==~0U)
			continue;
//		logger("\turl = '%s'\n",urlstr.c_str());
//		logger("\tname = '%s'\n\n",namestr.c_str());
		place e;
		e.used=false;
		e.prefix=pf;
		e.url=urlstr;
		e.name=namestr;
		outstr.push_back(e);
		++cnt;
	}
//	logger("cnt = %d\n",cnt);
}

// just send a file, header already in file
char* getfile(const C8* fname,int* fsize)
{
	C8 nfname[1000];
	strcpy(nfname,fname);
	C8* p=strstr(nfname,"?");
	if (p)
		*p=0;
	if (fileexist(nfname)) {
		C8* fbuff=fileload(nfname); // uses new char[] or new C8[]
		FILE* f=fopen2(nfname,"rb");
		*fsize=getfilesize(f);
		fclose(f);
		return fbuff;

	}
	*fsize=0;
	return 0;
}

string getdt(const char* fname)
{
	string ret="unk time";
	char t[100];
	struct stat b;
	if (!stat(fname,&b)) {
		strftime(t,100,"%a, %d %b %Y %H:%M:%S GMT",gmtime(&b.st_mtime) );
		ret=t;
	}
	return ret;
}
// send a file and and a header
char* getfile_addhead(const C8* fname,int* fsize)
{
	C8 headf1[]="HTTP/1.0 200 OK\r\nContent-Type: application/xml\r\nLast-Modified: %s\r\n\r\n";
	C8 headf2[]="HTTP/1.0 200 OK\r\nContent-Type: text/css\r\nLast-Modified: %s\r\n\r\n";
	C8 headf3[]="HTTP/1.0 200 OK\r\nLast-Modified: %s\r\n\r\n";
	C8* headf;
	if (isfileext(fname,"css"))
		headf = headf2;
	else if (isfileext(fname,"xml"))
		headf = headf1;
	else
		headf = headf3;
	C8 head[1000];
	C8 nfname[1000];
	strcpy(nfname,fname);
	C8* p=strstr(nfname,"?");
	if (p)
		*p=0;
	if (fileexist(nfname)) {
		C8* fbuff=fileload(nfname); // uses new char[] or new C8[]
		string dt=getdt(nfname);
		logger("server load file '%s' with '%s'\n",nfname,dt.c_str());
		sprintf(head,headf,dt.c_str());
		S32 hs=strlen(head);
		FILE* f=fopen2(nfname,"rb");
		S32 fs=getfilesize(f);
		fclose(f);
		*fsize=hs+fs;
		C8 *nfbuff = new C8[*fsize+1];
		memcpy(nfbuff,head,hs);
		memcpy(nfbuff+hs,fbuff,fs);
		nfbuff[*fsize]='\0';
		delete fbuff;
		return nfbuff;
	}
	*fsize=0;
	return 0;
}

// send a file and and a header
char* getfile_redir(const C8* parms,int* fsize)
{
	string head = fileload_string("redir_head.html");
	string mid = redirstrs->idx(1);
	string foot = fileload_string("redir_foot.html");
	const C8* p=strchr(parms,'=');
	if (p) {
		++p;
		con32_printf(acon,"REDIR '%s'\n",p);
		int i;
		for (i=0;i<redirstrs->num();i+=2) {
			if (redirstrs->idx(i)==p) {
				mid = redirstrs->idx(i+1);
			}
		}
	}
	string tot = head + mid + foot;
	*fsize = tot.length();
	C8 *nfbuff = new C8[*fsize+1];
	strcpy(nfbuff,tot.c_str());
	return nfbuff;
}

C8* searcher(const C8* command,S32* rsize)
{
	S32 len=strlen(command);
	C8* ret=new C8[len+1];
	memcpy(ret,command,len);
	ret[len]='\0';
	*rsize=len;
	return ret;
}

// I'd use string, but data returned might be binary
char* parse(char *in,int inlen,int* outlen,parseresult* us)
{
	*us=parse_file;
//	string ret;
// scan for slash
	while(*in) {
		if (*in=='/')
			break;
		in++;
	}
	if (!*in) {
//		errorexit("no slash found in GET");
		return getfile("index.html",outlen);
	}
	in++;
// scan for space or crlf
	C8* p=in;
#if 1
	while(*p) {
		if (*p=='/' || *p=='\\') {
//			errorexit("extra slash found in GET");
//			return getfile("index.html",outlen);
		}
		if (*p==' ' || *p=='\n' || *p=='\r')
			break;
		p++;
	}
#endif
	if (!*p) {
//		errorexit("eof in GET");
		return getfile("index.html",outlen);
	}
	*p = '\0';
	if (!strcmp(in,"search?q=")) // empty
		return getfile("indexo.html",outlen);
	else if (!strcmp(in,"search?&q=")) // empty , some proxy servers
		return getfile("indexo.html",outlen);
// look for .. and kill that nonsense
	if (strstr(in,".."))
		return getfile("indexo.html",outlen);
	S32 lenp=strlen(in);
	if (lenp==0 || lenp>200)
		return getfile("index.html",outlen);
//	logger("get command is '%s'\n",in);
	if (strstr(in,"search?r="))  { // redirect
		return getfile_redir(in,outlen);
	}
	else if (strstr(in,"search?")) { // search
		*us = parse_search;
		return searcher(in,outlen);
	}
	if (isfileext(in,"html")) {
		char* gf = getfile(in,outlen);
		if (gf)
			return gf;
		else
			return getfile("index.html",outlen);
	} else {
		if (strstr(in,"_mandl_")==in) { // starts with '_mand_'
			S32 i;
			for (i=0;in[i];++i) {
				if (in[i] == '?') { // remove anti cache stuff , remove all stuff after and including ?
					in[i] = '\0';
					break;
				}
			}
			if (fileexist(in)) {
				return getfile_addhead(in,outlen);
			} else {
				*us = parse_mand;
				C8* r=new C8[strlen(in)+1];
				strcpy(r,in);
				return r;
			}
		}
//		return getfile(in,outlen);
		return getfile_addhead(in,outlen);
	}
}

//enum cm {CM_OFF,CM_INIT};
//static cm crawlmode;

// a client server socket
class serv {
public:
// for client
	enum clstate {
		CL_OFF,			// all done or aborted, ready to be cleaned up
		CL_WAITREAD,	// wait for the whole get from searcher/client
		CL_CONNECT,		// connect to real search engine
		CL_SENDGET,		// wait for all get data sent
		CL_GETHTML,		// wait for html results
		CL_WAITWRITE,	// send filtered data back to searcher/client

		CL_WAITTASK
	};
//	enum clinfostate {CLI_OFF,CLI_WAITINFO}; // can't implement, would possibly slow down server
	socker *cl,*cl2;
	clstate clmode;
//	clinfostate climode;
	char getbuff[MAXGETBUFF]; // get client req
	int rdpos;

	int curus;
	string getbuff2; // get usearch
	S32 getbuff2len;
	int wtpos2;
	int gettime2;

	char htmlbuff2[MAXHTMLBUFF2]; // usearch results
	int rdpos2;

	char* htmlbuff;	// send to client
	int htmlbuffsize;
	int wtpos;

	char fname[300];
	int timr;
	vector<place> pg;
	const char* head;
	const char* tail;
	const char* front;
	const char* middle;
	const char* middle2;
	const char* back;
	serv(socker* s) : cl(s),cl2(0),clmode(CL_WAITREAD)/*,climode(CLI_WAITINFO)*/,
		rdpos(0),curus(0),wtpos2(0),gettime2(0),rdpos2(0),htmlbuff(0),htmlbuffsize(0),wtpos(0),timr(0),
		head(0),tail(0),front(0),middle(0),middle2(0),back(0)
	{
//		con32_printf(acon,"==========================================================\n");
	}
	~serv()
	{
		if (htmlbuff)
			delete[] htmlbuff;
		if (cl)
			freesocker(cl);
		if (cl2)
			freesocker(cl2);
		if (head)
			delete[] head;
		if (tail)
			delete[] tail;
		if (front)
			delete[] front;
		if (middle)
			delete[] middle;
		if (middle2)
			delete[] middle2;
		if (back)
			delete[] back;
	}
	bool isdone() { return clmode==CL_OFF; }
	void sproc()
	{
/*
// FSM client socket info
		switch(climode) {
		case CLI_WAITINFO:
			if (getninfoname()) { // doesn't work for simultaneous requests, commented out...
				con32_printf(acon,"info from ip is '%s'\n",getinfoname(0));
				climode=CLI_OFF;
			}
			break;
		}
*/
// FSM client socket reads and writes
//	S32 temppos;
		++timr;
		if (timr>=TIMEOUT) {
			clmode=CL_OFF;
			con32_printf(acon,"* -> CL_OFF : timeout!!\n");
			return;
		}
		S32 len;
		switch(clmode) {
		case CL_WAITREAD:
// read request packet
			tcpfillreadbuff(cl); // winsock ----> buff
			if (cl->flags&SF_CLOSED) {
				clmode=CL_OFF;
//				climode=CLI_OFF;
				con32_printf(acon,"CL_WAITREAD -> CL_OFF : cl premature close!!\n");
				break;
			}
			if (rdpos<MAXGETBUFF-2)
				rdpos+=tcpgetbuffdata(cl,getbuff+rdpos,1,MAXGETBUFF-1-rdpos);	// let data trickle in, max size (TCPRDBUFFSIZE, 16k enough)   old // 'GET / '
		//	logger("read data %d\n",rdpos);
			if ((rdpos>4 && getbuff[rdpos-4]=='\r' && // look for end of get
					getbuff[rdpos-3]=='\n' &&
					getbuff[rdpos-2]=='\r' &&
					getbuff[rdpos-1]=='\n')) {
				getbuff[rdpos]='\0';
/*
// print the data just 'got'
				logger("get buffer\n");
				for (i=0;i<rdpos;++i) {
					if (getbuff[i]<32 || getbuff[i]>126)
						logger("get '%d'\n",getbuff[i]);
					else
						logger("get '%d' '%c'\n",getbuff[i],getbuff[i]);
				}
*/
//				logger("done reading request packet, frame %d, len %d\n",frame,rdpos);
//				fwrite("--------------------\r\n",22,1,fw);
//				fwrite(getbuff,rdpos,1,fw);
// parse the clients request
				enum parseresult dousearch;
				htmlbuff=parse(getbuff,rdpos,&htmlbuffsize,&dousearch);
//				logger("results:::::::\n");
//				logger("%s\n",userbuff);
//				logger("end results:::::::\n");
// if the request is for a page, get the file loaded into memory
// get web page to send from file
#if 1
				con32_printf(acon,"getbuffer '%s:%d': '%s'",ip2str(cl->remoteip),cl->remoteport,getbuff);
				logger("getbuffer: '%s'",getbuff);
/*				if (dailylog)
					fprintf(dailylog,"getbuffer: '%s'\n",getbuff);
				if (hourlylog)
					fprintf(hourlylog,"getbuffer: '%s'\n",getbuff);
				if (minutelog)
					fprintf(minutelog,"getbuffer: '%s'\n",getbuff); */
				tl->printf("getbuffer '%s:%d': '%s'\n",ip2str(cl->remoteip),cl->remoteport,getbuff);

#endif
				if (dousearch == parse_file) { // it's a path, no searching, get a file
					con32_printf(acon,"CL_WAITREAD -> CL_WAITWRITE : get got %d bytes, send localfile\n",rdpos);
					clmode=CL_WAITWRITE;
				} else if (dousearch == parse_mand) { // redirect, connect to real search engine and set appropriate get
					con32_printf(acon,"build %s\n",htmlbuff);
					addtask(htmlbuff);
					clmode=CL_WAITTASK;
				} else { // redirect, connect to real search engine and set appropriate get
					const time_t ti=time(0);
					const tm* lt=gmtime(&ti);
					con32_printf(acon,"ip '%s' searching for '%s' at %s",ip2str(cl->remoteip),htmlbuff,asctime(lt));
					logger("ip '%s' searching for '%s' at %s",ip2str(cl->remoteip),htmlbuff,asctime(lt));
/*					if (dailylog)
						fprintf(dailylog,"ip '%s' searching for '%s' at %s",ip2str(cl->remoteip),htmlbuff,asctime(lt));
					if (hourlylog)
						fprintf(hourlylog,"ip '%s' searching for '%s' at %s",ip2str(cl->remoteip),htmlbuff,asctime(lt));
					if (minutelog)
						fprintf(minutelog,"ip '%s' searching for '%s' at %s",ip2str(cl->remoteip),htmlbuff,asctime(lt));
*/
					tl->printf("ip '%s' searching for '%s' at %s",ip2str(cl->remoteip),htmlbuff,asctime(lt));
					clmode=CL_CONNECT;
					con32_printf(acon,"CL_WAITREAD -> CL_CONNECT : get got %d bytes\n",rdpos);
					head=fileload("head.html",0);
					tail=fileload("tail.html",0);
					front=fileload("front.html",0);
					middle=fileload("middle.html",0);
					middle2=fileload("middle2.html",0);
					back=fileload("back.html",0);
				}
			} else if (rdpos>=MAXGETBUFF-1) { // overflow in get
				ndone++;
				clmode=CL_OFF;
				con32_printf(acon,"CL_WAITREAD -> CL_OFF : get buffer overflow, aborting!!\n");
			}
			break;
		case CL_WAITTASK:
			if (waittask(string(htmlbuff))) {
				if (!fileexist(htmlbuff)) { // hmm..., says task done, but no file
					resettask(htmlbuff); // catch it next time
					addtask(htmlbuff);
					break;
				}
				C8* fname = htmlbuff;
				clmode = CL_WAITWRITE;
				htmlbuff = getfile_addhead(fname,&htmlbuffsize);
				delete[] fname;
//				htmlbuff = getfile_addhead(htmlbuff,&htmlbuffsize);
			}
			break;
		case CL_CONNECT:
			cl2=tcpgetconnect(usearch[curus].ip,yport);
			if (!cl2) {
				con32_printf(acon,"CL_WAITREAD -> CL_OFF : can't connect to '%s'!!\n");
				clmode=CL_OFF;
				break;
			}
			getbuff2 = "GET ";
			getbuff2 += usearch[curus].getf(htmlbuff);//"/search?num=5&start=0&q=hi+ho"; //usearch[curus].command;
			getbuff2 += " HTTP/1.0\r\nHost: ";
			getbuff2 += usearch[curus].name;
			getbuff2 += "\r\n\r\n";
			getbuff2len = getbuff2.length();
//			logger("http send '%s'\n",getbuff2.c_str());
			con32_printf(acon,"CL_CONNECT -> CL_SENDGET : pass query to '%s'\n",usearch[curus].name);
			clmode=CL_SENDGET;
			wtpos2=0;
			break;
		case CL_SENDGET:
			++gettime2;
			if (gettime2>=GETTIMEOUT2) {
				con32_printf(acon,"CL_SENDGET -> CL_OFF: cl2 quick sendget timeout!!\n");
				clmode=CL_OFF;
			}
			if ((cl2->flags & SF_CONNECTED) && wtpos2!=getbuff2len) {
				wtpos2+=tcpputbuffdata(cl2,getbuff2.c_str()+wtpos2,1,getbuff2len-wtpos2);
//				logger("write data %d/%d\n",wtpos2,httpstr2len);
				if (cl2->flags & SF_CLOSED) {
					con32_printf(acon,"CL_SENDGET : cl2 early close during write!!\n");
					wtpos2=getbuff2len;
				}
			}
			if (tcpsendwritebuff(cl2) && wtpos2==getbuff2len) { // see if buffer data really all sent
				clmode=CL_GETHTML;
				con32_printf(acon,"CL_SENDGET -> CL_GETHTML : sent redirect get %d bytes\n",wtpos2);
				rdpos2=0;
			}
			break;
		case CL_GETHTML:
#if 1
// wait for reply
			tcpfillreadbuff(cl2);
			if (rdpos2<MAXHTMLBUFF2-2) {
				len=tcpgetbuffdata(cl2,htmlbuff2+rdpos2,1,MAXHTMLBUFF2-1-rdpos2);	// let data trickle in
				rdpos2+=len;
				if (!len && (cl2->flags&SF_CLOSED)) {
// all done
					freesocker(cl2);
					cl2=0;
//					logger("\ntotal data read %d\n",rdpos);
					htmlbuff2[rdpos2]='\0';
					con32_printf(acon,"CL_GETHTML -> CL_WAITWRITE : got html %d bytes\n",rdpos2);
					string gotname;
					gotname = "got_";
					gotname += usearch[curus].prefix;
					gotname += ".html";
					FILE* fw;
					fw=fopen2(gotname.c_str(),"wb");
					if (fw) {
						fwrite(htmlbuff2,1,rdpos2,fw);
						fclose(fw);
					}
//					vector<place> pg=parse_gog(htmlbuff2);
//					vector<place> pg;
					usearch[curus].parsef(pg,htmlbuff2,usearch[curus].prefix);
					++curus;
					if (usearch[curus].name==0) {
						U32 i,j;
						string out;
						out+=head;
						vector<place> merged;
						for (i=0;i<pg.size();++i) {
							string pf=pg[i].prefix;
							if (pg[i].used)
								continue;
							for (j=i+1;j<pg.size();++j) {
								if (pg[j].used)
									continue;
								if (pg[i].url==pg[j].url) {
									pf+=pg[j].prefix;
									pg[j].used=true;
								}
							}
							place mg=pg[i];
							mg.prefix=pf;
							merged.push_back(mg);
						}
						sort(merged.begin(),merged.end());
						for (i=0;i<merged.size();++i) {
							bool inc=false,exc=false;
							if (filtermode==1) {
								for (j=0;j<(U32)excludelist->num();++j) {
									if (~0U != merged[i].url.find(excludelist->idx(j))) {
										exc=true;
										break;
									}
								}
							}
							if (filtermode==2) {
								for (j=0;j<(U32)includelist->num();++j) {
									if (~0U != merged[i].url.find(includelist->idx(j))) {
										inc=true;
										break;
									}
								}
							}
							if (filtermode==0 ||
									(filtermode==1 && !exc) ||
									(filtermode==2 && inc)) {
								out+=merged[i].prefix;
								out+=" ";
								out+=front;
								out+=merged[i].url;//"www.rickiters.com";
								out+=middle;
								out+=merged[i].name;//"Ricks Place";
								out+=middle2;
								out+=merged[i].url;//"Ricks Place";
								out+=back;
							}
						}
						out+=tail;
						fw=fopen2("built.html","wb");
						if (fw) {
							fwrite(out.c_str(),1,out.size(),fw);
							fclose(fw);
						}
						clmode=CL_WAITWRITE;
						delete[] htmlbuff;
						htmlbuff=new C8[out.size()];
						memcpy(htmlbuff,out.c_str(),out.size());
						htmlbuffsize=out.size();
					} else {
//						out+="<hr/>\n";
						clmode=CL_CONNECT;
						con32_printf(acon,"CL_GETHTML -> CL_CONNECT : more searching to do\n");
					}
				}
			} else {
				con32_printf(acon,"CL_GETHTML -> CL_OFF : overflow in search !\n");
				clmode=CL_OFF;
			}
#endif
			break;
		case CL_WAITWRITE:
// send response packet
			len=0;
			if (wtpos!=htmlbuffsize) {
				len=tcpputbuffdata(cl,htmlbuff+wtpos,1,htmlbuffsize-wtpos);
//				logger("write data %d/%d\n",wtpos,htmlbuffsize);
				if (cl->flags&SF_CLOSED) {
					wtpos=htmlbuffsize;
					con32_printf(acon,"CL_WAITWRITE : cl early close during write!!\n");
				} else
					wtpos+=len;
			}
// close down
			if (tcpsendwritebuff(cl) && len==0 && wtpos==htmlbuffsize) { // see if buffer data really all sent
				ndone++;
				clmode=CL_OFF;
//				climode=CLI_OFF;
				con32_printf(acon,"CL_WAITWRITE -> CL_OFF : sent %d bytes, DONE\n",wtpos);
/*				FILE* fw=fopen2("sr.html","wb");
				if (fw) {
					fwrite(htmlbuff,1,htmlbuffsize,fw);
					fclose(fw);
				} */
			}
			break;
		case CL_OFF:
			break;
		}
	}
};

serv* service[MAXSERVE];
S32 nservice;

void initservice()
{
	U32 i;
	for (i=0;i<MAXSERVE;++i) {
		service[i]=0;
	}
	nservice=0;
}

void procservice()
{
	U32 i;
	for (i=0;i<MAXSERVE;++i) {
		if (service[i]) {
			service[i]->sproc();
		}
	}
}

bool isfreeservice()
{
	return nservice!=MAXSERVE;
}

void addservice(socker* s)
{
	U32 i;
	for (i=0;i<MAXSERVE;++i) {
		if (!service[i]) {
			service[i]=new serv(s);
			++nservice;
			return;
		}
	}
	errorexit("can't add service");
}

void garbservice()
{
	U32 i;
	for (i=0;i<MAXSERVE;++i) {
		if (service[i] && service[i]->isdone()) {
			delete service[i];
			service[i]=0;
			--nservice;
		}
	}
}

void killservice()
{
	U32 i;
	for (i=0;i<MAXSERVE;++i) {
		if (service[i]) {
			delete service[i];
			service[i]=0;
			--nservice;
		}
	}
}

} // namespace

using namespace u_s_searcher;

void searcher_init()
{
	threadinit();
	tl=new timelogger("searcher","srch_");
	pushandsetdir("searcher");
	video_setupwindow(1024,768);
	acon=con32_alloc(1024-800+976-224,768 - 600 + 704-168,C32BLACK,C32WHITE);
	frame=ndone=0;
	initsocker();
	factory2<shape> fact;
	script* sc= new script("searcher_res.ui");
	rl=fact.newclass_from_handle(*sc);
	delete sc;
	focus=oldfocus=0;
	pbutquit=rl->find<pbut>("QUIT");
	pbutreload=rl->find<pbut>("RELOAD");
	pbutfilt=rl->find<pbut>("FILTER");
//	crawlmode=CM_OFF;
	msmode=MS_WAITIP;
	initservice();
//	const time_t ti=time(0);
//	tm* temp=gmtime(&ti);
//	curtime=*temp;
	includelist=new script("include.lst");
	excludelist=new script("exclude.lst");
	script fms=script("filtermode.txt");
	filtermode=atoi(fms.idx(0).c_str());
	redirstrs=new script("redirstrs.txt");
	if ((redirstrs->num()<2)||(redirstrs->num()&1))
		errorexit("bad redirstrs.txt");
}

void searcher_proc()
{
	if (KEY==K_ESCAPE)
		poporchangestate(STATE_MAINMENU);
	if (wininfo.mleftclicks)
		focus=rl->getfocus();
	S32 ret=-1;
	if (focus) {
		ret=focus->proc();
	}
	if (oldfocus && oldfocus!=focus)
		oldfocus->deactivate();
	oldfocus=focus;
	if (focus == pbutquit) {
		if (ret==1)
			poporchangestate(STATE_MAINMENU);
	} else if (focus == pbutreload) {
		if (ret==1)
			changestate(STATE_SEARCHER);
	} else if (focus == pbutfilt) {
		if (ret==1) {
			++filtermode;
			if (filtermode==3)
				filtermode=0;
		}
	}
	S32 i;
	int gotmyip;
// FSM master socket
// wait for my ip
	if (nservice>0)
		wininfo.sleeper=100; // if backgnd, keep this task alive for at least 100 frames, until all writing done..
	switch(msmode) {
	case MS_WAITIP:
		gotmyip=getnmyname();
		for (i=0;i<gotmyip;i++) {
			logger("\nmynames(s) '%s'\n",getmyname(i));
			con32_printf(acon,"mynames(s) '%s'\n",getmyname(i));
		}
		gotmyip=getnmyip();
		for (i=0;i<gotmyip;i++) {
			logger("myip(s) '%s'\n",ip2str(getmyip(i)));
			con32_printf(acon,"myip(s) '%s'\n",ip2str(getmyip(i)));
		}
		if (gotmyip) {
			logger("gotmyip, make master socket at frame %d\n\n",frame);
			ms=tcpmakemaster(0,sport); // listen on all ips
			if (ms) {
				nus=0;
				con32_printf(acon,"listen on port %d\n",sport);
				if (usearch[nus].name==0) {
					msmode=MS_READY;
				} else {
					msmode=MS_WAITUS;
					U32 rt=getinfofromname(usearch[nus].name); // initiate getinfo
					if (!rt)
						errorexit("can't get info from name");
				}
			} else
				con32_printf(acon,"createmastersocket on port %d FAILED!\n",sport);
		}
		break;
	case MS_WAITUS:
		if (getinfoip(0)) {
			usearch[nus].ip=getinfoip(0);
			con32_printf(acon,"ip for '%s' is '%s'\n",usearch[nus].name,ip2str(usearch[nus].ip));
			logger("ip for '%s' is '%s'\n\n",usearch[nus].name,ip2str(usearch[nus].ip));
			++nus;
			if (usearch[nus].name==0) {
				msmode=MS_READY;
				con32_printf(acon,"ready!\n");
			} else {
				U32 rt=getinfofromname(usearch[nus].name); // initiate getinfo
				if (!rt)
					errorexit("can't get info from name");
			}
		}
		break;
	case MS_READY:
// accept connections
		{
			const time_t ti=time(0);
			tm* newtime=gmtime(&ti);
/*
#ifdef MINUTELOG
			if (!minutelog || newtime->tm_min!=curtime.tm_min) {
				if (minutelog) {
					fprintf(minutelog,"close minutelog at %s",asctime(newtime));
					fclose(minutelog);
					minutelog=0;
				}
				C8 fname[300];
				sprintf(fname,"%04d_%02d_%02d_%02d_%02d.log",
					newtime->tm_year+1900,newtime->tm_mon+1,newtime->tm_mday,newtime->tm_hour,newtime->tm_min);
				S32 fe=fileexist(fname);
				minutelog=fopen2(fname,"a");
				if (fe)
					fprintf(minutelog,"append minutelog at %s",asctime(newtime));
				else
					fprintf(minutelog,"open minutelog at %s",asctime(newtime));
				con32_printf(acon,"minutelog %s",asctime(newtime));
			}
#endif
#ifdef HOURLOG
			if (!hourlylog || newtime->tm_hour!=curtime.tm_hour) {
				if (hourlylog) {
					fprintf(hourlylog,"close hourlylog at %s",asctime(newtime));
					fclose(hourlylog);
					hourlylog=0;
				}
				C8 fname[300];
				sprintf(fname,"%04d_%02d_%02d_%02d.log",
					newtime->tm_year+1900,newtime->tm_mon+1,newtime->tm_mday,newtime->tm_hour);
				S32 fe=fileexist(fname);
				hourlylog=fopen2(fname,"a");
				if (fe)
					fprintf(hourlylog,"append hourlylog at %s",asctime(newtime));
				else
					fprintf(hourlylog,"open hourlylog at %s",asctime(newtime));
				con32_printf(acon,"hourlylog %s",asctime(newtime));
			}
#endif
#ifdef DAYLOG
			if (!dailylog || newtime->tm_mday!=curtime.tm_mday) {
				if (dailylog) {
					fprintf(dailylog,"close dailylog at %s",asctime(newtime));
					fclose(dailylog);
					dailylog=0;
				}
				C8 fname[300];
				sprintf(fname,"%04d_%02d_%02d.log",
					newtime->tm_year+1900,newtime->tm_mon+1,newtime->tm_mday);
				S32 fe=fileexist(fname);
				dailylog=fopen2(fname,"a");
				if (fe)
					fprintf(dailylog,"append dailylog at %s",asctime(newtime));
				else
					fprintf(dailylog,"open dailylog at %s",asctime(newtime));
				con32_printf(acon,"dailylog %s",asctime(newtime));
			}
#endif
			curtime=*newtime;
*/
			U32 ch=tl->getchange();
			if (ch&MINUTENEW)
				con32_printf(acon,"minutelog %s",asctime(newtime));
			if (ch&HOURNEW)
				con32_printf(acon,"hourlog %s",asctime(newtime));
			if (ch&DAYNEW)
				con32_printf(acon,"daylog %s",asctime(newtime));
			while (isfreeservice()) {
				socker* cl=tcpgetaccept(ms);
				if (cl) {
//					logger("frame %d, got an accept\n",frame);
					con32_printf(acon,"==========================================================\n");
					con32_printf(acon,"accept '%s':%d\n",ip2str(cl->remoteip),cl->remoteport);
//					getinfofromip(cl->remoteip);
					addservice(cl);
				} else
					break;
			}
			procservice();
			garbservice();
		}
		break;
	}
}

void searcher_draw2d()
{
	clipclear32(B32,C32(0,0,255));
	outtextxyf32(B32,10,10,C32WHITE,"frame %6d, done %d, messes used %d",frame,ndone,messesused());
	bitmap32* cbm=con32_getbitmap32(acon);
	clipblit32(cbm,B32,0,0,20,20,cbm->size.x,cbm->size.y);
	S32 i,j=0;
	for (i=0;i<MAXSERVE;++i) {
		serv* s=service[i];
		if (s) {
			if (s->wtpos)
				outtextxyf32(B32,10+100 * j++,760,C32WHITE,"%d/%d",s->wtpos,s->htmlbuffsize);
			else if (s->rdpos2)
				outtextxyf32(B32,10+100 * j++,760,C32RED,"%d",s->rdpos2);
			else if (s->wtpos2)
				outtextxyf32(B32,10+100 * j++,760,C32YELLOW,"%d",s->wtpos2,s->getbuff2len);
			else if (s->rdpos)
				outtextxyf32(B32,10+100 * j++,760,C32LIGHTRED,"%d",s->rdpos);
		}
	}
	outtextxyf32(B32,10,750,C32WHITE,"simultaneous connections %d",nservice);
	outtextxyf32(B32,300,750,C32WHITE,"Filter: %s",filtertext[filtermode]);
	rl->draw();
	frame++;
}

void searcher_exit()
{
	threadexit();
	if (ms)
		freesocker(ms);
	ms=0;
	killservice();
	uninitsocker();
	con32_free(acon);
	delete rl;
	rl=0;
//	const time_t ti=time(0);
//	tm* newtime=gmtime(&ti);
/*	if (minutelog) {
		fprintf(minutelog,"close minuteslog at %s",asctime(newtime));
		fclose(minutelog);
		minutelog=0;
	}
	if (hourlylog) {
		fprintf(hourlylog,"close hourlylog at %s",asctime(newtime));
		fclose(hourlylog);
		hourlylog=0;
	}
	if (dailylog) {
		fprintf(dailylog,"close dailylog at %s",asctime(newtime));
		fclose(dailylog);
		dailylog=0;
	} */
	delete tl;
	delete includelist;
	delete excludelist;
	FILE* fw=fopen2("filtermode.txt","w");
	if (fw) {
		fprintf(fw,"%d",filtermode);
		fclose(fw);
	}
	popdir();
	delete redirstrs;
}
