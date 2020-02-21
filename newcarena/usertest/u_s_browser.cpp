#include <m_eng.h>
#include <l_misclibm.h>
#include "system/u_states.h"

namespace {
shape* rl;
shape* focus,*oldfocus;
pbut* pbutquit;
pbut* pbutp24,*pbutm24,*pbutp16,*pbutm16,*pbutp8,*pbutm8,*pbutp0,*pbutm0;
pbut* pbutp28,*pbutm28,*pbutp20,*pbutm20,*pbutp12,*pbutm12,*pbutp4,*pbutm4;
pbut* pbutget,*pbutabort,*pbutname2ip,*pbutip2name;
edit* editip,*editname,*editfile;
text* textmyip,*textmyname,*textstatus;
listbox* liststatus;
//con32* con;
U32 theip;
S32 cnt;

S32 gotmyip;
socker* cly;
#define USERBUFFSIZE 10240
C8 userbuff[USERBUFFSIZE];
const S32 DUMPBUFFSIZE=10000000;
C8* dumpbuff;
S32 dumpnum;

S32 yport=80;

//C8 httpstr[]="GET / HTTP/1.0\r\n\r\n";
//S32 httpstrlen;
string httpstr2;
S32 httpstr2len;

S32 wtpos,rdpos;
enum bus {BUSY_NONE,BUSY_NAME2IP,BUSY_IP2NAME,BUSY_GET};
bus busy;
}

void browserinit()
{
//	con=con32_alloc(760,360,C32WHITE,C32BLACK);
	initsocker();
	video_setupwindow(800,600);
	factory2<shape> fact;
	pushandsetdir("browser");
	script sc("browserres.txt");
	rl=fact.newclass_from_handle(sc);
	pbutquit=rl->find<pbut>("PBUTQUIT");
	pbutp24=rl->find<pbut>("PBUTP24");
	pbutm24=rl->find<pbut>("PBUTM24");
	pbutp16=rl->find<pbut>("PBUTP16");
	pbutm16=rl->find<pbut>("PBUTM16");
	pbutp8=rl->find<pbut>("PBUTP8");
	pbutm8=rl->find<pbut>("PBUTM8");
	pbutp0=rl->find<pbut>("PBUTP0");
	pbutm0=rl->find<pbut>("PBUTM0");
	pbutp28=rl->find<pbut>("PBUTP28");
	pbutm28=rl->find<pbut>("PBUTM28");
	pbutp20=rl->find<pbut>("PBUTP20");
	pbutm20=rl->find<pbut>("PBUTM20");
	pbutp12=rl->find<pbut>("PBUTP12");
	pbutm12=rl->find<pbut>("PBUTM12");
	pbutp4=rl->find<pbut>("PBUTP4");
	pbutm4=rl->find<pbut>("PBUTM4");
	pbutget=rl->find<pbut>("PBUTGET");
	pbutabort=rl->find<pbut>("PBUTABORT");
	pbutip2name=rl->find<pbut>("PBUTIP2NAME");
	pbutname2ip=rl->find<pbut>("PBUTNAME2IP");
	editname=rl->find<edit>("EDITNAME");
	editfile=rl->find<edit>("EDITFILE");
	editip=rl->find<edit>("EDITIP");
	textmyip=rl->find<text>("TEXTMYIP");
	textmyname=rl->find<text>("TEXTMYNAME");
	textstatus=rl->find<text>("TEXTSTATUS");
	liststatus=rl->find<listbox>("LISTSTATUS");
	focus=oldfocus=0;
	theip=0;
	cnt=0;
	busy=BUSY_NONE;
	gotmyip=wtpos=rdpos=0;
//	httpstrlen=strlen(httpstr);
	dumpbuff=new C8[DUMPBUFFSIZE];
}

