// main menu
#include <math.h>
#include <string.h>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <engine1.h>
#include <misclib.h>

#include "mainres.h"
#include "globalres.h"


static struct reslist *rl;
//static unsigned char statenums[256];
static int mainidx;
extern char *videomainstrs[];
extern int maxvideomaindrivers;
static int vmd,vsd,imd,amd,asd,amsd,gamexy=-1;
static int defaultgamexy;
static struct wavehandle *twh;

#define MAXXYRES 50
static struct pointi2 xyres[MAXXYRES]={
	40,30,
	64,48,
	80,60,
	128,96,
	144,108,
	160,120,
	200,150,
	256,192,
	320,240,
	400,300,
	512,384,
	640,200,
	640,480,
	800,600,
	1024,768,
	1152,864,
	1280,1024,
	1600,1200,
};
static int nxyres=18;

// meant to be called from outside the mainmenu (plus and minus key)
int changexyres(int oldx,int oldy,int direction,int *newx,int *newy) {
	int gxy;
	if (direction!=-1 && direction!=1)
		return 0;
	if (nxyres<2)
		return 0;
	for (gxy=0;gxy<nxyres;gxy++)
		if (xyres[gxy].x==oldx && xyres[gxy].y==oldy)
			break;
	if (gxy==nxyres)
		return 0;
	gxy+=direction;
	if (gxy>=nxyres)
		return 0;
	if (gxy<0)
		return 0;
	gamexy=gxy;
	*newx=xyres[gamexy].x;
	*newy=xyres[gamexy].y;
	return 1;
}

