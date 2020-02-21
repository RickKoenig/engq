#include <graph32\graph32.h>

#define MAXNUMCIRCLES 16
#define MAXMOVES 16
int numcircles;
int speed=35;

/////// legal move table /////////
int movetable[MAXNUMCIRCLES][MAXNUMCIRCLES];

///////// board position's //////////
enum {B,X,O};
int startposition[MAXNUMCIRCLES];

int position[MAXNUMCIRCLES];

enum {NOTSET,WIN,LOSE};

int onepiece=1;

struct strategy
	{
	char nummoves;
	unsigned long id;
	int next[MAXMOVES];
	char status;
	unsigned short winmoves;
	unsigned short losemoves;
	short back;
	};
int numpositions;
#define MAXPOSITIONS 100000
struct strategy positions[MAXPOSITIONS];

struct point xypos[MAXNUMCIRCLES];	// graphic positions of pieces

///////// turn control ////////////
enum {COMPUTER,HUMAN};
int gamenum=1;
int players[2]={HUMAN,COMPUTER};
int wins[2];
char *turnmessage[2]={"Rat's turn","Scorp's turn"};
char *losemessage[2]={"Rat loses!","Scorp loses!"};
int turn=0;
int piecesel=-1;
int startpiece=-1,endpiece;

////////// globals ////////
struct bitmap b,v,d,a,x,o;
unsigned char dacs[256][3];
int key,but,oldbut=0,mx,my;

int findclose()
{
int dist,bdist=400;
int n,bn=-1;
for (n=0;n<numcircles;n++)
	{
	dist=(xypos[n].x-mx)*(xypos[n].x-mx)+(xypos[n].y-my)*(xypos[n].y-my);
	if (dist<bdist)
		{
		bdist=dist;
		bn=n;
		}
	}
return bn;
}

int checkmove(int st,int end)
{
return	position[st]==X &&
	    	position[end]==B &&
			movetable[end][st];
return 0;
}

enum {GETPIECE,MOVEPIECE,ENDTURN};
int winning;
char *wm[3]={"","WINNING","LOSING"};

unsigned long getid()
{
unsigned long id=0;
int i;
for (i=0;i<numcircles;i++)
	id+=position[i]<<(i<<1);
return id;
}

void showboard(int pce,int s)
{
int n,i,id;
char str[40];
char *ss[3]={"getp","movep","endturn"};
struct bitmap *p;
clipblit(&d,&b,0,0,0,0,XSIZE,YSIZE);
outtextxy(&b,0,0,turnmessage[turn],white);
outtextxy(&b,0,24,wm[winning],white);
for (n=0;n<numcircles;n++)
	{
	if (turn==0)
		if (position[n]==X)
			p=&x;
		else
			p=&o;
	else
		if (position[n]==X)
			p=&o;
		else
			p=&x;
	if ((position[n]==X || position[n]==O) && n!=pce)
		clipxpar(p,&b,0,0,xypos[n].x-p->size.x/2,xypos[n].y-2*p->size.y/3,XSIZE,YSIZE,black);
	}
if (pce>=0)
	{
	if (turn==0)
		p=&x;
	else
		p=&o;
	clipxpar(p,&b,0,0,mx-p->size.x/2,my-2*p->size.y/3,XSIZE,YSIZE,black);
	}
clipxpar(&a,&b,0,0,mx,my,XSIZE,YSIZE,black);
outtextxy(&b,0,8,ss[s],white);
id=getid();
for (i=0;i<numpositions;i++)
	if (positions[i].id==id)
		break;
n=positions[i].back;
sprintf(str,"Score: Rat %3d    Scorp %3d # %d",wins[0],wins[1],n);
outtextxy(&b,0,16,str,white);
clipblit(&b,&v,0,0,0,0,XSIZE,YSIZE);
}

void movepiece(int st,int end)
{
position[st]=B;
position[end]=X;
}

int flip[3]={B,O,X};
void changeturn(int nt)
{
int n;
if (turn==nt)
	return;
if (!onepiece)
	for (n=0;n<numcircles;n++)
		position[n]=flip[position[n]];
turn=nt;
}

