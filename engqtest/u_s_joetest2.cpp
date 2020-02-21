#include <m_eng.h>
#include <l_misclibm.h>

namespace {
shape* rl;
shape* focus,*oldfocus;
pbut* pbutquit,*pbutcuruserp,*pbutcuruserm;
pbut* pbutlogon,*pbutload,*pbutsave;
edit* editusername,*editpassword,*editloadsave;
text* textname,*textmyip,*textmyname,*textstatus;
con32* con;
U32 theip;
S32 cnt;
S32 curuser;

S32 gotmyip,gotserverip,doget;
socker* cly;
#define USERBUFFSIZE 2000
C8 userbuff[USERBUFFSIZE];

S32 yport=80;

string httpstr2;
S32 httpstr2len;

S32 wtpos,rdpos;
enum bus {BUSY_NONE,BUSY_NAME2IP,BUSY_GET};
bus busy;

C8* tags1[]={"result","gs","class",0};
C8* tags2[]={"result","gs","class","gamedata",0};
C8** checktags;
S32 checktagssize;

// Joe's stuff
#define MAXREADWRITE 40000
#define MAXHTTPBUFF 90000
C8 hexa[MAXREADWRITE*2+1];		// *** DO NOT ENLARGE THIS ARRAY ***
C8 buff[MAXREADWRITE+1];
const C8 *hexdigits = "0123456789abcdef";
C8 master_http_buf[MAXHTTPBUFF];
S32 retvals = 0;
C8 **tags = 0;
C8 **vals = 0;
/* turn buf into a text string of hex chars (2 per byte) and store in hex[] */
void hexify(const U8 *buf, S32 len)
{
	if (len>MAXREADWRITE)
		errorexit("hexify %d too big should be %d or less",len,MAXREADWRITE);
	S32 i;
	for (i = 0; i < len; ++i) {
		hexa[(i * 2) + 0] = hexdigits[(buf[i] & 0xf0) >> 4];
		hexa[(i * 2) + 1] = hexdigits[(buf[i] & 0x0f) >> 0];
	}
	hexa[len * 2] = 0;
}

U8 nibble(const C8 c)
{
	U8 b = 0;

	if((c >= '0') && (c <= '9'))
		b = c - '0';
	else if ((c >= 'A') && (c <= 'F'))
		b = c - 'A';
	else if ((c >= 'a') && (c <= 'f'))
		b = (c - 'a') + 10;
	return b;
}

/* break the response into tag/value pairs */
void cleanup_response()
{
	if (tags) {
		delete[] tags;
		tags = 0;
	}
	if (vals) {
		delete[] vals;
		vals = 0;
	}
}

void bustup_response()
{
	C8 *b;
// count # of newlines
	retvals = 0;
	b = master_http_buf;
	for( ; *b; b++)
	{
		if(*b == '\n')
			retvals++;
	}
// create array of char *  for tags and vals
	cleanup_response();
	tags = new char*[retvals];
	vals = new char*[retvals];
// stuff tag ptrs into place
	b = master_http_buf;
	for(S32 t = 0; t < retvals; t++)
	{
		tags[t] = b;
		b = strchr(b, '\n');
		if(!b)
			break;
		*b = 0;
		b++;
	}
// stuff val ptrs into place
	for(S32 i = 0; i < retvals; i++)
	{
		con32_printf(con,"tag '%2d' == '%s\n",i,tags[i]);
		C8 *v = strchr(tags[i], ' ');
		if(!v)
		{
			vals[i] = "";
		}
		else
		{
			*v = 0;
			vals[i] = v + 1;
		}
	}
}

// Fetches a response value from the last transaction given its tag
const char *master_retval(const char *tag, int n = 1)
{
	int o = 0;
	for(int i = 0; i < retvals; i++)
	{
		if(strcmp(tags[i], tag) == 0)
		{
			o++;
			if(o >= n)
				return vals[i];
		}
	}
	return 0;
}

// ignores instead of returning 0 when encountering non numeric chars
S32 sloppyatoi(const C8* s)
{
	C8 c;
	S32 ret=0;
	while((c=*s++)!=0) {
		if (c>='0' && c<='9') {
			ret*=10;
			ret+=c-'0';
		}
	}
	return ret;
}

}