void mainmenu_init()
{
//	void slecstdisable();
	FILE *fp;
	char str[10];
	char **sc;
	int nsc;
	int i,j;
	logger("main menu init\n");
//	video_setupwindow(800,600,565);
	pushandsetdir("mainmenu");
	rl=loadres("mainres.txt");
	sc=loadscript("mainmenu.txt",&nsc);
	nxyres=0;
	for (i=0,j=0;i<nsc;j++) {
		if (!strcmp(sc[i],"statedesc")) {
			addlistboxname(rl,LISTBOX1,-1,sc[i+2],SCL1);
			j=atoi(sc[i+1]);
			sprintf(str,"%4d",j);
			addlistboxname(rl,LISTBOX8,-1,str,-1);
			i+=3;
		} else if (!strcmp(sc[i],"gamexy")) {
			addlistboxname(rl,LISTXY,-1,sc[i+1],SCLXY);
			if (nxyres<MAXXYRES) {
				sscanf(sc[i+1],"%d %d",&xyres[nxyres].x,&xyres[nxyres].y);
				nxyres++;
			}
			i+=2;
		} else if (!strcmp(sc[i],"defaultgamexy")) {
			defaultgamexy=atoi(sc[i+1]);
			i+=2;
		} else
			errorexit("unknown main menu command '%s'",sc[i]);
	}
	freescript(sc,nsc);
//	sortlistbox(rl,LISTBOX1,LISTBOX8,-1,-1,0);
	mainidx=-1;
	if (fileexist("maincfg.txt")) {
//		slecstdisable();
		sc=loadscript("maincfg.txt",&nsc);
		for (i=0,j=0;i<nsc;i+=2,j++) {
			if (!strcmp(sc[i],"mainidx"))
				mainidx=atoi(sc[i+1]);
			else if (!strcmp(sc[i],"videomaindriver"))
				vmd=atoi(sc[i+1]);
			else if (!strcmp(sc[i],"videosubdriver"))
				vsd=atoi(sc[i+1]);
			else if (!strcmp(sc[i],"inputmaindriver"))
				imd=atoi(sc[i+1]);
			else if (!strcmp(sc[i],"audiomaindriver"))
				amd=atoi(sc[i+1]);
			else if (!strcmp(sc[i],"audiosubdriver"))
				asd=atoi(sc[i+1]);
			else if (!strcmp(sc[i],"audiomicsubdriver"))
				amsd=atoi(sc[i+1]);
			else if (!strcmp(sc[i],"mousemode"))
				mousemode=atoi(sc[i+1]);
			else if (!strcmp(sc[i],"gamexy")) {
				if (gamexy==-1)
					gamexy=atoi(sc[i+1]);
			} else
				errorexit("unknown server cfg command '%s'",sc[i]);
		}
		freescript(sc,nsc);
	}
	fp=fopen2("maincfg.txt","w");
	if (fp)
		fclose(fp);
	if (gamexy==-1)
		gamexy=defaultgamexy;
//	setresvis(rl,PBUT1,0);
//	setresvis(rl,PBUT2,0);
	if (mainidx<0)
		setresvis(rl,PBUT1,0);
	else {
		setcurlistbox(rl,LISTBOX1,mainidx,SCL1);
		setcurlistbox(rl,LISTBOX8,mainidx,-1);
		setlistboxoffset(rl,LISTBOX8,getlistboxoffset(rl,LISTBOX1));
	}
	settextfocus(rl,SCL1);
	for (i=0;i<maxvideomaindrivers;i++)
		addlistboxname(rl,LISTBOX2,-1,videomainstrs[i],-1);
	sc=video_getnsubdrivers(vmd,&nsc);
	for (i=0;i<nsc;i++)
		addlistboxname(rl,LISTBOX3,-1,sc[i],-1);
	freescript(sc,nsc);
	setcurlistbox(rl,LISTBOX2,vmd,-1);
	setcurlistbox(rl,LISTBOX3,vsd,-1);
	video_init(vmd,vsd);
	video_setupwindow(800,600,565);

//	addlistboxname(rl,LISTBOX2,-1,"ddraw",-1);
//	addlistboxname(rl,LISTBOX2,-1,"d3d",-1);
	addlistboxname(rl,LISTBOX4,-1,"wave player",-1);
	addlistboxname(rl,LISTBOX4,-1,"dsound",-1);
	sc=audio_getnsubdrivers(amd,&nsc);
	for (i=0;i<nsc;i++)
		addlistboxname(rl,LISTBOX5,-1,sc[i],-1);
	freescript(sc,nsc);
	sc=mic_getnsubdrivers(amd,&nsc);
	for (i=0;i<nsc;i++)
		addlistboxname(rl,LISTMIC,-1,sc[i],-1);
	freescript(sc,nsc);
	setcurlistbox(rl,LISTBOX4,amd,-1);
	setcurlistbox(rl,LISTBOX5,asd,-1);
	setcurlistbox(rl,LISTMIC,amsd,-1);
	audio_init(amd,asd,amsd);
	twh=wave_load("synflute.wav",0);
	addlistboxname(rl,LISTBOX6,-1,"message based",-1);
	addlistboxname(rl,LISTBOX6,-1,"dinput",-1);
	addlistboxname(rl,LISTBOX7,-1,"normal",-1);
	addlistboxname(rl,LISTBOX7,-1,"infinite",-1);

	setcurlistbox(rl,LISTBOX7,mousemode,-1);
	setcurlistbox(rl,LISTBOX6,imd,-1);
	setcurlistbox(rl,LISTXY,gamexy,SCLXY);
	input_init(imd);
	popdir();
}
	