int canmove()
{
int s,e;
for (s=0;s<numcircles;s++)
	for (e=0;e<numcircles;e++)
		if (checkmove(s,e))
			return 1;
return 0;
}

int idpos[MAXNUMCIRCLES];
int newidpos[MAXNUMCIRCLES];

void breakid(unsigned long id,int *pos)
{
int i;
for (i=0;i<numcircles;i++)
	pos[i]=(id>>(i<<1))&3;
}

int getcompturn(int *s,int *e)
{
int mv,i,j;
int *next;
int newid;
int id;
int best=0;
id=getid();
for (i=0;i<numpositions;i++)
	if (positions[i].id==id)
		break;
next=positions[i].next;
if (positions[i].status==WIN)
	{
	for (mv=0;mv<positions[i].nummoves;mv++)
		{
		if (positions[next[mv]].back>best && positions[next[mv]].status==LOSE)
			best=positions[next[mv]].back;
		}
	while(1)
		{
		mv=random(positions[i].nummoves);
		if (positions[next[mv]].status==LOSE && positions[next[mv]].back==best)
			break;
		} 
	}
else if (positions[i].status==LOSE)
	{
	best=30000;
	for (mv=0;mv<positions[i].nummoves;mv++)
		{
		if (positions[next[mv]].back<best)
			best=positions[next[mv]].back;
		}
	while(1)
		{
		mv=random(positions[i].nummoves);
		if (positions[next[mv]].back==best)
			break;
		}
	}
else
	while(1)
		{
		mv=random(positions[i].nummoves);
		if (positions[next[mv]].status!=WIN)
			break;
		}
newid=positions[next[mv]].id;
breakid(id,idpos);
breakid(newid,newidpos);
for (j=0;j<numcircles;j++)
	{
	if (idpos[j]!=B && newidpos[j]==B)
		*s=j;
	if (idpos[j]==B && newidpos[j]!=B)
		*e=j;
	}
return positions[i].status;
}

/*
void getcompturn(int *s,int *e)
{
while(1)
	{
	*s=random(numcircles);
	*e=random(numcircles);
	if (checkmove(*s,*e))
		break;
	}
return;
}
*/

void showcompturn(int s,int e)
{
int i;
for (i=0;i<speed;i++)
	{
	mx=((speed-i)*xypos[s].x+i*xypos[e].x)/speed;
	my=((speed-i)*xypos[s].y+i*xypos[e].y)/speed;
	showboard(startpiece,MOVEPIECE);
	waitvb();
	}
}

int getmove()
{
int state=GETPIECE;
startpiece=-1;
showboard(startpiece,0);
key=getkey();
but=getmousexy(&mx,&my);
if (key==K_ESC || key=='q')
	return 0;
if (!canmove())
	{
	wins[turn^1]++;
	showboard(startpiece,0);
	outtextxy(&v,50,50,losemessage[turn],white);
	delay(750);
	initboard();
	changeturn(gamenum>=5);
	gamenum++;
	if (gamenum==10)
		gamenum=0;
	return 1;
	}
if (players[turn]==COMPUTER)
	{
	winning=getcompturn(&startpiece,&endpiece);
	showcompturn(startpiece,endpiece);
	winning=0;
	movepiece(startpiece,endpiece);
	changeturn(turn^1);
	return 1;
	}
while (1)
	{
	key=getkey();
	but=getmousexy(&mx,&my);
	if (key==K_ESC || key=='q')
		return 0;
	if (key=='r')
		{
		initboard();
		changeturn(random(2));
		return 1;
		}
	if (key=='t')
		{
		changeturn(turn^1);
		return 1;
		}
	switch (state)
		{
		case GETPIECE:	if (but&!oldbut)
								{
								startpiece=findclose();
								if (startpiece>=0 &&
									 position[startpiece]==X)
									state=MOVEPIECE;
								else
									startpiece=-1;
								}
							break;
		case MOVEPIECE:if (!but)
								{
								endpiece=findclose();
							  	if (endpiece>=0)
									{
									if (checkmove(startpiece,endpiece))
										{
										movepiece(startpiece,endpiece);
										state=ENDTURN;
										}
									else
										{
										state=GETPIECE;
										startpiece=-1;
										}
									}
								}
							break;
		case ENDTURN:	changeturn(turn^1);
							return 1;
		}
	showboard(startpiece,state);
	oldbut=but;
	}
}
	
