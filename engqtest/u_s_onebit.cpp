#include <m_eng.h>

namespace {
	
C32 dacs[256];
bitmap32* cuts[10];
bitmap32* cbm;
bitmap32* big;
S32 lastcut;
S32 curcut;
S32 scramx,scramy=1,flipb=1;
S32 whitebits[16+7]={
    6,
    2,
    3,
    4,
    4,
    4,
    5,
    5,
    5,
    5,
    5,
    5,
    6,
    6,
    6,
    6,
	
    3,
    5,
    7,
    9,
   10,
   10,
    8,
};
S32 blackbits[16+7]={
   12,
    1,
    2,
    3,
    4,
    6,
    6,
    7,
    7,
    8,
    8,
    9,
   10,
   10,
   12,
   10,

   11,
   11,
   12,
   12,
   12,
   12,
   11,
};

S32* whiteruns;
S32* blackruns;


void hammerpalette()
{
	S32 i;
	for (i=2;i<256;++i)
		dacs[i]=C32LIGHTMAGENTA;
	dacs[0]=C32WHITE;
	dacs[1]=C32BLACK;
}

void drawpalette()
{
	S32 i,j;
	for (j=0;j<16;++j) {
		for (i=0;i<16;++i) {
			cliprect32(B32,20+(i<<3),300+(j<<3),7,7,dacs[(j<<4)+i]);
		}
	}
}

void flipborder(bitmap32* cc)
{
	S32 i,j;
	U32 f=C32WHITE.c32^C32BLACK.c32;
	for (i=0;i<cc->size.x;++i)
		clipputpixel32(cc,i,0,clipgetpixel32(cc,i,0).c32^f);
	for (i=0;i<cc->size.x;++i)
		clipputpixel32(cc,i,cc->size.y-1,clipgetpixel32(cc,i,cc->size.y-1).c32^f);
	for (j=1;j<cc->size.y-1;++j)
		clipputpixel32(cc,0,j,clipgetpixel32(cc,0,j).c32^f);
	for (j=1;j<cc->size.y-1;++j)
		clipputpixel32(cc,cc->size.x-1,j,clipgetpixel32(cc,cc->size.x-1,j).c32^f);
}

S32 findrun(C32 *d,S32 sz,C32 col)
{
	S32 ret=0;
	while(ret<sz && col==d[ret])
		++ret;
	return ret;
}

void scramblepicy(bitmap32* cc)
{
	S32 i,j;
	for (j=cc->size.y-2;j>=0;--j) {
		for (i=0;i<cc->size.x;++i) {
			if (clipgetpixel32(cc,i,j)==clipgetpixel32(cc,i,j+1))
				clipputpixel32(cc,i,j+1,C32WHITE);
			else
				clipputpixel32(cc,i,j+1,C32BLACK);
		}
	}
}

void scramblepicx(bitmap32* cc)
{
	S32 i,j;
	for (i=cc->size.x-2;i>=0;--i) {
		for (j=0;j<cc->size.y;++j) {
			if (clipgetpixel32(cc,i,j)==clipgetpixel32(cc,i+1,j))
				clipputpixel32(cc,i+1,j,C32WHITE);
			else
				clipputpixel32(cc,i+1,j,C32BLACK);
		}
	}
}

void unscramblepicy(bitmap32* cc)
{
	S32 i,j;
	for (j=0;j<cc->size.y-1;++j) {
		for (i=0;i<cc->size.x;++i) {
			if (clipgetpixel32(cc,i,j)==clipgetpixel32(cc,i,j+1))
				clipputpixel32(cc,i,j+1,C32WHITE);
			else
				clipputpixel32(cc,i,j+1,C32BLACK);
		}
	}
}

void unscramblepicx(bitmap32* cc)
{
	S32 i,j;
	for (i=0;i<cc->size.x-1;++i) {
		for (j=0;j<cc->size.y;++j) {
			if (clipgetpixel32(cc,i,j)==clipgetpixel32(cc,i+1,j))
				clipputpixel32(cc,i+1,j,C32WHITE);
			else
				clipputpixel32(cc,i+1,j,C32BLACK);
		}
	}
}

void analyze(bitmap32* cc)
{
	bitmap32* cop=bitmap32copy(cc);
	if (flipb)
		flipborder(cop);
	if (scramx)
		scramblepicx(cop);
	if (scramy)
		scramblepicy(cop);
//	S32 i;
//	cliprect32(cop,0,0,64,64,C32WHITE);
//	for (i=0;i<64;++i)
//		clipputpixel32(cop,i,i,C32BLACK);
	bitmap32* t=bitmap32double(cop);
	if (big)
		bitmap32free(big);
	big=bitmap32double(t);
	bitmap32free(t);


	C32* cd=cop->data;
	S32 size=cop->size.x*cop->size.y;
	
	C32 curcolor=C32WHITE;
	S32 p=0;
	S32 huffbits=0;
	while(p<size) {
		S32 run=findrun(cd+p,size-p,curcolor);
		S32 orun=run;
		if (curcolor==C32WHITE) {
			++whiteruns[run];
//			logger("found white run of %d\n",run);
			S32 hb=0;
			while (run>=112) {
				hb+=whitebits[22]; // white 112
				run-=112;
			}
			S32 d=run/16;
			S32 m=run%16;
			if (d>0)
				hb+=whitebits[d];
			hb+=whitebits[m];
			huffbits+=hb;
//			logger("  %d bits\n",hb); 
		} else {
			++blackruns[run];
//			logger("found black run of %d\n",run);
			S32 hb=0;
			while (run>=112) {
				hb+=blackbits[22];
				run-=112;
			}
			S32 d=run/16;
			S32 m=run%16;
			if (d>0)
				hb+=blackbits[d];
			hb+=blackbits[m];
			huffbits+=hb;
//			logger("  %d bits\n",hb); 
		}
		curcolor=curcolor.c32^C32WHITE.c32^C32BLACK.c32;
		p+=orun;
	}
	bitmap32free(cop);
	logger("%d/%d\n",huffbits,4096);
	gcon_printf("%d/%d\n",huffbits,4096);
}

void loadandcutthem()
{
	struct bitmap32* bs;
	struct bitmap8* b8;
	pushandsetdir("1bitpics");
	b8=gfxread8("10sq_all.pcx",dacs);
	popdir();
	hammerpalette();
	S32 sv=colorkeyinfo.lasthascolorkey;
	colorkeyinfo.usecolorkey=0;
	bs=convert8to32(b8,dacs);
	bitmap8free(b8);
	colorkeyinfo.usecolorkey=sv;
	S32 i,j;
	for (j=0;j<2;++j) {
		for (i=0;i<5;++i) {
			S32 k=j*5+i;
			cuts[k]=bitmap32alloc(64,64,C32BLACK);
			clipblit32(bs,cuts[k],64*i,64*j,0,0,64,64);
		}
	}
	bitmap32free(bs);
}

struct huffnode {
	S32 val;
	S32 freq;
	S32 parent;
	S32 leftc,rightc;
	S32 lev;
	S32 nbits;
};

void showbitcodes(huffnode* h,S32 idx,S32 lev)
{
	static C8 bitstr[50];
	if (h[idx].leftc>=0) {
		bitstr[lev]='0';
		bitstr[lev+1]='\0';
		showbitcodes(h,h[idx].leftc,lev+1);
		bitstr[lev]='1';
		bitstr[lev+1]='\0';
		showbitcodes(h,h[idx].rightc,lev+1);
	} else {
		h[idx].nbits=lev;
		logger("node %3d, bits %d '%s'\n",idx,lev,bitstr);
	}
}

void buildhuff(huffnode* ohn,S32 nohn)
{
	S32 i,j;
	S32 maxn=2*nohn-1;
	huffnode* hlist=new huffnode[maxn];
	struct huffnode zer={-1,-1,-1,-1,-1,0};
	S32 nhlist=nohn;
	for (i=0;i<maxn;++i)
		hlist[i]=zer;
	for (i=0;i<nohn;++i) {
		hlist[i].val=ohn[i].val;
		hlist[i].freq=ohn[i].freq;
	}
	S32 bidx[2];
	while(1) {
		for (j=0;j<2;++j) {
			S32 bi=-1;
			for (i=0;i<nhlist;++i)
				if (hlist[i].parent==-1)
					if (bi==-1 || (hlist[i].freq==hlist[bi].freq && hlist[i].lev<hlist[bi].lev) || hlist[i].freq<hlist[bi].freq)
						bi=i;
			if (bi>=0) {
				hlist[bi].parent=0;
				bidx[j]=bi;
			} else {
				break;
			}
		}
		if (j==2) {
			logger("bidx[0] = %d, bidx[1]= %d\n",bidx[0],bidx[1]);
			hlist[nhlist].val=-1;
			hlist[nhlist].freq=hlist[bidx[0]].freq+hlist[bidx[1]].freq;
			hlist[nhlist].lev=1+max(hlist[bidx[0]].lev,hlist[bidx[1]].lev);
			hlist[nhlist].rightc=bidx[0];
			hlist[nhlist].leftc=bidx[1];
			hlist[bidx[0]].parent=nhlist;
			hlist[bidx[1]].parent=nhlist;
			++nhlist;
		} else {
			hlist[nhlist-1].parent=-1;
			break;
		}
	}
	showbitcodes(hlist,nhlist-1,0);
	for (i=0;i<nhlist;++i)
		logger("%3d: val %3d, freq %4d, lev %d, bits %d, par %3d, leftc %3d, rightc %3d\n",
			i,hlist[i].val,hlist[i].freq,hlist[i].lev,hlist[i].nbits,hlist[i].parent,hlist[i].leftc,hlist[i].rightc);
	for (i=0;i<nohn;++i)
		logger("%2d,\n",hlist[i].nbits);
	delete[] hlist;
}

}

