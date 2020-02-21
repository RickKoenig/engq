#if 0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <engine1.h>
#include "res.h"
#include "data/connect4/connect4res.h"

static struct reslist *rl;

enum {PLAY_NO,PLAY_RED,PLAY_REDFALL,PLAY_BLUE,PLAY_BLUEFALL,PLAY_END,PLAY_ABORTED,PLAY_TIED};
enum {WIN_YES,WIN_TIE,WIN_NOTYET};
enum {AI_LOSE,AI_TIE,AI_WIN,AI_NUM};

static char comp_pred[10];

static int playing;
static int boardx,boardy;
static int depth;
static int connectnum;
static int playtype[2];
static int playfirst;
static int playcounter;
static int score[2];
static int fallpos;
static char winnerstr[50];

#define MAXSIZEX 7
#define MINSIZEX 2
#define MAXSIZEY 6
#define MINSIZEY 2
#define MAXNUMCONNECT 5
#define MINNUMCONNECT 1
#define MAXDEPTH 10
#define MINDEPTH 1

#define BOARDSTARTX 112
#define BOARDSTARTY 100
#define SQUARESIZE 75
#define CIRCLERAD 30

struct possmoves {
	int nummoves[AI_NUM];
	int movex[AI_NUM][MAXSIZEX];
	int movey[AI_NUM][MAXSIZEX];
};

static char theboard[MAXSIZEY][MAXSIZEX];

static void initgame()
{
	memset(theboard,0,sizeof(theboard));
	comp_pred[0]='\0';
}

static int checkboardwinner(int player)
{
	int i,j,k;
	player++;
	for (j=0;j<boardy;j++)
		for (i=0;i<boardx-connectnum+1;i++) {
			for (k=0;k<connectnum;k++)
				if (player!=theboard[j][i+k])
					break;
			if (k==connectnum)
				return WIN_YES;
		}
	for (j=0;j<boardy-connectnum+1;j++)
		for (i=0;i<boardx;i++) {
			for (k=0;k<connectnum;k++)
				if (player!=theboard[j+k][i])
					break;
			if (k==connectnum)
				return WIN_YES;
		}
	for (j=0;j<boardy-connectnum+1;j++)
		for (i=0;i<boardx-connectnum+1;i++) {
			for (k=0;k<connectnum;k++)
				if (player!=theboard[j+k][i+k])
					break;
			if (k==connectnum)
				return WIN_YES;
		}
	for (j=0;j<boardy-connectnum+1;j++)
		for (i=0;i<boardx-connectnum+1;i++) {
			for (k=0;k<connectnum;k++)
				if (player!=theboard[j+connectnum-1-k][i+k])
					break;
			if (k==connectnum)
				return WIN_YES;
		}
	for (j=0;j<boardy;j++)
		for (i=0;i<boardx;i++)
			if (theboard[j][i]==0)
				return WIN_NOTYET;
	return WIN_TIE;
}

static int checkpiece(int pos)
{
	int j;
	if (theboard[0][pos])
		return -1;
	for (j=boardy-1;j>0;j--)
		if (!theboard[j][pos])
			break;
	return j;
}

static void addpiece(int player,int pos,int down)
{
	theboard[down][pos]=player+1;
}

static void removepiece(int pos,int down)
{
	theboard[down][pos]=0;
}

static int checkaddpiece(int player,int pos)
{
	int j;
	if (theboard[0][pos])
		return -1;
	for (j=boardy-1;j>0;j--)
		if (!theboard[j][pos])
			break;
	theboard[j][pos]=player+1;
	return j;
}

