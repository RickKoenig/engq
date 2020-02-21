// simple tests and one time utils
#include <m_eng.h>
#include "u_states.h"

#include "u_list.h"
//#include "u_file_nh.h"

#if 0 // working stub
void digital_init()
{
	video_setupwindow(640,480);
}

void digital_proc()
{
	if (KEY==K_ESCAPE)
		poporchangestate(STATE_MAINMENU);
}

void digital_draw2d()
{
	clipclear32(B32,C32(0,0,255));
	outtextxyf32(B32,WX/2,WY-16,C32LIGHTRED,"D I G I T A L");
}

void digital_exit()
{
}

#else // original digital.c from engine1test, modified to make it work here

// handleless file io

// handleless fileio
#define READ	0
#define WRITE	1
//#define RW	2 // not used
/*void fileopen(const char *filename,const char* fmode);
unsigned int fileread(void *buffer,unsigned int count);
unsigned int filewrite(const void *buffer,unsigned int count);
unsigned int filereadlong(void);  // big endian (mac)
void filewritelong(unsigned int); // big endian (mac)
void fileclose(void);
*/
static FILE* fp;

static void fileopen(const char* fname,const char* fmode)
{
	fp = fopen2(fname,fmode);
}

static void fileclose()
{
	fclose(fp);
	fp = 0;
}

static unsigned int fileread(void *buffer,unsigned int count)
{
	return fread(buffer,1,count,fp);
}

static unsigned int filewrite(const void *buffer,unsigned int count)
{
	unsigned int retval;
	retval=fwrite(buffer,1,count,fp);
	if (retval!=count)
		errorexit("error writing file\n","","");
	return retval;
}

static unsigned int filereadbyte()
{
	unsigned char i;
	if (fread(&i,1,1,fp)==0) {
	//	printf("EOF ");
		return(EOF);
	}
	return(i);
}

static unsigned int filereadword()
{
	unsigned int i;
	i=filereadbyte();
	i=(i<<8)+filereadbyte();
	return(i);
}

/////// reads using little endian 8086 //////
static unsigned int filereadword2()
{
	unsigned int retval;
	retval=filereadbyte();
	return (filereadbyte()<<8) + retval;
}

static unsigned int filereadlong()
{
	unsigned int i;
	i=filereadword();
	i=(i<<16)+filereadword();
	return(i);
}

static unsigned int filereadlong2()
{
	unsigned int retval;
	retval=filereadword2();
	return (filereadword2()<<16) + retval;
}

void filewritebyte(unsigned int val)
{
	unsigned char c;
	c=val;
	filewrite(&c,1);
}

void filewriteword(unsigned int val)
{
	filewritebyte(val>>8);
	filewritebyte(val);
}

void filewritelong(unsigned int val)
{
	filewriteword(val>>16);
	filewriteword(val);
}

void filewriteword2(unsigned int val)
{
	filewritebyte(val);
	filewritebyte(val>>8);
}

void filewritelong2(unsigned int val)
{
	filewriteword2(val);
	filewriteword2(val>>16);
}