void onebitinit()
{
	video_setupwindow(640,480);
	loadandcutthem();
	lastcut=-1;
	gcon=con32_alloc(320,240,C32WHITE,C32BLACK);
	cbm=con32_getbitmap32(gcon);
	whiteruns=new S32[4096+1];
	blackruns=new S32[4096+1];
	memset(whiteruns,0,sizeof(S32)*(4096+1));
	memset(blackruns,0,sizeof(S32)*(4096+1));
}

void onebitproc()
{
	switch(KEY) {
	case K_ESCAPE:
		popstate();
		break;
	case K_RIGHT:
		++curcut;
		break;
	case K_LEFT:
		--curcut;
		break;
	case 'b':
		flipb^=1;
		lastcut=-1;
		break;
	case 'x':
		scramx^=1;
		lastcut=-1;
		break;
	case 'y':
		scramy^=1;
		lastcut=-1;
		break;
	}
	if (curcut<0)
		curcut+=10;
	if (curcut>=10)
		curcut-=10;
	if (curcut!=lastcut) {
		bitmap32* cc=cuts[curcut];
		gcon_printf("analyze cut %d\n",curcut);
		logger("analyze cut %d\n",curcut);
		analyze(cc);
		lastcut=curcut;
	}
}

void onebitdraw2d()
{
//	video_lock();
	clipclear32(B32,C32LIGHTBLUE);	
	outtextxyf32(B32,10,WY-8,C32BLACK,"ONE BIT PIXEL COMPRESSION curcut = %d",curcut);
	if (big)
		clipblit32(big,B32,0,0,30,30,big->size.x,big->size.y);
	clipblit32(cbm,B32,0,0,290,30,cbm->size.x,cbm->size.y);
	drawpalette();
//	video_unlock();
}