static void aimove(int player,struct possmoves *pm)
{
	int i,j,w,m,winfound=0;
//	static int count;
	struct possmoves pm2;
//	count++;
//	if (count>20)
//		errorexit("count > 20");
//	if ((int)&pm2<0)
//		errorexit("pm2 < 0");
//	if ((int)pm<0)
//		errorexit("pm < 0");
//	for (j=0;j<AI_NUM;j++)
//		pm->nummoves[j]=0;
	pm->nummoves[0]=pm->nummoves[1]=pm->nummoves[2]=0;
	for (i=0;i<boardx;i++) {
		j=checkpiece(i);
		if (j>=0) {
			addpiece(player,i,j);
			w=checkboardwinner(player);
			if (w==WIN_YES) {
				winfound=1;
				pm->movex[AI_WIN][pm->nummoves[AI_WIN]]=i;
				pm->movey[AI_WIN][pm->nummoves[AI_WIN]]=j;
				pm->nummoves[AI_WIN]++;
				}
			removepiece(i,j);
		}
	}
	if (winfound)
		return;
	pm->nummoves[AI_WIN]=0;
	for (i=0;i<boardx;i++) {
		j=checkpiece(i);
		if (j>=0) {
			addpiece(player,i,j);
			w=checkboardwinner(player);
			if (w==WIN_YES)
				m=AI_WIN;
			else if (w==WIN_TIE || depth==1)
				m=AI_TIE;
			else {
				depth--;
				aimove(player^1,&pm2);
				if (pm2.nummoves[AI_WIN])
					m=AI_LOSE;
				else if (pm2.nummoves[AI_TIE])
					m=AI_TIE;
				else
					m=AI_WIN;
				depth++;
			}
			pm->movex[m][pm->nummoves[m]]=i;
			pm->movey[m][pm->nummoves[m]]=j;
			pm->nummoves[m]++;
			removepiece(i,j);
		}
	}
//	count--;
}
//static int checkboardwinner(int player)
//enum {WIN_YES,WIN_TIE,WIN_NOTYET};
//enum {AI_NOTALLOWED,AI_LOSE,AI_TIE,AI_WIN,AI_NUM};

static void compmove(int player)
{
	int i,j;
	int savedepth=depth;
	struct possmoves pm;
	while(depth>=1) {
		aimove(player,&pm);
		for (j=AI_NUM-1;j>=0;j--)
			if (pm.nummoves[j])
				break;
		if (j<0)
			errorexit("connect4: no moves");
		if (j==AI_WIN) {
			if (savedepth==depth)
				strcpy(comp_pred,"W");
			break;
		} else if (j==AI_TIE) {
			if (savedepth==depth)
				strcpy(comp_pred,"T");
			break;
		} else
			if (savedepth==depth)
				strcpy(comp_pred,"L");
		depth--;
	}
	i=random(pm.nummoves[j]);
	fallpos=pm.movex[j][i];
	playing++;
	depth=savedepth;
}

static void drawgame()
{
	int i,j;
	if (playing==PLAY_NO)
		return;
	for (j=0;j<boardy;j++)
		for (i=0;i<boardx;i++) {
			cliprecto16(B16,
			BOARDSTARTX+SQUARESIZE*i,
			BOARDSTARTY+SQUARESIZE*j,
			BOARDSTARTX+SQUARESIZE*(i+1),
			BOARDSTARTY+SQUARESIZE*(j+1),hiblack);
			if (theboard[j][i]==1)
				clipcircle16(B16,BOARDSTARTX+SQUARESIZE*i+SQUARESIZE/2,BOARDSTARTY+SQUARESIZE*j+SQUARESIZE/2,CIRCLERAD,hired);
			else if (theboard[j][i]==2)
				clipcircle16(B16,BOARDSTARTX+SQUARESIZE*i+SQUARESIZE/2,BOARDSTARTY+SQUARESIZE*j+SQUARESIZE/2,CIRCLERAD,hiblue);
		}
	if (playing==PLAY_RED)
		clipcircle16(B16,50,50,CIRCLERAD,hired);
	else if (playing==PLAY_BLUE)
		clipcircle16(B16,50,50,CIRCLERAD,hiblue);
	else if (playing==PLAY_REDFALL)
		clipcircle16(B16,BOARDSTARTX+fallpos*SQUARESIZE+SQUARESIZE/2,(playcounter<<2)+10,CIRCLERAD,hired);
	else if (playing==PLAY_BLUEFALL)
		clipcircle16(B16,BOARDSTARTX+fallpos*SQUARESIZE+SQUARESIZE/2,(playcounter<<2)+10,CIRCLERAD,hiblue);
	outtextxy16(B16,0,WY-8,comp_pred,hiblack);
}