// end Joe's stuff

void joetest2init()
{
	con=con32_alloc(760,360,C32WHITE,C32BLACK);
	initsocker();
	video_setupwindow(800,600);
	factory2<shape> fact;
	pushandsetdir("joetest");
	script sc("joetestres.txt");
	popdir();
	rl=fact.newclass_from_handle(sc);
	pbutquit=rl->find<pbut>("PBUTQUIT");
	pbutcuruserp=rl->find<pbut>("PBUTCURUSERP");
	pbutcuruserm=rl->find<pbut>("PBUTCURUSERM");
	pbutlogon=rl->find<pbut>("PBUTLOGON");
	pbutload=rl->find<pbut>("PBUTLOAD");
	pbutsave=rl->find<pbut>("PBUTSAVE");
	textname=rl->find<text>("TEXTNAME");
	editusername=rl->find<edit>("EDITUSERNAME");
	editpassword=rl->find<edit>("EDITPASSWORD");
	editloadsave=rl->find<edit>("EDITLOADSAVE");
	textmyip=rl->find<text>("TEXTMYIP");
	textmyname=rl->find<text>("TEXTMYNAME");
	textstatus=rl->find<text>("TEXTSTATUS");
	focus=oldfocus=0;
	theip=0;
	cnt=0;
	busy=BUSY_NONE;
	gotmyip=gotserverip=wtpos=rdpos=doget=0;
	curuser=1;
}