void initboard()
{
move32(startposition,position,sizeof(position));
turn=0;
}

void loadconfig(char *s)
{
int i,j,v,c,cnt;
fileopen(s,READ);
while(1)
	{
	v=filereadbyte();
	if (v=='X' || v=='O' || v=='B')
		{
		if (numcircles>=MAXNUMCIRCLES)
			{
			printf("too many circles, aborting..\n");
			exit(1);
			}
		switch(v)
			{
			case 'X':	c=X;
							break;
			case 'O':	c=O;
							onepiece=0;
							break;
			case 'B':	c=B;
							break;
			}
		startposition[numcircles++]=c;
		}
	else if (v=='1' || v=='0')
		break;
	}
fileskip(-1,FILE_CURRENT);
for (j=0;j<numcircles;j++)
	for (i=0;i<numcircles;i++)
		{
		cnt=0;
		while(1)
			{
			v=filereadbyte()-'0';
			if (v==0 || v==1)
				break;
			cnt++;
			if (cnt==100)
				{
				printf("problem with dat table\n");
				exit(1);
				}
			}
		movetable[i][j]=v;
		}
fileclose();
}

void loadboard(char *s)
{
#define HOTCOLOR 255
int i,j,n=0;
make_video_bitmap(&v);
alloc_bitmap(&b,XSIZE,YSIZE,-1);
fileopen(s,READ);
iffread(&d,dacs,0,0,0,0,0,0);
fileclose();
fileopen("arrow.bbm",READ);
iffread(&a,0,0,0,0,0,0,0);
fileclose();
fileopen("x.bbm",READ);
iffread(&x,0,0,0,0,0,0,0);
fileclose();
fileopen("o.bbm",READ);
iffread(&o,0,0,0,0,0,0,0);
fileclose();
for (j=0;j<YSIZE;j++)
	for (i=0;i<XSIZE;i++)
		if (clipgetpixel(&d,i,j)==HOTCOLOR)
			{
			if (n>=numcircles)
				{
				printf("lbm %d+? had more points than dat %d\n",n,numcircles);
				exit(1);
				}
			clipputpixel(&d,i,j,clipgetpixel(&d,i-1,j));
			xypos[n].x=i;
			xypos[n].y=j;
			n++;
			}
if (numcircles!=n)
	{
	printf("lbm %d had less points than dat %d\n",n,numcircles);
	exit(1);
	}
}

int registerposition(int *pos)
{
int i;
int id;
id=getid();
for (i=0;i<numpositions;i++)
	if (positions[i].id==id)
		{
		*pos=i;
		return 0;
		}
if (numpositions>=MAXPOSITIONS)
	{
	printf("too many board positions\n");
	exit(1);
	}
positions[numpositions].id=id;
numpositions++;
if (numpositions%25==0)
	printf("numpositions = %d\n",numpositions);
*pos=i;
return 1;
}

int studygame()
{
int s,e,pos,nm,np;
if (!registerposition(&pos))
	return pos;
if (stackavail()<1000)
	{
	printf("out of stack! numboards = %d\n",numpositions);
	exit(1);
	}
np=numpositions-1;	// a new position will be studied further
nm=0;
for (s=0;s<numcircles;s++)
	for (e=0;e<numcircles;e++)
		if (checkmove(s,e))
			{
			movepiece(s,e);
			changeturn(turn^1);
			nm=positions[np].nummoves++;
			positions[np].next[nm]=studygame();
			changeturn(turn^1);
			movepiece(e,s);
			}
/* if (nm==0)
	positions[np].status=LOSE; */
return pos;
}