static void updatebuttons()
{
	char str[150];
	int i;
	switch (playing) {
	case PLAY_NO:
		sprintf(str,"CHOOSE YOUR GAME : RED %f : BLUE %f",score[0]/2.0f,score[1]/2.0f);
		setresname(rl,TEXTSTATUS,str);
		break;
	case PLAY_RED:
		setresname(rl,TEXTSTATUS,"RED'S TURN");
		break;
	case PLAY_REDFALL:
		setresname(rl,TEXTSTATUS,"RED'S FALL");
		break;
	case PLAY_BLUE:
		setresname(rl,TEXTSTATUS,"BLUE'S TURN");
		break;
	case PLAY_BLUEFALL:
		setresname(rl,TEXTSTATUS,"BLUE'S FALL");
		break;
	case PLAY_END:
		setresname(rl,TEXTSTATUS,winnerstr);
		break;
	case PLAY_ABORTED:
		setresname(rl,TEXTSTATUS,"ABORTED");
		break;
	case PLAY_TIED:
		setresname(rl,TEXTSTATUS,"TIED");
		break;
	}
	if (playing==PLAY_NO) {
		setresname(rl,PBPLAY,"PLAY");
		setresvis(rl,TEXTBOARDSIZE,1);
		setresvis(rl,PBSIZEX,1);
		setresvis(rl,PBSIZEY,1);
		setresvis(rl,TEXTRED,1);
		setresvis(rl,PBREDTYPE,1);
		setresvis(rl,TEXTBLUE,1);
		setresvis(rl,PBBLUETYPE,1);
		setresvis(rl,TEXTCONNECTNUM,1);
		setresvis(rl,PBCONNECTNUM,1);
		setresvis(rl,TEXTDEPTH,1);
		setresvis(rl,PBDEPTH,1);
		setresvis(rl,TEXTFIRST,1);
		setresvis(rl,PBFIRST,1);
	} else {
		setresname(rl,PBPLAY,"STOP");
		setresvis(rl,TEXTBOARDSIZE,0);
		setresvis(rl,PBSIZEX,0);
		setresvis(rl,PBSIZEY,0);
		setresvis(rl,TEXTRED,0);
		setresvis(rl,PBREDTYPE,0);
		setresvis(rl,TEXTBLUE,0);
		setresvis(rl,PBBLUETYPE,0);
		setresvis(rl,TEXTCONNECTNUM,0);
		setresvis(rl,PBCONNECTNUM,0);
		setresvis(rl,TEXTDEPTH,0);
		setresvis(rl,PBDEPTH,0);
		setresvis(rl,TEXTFIRST,0);
		setresvis(rl,PBFIRST,0);
	}
	if (playing==PLAY_ABORTED || playing==PLAY_END || playing==PLAY_TIED)
		setresvis(rl,PBPLAY,0);
	else
		setresvis(rl,PBPLAY,1);
	if (playing==PLAY_RED || playing==PLAY_BLUE) {
		for (i=0;i<boardx;i++)
			setresvis(rl,PBMOVE0+i,1);
		for (i=boardx;i<MAXSIZEX;i++)
			setresvis(rl,PBMOVE0+i,0);
	} else
		for (i=0;i<MAXSIZEX;i++)
			setresvis(rl,PBMOVE0+i,0);
	sprintf(str,"%d",boardx);
	setresname(rl,PBSIZEX,str);
	sprintf(str,"%d",boardy);
	setresname(rl,PBSIZEY,str);
	sprintf(str,"%d",depth);
	setresname(rl,PBDEPTH,str);
	sprintf(str,"%d",connectnum);
	setresname(rl,PBCONNECTNUM,str);
	for (i=0;i<2;i++)
		if (playtype[i]==0)
			setresname(rl,PBREDTYPE+(i<<1),"HUMAN");
		else
			setresname(rl,PBREDTYPE+(i<<1),"COMPUTER");
	if (playfirst==0)
		setresname(rl,PBFIRST,"RED");
	else
		setresname(rl,PBFIRST,"BLUE");
}