void joetest2proc()
{
// bail
	if (KEY==K_ESCAPE)
		popstate();
// handle buttons and editboxes
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
			popstate();
	} else if (focus == pbutcuruserp) {
		if (ret==1) {
			++curuser;
			curuser=range(1,curuser,99);
			char str[100];
			sprintf(str,"tester%02d",curuser);
			editusername->settname(str);
			editpassword->settname(str);
		}
	} else if (focus == pbutcuruserm) {
		if (ret==1) {
			--curuser;
			curuser=range(1,curuser,99);
			char str[100];
			sprintf(str,"tester%02d",curuser);
			editusername->settname(str);
			editpassword->settname(str);
		}
	} else if (focus == pbutlogon) {
		if (ret==1) {
			textstatus->settname("logging on");
			httpstr2 = "GET ";
			httpstr2 += "/auth.php?action=auth&username=";
			httpstr2 += editusername->gettname();
			httpstr2 += "&password=";
			httpstr2 += editpassword->gettname();
			httpstr2 += " HTTP/1.0\r\nHost: ";
			httpstr2 += textname->gettname();
			httpstr2 += "\r\n\r\n";
			httpstr2len = httpstr2.length();
			doget=1;
			checktags=tags1;
		}
	} else if (focus == pbutload) {
		if (ret==1) {
			textstatus->settname("loading data");
			httpstr2 = "GET ";
			httpstr2 += "/auth.php?action=gameload&username=";
			httpstr2 += editusername->gettname();
			httpstr2 += "&password=";
			httpstr2 += editpassword->gettname();
			httpstr2 += " HTTP/1.0\r\nHost: ";
			httpstr2 += textname->gettname();
			httpstr2 += "\r\n\r\n";
			httpstr2len = httpstr2.length();
			doget=1;
			checktags=tags2;
		}
	} else if (focus == pbutsave) {
		if (ret==1) {
			const char *bndry = "---------------------------XXXXXXXXXXXXX";
			textstatus->settname("saving data");
			char conthdr[300];
			conthdr[0] = 0;
			strcat(conthdr, "Content-Disposition: form-data; name=\"");
			strcat(conthdr, "userfile");
			strcat(conthdr, "\"; filename=\"");
			strcat(conthdr, "gamedata");
			strcat(conthdr, "\"\r\n");
			strcat(conthdr, "Content-Type: ");
			strcat(conthdr, "text/hexed");
			strcat(conthdr, "\r\n");
			strcat(conthdr, "\r\n");
			httpstr2 = "POST ";
			httpstr2 += "/auth.php?action=gamesave2&username=";
			httpstr2 += editusername->gettname();
			httpstr2 += "&password=";
			httpstr2 += editpassword->gettname();
			const C8* data=editloadsave->gettname();
			S32 datalen=strlen(data);
			hexify((U8*)data,datalen);
			int postlen = strlen(hexa);
//			httpstr2 += hexa;
			httpstr2 += " HTTP/1.0\r\nHost: ";
			httpstr2 += textname->gettname();
			httpstr2 += "\r\n";

			httpstr2 +="Content-Type: multipart/form-data; boundary=";
			httpstr2 += bndry;
			httpstr2 += "\r\n";
			int contlen = 0;
			contlen += strlen(bndry)+4;	// "--" + the bndry string + CR/LF
			contlen += strlen(conthdr);
			contlen += postlen + 2;	// the actual data + CR/LF
			contlen += strlen(bndry)+6;	// "--" + the bndry string + "--" + CR/LF
			httpstr2 +="Content-Length: ";
			char contlenstr[20];
			my_itoa(contlen,contlenstr,10);
			httpstr2 += contlenstr;
			httpstr2 += "\r\n";

			httpstr2 += "\r\n";

			// indicate start of data
			httpstr2 += "--";
			httpstr2 += bndry;
			httpstr2 += "\r\n";

			httpstr2 += conthdr;

			httpstr2 += hexa;
			httpstr2 += "\r\n";

			// indicate end of data
			httpstr2 += "--";
			httpstr2 += bndry;
			httpstr2 += "--";
			httpstr2 += "\r\n";



			httpstr2len = httpstr2.length();
			FILE* fw=fopen("post.txt","wb");
			fwrite(httpstr2.c_str(),1,httpstr2len,fw);
			fclose(fw);
			doget=1;
			checktags=tags1;
		}
/*	} else if (focus == pbutsave) {
		if (ret==1) {
			textstatus->settname("saving data");
			httpstr2 = "GET ";
			httpstr2 += "/auth.php?action=gamesave&username=";
			httpstr2 += editusername->gettname();
			httpstr2 += "&password=";
			httpstr2 += editpassword->gettname();
			httpstr2 += "&gamedata=";
			const C8* data=editloadsave->gettname();
			S32 datalen=strlen(data);
			hexify((U8*)data,datalen);
			httpstr2 += hexa;
			httpstr2 += " HTTP/1.0\r\nHost: ";
			httpstr2 += textname->gettname();
			httpstr2 += "\r\n\r\n";
			httpstr2len = httpstr2.length();
			doget=1;
			checktags=tags1;
		} */
	}
// done handle buttons and editboxes
// wait for my ip
	if (!gotmyip) {
		gotmyip=getnmyname();
		S32 i;
		for (i=0;i<gotmyip;i++)
			logger("mynames(s) '%s'\n",getmyname(i));
		gotmyip=getnmyip();
		for (i=0;i<gotmyip;i++)
			logger("myip(s) '%s'\n",ip2str(getmyip(i)));
		if (gotmyip) {
			textmyip->settname(ip2str(getmyip(0)));
			textmyname->settname(getmyname(0));
			textstatus->settname("ready");
			logger("got my name on frame %d\n",cnt);
		}
	}
	if (busy==BUSY_NONE && gotmyip && !gotserverip) {
		U32 rt=getinfofromname(textname->gettname()); // initiate getinfo
		if (rt) {
			busy=BUSY_NAME2IP;
			textstatus->settname("name to ip");
		}
	}
// do a get
	if (doget) {
		if (cly) {
			freesocker(cly);
			cly=0;
		}
		cly=tcpgetconnect(theip,yport);
		wtpos=0;
		if (!cly)
			errorexit("can't get connect");
		textstatus->settname("getting");
		busy=BUSY_GET;
		con32_printf(con,"============= GET =======================\n");
		con32_printf(con,"ip '%s'\n",ip2str(theip));
		con32_printf(con,"%s\n",httpstr2.c_str());
		con32_printf(con,"============= REPLY =======================\n");
		doget=0;
	}