/* void deb()
{
printf("-");
}	*/

void filltree()
{
int np,nm,w,l,stat,change,bck,nxt;
int *next;
do
	{
	change=0;
	for (np=0;np<numpositions;np++)
		{
		w=l=0;
		stat=LOSE;
		next=positions[np].next;
		for (nm=0;nm<positions[np].nummoves;nm++)
			switch(positions[next[nm]].status)
				{
				case LOSE:	w+=1<<nm;
								stat=WIN;
								break;
				case WIN:	l+=1<<nm;
								break;
				default:		if (stat!=WIN)
									stat=NOTSET;
								break;
				}
		if (nm==0)
			positions[np].back=30000;
		if (w!=positions[np].winmoves || l!=positions[np].losemoves ||
													  positions[np].status!=stat)
			{
			positions[np].winmoves=w;
			positions[np].losemoves=l;
			positions[np].status=stat;
			change=1;
			}
		}
	}
  while (change);
do
	{
	change=0;
	for (np=0;np<numpositions;np++)
		{
		stat=positions[np].status;
		next=positions[np].next;
		if (stat==WIN)
			{
			bck=0;
			for (nm=0;nm<positions[np].nummoves;nm++)
				{
/*				if (np==6)
					deb();
				if (np==7)
					deb(); */
				if (positions[next[nm]].status==LOSE)
					{
					nxt=positions[next[nm]].back;
					bck=max(bck,nxt-1);
					}
				}
			}
		else if (stat==LOSE)
			{
			bck=30000;
/*			if (np==9)
				deb(); */
			for (nm=0;nm<positions[np].nummoves;nm++)
				{
				if (positions[next[nm]].status==WIN)
					{
					nxt=positions[next[nm]].back;
					if (nxt)
						bck=min(bck,nxt-1);
					}
				}
			}
		if (positions[np].back!=bck)
			{
			positions[np].back=bck;
			change=1;
			}
		}
	}
  while (change);
}

void main(int argc,char **argv)
{
char s[30];
int i,j;
while(argc>1 && argv[1][0]=='-')
	{
	if (argv[1][1]=='c')
		players[0]=COMPUTER;
	else if (argv[1][1]=='h')
		players[1]=HUMAN;
	else if (argv[1][1]=='f')
		speed=atoi(&argv[1][2]);
	else
		{
		printf("unknown option -%c\n",argv[1][1]);
		exit(1);
		}
	argc--;
	argv++;
	}
if (argc!=2)
	{
	printf("usage: h [-h] [-c] [-fnn] board1\n");
	printf("reads in board1.lbm and board1.dat\n");
        printf("options are 2 humans or 2 compplayers, if h and c then switch sides\n");
	printf("and frame rate\n");
	exit(1);
	}
mem_init();
randomize();
sprintf(s,"%s.dat",argv[1]);
loadconfig(s);
sprintf(s,"%s.lbm",argv[1]);
loadboard(s);

if (players[0]==COMPUTER || players[1]==COMPUTER)
	{
	initboard();
	studygame();

	initboard();
	changeturn(1);
	studygame();
	}
filltree();
initboard();
initgraph();
delay(1000);
initmouse();
setalldac(dacs);
while(1)
	{
	if (!getmove())
		break;
	}
closegraph();
free_bitmap(&b);
free_bitmap(&d);
free_bitmap(&a);
free_bitmap(&x);
free_bitmap(&o);
/* printf("sizeof positions = %d\n",sizeof(positions));
printf("# of board positions = %d\n",numpositions);
for (i=0;i<numpositions;i++)
	{
	printf("******** position # %d *******\n",i);
	printf("\tnummoves = %d, stat = %d, winm = %d, losem = %d, back = %d\n",
		positions[i].nummoves,positions[i].status,positions[i].winmoves,
			positions[i].losemoves,positions[i].back);
	printf("\tnextmoves =");
	for (j=0;j<positions[i].nummoves;j++)
		printf(" %d",positions[i].next[j]);
	printf("\n");
	} */
}

