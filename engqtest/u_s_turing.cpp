// run a ported turing machine that was coded in 1998
#include <m_eng.h>
#include <l_misclibm.h>
#include "u_states.h"


#define DONAMESPACE
#ifdef DONAMESPACE
namespace turing {
#endif

//char* turfilelist[] = {
//	"binary.tur",
//	"copy.tur",
//};
//const int nturfilelist = NUMELEMENTS(turfilelist);
//int curturfileidx;

// turing bitmap, 320 by 200 (lo res)
bitmap32* B32M;


// UI
shape* rl;
shape* focus2,*oldfocus;

listbox* lfiles;
edit* efile;
pbut* bload,*bsave,*bdelete,*breset;



//#include <stdio.h>
//#include <graph32r/graph32r.h>

FILE *f;
//struct bitmap32 b,v;
char str[80];
//char *str2;
int rawbut;
int akey;
int quit=0;
int mx,my,but=0,oldbut=0;

#define TAPESIZE 40
char tape[TAPESIZE+1]="0000000000000000000000000000000000000000";
int readval;
int running=0;
int ack=0;
int speedraw;
int speedmul = 4;
int speed;
int maxspeed = 250;
short tapepos=20;
int clk;
int st;
int state;
short writes[8][3]={'0','1','-','0','1','-','0','1','-','0','1','-',
'0','1','-','0','1','-','0','1','-','0','1','-'};
short actions[8][3]={'.','.','.','.','.','.','.','.','.','.','.','.',
'.','.','.','.','.','.','.','.','.','.','.','.'};
short nexts[8][3]={0,0,0,1,1,1,2,2,2,3,3,3,4,4,4,5,5,5,6,6,6,7,7,7};

char tristr[]="01-";
char statestr[]="01234567H";

struct act
	{
	int x0,y0,x1,y1;
	void (*funct)(int,int,int);
	int nopress;
	};

void next_funct(int mx,int my,int but);
void action_funct(int mx,int my,int but);
void write_funct(int mx,int my,int but);
void state_funct(int mx,int my,int but);
void tape_funct(int mx,int my,int but);
void head_funct(int mx,int my,int but);
void continue_funct(int mx,int my,int but);
void start_funct(int mx,int my,int but);
void stop_funct(int mx,int my,int but);
void speed_funct(int mx,int my,int but);

struct act acts[]={
{48, 64,295, 71,next_funct,0},
{48, 56,295, 63,action_funct,0},
{48, 48,295, 55,write_funct,0},
{56, 32,319, 39,state_funct,0},
{ 0,  0,319, 7,tape_funct,0},
{ 0,  8,319, 15,head_funct,1},
{ 0,144, 63,151,continue_funct,0},
{ 0,128, 39,135,start_funct,0},
{ 0,136, 31,143,stop_funct,0},
{ 0,172, 70,179,speed_funct,1}};

void initmachine()
{
	quit=0;
	but=0,oldbut=0;

	running=0;
	ack=0;
	speedraw=speedmul;
	speed = 1;
	tapepos=20;
	clk=0;
	st=0;
	state=0;
	int i,j;
	for (j=0;j<8;++j) {
		for (i=0;i<3;++i) {
			writes[j][i] = tristr[i];
			actions[j][i] = '.';
			nexts[j][i] = j;
		}
	}
	for (j=0;j<TAPESIZE;++j)
		tape[j] = '0';
}

void loadmachine(const char* fname)
{
	initmachine();
	if (f=fopen(fname,"rb")) {
		fread(&tapepos,2,1,f);
		fread(tape,1,TAPESIZE,f);
		fread(writes,2,24,f);
		fread(actions,2,24,f);
		fread(nexts,2,24,f);
		fclose(f);
	}
}

void savemachine(const char* fname)
{
	if (f=fopen2(fname,"wb")) {
		fwrite(&tapepos,2,1,f);
		fwrite(tape,1,TAPESIZE,f);
		fwrite(writes,2,24,f);
		fwrite(actions,2,24,f);
		fwrite(nexts,2,24,f);
		fclose(f);
	}
}

void outtextxy2(struct bitmap32 *b,int x,int y,char *s,C32 color,int reverse)
{
if (reverse)
	{
	cliprect32(b,x,y,strlen(s)<<3,8,color);
	color=C32BLACK;
	}
outtextxy32(b,x,y,color,s);
}

void drawcursor(struct bitmap32 *b,int mx,int my)
{
clipline32(b,mx-5,my,mx-2,my,C32GREEN);
clipline32(b,mx+5,my,mx+2,my,C32GREEN);
clipline32(b,mx,my-5,mx,my-2,C32GREEN);
clipline32(b,mx,my+5,mx,my+2,C32GREEN);
}

void drawtape(struct bitmap32 *b)
{
outtextxy32(b,0,0,C32WHITE,tape);
}

void drawtapehead(struct bitmap32 *b)
{
char str[2];
str[1]='\0';
switch(st)
	{
	case	5:
	case	6:
	case	7:
	case	0:		str[0]='H';
					break;
	case	1:
	case	2:		str[0]='v';
					break;
	case	3:
	case	4:		str[0]='^';
					break;
	}
outtextxy32(b,tapepos<<3,8,C32WHITE,str);
}

void drawcontrols(struct bitmap32 *b)
{
char str[80];
outtextxy32(b,0,128,C32WHITE,"Start");
outtextxy32(b,0,136,C32WHITE,"Stop");
outtextxy32(b,0,144,C32WHITE,"Continue");
sprintf(str,"speed %d",ack ? speed : 0);
outtextxy32(b,0,172,C32WHITE,str);
}

void drawmachine(struct bitmap32 *b)
{
char str[80];
int i,j;
str[1]='\0';
outtextxy2(b,0,32,"State",C32WHITE,st==0);
outtextxy2(b,0,40,"Read",C32WHITE,(st==1)||(st==2));
outtextxy2(b,0,48,"Write",C32WHITE,(st==3)||(st==4));
outtextxy2(b,0,56,"Action",C32WHITE,(st==5)||(st==6));
outtextxy2(b,0,64,"Next",C32WHITE,st==7);
for (i=0;i<9;i++)
	{
	str[0]=statestr[i];
	outtextxy2(b,(i<<5)+8+48,32,str,C32WHITE,i==state);
	}
for (i=0;i<8;i++)
	{
	for (j=0;j<3;j++)
		{
		str[0]=tristr[j];
		outtextxy2(b,(i<<5)+48+(j<<3),40,str,C32WHITE,(st==2)&&(readval==j)&&(state==i));
		}
	for (j=0;j<3;j++)
		{
		str[0]=(char)writes[i][j];
		outtextxy2(b,(i<<5)+48+(j<<3),48,str,C32WHITE,((st==3)||(st==4))&&(readval==j)&&(state==i));
		}
	for (j=0;j<3;j++)
		{
		str[0]=(char)actions[i][j];
		outtextxy2(b,(i<<5)+48+(j<<3),56,str,C32WHITE,((st==5)||(st==6))&&(readval==j)&&(state==i));
		}
	for (j=0;j<3;j++)
		{
		str[0]=(char)statestr[nexts[i][j]];
		outtextxy2(b,(i<<5)+48+(j<<3),64,str,C32WHITE,(st==7)&&(readval==j)&&(state==i));
		}
	}
}

void speed_funct(int mx,int my,int but)
{
	if (!running)
		return;
speedraw+=(but*(1+speedraw/100));
if (speedraw<speedmul)
	speedraw=speedmul;
else if (speedraw>maxspeed*speedmul)
	speedraw=maxspeed*speedmul;
speed = speedraw / speedmul;
}


int tristate(int val,int but)
{
if (but==1)
	{
	if (val=='0')
		val='1';
	else if (val=='1')
		val='-';
	else
		val='0';
	}
else
	{
	if (val=='0')
		val='-';
	else if (val=='1')
		val='0';
	else
		val='1';
	}
return val;
}

int tristate2(int val,int but)
{
if (but==1)
	{
	if (val=='>')
		val='<';
	else if (val=='<')
		val='.';
	else
		val='>';
	}
else
	{
	if (val=='>')
		val='.';
	else if (val=='<')
		val='>';
	else
		val='<';
	}
return val;
}

void tape_funct(int mx,int my,int but)
{
int val;
if (ack)
	return;
val=tape[mx>>3];
val=tristate(val,but);
tape[mx>>3]=val;
}

void next_funct(int mx,int my,int but)
{
int modx,val;
if (ack)
	return;
modx=mx&31;
if (modx<24)
	{
	val=nexts[mx>>5][modx>>3]+but;
	if (val==9)
		val=0;
	else if (val<0)
		val=8;
	nexts[mx>>5][modx>>3]=val;
	}
}

void state_funct(int mx,int my,int but)
{
int modx;
if (ack)
	return;
modx=mx&31;
if (modx<8)
	state=mx>>5;
}

void head_funct(int mx,int my,int but)
{
if (ack)
	return;
tapepos=mx>>3;
}

void write_funct(int mx,int my,int but)
{
int modx,val;
if (ack)
	return;
modx=mx&31;
if (modx<24)
	{
	val=writes[mx>>5][modx>>3];
	val=tristate(val,but);
	writes[mx>>5][modx>>3]=val;
	}
}

void action_funct(int mx,int my,int but)
{
int modx,val;
if (ack)
	return;
modx=mx&31;
if (modx<24)
	{
	val=actions[mx>>5][modx>>3];
	val=tristate2(val,but);
	actions[mx>>5][modx>>3]=val;
	}
}

void stop_funct(int mx,int my,int but)
{
running=0;
}

void continue_funct(int mx,int my,int but)
{
if (state==8)
	return;
if (!ack)
	{
	running=1;
	ack=1;
	clk = 0;
	}
}

void start_funct(int mx,int my,int but)
{
if (!ack)
	{
	running=1;
	state=0;
	ack=1;
	clk = 0;
	}
}

void mouseaction(int mx,int my,int but,int oldbut)
{
int i;
for (i=0;i<sizeof(acts)/sizeof(struct act);i++)
	if ((mx<=acts[i].x1)&&(my<=acts[i].y1)&&(mx>=acts[i].x0)&&(my>=acts[i].y0))
		{
		if ((acts[i].nopress)||(oldbut==0))
			(acts[i].funct)(mx-acts[i].x0,my-acts[i].y0,but);
		}
}

void domachine()
{
	if (!ack)
		return;
	clk+=speed;
	while(clk>=25 && ack) {
		clk-=25;
		switch(st) {
		case 1:	readval=tape[tapepos];
			if (readval=='0')
				readval=0;
			else if (readval=='1')
				readval=1;
			else if (readval=='-')
				readval=2;
			break;
		case 3:	tape[tapepos]=(char)writes[state][readval];
			break;
		case 5:
			switch(actions[state][readval])	{
			case '>':
				if (tapepos<39)
					tapepos++;
				break;
			case '<':
				if (tapepos>0)
					tapepos--;
				break;
			}
			break;
		case 7:
			state=nexts[state][readval];
			if (state==8)
				running=0;
			if (!running)
				ack=0;
			break;
		}
		st++;
		st&=7;
	}
}

void refreshfilelist()
{
	U32 i;
	/*for (i=0;i<=100;++i) {
		C8 s[100];
		sprintf(s,"%3d * %3d = %5d",i,i,i*i);
		lfiles->addstring(s);
	}*/
	S32 oldidx = lfiles->getidx();
	lfiles->clear();
	scriptdir qdir(false);
	U32 n = qdir.num();
	for (i=0;i<n;++i) {
		const C8* fnameext = qdir.idx(i).c_str();
		C8 ext[200];
		mgetext(fnameext,ext,200);
		if (!my_stricmp(ext,"tur")) {
			C8 fname[200];
			fname[0] = '\0';
			mgetname(fnameext,fname);
			lfiles->addstring(fname);
		}
	}
	lfiles->setidxc(oldidx);
}

void savelastfile(const string& lf)
{
	FILE* fw = fopen2("lastfile.txt","w");
	if (fw) {
		fprintf(fw,"\"%s\"\n",lf.c_str());
		fclose(fw);
	}
}


#ifdef DONAMESPACE
} // end namespace turing