// wait for name2ip
	if (busy==BUSY_NAME2IP) {
		if (getninfoip()) {
			theip=getinfoip(0);
			busy=BUSY_NONE;
			textstatus->settname("ready");
			gotserverip=1;
			con32_printf(con,"== READY ==\n");
		}
	} else if (busy==BUSY_GET) {
// wait/process HTTP get and HTML reply
// wait for send
		if (cly && (cly->flags & SF_CONNECTED) && wtpos!=httpstr2len) {
			S32 len;
			len=tcpputbuffdata(cly,httpstr2.c_str(),httpstr2len,httpstr2len);
			logger("write data %d/%d\n",wtpos,httpstr2len);
			logger("%s\n",httpstr2.c_str());
			if (len || (cly->flags & SF_CLOSED)) {
				wtpos=httpstr2len;
				rdpos=0;
			}
		}
// wait for reply
		if (cly && tcpsendwritebuff(cly)) {
			if (wtpos==httpstr2len) {
				S32 len;
				tcpfillreadbuff(cly);
				len=tcpgetbuffdata(cly,userbuff,1,USERBUFFSIZE);
				logger("\nread data %d, len %d\n",rdpos,len);
				if (len) {
					if (len+rdpos>=MAXHTTPBUFF) // leave room for the '\0'
						errorexit("the server is killing me with too much data");
					con32_printf(con,"read data to %d, len %d\n",rdpos,len);
					memcpy(master_http_buf+rdpos,userbuff,len);
					rdpos+=len;
				}
				if (cly && !len && (cly->flags&SF_CLOSED)) {
// all done
					busy=BUSY_NONE;
					textstatus->settname("ready");
					if (cly)
						freesocker(cly);
					cly=0;
					master_http_buf[rdpos]='\0';
//					con32_printf(con,"%s\n",master_http_buf); // dangerous
					logger("%s\n",master_http_buf);
					logger("\ntotal data read %d\n",rdpos);
					con32_printf(con,"\ntotal data read %d\n",rdpos);
					if (checktags) {
						bustup_response();
						S32 i=0;
						while(checktags[i]) {
							const C8* tag=checktags[i];
							const C8* val=master_retval(tag);
							if (val) {
								con32_printf(con,"tag '%s', val '%s'\n",checktags[i],val);
								if (!strcmp(tag,"result")) {
									if (!strcmp(val,"ok")) {
										con32_printf(con,"action a success\n");
									} else {
										con32_printf(con,"action a failure\n");
									}
								} else if (!strcmp(tag,"class")) {
										con32_printf(con,"got class %d\n",atoi(val));
								} else if (!strcmp(tag,"gs")) {
									const C8* t=strchr(val,':');
									if (t)
										t=strchr(t+1,':');
									if (t) {
										con32_printf(con,"got testgroup %d\n",sloppyatoi(t));
									} else {
										con32_printf(con,"can't find testgroup in '%s'\n",val);
									}
								} else if (!strcmp(tag,"gamedata")) {
									con32_printf(con,"gamedata = '%s'\n",val);
// unhexify
									S32 len = strlen(val) / 2;
									for(S32 i = 0; i < len; ++i) {
										U8 hn = nibble(val[(i * 2) + 0]);
										U8 ln = nibble(val[(i * 2) + 1]);
										buff[i] = (hn << 4) + ln;
									}
									buff[len]='\0';
//									con32_printf(con,"%s\n",buff); // dangerous buffer overflow 40k buff max
									editloadsave->settname(buff);
								}
							}
							++i;
						}
						cleanup_response();
					}
					con32_printf(con,"== READY ==\n");
					checktags=0;
				}
			}
		}
	}
	++cnt;
}

void joetest2draw2d()
{
//	video_lock();
	clipclear32(B32,C32BLUE);
	rl->draw();
	clipblit32(con32_getbitmap32(con),B32,0,0,20,190,WX,WY);
//	video_unlock();
}

void joetest2exit()
{
	delete rl;
	if (cly) {
		freesocker(cly);
		cly=0;
	}
	uninitsocker();
	con32_free(con);
}