void mainmenu_proc()
{
	int x,y;
	int nsc;
	char **sc;
	int i;
	struct rmessage rm;
	if (KEY==K_ESCAPE)
		popstate();
//	if (KEY==K_RETURN) {
//		i=atoi(findlistboxname(rl,LISTBOX8,mainidx));
//		if (i>=0)
//			pushandchangestatenum(i);
//	}
	checkres(rl);
	while(getresmess(rl,&rm)) {
		switch(rm.id) {
		case PBUT1:
			pushandchangestatenum(atoi(findlistboxname(rl,LISTBOX8,mainidx)));
			break;
		case PBUT2:
			justchangestate(NULL);
			break;
		case LISTBOX1:
			setresvis(rl,PBUT1,1);
			mainidx=rm.val;
			setcurlistbox(rl,LISTBOX8,mainidx,-1);
			break;
		case LISTBOX8:
			setresvis(rl,PBUT1,1);
			mainidx=rm.val;
			setcurlistbox(rl,LISTBOX1,mainidx,-1);
			break;
		case SCL1:
			setlistboxoffset(rl,LISTBOX1,rm.val);
			setlistboxoffset(rl,LISTBOX8,rm.val);
			break;
		case LISTBOX2:
			clearlistbox(rl,LISTBOX3,-1);
			sc=video_getnsubdrivers(rm.val,&nsc);
			for (i=0;i<nsc;i++)
				addlistboxname(rl,LISTBOX3,-1,sc[i],-1);
			freescript(sc,nsc);
			setcurlistbox(rl,LISTBOX3,0,-1);
			break;
		case LISTBOX4:
			clearlistbox(rl,LISTBOX5,-1);
			clearlistbox(rl,LISTMIC,-1);
			sc=audio_getnsubdrivers(rm.val,&nsc);
			for (i=0;i<nsc;i++)
				addlistboxname(rl,LISTBOX5,-1,sc[i],-1);
			freescript(sc,nsc);
			setcurlistbox(rl,LISTBOX5,0,-1);
			sc=mic_getnsubdrivers(rm.val,&nsc);
			for (i=0;i<nsc;i++)
				addlistboxname(rl,LISTMIC,-1,sc[i],-1);
			freescript(sc,nsc);
			setcurlistbox(rl,LISTMIC,0,-1);
			break;
		case PBUT3:
			vmd=getcurlistbox(rl,LISTBOX2);
			vsd=getcurlistbox(rl,LISTBOX3);
			if (vmd>=0 && vsd>=0) {
				video_init(vmd,vsd);
				video_setupwindow(xyres[gamexy].x,xyres[gamexy].y,565);
				video_setupwindow(800,600,565);
			}
			break;
		case PBUT4:
			amd=getcurlistbox(rl,LISTBOX4);
			asd=getcurlistbox(rl,LISTBOX5);
			amsd=getcurlistbox(rl,LISTMIC);
			if (amd>=0 && asd>=0 && amsd>=0) {
				wave_unload(twh);
				audio_init(amd,asd,amsd);
				video_setupwindow(800,600,565);
				pushandsetdir("mainmenu");
				twh=wave_load("synflute.wav",0);
				popdir();
			}
			break;
		case PBUT5:
			playasound(twh);
			break;
		case PBUT6:
			imd=getcurlistbox(rl,LISTBOX6);
			input_init(imd);
			imd=usedirectinput;
			break;
		case LISTBOX7:
			mousemode=rm.val;
			break;
		case SCLXY:
			setlistboxoffset(rl,LISTXY,rm.val);
			break;
		case LISTXY:
			x=MX;
			y=MY;
			gamexy=rm.val;
			video_setupwindow(xyres[gamexy].x,xyres[gamexy].y,565);
			video_setupwindow(800,600,565);
			setmousexy(x,y);
			break;
		}
	}

	video_lock();
	drawres(rl);
	outtextxyf16(B16,WX/2-5*4,10,hiwhite,"main menu");
//	outtextxyf16(B16,10,580,hiwhite,"lobby server, numclients %d",numcl);
//	for (i=0;i<numcl;i++)
//		outtextxyf16(B16,360,8+(i<<3),hiwhite,clients[i].name);
//	clipblit16(constat->b16,B16,0,0,20,20,constat->b16->x,constat->b16->y);
	video_unlock();
}

void mainmenu_exit()
{
	FILE *fp;
	globalxres=xyres[gamexy].x;
	globalyres=xyres[gamexy].y;
	pushandsetdir("mainmenu");
	fp=fopen2("maincfg.txt","w");
	fprintf(fp,"mainidx %d\n",getcurlistbox(rl,LISTBOX1));
	fprintf(fp,"videomaindriver %d\n",vmd);
	fprintf(fp,"videosubdriver %d\n",vsd);
	fprintf(fp,"audiomaindriver %d\n",amd);
	fprintf(fp,"audiosubdriver %d\n",asd);
	fprintf(fp,"audiomicsubdriver %d\n",amsd);
	fprintf(fp,"inputmaindriver %d\n",imd);
	fprintf(fp,"mousemode %d\n",mousemode);
	fprintf(fp,"gamexy %d\n",gamexy);
	fclose(fp); 
	popdir();
	freeres(rl);
	mousemode=0;
	wave_unload(twh);
}