using namespace turing;
#endif

void turinginit()
{
	video_setupwindow(640,400);
	B32M = bitmap32alloc(WX/2,WY/2,C32BLACK);
	pushandsetdir("turing");

	rl = res_loadfile("turingres.txt");
	lfiles = rl->find<listbox>("LFILES");
	efile = rl->find<edit>("EFILE");
	bload = rl->find<pbut>("BLOAD");
	bsave = rl->find<pbut>("BSAVE");
	bdelete = rl->find<pbut>("BDELETE");
	breset = rl->find<pbut>("RESET");
	oldfocus = 0;
	focus2 = lfiles;
	refreshfilelist(); // update listbox from file directory

	bool goload = false;
	script* lf = new script("lastfile.txt");
	string fname;
	if (lf->num() == 1) {
		fname = lf->read();
		if (fileexist(fname.c_str()))
			goload = true;
	}
	if (goload) {
		//qf = new qfield(fname,-1);
		C8 name[100];
		mgetname(fname.c_str(),name);
		efile->settname(name);
		S32 idx = lfiles->findstring(name);
		if (idx >=0)
			lfiles->setidxc(idx);
		loadmachine(fname.c_str());
	} else {
		//qf = new qfield(qcolumn::defaultqubits);
		initmachine(); // blank
	}
	delete lf;





//meminitsize=2000000;
//mem_init();
//initgraph();
//initmouse();
//alloc_bitmap(&b,XSIZE,YSIZE,-1);
//make_video_bitmap(&v);
	//int argc = 2;
	//char* fname = "binary.tur";
	//const char* fname = turfilelist[curturfileidx];//"copy.tur";
	//++curturfileidx;
	//if (curturfileidx==nturfilelist)
	//	curturfileidx = 0;
//if (argc==2)
//	{
	//sprintf(str,"%s.tur",argv[1]);

}