void onebitexit()
{
	S32 i,j;
	for (i=0;i<10;++i)
		bitmap32free(cuts[i]);
	con32_free(gcon);
	gcon=0;
	if (big) {
		bitmap32free(big);
		big=0;
	}
	logger("========== cumulative runs of all pics analyzed ==========\n");
	huffnode whitenodes[23]={
		{0,0},
		{1,0},
		{2,0},
		{3,0},
		{4,0},
		{5,0},
		{6,0},
		{7,0},
		{8,0},
		{9,0},
		{10,0},
		{11,0},
		{12,0},
		{13,0},
		{14,0},
		{15,0},
		{16,0},
		{32,0},
		{48,0},
		{64,0},
		{80,0},
		{96,0},
		{112,0}
	};
	huffnode blacknodes[23]={
		{0,0},
		{1,0},
		{2,0},
		{3,0},
		{4,0},
		{5,0},
		{6,0},
		{7,0},
		{8,0},
		{9,0},
		{10,0},
		{11,0},
		{12,0},
		{13,0},
		{14,0},
		{15,0},
		{16,0},
		{32,0},
		{48,0},
		{64,0},
		{80,0},
		{96,0},
		{112,0}
	};
	logger("white 0 to 15\n");
	S32 c;
	S32 d=1;
	for (j=0;j<16;++j) {
		c=0;
		for (i=j;i<=4096;i+=16)
			c+=whiteruns[i];
		if (!c)
			c+=d;
		if (c) {
			logger("%4d white runs of len %4d\n",c,j);
			whitenodes[j].freq+=c;
		}
	}
	logger("white mult of 16\n");
	for (j=16;j<112;j+=16) {
		c=0;
		for (i=0;i<16;++i)
			c+=whiteruns[i+j];
		if (!c)
			c+=d;
		if (c) {
			logger("%4d white runs of len %4d\n",c,j);
			whitenodes[j/16+16-1].freq+=c;
		}
	}
	c=0;
	for (j=112;j<=4096;++j) {
		c+=whiteruns[j];
	}
	if (!c)
		c+=d;
	if (c) {
		logger("%4d white runs of >=112\n",c,j);
		whitenodes[22].freq+=c;
	}
	logger("black 0 to 15\n");
	for (j=0;j<16;++j) {
		c=0;
		for (i=j;i<=4096;i+=16)
			c+=blackruns[i];
		if (!c)
			c+=d;
		if (c) {
			logger("%4d black runs of len %4d\n",c,j);
			blacknodes[j].freq+=c;
		}
	}
	logger("black mult of 16\n");
	for (j=16;j<112;j+=16) {
		c=0;
		for (i=0;i<16;++i)
			c+=blackruns[i+j];
		if (!c)
			c+=d;
		if (c) {
			logger("%4d black runs of len %4d\n",c,j);
			blacknodes[j/16+16-1].freq+=c;
		}
	}
	c=0;
	for (j=112;j<=4096;++j) {
		c+=blackruns[j];
	}
	if (!c)
		c+=d;
	if (c) {
		logger("%4d black runs of >=112\n",c,j);
		blacknodes[22].freq+=c;
	}
	logger("========== huffman builder ==========\n");
	huffnode test1[]={
		{0,2},
		{1,2},
		{2,2},
		{3,2},
		{4,4},
	};
	buildhuff(test1,sizeof(test1)/sizeof(test1[0]));
	huffnode test2[]={
		{0,2},
		{1,20},
		{2,200},
		{3,2000},
		{4,20000},
		{5,200000},
		{6,2000000},
		{7,20000000},
	};
	buildhuff(test2,sizeof(test2)/sizeof(test2[0]));
	buildhuff(whitenodes,23);
	buildhuff(blacknodes,23);
	delete[] whiteruns;
	delete[] blackruns;
}