#define DONAMESPACE
#ifdef DONAMESPACE
namespace digital {
#endif

#define XRES 640
#define YRES 480

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//#include <engine1.h>
//#include "debprint.h"

#define SWAP(a,b)		{ (a) ^= (b); (b) ^= (a); (a) ^= (b); }

#define XSTART 6
#define XSPACE	17
#define XGATE	17

#define YSTART	2
#define YSPACE	(12+1)
#define YGATE	17

#define VERSION "1.00"	// save version

#define REALWIRES
// #define TESTWIRES
// #define TESTWIRELIST

#define sqr(x) ((x)*(x))


static struct bitmap32 *arrow,*choice,*icons;

static int oldbut,oldmx,oldmy,butchange;
static int quit;
static int skiptime;
static int enabledebprintsave;

#define CELX 16
#define CELY 15
#define NUMINPUTS 4
#define NUMGATES 16

static struct pointi2 wirestart,gridstart;

struct input {
	struct pointi2 inputloc;  // output cel row column ,inputloc.x>=0 is used
	struct pointi2 wires[6];	// list of wire points from srs to dest
	int numnodes;				//	# of wire points 2 4 or 6
};

struct cel {
	int type;	// UNUSED to P2
	struct input inputs[NUMINPUTS];
	int newlogic;
	int logic;
};

static struct cel cels[CELX][CELY];

struct gate {
	int numinputs;
	int numoutputs;
	struct pointi2 outputloc;
	struct pointi2 inputlocs[NUMINPUTS];
};

enum {UNUSED,TOGGLE,NO,NC,LED,LEDBAR,BUF,NOT,OR,NOR,AND,NAND,XOR,NXOR,P1,P2};

static struct gate gates[NUMGATES];

static int mode='n';	// no power

static int clockspd=5;
static int simspd=7;
static int phaseclock;
static int curtick,lasttick,lastclock;
static int spdtable[8]={240,60,20,15,10,6,3,1};
static int clocktable[8]={240,60,20,15,10,6,3,1};

//static int nloadfiles;
//static char **loadfiles;
static script* loadfiles;
static C8 savestr[200];
static int curicon=0;

struct wireline {
	struct node *next;
	struct node *prev;
	struct pointi2 p0,p1;
	int id;
	int used;
};

#define WIREMAX 20000
static struct wireline wirepool[WIREMAX];

static struct C32 dacs[256];

static struct header hwires[YRES];
static struct header vwires[XRES-64];

static void showicon(struct bitmap32 *b,int x,int y,int icon,int logic)
{
	clipxpar32(icons,b,1+logic*18,1+icon*20,x-8,y-8,17,19,C32LIGHTBLUE);
}

static void drawcursor()
{
	switch(mode) {
	case 'p':
	case 'l':
	case 's':
		clipxpar32(arrow,B32,0,0,MX,MY,arrow->size.x,arrow->size.y,C32LIGHTBLUE);
		break;
	case 'n':
		showicon(B32,MX,MY,curicon,0);
		break;
	}
	if (wirestart.x!=-1)
		clipline32(B32,MX,MY,wirestart.x,wirestart.y,C32RED);
}

static void showobjs2()
{
//	struct wireline *w;
	C32 color;
	int i,j,k,el;
//	char debugstr[80];
	cliprect32(B32,0,0,XRES,YRES,C32LIGHTBLUE);
	clipblit32(choice,B32,0,0,XRES-64,0,choice->size.x,choice->size.y);
	outchar32(B32,XRES-64+16-4,276,C32BLACK,clockspd+'0');
	outchar32(B32,XRES-32+16-4,276,C32BLACK,simspd+'0');
	for (i=0;i<CELX;i++)
		for (j=0;j<CELY;j++) {
			if (cels[i][j].type)
				showicon(B32,
					XSTART+XSPACE+XGATE/2+i*(XSPACE+XGATE),
					YSTART+YSPACE+YGATE/2+j*(YSPACE+YGATE),
					cels[i][j].type,cels[i][j].logic ? 1 : 0);
			else
				clipputpixel32(B32,XSTART+XSPACE+XGATE/2+i*(XSPACE+XGATE),
					YSTART+YSPACE+YGATE/2+j*(YSPACE+YGATE),C32WHITE);
		}
#ifdef TESTWIRES
	for (i=0;i<=CELX;i++)	// vertical
		for (j=0;j<XSPACE+1;j+=2)
			clipline32(B32,XSTART+i*(XSPACE+XGATE)+j,YSTART+2,XSTART+i*(XSPACE+XGATE)+j,YRES-YSTART-14,C32LIGHTRED);
	for (i=0;i<=CELY;i++)	// horizontal
		for (j=0;j<YSPACE-2;j+=2)
			clipline32(B32,XSTART,YSTART+1+1+i*(YSPACE+YGATE)+j,XRES-64-XSTART-5+1,YSTART+1+1+i*(YSPACE+YGATE)+j,C32LIGHTGREEN);
#endif
#ifdef REALWIRES
	////// draw wires /////
	for (i=0;i<CELX;i++)
		for (j=0;j<CELY;j++)
			for (k=0;k<NUMINPUTS;k++)
				for (el=0;el<cels[i][j].inputs[k].numnodes-1;el++) {
					if (mode=='p')
						if (cels[cels[i][j].inputs[k].inputloc.x][cels[i][j].inputs[k].inputloc.y].newlogic)
							color=C32RED;
						else
							color=C32BLACK;
					else
						color=C32YELLOW;
					clipline32(B32,cels[i][j].inputs[k].wires[el].x,
					cels[i][j].inputs[k].wires[el].y,
					cels[i][j].inputs[k].wires[el+1].x,
					cels[i][j].inputs[k].wires[el+1].y,color);
				}
#endif
#ifdef TESTWIRELIST
	for (i=0;i<XRES-64;i++)	{
		for (w=(struct wireline *)vwires[i].first;w;w=(struct wireline *)w->next)
			clipline32(B32,w->p0.x,w->p0.y,w->p1.x,w->p1.y,C32BLUE);
	}
	for (i=0;i<YRES;i++) {
		for (w=(struct wireline *)hwires[i].first;w;w=(struct wireline *)w->next)
			clipline32(B32,w->p0.x,w->p0.y,w->p1.x,w->p1.y,C32BLUE);
	}
#endif
	switch(mode) {
	case 'p':
		break;
	case 'n':
		break;
	case 's':
		cliprect32(B32,XRES/2+96/2+8 -16,0,16+13*8,8,C32WHITE);
		outtextxy32(B32,XRES/2+96/2+8,0,C32BLACK,savestr);
		if (strlen(savestr)<8)
			clipcircle32(B32,XRES/2+96/2+8+strlen(savestr)*8+4,4,3,C32BLUE);
	case 'l':
		if (loadfiles) {
			cliprect32(B32,XRES/2-96/2-16,0,96,loadfiles->num()*8-1,C32WHITE);
			for (i=0;i<loadfiles->num();i++)
				outtextxy32(B32,XRES/2-96/2,i*8,C32BLACK,loadfiles->idx(i).c_str());
		}
		break;
	}
//	sprintf(debugstr,"%3d %3d",wirestart.x,wirestart.y);
//	outtextxy32(B32,500,472,debugstr,C32RED);
}

static void seticons()	// find inputs and outputs from icon colors
{
	int i;
	int x,y;
	C32 val;
	for (i=0;i<NUMGATES;i++)
		for (x=0;x<17;x++)
			for (y=0;y<19;y++) {
				val=clipgetpixel32(icons,1+x,1+y+i*20);
				if (val==C32YELLOW) {
					gates[i].outputloc.x=x-8+1;
					gates[i].outputloc.y=y-9+1;
					gates[i].numoutputs++;
					clipputpixel32(icons,1+x,1+y+i*20,C32BLACK);
				} else if (val==C32LIGHTGREEN) {
					gates[i].inputlocs[gates[i].numinputs].x=x-8-1;
					gates[i].inputlocs[gates[i].numinputs].y=y-9+1;
					gates[i].numinputs++;
					clipputpixel32(icons,1+x,1+y+i*20,C32BLACK);
				}
			}
}

static void initcels()
{
	int i,j,k;
	for (i=0;i<CELX;i++)
		for (j=0;j<CELY;j++)
			for (k=0;k<NUMINPUTS;k++)
				cels[i][j].inputs[k].inputloc.x=-1;
}

static struct wireline *allocwire()
{
	struct wireline *w;
	for (w=wirepool;w<wirepool+WIREMAX;w++)
		if (!w->used) {
			w->used=1;
			return w;
		}
	errorexit("not enough wires");
	return 0;
}

static int insertwire(int sx,int sy,int dx,int dy,int id)
{
	struct wireline *w;
// printf("inserting wire %d %d %d %d %d\n",sx,sy,dx,dy,id);
	if (sx<0 || sy<0 || sx>=XRES-64 || sy>=YRES)
		return 0;
	if (sx==dx) {
		if (sy>dy)
			SWAP(sy,dy)
		for (w=(struct wireline *)(vwires[sx].first);w;w=(struct wireline *)(w->next))
			if (w->id != id && sy<=w->p1.y && dy>=w->p0.y)
				return 0;
		w=allocwire();
		w->p0.x=sx;
		w->p0.y=sy;
		w->p1.x=dx;
		w->p1.y=dy;
		w->id=id;
		list_insert_head(&vwires[sx],(struct node *)w);
		return 1;
	}
	if (sy==dy) {
		if (sx>dx)
			SWAP(sx,dx)
		for (w=(struct wireline *)(hwires[sy].first);w;w=(struct wireline *)w->next)
			if (w->id != id && sx<=w->p1.x && dx>=w->p0.x)
				return 0;
		w=allocwire();
		w->p0.x=sx;
		w->p0.y=sy;
		w->p1.x=dx;
		w->p1.y=dy;
		w->id=id;
		list_insert_head(&hwires[sy],(struct node *)w);
		return 1;
	}
	return 0;
}

static void initwirelists()
{
	int i,j,x,y;
	int count=0;
	for (i=0;i<CELX;i++)
		for (j=0;j<CELY;j++) {
			x=-10;
			insertwire(i*(XGATE+XSPACE)+x+XSTART+26,
				j*(YGATE+YSPACE)+  YSTART+22-6,
				i*(XGATE+XSPACE)+x+XSTART+26,
				j*(YGATE+YSPACE)+  YSTART+22+6,0);
			x=8;
			insertwire(i*(XGATE+XSPACE)+x+XSTART+26,
				j*(YGATE+YSPACE)+  YSTART+22-2,
				i*(XGATE+XSPACE)+x+XSTART+26,
				j*(YGATE+YSPACE)+  YSTART+22+2,0);
			for (x=-8;x<8;x+=2)
				insertwire(i*(XGATE+XSPACE)+x+XSTART+26,
					j*(YGATE+YSPACE)+  YSTART+22-8,
					i*(XGATE+XSPACE)+x+XSTART+26,
					j*(YGATE+YSPACE)+  YSTART+22+8,0);
			for (y=-8;y<9;y+=2)
				insertwire(i*(XGATE+XSPACE)+  XSTART+26-8,
					j*(YGATE+YSPACE)+y+YSTART+22,
					i*(XGATE+XSPACE)+  XSTART+26+6,
					j*(YGATE+YSPACE)+y+YSTART+22,0);
			logger("%3d%%\n",count*100/CELX/CELY);
			count++;
		}
}

static void freewire(struct wireline *w)
{
	w->used=0;
}

static void clearallwirelists()
{
	int i;
	struct wireline *ptr;
	for (i=0;i<XRES-64;i++)
		while (ptr=(struct wireline *)vwires[i].first) {
			list_remove_this(&vwires[i],ptr);
			freewire(ptr);
		}
	for (i=0;i<YRES;i++)
	while (ptr=(struct wireline *)hwires[i].first) {
		list_remove_this(&hwires[i],ptr);
		freewire(ptr);
	}
}

static struct pointi2 getgrid()
{
	struct pointi2 retval;
	retval.x=(MX-XSTART-XSPACE/2-XGATE/2+XGATE+XSPACE+8)/(XGATE+XSPACE)-1;
	retval.y=(MY-YSTART-YSPACE/2-YGATE/2+YGATE+YSPACE+8)/(YGATE+YSPACE)-1;
	if (retval.y<0 || retval.x<0 || retval.x>=CELX || retval.y>=CELY)
		retval.x=-1;
	return retval;
}

static struct pointi2 gridtowires(struct pointi2 g)
{
	struct pointi2 w;
	w.x=XSTART+XSPACE+XGATE/2+g.x*(XSPACE+XGATE);
	w.y=YSTART+YSPACE+YGATE/2+g.y*(YSPACE+YGATE);
	return w;
}

static void deletewire(int sx,int sy,int dx,int dy)
{
	struct header *h;
	struct wireline *w;
	if (sx<0 || sy<0 || sx>=XRES-64 || sy>=YRES)
		return;
	if (sx==dx) {
		if (sy>dy)
			SWAP(sy,dy)
		h=&vwires[sx];
	} else if (sy==dy) {
		if (sx>dx)
			SWAP(sx,dx)
		h=&hwires[sy];
	} else
		return;
	for (w=(struct wireline *)h->first;w;w=(struct wireline *)w->next)
		if (w->p0.x==sx && w->p0.y == sy && w->p1.x==dx && w->p1.y == dy) {
			list_remove_this(h,w);
			freewire(w);
			return;
		}
}

void clearinput(struct input *ip)
{
	int i;
	for (i=0;i<ip->numnodes-1;i++)
		deletewire(ip->wires[i].x,ip->wires[i].y,ip->wires[i+1].x,ip->wires[i+1].y);
	ip->inputloc.x=-1;
	ip->numnodes=0;
}

static void clearinputs(struct cel *cp)
{
	int i;
	for (i=0;i<NUMINPUTS;i++) {
		clearinput(&cp->inputs[i]);
		cp->logic=0;	// takes care of any stray TOG's
	}
}

static void clearoutputs(struct pointi2 gl)
{
	int i,j,k;
	for (i=0;i<CELX;i++)
		for (j=0;j<CELY;j++)
			for (k=0;k<NUMINPUTS;k++)
				if (cels[i][j].inputs[k].inputloc.x==gl.x && cels[i][j].inputs[k].inputloc.y==gl.y)
					clearinput(&cels[i][j].inputs[k]);
}

static int getcommand()
{
	int retval;
	if (MX<XRES-64)
		return -1;
	retval=(MX-XRES+64)/32+MY/32*2;
	if (retval<16)
		return retval;
	switch(retval) {
	case 16:
		return 'c';
	case 17:
		return 'g';
	case 18:
		return 'n';
	case 19:
		return 'p';
	case 22:
		return 'l';
	case 23:
		return 's';
	case 28:
	case 29:
		return 'q';
	}
	return -1;
}

static void clearwirelists()
{
	int i;
	struct wireline *ptr,*ptr2;
	for (i=0;i<XRES-64;i++) {
		ptr=(struct wireline *)vwires[i].first;
		while (ptr) {
			ptr2=(struct wireline *)ptr->next;
			if (ptr->id!=0) {
				list_remove_this(&vwires[i],ptr);
				freewire(ptr);
			}
			ptr=ptr2;
		}
	}
	for (i=0;i<YRES;i++) {
		ptr=(struct wireline *)hwires[i].first;
		while (ptr) {
			ptr2=(struct wireline *)ptr->next;
			if (ptr->id!=0) {
				list_remove_this(&hwires[i],ptr);
				freewire(ptr);
			}
			ptr=ptr2;
		}
	}
}

static void digsave(char *file)
{
	char str[100];
	strcpy(str,file);
	strcat(str,".dig");
#if 1
	fileopen(str,"wb");
	filewrite(VERSION,4);
	filewritelong(CELX);
	filewritelong(CELY);
	filewrite(cels,sizeof(struct cel)*CELX*CELY);
	fileclose();
#else
#endif
}

static void digload(const char *file)
{
	char str[100];
	char verstr[5];
	int i,j,k,el,id;
	verstr[4]='\0';
	strcpy(str,file);
	strcat(str,".dig");
	fileopen(str,"rb");
	fileread(verstr,4);
	if (!strcmp(verstr,"1.00")) {
		filereadlong();
		filereadlong();
		fileread(cels,sizeof(struct cel)*CELX*CELY);
		for (i=0;i<CELX;i++)
			for (j=0;j<CELY;j++)
				cels[i][j].logic=0;
	} else { // no version
		fileclose();
		errorexit("wrong format!");
	}
	clearwirelists();
	for (i=0;i<CELX;i++)
		for (j=0;j<CELY;j++)
			for (k=0;k<NUMINPUTS;k++) {
				id=cels[i][j].inputs[k].inputloc.x+(cels[i][j].inputs[k].inputloc.y<<4)+1;
				for (el=0;el<cels[i][j].inputs[k].numnodes-1;el++) {
					if (!insertwire(cels[i][j].inputs[k].wires[el].x,
						  cels[i][j].inputs[k].wires[el].y,
						  cels[i][j].inputs[k].wires[el+1].x,
						  cels[i][j].inputs[k].wires[el+1].y,id))
							  errorexit("problem inserting wire from load");
				}
			}
	fileclose();
}

// replace with string class
static C8* strclone(const C8* orig)
{
	C8* r=new C8[strlen(orig)+1];
	strcpy(r,orig);
	return r;
}

//// id of 0 are wires to cover gates, prevent wires crossing gates
static void processinput()
{
	int id;
	int i,j,k;
	int a,b,c;
	struct gate *gateptr;
	struct cel *celptr,*celptr2;
	struct input *inputptr;
	static int dragstate;
	struct pointi2 gridloc;
	int command=0;
	int down=0;
	//int up;
	int pin[4];
/////// save mode ///////
	if (mode=='s') {
		if (KEY==K_RETURN) {
			digsave(savestr);
			mode='n';
			return;
		} else if (KEY==K_BACKSPACE || KEY==K_DELETE) {
			if (strlen(savestr)>0)
				savestr[strlen(savestr)-1]='\0';
			else
				mode='n';
			return;
		} else if (KEY==K_ESCAPE) {
			mode='n';
			return;
		} else if (isalnum(KEY)) {
			if (strlen(savestr)<8) {
				savestr[strlen(savestr)+1]='\0';
				savestr[strlen(savestr)]=KEY;
			}
			return;
		} else if (butchange) {
			if (loadfiles && MX>=XRES/2-96/2-16 && MX<XRES/2+96/2-1-16 && MY<loadfiles->num()*8-1)
				strcpy(savestr,loadfiles->idx(MY/8).c_str());
			else
				mode='n';
			return;
		}
	}
///// power on mode /////
	if (mode=='p') {
/////// simulation /////////
		if (curtick>lastclock+clocktable[clockspd]) {
			phaseclock++;
			phaseclock&=0xf;
			lastclock=curtick;
		}
		if (curtick>lasttick+spdtable[simspd]) {
			lasttick=curtick;
			skiptime++;
			if (skiptime==3)
				skiptime=0;
			for (i=0;i<CELX;i++)
				for (j=0;j<CELY;j++)
					if (skiptime || mt_random(2))
						if (cels[i][j].type==NC)
							cels[i][j].newlogic=(cels[i][j].logic==0);
						else
							cels[i][j].newlogic=(cels[i][j].logic!=0);
			for (i=0;i<CELX;i++)
				for (j=0;j<CELY;j++) {
					celptr=&cels[i][j];
					for (k=0;k<4;k++) {
						inputptr=&celptr->inputs[k];
						if (inputptr->inputloc.x>=0) {
							celptr2=&cels[inputptr->inputloc.x][inputptr->inputloc.y];
							pin[k]=celptr2->newlogic;
						} else if (celptr->type == AND || celptr->type == NAND)
							pin[k]=1;
						else
							pin[k]=0;
					}
					switch (celptr->type) {
					case BUF:
					case LED:
						celptr->logic=pin[0];
						break;
					case NOT:
					case LEDBAR:
						celptr->logic=pin[0]^1;
						break;
					case OR:
						if (pin[0]+pin[1]+pin[2]+pin[3]>=1)
							celptr->logic=1;
						else
							celptr->logic=0;
						break;
					case NOR:
						if (pin[0]+pin[1]+pin[2]+pin[3]>=1)
							celptr->logic=0;
						else
							celptr->logic=1;
						break;
					case AND:
						if (pin[0]+pin[1]+pin[2]+pin[3]==4)
							celptr->logic=1;
						else
							celptr->logic=0;
						break;
					case NAND:
						if (pin[0]+pin[1]+pin[2]+pin[3]==4)
							celptr->logic=0;
						else
							celptr->logic=1;
						break;
					case XOR:
						celptr->logic=(pin[0]+pin[1]+pin[2]+pin[3])&1;
						break;
					case NXOR:
						celptr->logic=((pin[0]+pin[1]+pin[2]+pin[3])&1)^1;
						break;
					case P1:
						celptr->logic=(phaseclock>=4 && phaseclock <=8);
						break;
					case P2:
						celptr->logic=(phaseclock>=12 && phaseclock <=16);
						break;
					}
				}
		}
/////// end simulation /////////
		gridloc=getgrid();
		if (gridloc.x>=0) {
			celptr=&cels[gridloc.x][gridloc.y];
			for (i=0;i<CELX;i++)
				for (j=0;j<CELY;j++)
					if (cels[i][j].type==NC || cels[i][j].type ==NO)
						if (cels[i][j].logic)
							cels[i][j].logic--;
			if (celptr->type==TOGGLE) {
				if (butchange)
					celptr->logic^=1;
			} else if (celptr->type==NO || celptr->type==NC)
				if (MBUT)
					celptr->logic= 5;
			return;
		}
		command=getcommand();
		if (butchange==1) {
			if (command=='n') {
				mode='n';
					for (i=0;i<CELX;i++)
						for (j=0;j<CELY;j++) {
							if (cels[i][j].type!=TOGGLE)
								cels[i][j].logic=0;
							cels[i][j].newlogic=0;
						}
			}
		}
		if (butchange) {
			switch (command) {
			case 'c':
				if (butchange==2)
					clockspd++;
				if (butchange==1)
					clockspd--;
				if (clockspd<0)
					clockspd=0;
				if (clockspd>=7)
					clockspd=7;
				break;
			case 'g':
				if (butchange==2)
					simspd++;
				if (butchange==1)
					simspd--;
				if (simspd<0)
					simspd=0;
				if (simspd>=7)
					simspd=7;
				break;
			}
		}
		return;
	}
///////// immediate action ///////
/////// normal mode /////
	if (butchange && mode !='l' && mode!='s') {
////// check grids /////
		gridloc=getgrid();
		if (gridloc.x>=0) {
			celptr=&cels[gridloc.x][gridloc.y];
			if (butchange==1) {
				if (curicon) {	// drop new part in
					clearoutputs(gridloc);
					celptr->type=curicon;
					clearinputs(celptr);
				} else {	// start a wire
					gateptr=&gates[celptr->type];
					if (wirestart.x==-1 && gateptr->numoutputs) {
						gridstart=gridloc;
						wirestart=gridtowires(gridloc);
						wirestart.x+=gateptr->outputloc.x;
						wirestart.y+=gateptr->outputloc.y;
					} else if (wirestart.x!=-1) {
///// insert wire //////
//// find an open input //
						for (j=0;j<gateptr->numinputs;j++)
							if (celptr->inputs[j].inputloc.x<0)
								break;
						if (j!=gateptr->numinputs) {
							id=gridstart.x+(gridstart.y<<4)+1;
							inputptr=&celptr->inputs[j];
							inputptr->wires[0]=gridtowires(gridloc);
							inputptr->wires[0].x+=gateptr->inputlocs[j].x;
							inputptr->wires[0].y+=gateptr->inputlocs[j].y;
/// try 1 line /////
/*
							if (insertwire(wirestart.x,wirestart.y,inputptr->wires[0].x,inputptr->wires[0].y,id)) {
								inputptr->wires[1]=wirestart;
								inputptr->numnodes=2;
								inputptr->inputloc=gridstart;
								goto donewire;
							}
*/
/// try 3 lines ///
							for (a=wirestart.x;a<=inputptr->wires[0].x;a+=2)
								if (insertwire(wirestart.x,wirestart.y,a,wirestart.y,id)) {
									if (insertwire(a,wirestart.y,a,inputptr->wires[0].y,id)) {
										if (insertwire(a,inputptr->wires[0].y,inputptr->wires[0].x,inputptr->wires[0].y,id)) {
											inputptr->wires[1].x=a;
											inputptr->wires[1].y=inputptr->wires[0].y;
											inputptr->wires[2].x=a;
											inputptr->wires[2].y=wirestart.y;
											inputptr->wires[3]=wirestart;
											inputptr->numnodes=4;
											inputptr->inputloc=gridstart;
											goto donewire;
										}
										deletewire(a,wirestart.y,a,inputptr->wires[0].y);
									}
									deletewire(wirestart.x,wirestart.y,a,wirestart.y);
								}
/// try 5 lines ///
							for (a=wirestart.x+2;a<=wirestart.x+18;a+=2)
								for (c=inputptr->wires[0].x-2;c>=inputptr->wires[0].x-18;c-=2)
									for (b=min(wirestart.y,inputptr->wires[0].y)+2;b<=max(wirestart.y,inputptr->wires[0].y)-2;b+=2)
										if (insertwire(wirestart.x,wirestart.y,a,wirestart.y,id)) {
											if (insertwire(a,wirestart.y,a,b,id)) {
												if (insertwire(a,b,c,b,id)) {
													if (insertwire(c,b,c,inputptr->wires[0].y,id)) {
														if (insertwire(c,inputptr->wires[0].y,inputptr->wires[0].x,inputptr->wires[0].y,id)) {
															inputptr->wires[1].x=c;
															inputptr->wires[1].y=inputptr->wires[0].y;
															inputptr->wires[2].x=c;
															inputptr->wires[2].y=b;
															inputptr->wires[3].x=a;
															inputptr->wires[3].y=b;
															inputptr->wires[4].x=a;
															inputptr->wires[4].y=wirestart.y;
															inputptr->wires[5]=wirestart;
															inputptr->numnodes=6;
															inputptr->inputloc=gridstart;
															goto donewire;
														}
														deletewire(c,b,c,inputptr->wires[0].y);
													}
													deletewire(a,b,c,b);
												}
												deletewire(a,wirestart.y,a,b);
											}
											deletewire(wirestart.x,wirestart.y,a,wirestart.y);
										}
/// try 5 lines again ///
							for (a=wirestart.x+2;a<=wirestart.x+18;a+=2)
								for (c=inputptr->wires[0].x-2;c>=inputptr->wires[0].x-18;c-=2)
									for (b=min(wirestart.y,inputptr->wires[0].y)-2;b>=max(wirestart.y,inputptr->wires[0].y)-60;b-=2)
										if (insertwire(wirestart.x,wirestart.y,a,wirestart.y,id)) {
											if (insertwire(a,wirestart.y,a,b,id)) {
												if (insertwire(a,b,c,b,id)) {
													if (insertwire(c,b,c,inputptr->wires[0].y,id)) {
														if (insertwire(c,inputptr->wires[0].y,inputptr->wires[0].x,inputptr->wires[0].y,id)) {
															inputptr->wires[1].x=c;
															inputptr->wires[1].y=inputptr->wires[0].y;
															inputptr->wires[2].x=c;
															inputptr->wires[2].y=b;
															inputptr->wires[3].x=a;
															inputptr->wires[3].y=b;
															inputptr->wires[4].x=a;
															inputptr->wires[4].y=wirestart.y;
															inputptr->wires[5]=wirestart;
															inputptr->numnodes=6;
															inputptr->inputloc=gridstart;
															goto donewire;
														}
														deletewire(c,b,c,inputptr->wires[0].y);
													}
													deletewire(a,b,c,b);
												}
												deletewire(a,wirestart.y,a,b);
											}
											deletewire(wirestart.x,wirestart.y,a,wirestart.y);
										}
/// try 5 lines again ///
							for (a=wirestart.x+2;a<=wirestart.x+18;a+=2)
								for (c=inputptr->wires[0].x-2;c>=inputptr->wires[0].x-18;c-=2)
									for (b=min(wirestart.y,inputptr->wires[0].y)+2;b<=max(wirestart.y,inputptr->wires[0].y)+60;b+=2)
										if (insertwire(wirestart.x,wirestart.y,a,wirestart.y,id)) {
											if (insertwire(a,wirestart.y,a,b,id)) {
												if (insertwire(a,b,c,b,id)) {
													if (insertwire(c,b,c,inputptr->wires[0].y,id)) {
														if (insertwire(c,inputptr->wires[0].y,inputptr->wires[0].x,inputptr->wires[0].y,id)) {
															inputptr->wires[1].x=c;
															inputptr->wires[1].y=inputptr->wires[0].y;
															inputptr->wires[2].x=c;
															inputptr->wires[2].y=b;
															inputptr->wires[3].x=a;
															inputptr->wires[3].y=b;
															inputptr->wires[4].x=a;
															inputptr->wires[4].y=wirestart.y;
															inputptr->wires[5]=wirestart;
															inputptr->numnodes=6;
															inputptr->inputloc=gridstart;
															goto donewire;
														}
														deletewire(c,b,c,inputptr->wires[0].y);
													}
													deletewire(a,b,c,b);
												}
												deletewire(a,wirestart.y,a,b);
											}
											deletewire(wirestart.x,wirestart.y,a,wirestart.y);
										}
						}
donewire:
						wirestart.x=-1;
					}
				}

			} else if (butchange==2) {
				if (wirestart.x==-1) { // pick up and delete
					clearoutputs(gridloc);
					curicon=celptr->type;
					celptr->type=UNUSED;
					clearinputs(celptr);
				} else
					wirestart.x=-1;
			}
			return;
		}
////// check menu board /////
		wirestart.x=-1;
		command=getcommand();
		if (command<16 && command>=0) {
			curicon=command;
			command='n';
		} else switch(command) {
		case 'c':
			if (butchange==2)
				clockspd++;
			if (butchange==1)
				clockspd--;
			if (clockspd<0)
				clockspd=0;
			if (clockspd>=7)
				clockspd=7;
			break;
		case 'g':
			if (butchange==2)
				simspd++;
			if (butchange==1)
				simspd--;
			if (simspd<0)
				simspd=0;
			if (simspd>=7)
				simspd=7;
			break;
		case 'q':
//			popstate();
			quit=1;
			break;
		case 's':
			savestr[0]='\0';
		case 'l':
			{
				//char **sc;
				//int nsc;
				//char *tempstr;
				if (loadfiles) {
					//freescript(loadfiles,nloadfiles);
					//loadfiles=NULL;
					//nloadfiles=0;
					delete loadfiles;
					loadfiles = 0;
				}
				loadfiles = new script;
				script sc = scriptdir(0);
				//sc=doadir(&nsc,0);
				for (i=0;i<sc.num();i++)
					if (isfileext(sc.idx(i).c_str(),"dig")) {
						char fname[100];
						mgetname(sc.idx(i).c_str(),fname);
						//loadfiles=addscript(loadfiles,strclone(fname),&nloadfiles);
						loadfiles->addscript(fname);
					}
				//freescript(sc,nsc);
				if (loadfiles && loadfiles->num()==0) {
					if (command!='s')
						mode='n';
					break;
				}
				/*while (down!=nloadfiles-1) {
					up=down;
					while( up>=0 && strcmp(loadfiles[up],loadfiles[up+1]) > 0) {
						tempstr=loadfiles[up];
						loadfiles[up]=loadfiles[up+1];
						loadfiles[up+1]=tempstr;
						up--;
					}
					down++;
				} */
				butchange=0;
			}
			break;
		}
	}
/////// modes ////////////
	switch(command) {
	case 'p':
		phaseclock=0;
// power
	case 'n':	 // nopower
	case 'l':	 // load
	case 's':	 // save
		mode=command;
		break;
	}
	switch(mode) {
	case 'l':
		if (butchange==1) {
			if (loadfiles && MX>=XRES/2-96/2-16 && MX<XRES/2+96/2-1-16 && MY<loadfiles->num()*8-1)
				digload(loadfiles->idx(MY/8).c_str());
			mode='n';
		}
		if (KEY==K_ESCAPE)
			mode='n';
		break;
	}
}

void dumpbitmap(bitmap32* bm,const C8* name)
{
	logger("dump bitmap of '%s'\n",name);
	logger("\t size is %4d %4d\n",bm->size.x,bm->size.y);
	S32 sizey = min(10,bm->size.y);
	S32 i,j;
	for (j=0;j<sizey;++j) {
		logger("line %3d : ",j);
		for (i=0;i<bm->size.x;++i) {
			C32 val = fastgetpixel32(bm,i,j);
			logger(" (R%3d,G%3d,B%3d,A%3d),",val.r,val.g,val.b,val.a);
		}
		logger("\n");
	}
}


#ifdef DONAMESPACE
} // end namespace digital