void turingproc()
{
	//if (KEY==K_ESCAPE)
	//	poporchangestate(STATE_MAINMENU);
	if (quit)
		poporchangestate(STATE_MAINMENU);
	akey=KEY;//getkey();
	oldbut=but;
	//rawbut=but=getmousexy(&mx,&my);
	rawbut = but = MBUT;
	mx = MX>>1;
	my = MY>>1;
	switch(akey)
		{
		case	'q':
		case	K_ESCAPE:	quit=1;
							break;
		}
	if ((but == 1)||(but == 2))
		{
		if (but==2)
			but=-1;
		mouseaction(mx,my,but,oldbut);
		}
// proc ui
	if (wininfo.mleftclicks)
		focus2=rl->getfocus();
	S32 ret=-1;
	if (focus2) {
		ret=focus2->proc();
	}
	if (oldfocus && oldfocus!=focus2)
		oldfocus->deactivate();
	oldfocus=focus2;

	if (ret == 1) { // see if something changed in the UI
		// load file
		if (focus2 == bload) {
			//hqfxoffset->setidx(0);
			S32 idx = lfiles->getidx();
			if (idx >= 0) {
				const C8* idxname = lfiles->getidxname();
				efile->settname(idxname);
				string fileextname = string(idxname) + ".tur";
				const C8* fname = fileextname.c_str();
				if (fileexist(fname)) {
					loadmachine(fname);
					savelastfile(fileextname);
				} else {
					initmachine();
				}
				//lastshowcol = invalidcol;
			}
			focus2 = lfiles;
			//resetui();
			//lastshowcol = invalidcol; // update console
	// save file
		} else if (focus2 == bsave) {
			//const C8* idxname = lfiles->getidxname();
			//efile->settname(idxname);
			const C8* savename = efile->gettname();
			if (savename[0] != '\0') { // don't save 0 length names
				string fileextname = string(savename) + ".tur";
#if 0
				filesave_string(fileextname.c_str(),econtents->gettname());
				//econtents->settname(filedata);
				//delete[] filedata;
#else
				const C8* fname = fileextname.c_str();
				savemachine(fname);
				savelastfile(fileextname);
#endif
				refreshfilelist();
				S32 idx = lfiles->findstring(savename);
				if (idx >= 0)
					lfiles->setidxc(idx);
			focus2 = lfiles;
			}
	// delete file
		} else if (focus2 == bdelete) {
			string fileextnamedel = string(efile->gettname()) + ".tur";
			remove(fileextnamedel.c_str());
			refreshfilelist();
		// reset machine
		} else if (focus2 == breset) {
			initmachine();
		}
	}


	domachine();
}

void turingdraw2d()
{
	clipclear32(B32M,C32(0,0,0));
	//fastrect(&b,0,0,XSIZE-1,YSIZE-1,black);
	//clipclear32(B32,C32BLACK);
	drawtape(B32M);
	drawtapehead(B32M);
	drawmachine(B32M);
	drawcontrols(B32M);
	sprintf(str,"%d %d $%x",mx,my,rawbut);
 	outtextxy32(B32M,100,182,C32WHITE,str);
	drawcursor(B32M,mx,my);
	bitmap32double(B32M,B32);
	//outtextxyfc32(B32,7*WX/8,8,C32LIGHTRED,"Turing Machine");
	//fastblit(&b,&v,0,0,0,0,XSIZE,YSIZE);

	rl->draw(); // draw UI

}

void turingexit()
{
	delete rl;
	bitmap32free(B32M);
	popdir();
}

