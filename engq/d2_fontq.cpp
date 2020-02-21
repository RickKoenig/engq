// test sprites
#define D2_3D // all the 3d baggage
#include <m_eng.h>

fontq::fontq(const C8* dir) : centerx(false),centery(false),fixed(false),fixedwidth(0),scl(1),spc(10),sc(2),leftmarg(0),topmarg(0),maxxsize(0),maxysize(0)
{
	U32 i;
	memset(glyphs,0,sizeof(glyphs));
	for (i=32;i<128;++i) {
		C8 n[200];
		sprintf(n,"%s/%03d.png",dir,i);
		if (fileexist(n)) {
//			logger("%c '%s' exists\n",i,n);
			glyphs[i]=texture_create(n);
			if (texture_getrc(glyphs[i])==1) {
				glyphs[i]->load();
			}
			if (maxxsize<glyphs[i]->origsize.x)
				maxxsize=(float)glyphs[i]->origsize.x;
			if (maxysize<glyphs[i]->origsize.y)
				maxysize=(float)glyphs[i]->origsize.y;
		}
	}
}

void fontq::setfixedwidth(float fwa)
{
	if (fwa>0) {
		fixedwidth=fwa;
		fixed=true;
	} else {
		fixed=false;
	}
}

void fontq::print(float xorig,float y,float xs,float ys,const pointf3& colf,const C8* fmt,...)
{
	static C8 str[1024]; // not secure, watch out, beware, should use new secure vsprintf !?
	va_list arglist;
	va_start(arglist,fmt);
	vsprintf(str,fmt,arglist);
	va_end(arglist);
//	logger("fontq::print '%s' with cent %d,%d fixed %d, fixedwidth %f, scl %f, spc %f, sc %f, leftmarg %f, topmarg %f, xsize %f, ysize %f\n",
//		str,centerx,centery,fixed,fixedwidth,scl,spc,sc,leftmarg,topmarg,xsize,ysize);
// apply margins
	xorig += leftmarg;
	y += topmarg;
	xs -= 2*leftmarg;
	ys -= 2*topmarg;
//	U32 len=strlen(str);
	if (xs<=0)
		xs=.1f;
	if (ys<=0)
		ys=.1f;
//	logger("printing: '%s'\n",str);
//	strcpy(str,"ABCDEFG");
	C8* sp=str;
	float fx=0;
	if (fixed) {
		while(*sp) {
//			U32 val=*sp;
			fx+=fixedwidth;
			++sp;
			if (!*sp)
				break;
			fx+=sc;
		}
	} else {
		while(*sp) {
			U32 val=*sp;
			textureb* t=getglyph(val);
			if (t) {
				fx+=t->origsize.x;
			} else {
				fx+=spc;
			}
			++sp;
			if (!*sp)
				break;
			fx+=sc;
		}
	}
	float dsclx;
	if (fx*scl>xs)
		dsclx=xs/fx;
	else
		dsclx=scl;
	float dscly;
	float maxrxsize=1.0f/maxxsize;
	float maxrysize=1.0f/maxysize;
//	if (fixed) {
//		if ((maxysize*fixedwidth*maxrxsize)*dsclx>ys)
//			dscly=ys*maxrysize;
//		else
//			dscly=dsclx*fixedwidth*maxrxsize;
//	} else {
		//if (maxysize*dsclx>ys)
			dscly=ys*maxrysize;
		//else
		//	dscly=dsclx;
//	}
	sp=str;
	float x=xorig;
	video_sprite_setaddmode(true); // font sprites
	if (centerx)
		x+=(xs-fx*dsclx)*.5f;
	if (centery)
		y+=(ys-maxysize*dscly)*.5f;
	if (fixed) {
		while(*sp) {
			U32 val=*sp;
			textureb* t=getglyph(val);
			if (t) {
				float gap = (fixedwidth-fixedwidth*t->origsize.x*maxrxsize)*dsclx; // width of gap - width of draw, good for centering
//				logger("gap = %f\n",gap);
				video_sprite_draw(t,colf,
// left
//					x,y,
// right
//					x+gap,y,
// centered
					x+gap*.5f,y,
// size
				(fixedwidth*t->origsize.x*maxrxsize)*dsclx,maxysize*dscly);
			}
			x+=(fixedwidth+sc)*dsclx;
			++sp;
		}
	} else {
		while(*sp) {
			U32 val=*sp;
			textureb* t=getglyph(val);
			if (t) {
				video_sprite_draw(t,colf,x,y,t->origsize.x*dsclx,maxysize*dscly);
				x+=(t->origsize.x+sc)*dsclx;
			} else {
				x+=spc*dsclx;
			}
			++sp;
		}
	}
	video_sprite_setaddmode(false); // normal sprites
}

fontq::~fontq()
{
	U32 i;
	for (i=0;i<256;++i) {
//		if (glyphs[i]) {
//			logger("%c is %d by %d\n",i,glyphs[i]->origsize.x,glyphs[i]->origsize.y);
//		}
		textureb::rc.deleterc(glyphs[i]);
	}
}

textureb* fontq::getglyph(U32 idx)
{
	if (idx>=256)
		return 0;
	if (!glyphs[idx] && idx>='a' && idx<='z')
		idx += 'A' - 'a';
	return glyphs[idx];
}
