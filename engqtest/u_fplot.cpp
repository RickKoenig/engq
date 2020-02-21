#include <m_eng.h>
#include "u_fplot.h"

// fplot class

fplot::fplot(S32 xs0a,S32 ys0a, S32 xswa,S32 ysha,
			 float x0a,float y0a,float x1a,float y1a,
			 const bitmap32* da,const C8* xlabela,const C8* ylabela,const C32* ycolorsa) :
	xs0(xs0a),ys0(ys0a),xsw(xswa),ysh(ysha),
		x0(x0a),y0(y0a),x1(x1a),y1(y1a),
		d(da),xlabel(xlabela),ylabel(ylabela),ycolors(ycolorsa),
		lastline(false),lx(0),ly(0)
{
	mx=(xsw-1)/(x1-x0);
	my=-(ysh-1)/(y1-y0);
	bx=xs0-(xsw-1)*x0/(x1-x0);
	by=ys0+(ysh-1)*y1/(y1-y0);
}

void fplot::f2s(float cxa,float cya,S32* cxsa,S32* cysa)
{
	*cxsa=(S32)(mx*cxa+bx);
	*cysa=(S32)(my*cya+by);
}

void fplot::drawaxis()
{
	cliprect32(d,xs0,ys0,xsw,ysh,C32BLACK);
	fline(x0,0,x1,0,C32WHITE);
	fline(0,y0,0,y1,C32WHITE);
}

void fplot::drawlabels()
{
	S32 cx,cy;
	f2s(x1,0.0,&cx,&cy);
	cx-=4+8*strlen(xlabel);
	cy-=12;
	outtextxy32(d,cx,cy,C32WHITE,xlabel);
	f2s(0.0,y1,&cx,&cy);
	cx+=4;
	cy+=4;
	if (ycolors) {
		 S32 i=0;
		 while(ylabel[i]) {
 			outchar32(d,cx,cy,ycolors[i],ylabel[i]);
			cx+=8;
			 ++i;
		 }
	} else {
		outtextxy32(d,cx,cy,C32WHITE,ylabel);
	}
}

void fplot::fline(float x0a,float y0a,float x1a,float y1a,C32 c)
{
	S32 cx0,cy0,cx1,cy1;
	f2s(x0a,y0a,&cx0,&cy0);
	f2s(x1a,y1a,&cx1,&cy1);
	clipline32(d,cx0,cy0,cx1,cy1,c);
}

void fplot::startlinev()
{
	lastline=false;
}

void fplot::flinev(float xa,float ya,C32 c)
{
	if (lastline) {
		fline(lx,ly,xa,ya,c);
	} else {
		lastline=true;
	}
	lx=xa;
	ly=ya;
}

// fplot 3d class

fplot3d::fplot3d(S32 xs0a,S32 ys0a, S32 xswa,S32 ysha,
			 float x0a,float y0a,float z0a,float x1a,float y1a,float z1a,
			 const bitmap32* da,const C8* xlabela,const C8* ylabela,const C8* zlabela,
			 float pitcha,float yawa) :
		xs0(xs0a),ys0(ys0a),xsw(xswa),ysh(ysha),
		x0(x0a),y0(y0a),z0(z0a),x1(x1a),y1(y1a),z1(z1a),
		d(da),xlabel(xlabela),ylabel(ylabela),zlabel(zlabela),
		lastline(false),lx(0),ly(0),lz(0)
{
	mx=(xsw-1)/(x1-x0);
	my=-(ysh-1)/(y1-y0);
	bx=xs0-(xsw-1)*x0/(x1-x0);
	by=ys0+(ysh-1)*y1/(y1-y0);
	ps=sinf(pitcha*PIOVER180);
	pc=cosf(pitcha*PIOVER180);
	ys=sinf(yawa*PIOVER180);
	yc=cosf(yawa*PIOVER180);
}

void fplot3d::f2s(float cxa,float cya,float cza,S32* cxsa,S32* cysa)
{
// yaw
	float t0x= cxa*yc+cza*ys;
	float t0y= cya;
	float t0z=-cxa*ys+cza*yc;
// pitch
	float t1x= t0x;
	float t1y= t0y*pc-t0z*ps;
	*cxsa=(S32)(mx*t1x+bx);
	*cysa=(S32)(my*t1y+by);
}

void fplot3d::drawaxis()
{
	cliprect32(d,xs0,ys0,xsw,ysh,C32BLACK);
	fline(x0,0,0,x1,0,0,C32WHITE);
	fline(0,y0,0,0,y1,0,C32WHITE);
	fline(0,0,z0,0,0,z1,C32WHITE);
}

void fplot3d::drawlabels()
{
	S32 cx,cy;
// do x
	f2s(x1,0.0,0.0,&cx,&cy);
	cx-=4+8*strlen(xlabel);
	cy-=12;
	outtextxy32(d,cx,cy,C32WHITE,xlabel);
// do y
	f2s(0.0,y1,0.0,&cx,&cy);
	cx+=4;
	cy+=4;
	outtextxy32(d,cx,cy,C32WHITE,ylabel);
// do z
	f2s(0.0,0.0,z1,&cx,&cy);
	cx+=4;
	cy-=12;
	outtextxy32(d,cx,cy,C32WHITE,zlabel);
}

void fplot3d::fline(float x0a,float y0a,float z0a,float x1a,float y1a,float z1a,C32 c,float phasehilight)
{
	S32 cx0,cy0,cx1,cy1;
	f2s(x0a,y0a,z0a,&cx0,&cy0);
	f2s(x1a,y1a,z1a,&cx1,&cy1);
	if (phasehilight) {
		pointf3 fc=C32topointf3(c);
		float cx=.5f*(x0a+x1a);
		float cy=.5f*(y0a+y1a);
		float dr=cx*phasehilight;
		float dg=cy*phasehilight;
		fc.x+=dr;
		fc.y+=dg;
		fc.x=range(0.0f,fc.x,1.0f);
		fc.y=range(0.0f,fc.y,1.0f);
		c=pointf3toC32(&fc);
	}
	clipline32(d,cx0,cy0,cx1,cy1,c);
}

void fplot3d::startlinev()
{
	lastline=false;
}

void fplot3d::flinev(float xa,float ya,float za,C32 c,float p)
{
	if (lastline) {
		fline(lx,ly,lz,xa,ya,za,c,p);
	} else {
		lastline=true;
	}
	lx=xa;
	ly=ya;
	lz=za;
}