using namespace digital;
#endif

void digital_init()
{
	video_setupwindow(XRES,YRES);
	//video_setupwindow(XRES,YRES,8);
	pushandsetdir("digital");
	//enabledebprintsave=enabledebprint;
	//enabledebprint=0;
	memset(hwires,0,sizeof(hwires));
	memset(vwires,0,sizeof(vwires));
	memset(wirepool,0,sizeof(wirepool));
	memset(cels,0,sizeof(cels));
	memset(gates,0,sizeof(gates));
	quit=0;
	//changeescbehavior(ESC_NONE);
	//pushandsetdir("digital");
	//FILE* fh = fopen2("arrow.bbm","rb");
	arrow=gfxread32("arrow.bbm");
	dumpbitmap(arrow,"arrow bitmap");
	//fileclose();
	//video_setpalette(dacs);
	//fileopen("icons.bbm",READ);
	//icons=gfxread8(dacs);
	//fileclose();




//	icons = gfxread32("icons.bbm");
//	dumpbitmap(icons,"icons bitmap");

	choice = gfxread32("choice.bbm");
	dumpbitmap(choice,"choice bitmap");

/*	bitmap8* choice8 = gfxread8("choice.bbm",dacs);
	copy(C32stdcolors,C32stdcolors+16,dacs);
	choice = convert8to32(choice8,dacs);
	bitmap8free(choice8);
	dumpbitmap(icons,"icons bitmap"); */

	C32 dacs[256];
	bitmap8* icons8 = gfxread8("icons.bbm",dacs);
	copy(C32stdcolors,C32stdcolors+16,dacs); // use a more 'standard' palette for convert8to32 not the one from the file (slightly off)
	icons = convert8to32(icons8,dacs);
	bitmap8free(icons8);
	dumpbitmap(icons,"icons bitmap");




	seticons();
	initcels();
	initwirelists();
	wirestart.x=-1;
	//setmousexy(620,457);
	showcursor(0);
	mode='n';
	savestr[0] = '\0';
	curicon=0;
}