void browserproc()
{
// bail
	if (KEY==K_ESCAPE)
		poporchangestate(STATE_MAINMENU);
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
	} else if (focus == pbutp0) {
		if (ret==1) {
			++theip;
			editip->settname(ip2str(theip));
		}
	} else if (focus == pbutm0) {
		if (ret==1) {
			--theip;
			editip->settname(ip2str(theip));
		}
	} else if (focus == pbutp4) {
		if (ret==1) {
			theip+=0x10;
			editip->settname(ip2str(theip));
		}
	} else if (focus == pbutm4) {
		if (ret==1) {
			theip-=0x10;
			editip->settname(ip2str(theip));
		}
	} else if (focus == pbutp8) {
		if (ret==1) {
			theip+=0x100;
			editip->settname(ip2str(theip));
		}
	} else if (focus == pbutm8) {
		if (ret==1) {
			theip-=0x100;
			editip->settname(ip2str(theip));
		}
	} else if (focus == pbutp12) {
		if (ret==1) {
			theip+=0x1000;
			editip->settname(ip2str(theip));
		}
	} else if (focus == pbutm12) {
		if (ret==1) {
			theip-=0x1000;
			editip->settname(ip2str(theip));
		}
	} else if (focus == pbutp16) {
		if (ret==1) {
			theip+=0x10000;
			editip->settname(ip2str(theip));
		}
	} else if (focus == pbutm16) {
		if (ret==1) {
			theip-=0x10000;
			editip->settname(ip2str(theip));
		}
	} else if (focus == pbutp20) {
		if (ret==1) {
			theip+=0x100000;
			editip->settname(ip2str(theip));
		}
	} else if (focus == pbutm20) {
		if (ret==1) {
			theip-=0x100000;
			editip->settname(ip2str(theip));
		}
	} else if (focus == pbutp24) {
		if (ret==1) {
			theip+=0x1000000;
			editip->settname(ip2str(theip));
		}
	} else if (focus == pbutm24) {
		if (ret==1) {
			theip-=0x1000000;
			editip->settname(ip2str(theip));
		}
	} else if (focus == pbutp28) {
		if (ret==1) {
			theip+=0x10000000;
			editip->settname(ip2str(theip));
		}
	} else if (focus == pbutm28) {
		if (ret==1) {
			theip-=0x10000000;
			editip->settname(ip2str(theip));
		}
	} else if (focus == editip) {
		theip=str2ip(editip->gettname());
	} else if (focus == pbutget) {
		if (ret==1) {
			if (cly) {
				freesocker(cly);
				cly=0;
			}
			if (!cly) {
				cly=tcpgetconnect(theip,yport);
				wtpos=0;
				if (!cly)
					errorexit("can't get connect");
				textstatus->settname("getting");
				busy=BUSY_GET;
//				con32_printf(con,"=======================================\n");
				liststatus->printf("=======================================");
				httpstr2 = "GET ";
				httpstr2 += editfile->gettname();
				httpstr2 += " HTTP/1.0\r\nHost: ";
				httpstr2 += editname->gettname();
				httpstr2 += "\r\n\r\n";
				httpstr2len = httpstr2.length();
				logger("http send '%s'\n",httpstr2.c_str());
//C8 httpstr[]="GET / HTTP/1.0\r\n\r\n";
//S32 httpstrlen;
			}
		}
	} else if (focus == pbutname2ip) {
		if (ret==1) {
			U32 rt=getinfofromname(editname->gettname()); // initiate getinfo
			if (rt) {
				busy=BUSY_NAME2IP;
				textstatus->settname("name to ip");
			}
		}
	} else if (focus == pbutip2name) {
		if (ret==1) {
			U32 rt=getinfofromip(theip); // initiate getinfo
			if (rt) {
				busy=BUSY_IP2NAME;
				textstatus->settname("ip to name");
			}
		}
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
// wait for name2ip
	if (busy==BUSY_NAME2IP) {
		if (getninfoip()) {
			theip=getinfoip(0);
			editip->settname(ip2str(theip));
			busy=BUSY_NONE;
			textstatus->settname("ready");
		}
	}
// wait for ip2name
	else if (busy==BUSY_IP2NAME) {
		if (getninfoname()) {
			editname->settname(getinfoname(0));
			busy=BUSY_NONE;
			textstatus->settname("ready");
		}
	} else if (busy==BUSY_GET) {
// wait/process HTTP get and HTML reply
// wait for send
		if (cly && (cly->flags & SF_CONNECTED) && wtpos!=httpstr2len) {
			S32 len;
			len=tcpputbuffdata(cly,httpstr2.c_str(),httpstr2len,httpstr2len);
//			len=writedata(cly,httpstr+wtpos,httpstrlen-wtpos);
			logger("write data %d/%d\n",wtpos,httpstr2len);
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
				len=tcpgetbuffdata(cly,userbuff,1,2000);
				logger("\nread data %d, len %d\n",rdpos,len);
//				con32_printf(con,"read data %d, len %d\n",rdpos,len);
				liststatus->printf("read data %d, len %d",rdpos,len);
				if (len) {
					memcpy(dumpbuff+rdpos,userbuff,len);
					C8* str = new C8[len+1];
					memcpy(str,userbuff,len);
					str[len]='\0';
//					con32_printf(con,"%s\n",str);
					liststatus->printf("%s",str);
//					logger("%s",str);
					delete[] str;
//					fwrite(userbuff,len,1,fw);
					rdpos+=len;
				}
				if (cly && !len && (cly->flags&SF_CLOSED)) {
// all done
					busy=BUSY_NONE;
					textstatus->settname("ready");
					if (cly)
						freesocker(cly);
					cly=0;
					logger("\ntotal data read %d\n",rdpos);
					C8 str[50];
					sprintf(str,"dump%d.bin",dumpnum++);
					FILE* fw=fopen2(str,"wb");
					fwrite(dumpbuff,1,rdpos,fw);
					fclose(fw);
//					con32_printf(con,"\ntotal data read %d\n",rdpos);
					liststatus->printf("total data read %d\n",rdpos);
				}
			}
		}
	}
	++cnt;
}

void browserdraw2d()
{
//	video_lock();
	clipclear32(B32,C32BLUE);
	rl->draw();
//	clipblit32(con32_getbitmap32(con),B32,0,0,20,190,WX,WY);
//	video_unlock();
}

void browserexit()
{
	delete rl;
	if (cly) {
		freesocker(cly);
		cly=0;
	}
	uninitsocker();
//	con32_free(con);
	popdir();
	delete dumpbuff;
}