static void updateplay()
{
	int winstat;
	switch(playing) {
	case PLAY_ABORTED:
	case PLAY_TIED:
	case PLAY_END:
		playcounter++;
		if (playcounter==60) {
			playcounter=0;
			playing=PLAY_NO;
		}
		break;
	case PLAY_REDFALL:
		playcounter++;
		if (playcounter==15) {
			playcounter=0;
			if (checkaddpiece(0,fallpos)>=0) {
				winstat=checkboardwinner(0);
				if (winstat==WIN_YES) {
					playing=PLAY_END;
					strcpy(winnerstr,"RED WINS !");
					score[0]+=2;
					playfirst^=1;
				} else if (winstat==WIN_TIE) {
					playing=PLAY_END;
					strcpy(winnerstr,"TIE !");
					score[0]++;
					score[1]++;
					playfirst^=1;
				} else
					playing=PLAY_BLUE;
			} else 
				playing=PLAY_RED;
		}
		break;
	case PLAY_BLUEFALL:
		playcounter++;
		if (playcounter==15) {
			playcounter=0;
			if (checkaddpiece(1,fallpos)>=0) {
				winstat=checkboardwinner(1);
				if (winstat==WIN_YES) {
					playing=PLAY_END;
					strcpy(winnerstr,"BLUE WINS !");
					score[1]+=2;
					playfirst^=1;
				} else if (winstat==WIN_TIE) {
					playing=PLAY_END;
					strcpy(winnerstr,"TIE !");
					score[0]++;
					score[1]++;
					playfirst^=1;
				} else
					playing=PLAY_RED;
			} else
				playing=PLAY_BLUE;
		}
		break;
	}
}

void connect4_init()
{
	video_setupwindow(800,600,565);
	pushandsetdir("connect4");
	rl=loadres("connect4res.txt");
	popdir();
	playing=0;
	boardx=7;
	boardy=6;
	connectnum=4;
	depth=8;
	playfirst=0;
	playtype[0]=0;
	playtype[1]=1;
	score[0]=0;
	score[1]=0;
	playcounter=0;
	initgame();
	updatebuttons();
}

void connect4_proc()
{
	struct rmessage rm;
	switch(KEY) {
	}
	checkres(rl);
	while(getresmess(rl,&rm)) {
		if (rm.id>=PBMOVE0 && rm.id<=PBMOVE6) {
			if (playing==PLAY_RED && playtype[0]==0 || playing==PLAY_BLUE && playtype[1]==0) {
				fallpos=rm.id-PBMOVE0;
				playing++;
			}
		} else {
			switch(rm.id) {
			case PBQUIT:
				popstate();
				break;
			case PBSIZEX:
				boardx++;
				if (boardx>MAXSIZEX)
					boardx=MINSIZEX;
				if (connectnum>boardx)
					connectnum=boardx;
				break;
			case PBSIZEY:
				boardy++;
				if (boardy>MAXSIZEY)
					boardy=MINSIZEY;
				if (connectnum>boardy)
					connectnum=boardy;
				break;
			case PBREDTYPE:
				playtype[0]^=1;
				break;
			case PBBLUETYPE:
				playtype[1]^=1;
				break;
			case PBCONNECTNUM:
				connectnum++;
				if (connectnum>MAXNUMCONNECT || connectnum>boardx || connectnum>boardy)
					connectnum=MINNUMCONNECT;
				break;
			case PBPLAY:
				if (playing==PLAY_NO) {
					initgame();
					if (playfirst)
						playing=PLAY_BLUE;
					else
						playing=PLAY_RED;
				} else {
					playing=PLAY_ABORTED;
					playfirst^=1;
				}
				break;
			case PBDEPTH:
				depth++;
				if (depth>MAXDEPTH)
					depth=MINDEPTH;
				break;
			case PBFIRST:
				playfirst^=1;
				break;
			}
		}
	}
	if (playing==PLAY_RED && playtype[0]!=0)
		compmove(0);
	else if (playing==PLAY_BLUE && playtype[1]!=0)
		compmove(1);
	updateplay();
	updatebuttons();
	video_lock();
	cliprect16(B16,0,0,WX-1,WY-1,himagenta);
	drawgame();
	drawres(rl);
	video_unlock();
}

void connect4_exit()
{
	freeres(rl);
}
#endif