void digital_proc()
{
	if (quit && !MBUT)
		poporchangestate(STATE_MAINMENU);
		//popstate();
	if (KEY==K_ESCAPE)
		poporchangestate(STATE_MAINMENU);
	curtick++;
	butchange=MBUT&~oldbut;
	processinput();
	oldbut=MBUT;
	oldmx=MX;
	oldmy=MY;
}

void digital_draw2d()
{
	//clipclear32(B32,C32(0,0,255));
	//outtextxyf32(B32,WX/2,WY-16,C32LIGHTRED,"D I G I T A L");
	//video_lock();
	showobjs2();
	drawcursor();
	//video_unlock();
}

void digital_exit()
{
	int i;
	//enabledebprint=enabledebprintsave;
	bitmap32free(arrow);
	logger("icons size = %d,%d\n",icons->size.x,icons->size.y);
	bitmap32free(icons);
	logger("choice size = %d,%d \n",choice->size.x,choice->size.y);
	bitmap32free(choice);
	clearallwirelists();
	for (i=0;i<NUMGATES;i++)
		logger("%3d:  numin %3d, numout %3d, (%3d,%3d), %3d,%3d, %3d,%3d, %3d,%3d, %3d,%3d\n",i,
			gates[i].numinputs,gates[i].numoutputs,gates[i].outputloc.x,gates[i].outputloc.y,
			gates[i].inputlocs[0].x,gates[i].inputlocs[0].y,
			gates[i].inputlocs[1].x,gates[i].inputlocs[1].y,
			gates[i].inputlocs[2].x,gates[i].inputlocs[2].y,
			gates[i].inputlocs[3].x,gates[i].inputlocs[3].y);
	popdir();
	showcursor(1);
	//changeescbehavior(ESC_POP);
	if (loadfiles) {
		//freescript(loadfiles,nloadfiles);
		//loadfiles=NULL;
		delete loadfiles;
		loadfiles=0;
	}
}

