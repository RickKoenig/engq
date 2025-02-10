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
	fline(x0,0,0,x1,0,0,F32WHITE, 0);
	fline(0,y0,0,0,y1,0,F32WHITE, 0);
	fline(0,0,z0,0,0,z1,F32WHITE, 0);
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

void fplot3d::fline(float x0a,float y0a,float z0a,float x1a,float y1a,float z1a, pointf3 startCol, float gain)
{
	S32 cx0,cy0,cx1,cy1;
	f2s(x0a,y0a,z0a,&cx0,&cy0);
	f2s(x1a,y1a,z1a,&cx1,&cy1);
	float cr = .5f*(x0a + x1a);
	float ci = .5f*(y0a + y1a);
	C32 col = fplot3d::makeComplexColor(cr, ci, startCol, gain);
	clipline32(d,cx0,cy0,cx1,cy1,col);
}

void fplot3d::startlinev()
{
	lastline=false;
}

void fplot3d::flinev(float xa,float ya,float za, pointf3 startCol, float gain)
{
	if (lastline) {
		fline(lx,ly,lz,xa,ya,za,startCol,gain);
	} else {
		lastline=true;
	}
	lx=xa;
	ly=ya;
	lz=za;
}

#define NEW_MAKE_COLOR
#ifdef NEW_MAKE_COLOR
C32 fplot3d::makeComplexColor(float real, float imag, pointf3 fColor, float gain)
{
	if (!gain) {
		C32 col = pointf3toC32(&fColor);
		return col;
	}

	float mag = gain * sqrt(real * real + imag * imag);
	float ang = atan2(imag, real);
	float red = mag * cos(ang);
	float green = mag * cos(ang - 2 * PI / 3);
	float blue = mag * cos(ang + 2 * PI / 3);
	fColor.x += red;
	fColor.y += green;
	fColor.z += blue;
	fColor.x = range(0.0f, fColor.x, 1.0f);
	fColor.y = range(0.0f, fColor.y, 1.0f);
	fColor.z = range(0.0f, fColor.z, 1.0f);
	C32 col = pointf3toC32(&fColor);
	return col;
}
#else
C32 fplot3d::makeComplexColor(float real, float imag, pointf3 fColor, float gain)
{
	//const float start = .66f;
	//const float gain = 1.5f;
	//pointf3 fColor = { start, start, start };
	if (!gain) {
		C32 col = pointf3toC32(&fColor);
		return col;
	}
	float dr = real * gain;
	float dg = imag * gain;
	fColor.x += dr;
	fColor.y += dg;
	fColor.x = range(0.0f, fColor.x, 1.0f);
	fColor.y = range(0.0f, fColor.y, 1.0f);
	C32 col = pointf3toC32(&fColor);
	return col;
}
#endif

/*
scratch.getCssColorFromComplex = function(val) {
	var mg = compf.abs(val);
	var ang = Math.atan2(val[1],val[0]);
	ang *= 180/Math.PI;
	brt = 55*mg;
	return "hsl(" + ang + ",100%," + brt + "%)";
//	return "rgb(" + r + "," + g + "," + b + ")";
};
*/