#if 0
#include <stdio.h>
#include <graph32\graph32.h>
#include <dos.h>
#include <string.h>




void digload(char *file)
{

}

void digsave(char *file)
{
char str[13];
strcpy(str,file);
strcat(str,".dig");
fileopen(str,WRITE);
filewrite(VERSION,4);
filewritelong(CELX);
filewritelong(CELY);
filewrite(cels,sizeof(struct cel)*CELX*CELY);
fileclose();
}


void clipblitmouse()
{
int blitx,blity;
blitx=mx-160;
if (blitx<0)
	blitx=0;
if (blitx>320)
	blitx=320;
blity=my-100;
if (blity<0)
	blity=0;
if (blity>280)
	blity=280;
clipblit32(&b,&v,blitx,blity,0,0,320,200);
}

int getcommand()
{
int retval;
if (mx<XRES-64)
	return -1;
retval=(mx-XRES+64)/32+my/32*2;
if (retval<16)
	return retval;
switch(retval)
	{
	case 16:	return 'c';
	case 17:	return 'g';
	case 18:	return 'n';
	case 19:	return 'p';
	case 22:	return 'l';
	case 23:	return 's';
	case 28:
	case 29:	return 'q';
	}
return -1;
}

void clearinputs(struct cel *cp)
{
int i;
for (i=0;i<NUMINPUTS;i++)
	{
	clearinput(&cp->inputs[i]);
	cp->logic=0;	// takes care of any stray TOG's
	}
}


void main()
{
int i;
mem_init();
fileopen("arrow.bbm",READ);
iffread(&arrow,0,0,0,0,0,0,0);
fileclose();
fileopen("icons.bbm",READ);
iffread(&icons,0,0,0,0,0,0,0);
fileclose();
seticons();
initcels();
initwirelists();
wirestart.x=-1;

fileopen("choice.bbm",READ);
iffread(&choice,0,0,0,0,0,0,0);
fileclose();
graphics();
initmouse();
make_video_bitmap(&v);
alloc_bitmap(&b,XRES,YRES,-1);
setmousearea(0,0,XRES-1,YRES-1);
setmousexy(620,457);
install_usertick();	// run timer at 60hz, bios clock invalid!
while(!quit)
	{
	curtick=*usertick;
	key=getkey();
	but=getmousexy(&mx,&my);
	butchange=but&~oldbut;
	processinput();
	showobjs2();
	drawcursor();
	waitvb();
	blit();
	oldbut=but;
	oldmx=mx;
	oldmy=my;
	}
closegraph();
free_bitmap(&b);
free_bitmap(&arrow);
printf("icons size = %d,%d\n",icons.size.x,icons.size.y);
free_bitmap(&icons);
printf("choice size = %d,%d \n",choice.size.x,choice.size.y);
free_bitmap(&choice);
clearallwirelists();
for (i=0;i<NUMGATES;i++)
	printf("%3d:  numin %3d, numout %3d, (%3d,%3d), %3d,%3d, %3d,%3d, %3d,%3d, %3d,%3d\n",i,
		gates[i].numinputs,gates[i].numoutputs,gates[i].outputloc.x,gates[i].outputloc.y,
		gates[i].inputlocs[0].x,gates[i].inputlocs[0].y,
		gates[i].inputlocs[1].x,gates[i].inputlocs[1].y,
		gates[i].inputlocs[2].x,gates[i].inputlocs[2].y,
		gates[i].inputlocs[3].x,gates[i].inputlocs[3].y);
uninstall_usertick();	// fix EVERYTHING!, do this before disk acess
}
#endif
#endif
