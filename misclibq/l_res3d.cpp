#define RES3D
#define D2_3D
#include <m_eng.h>
#include <l_misclibm.h>
//#include "l_res3d.h"		// shape hierarchal polymorphic class for ui in 3d

namespace res3d {

textureb* backing,*wt; // backing for all ui, white tex
fontq* res3dfont;
float vx,vy; // sizeof of virtual screen
float VMX,VMY;	// virtual mouse
#define BACKUV 1.0f// .2f
/////////// shape base class
void shape::loglevel(S32 lev) const
{
	S32 i;
	for (i=0;i<lev;++i)
		logger(" ");
}

shape* shape::findrec(const C8* namea)
{
	if (!strcmp(name.c_str(),namea))
		return this;
	S32 i,n=children.size();
	for (i=0;i<n;++i) {
		shape* ret=children[i]->findrec(namea);
		if (ret)
			return ret;
	}
	return 0;
}

shape* shape::find(const C8* namea)
{
	shape* ret=findrec(namea);
	if (!ret)
		errorexit("can't find '%s'\n",namea);
	return ret;
}

shape::shape(script& sc) : vis(1),parent(0)
{
	name=sc.read();
	x=atoi(sc.read());
	y=atoi(sc.read());
}

shape::~shape()
{
	if (parent)
		errorexit("can't delete because have a parent");
//	logger("deleting shape\n");
	S32 i,n=children.size();
	for (i=0;i<n;++i) {
		children[i]->parent=0;
		delete children[i];
	}
}

void shape::show() const
{
	static S32 level;
	logger("shape '%s' x %d y %d '%s' vis %d\n",name.c_str(),x,y,typeid(*this).name(),vis);
	S32 i,n=children.size();
	++level;
	for (i=0;i<n;++i) {
		loglevel(level);
		children[i]->show();
		--level;
	}
}

#if 0
void shape::draw() const
{
	if (!vis)
		return;
	S32 i,n=children.size();
	for (i=0;i<n;++i)
		children[i]->draw();
}
#endif
void shape::draw3d() const
{
	if (!vis)
		return;
//	video_sprite_begin(
//	800,600,	// intended resolution of screen (virtual screen space for video_sprite_draw)
//	0 // vpflags see d2_viewport.h, use VP_CLEARBG if no 3d called (no setviewports)
//	//C32 backcolor=C32BLACK); // if flags has VP_CLEARBG
//	);
//	video_sprite_draw(backing,F32YELLOW,0,0,100,100,0,0,0,SPT_U0,SPT_V0,10-SPT_U0,10-SPT_V0);
//	video_sprite_end(); 

	S32 i,n=children.size();
	for (i=0;i<n;++i)
		children[i]->draw3d(); 
}

shape* shape::getover()
{
	static S32 lev;
	if (lev==0) {
		VMX=MX*vx/WX;
		VMY=MY*vy/WY;
	}
	S32 i,n=children.size();
	if (!vis)
		return 0;
	for (i=n-1;i>=0;--i) {
		++lev;
		shape* ret=children[i]->getover();
		--lev;
		if (ret)
			return ret;
	}
	if (isinside())
		return this;
	return 0;
}

S32 shape::procfocus()
{
	return -1;
}

void shape::procover()
{
}


void shape::procnotover()
{
}


// proc inactive shapes, fades..
void shape::over_no_over_proc(shape* focus,shape* over)
{
	S32 i,n=children.size();
	if (this==over)
		procover();
	else /*if (this!=focus)*/
		procnotover();
	for (i=0;i<n;++i) {
		shape* chd=children[i];
		chd->over_no_over_proc(focus,over);
	}
}

/////////// back derived class
back::back(script& sc) : shape(sc),col(0)
{
	xs=atoi(sc.read());
	ys=atoi(sc.read());
}

back::back(const C8* namea,S32 xa,S32 ya,S32 xsa,S32 ysa) : shape(namea,xa,ya),col(0),xs(xsa),ys(ysa)
{
}

void back::show() const
{
	logger("back xs %d ys %d : ",xs,ys);
	shape::show();
}

#if 0
void back::draw() const
{
	if (!vis)
		return;
//	cliprect32(B32,x,y,xs,ys,C32(0,0,85));
	shape::draw(); // children
//	vis=0; // draw back just once..
}
#endif
void back::draw3d() const
{
	if (!vis)
		return;
	if (videoinfo.video_maindriver!=VIDEO_GDI && videoinfo.video_maindriver!=VIDEO_DDRAW) {
		video_sprite_draw(backing,F32WHITE,(float)x,(float)y,(float)xs,(float)ys,0,0,0);
	} else {
		video_sprite_draw(backing,F32LIGHTBLUE,(float)x,(float)y,(float)xs,(float)ys,0,0,0);
	}
}

/*
/////////// shaperoot derived class
shaperoot::shaperoot(script& sc) : shape(sc),focus(0)
{
}

shaperoot::shaperoot(const C8* namea,S32 xa,S32 ya) : shape(namea,xa,ya),focus(0)
{
}

void shaperoot::show() const
{
	logger("shaperoot ");
	shape::show();
}
*/
/////////// text derived class
text::text(script& sc) : back(sc)
{
	texts=sc.read();
}

void text::show() const
{
	logger("text '%s' : ",texts.c_str());
	back::show();
}

#if 0
void text::draw() const
{
	if (!vis)
		return;
//	cliprect32(B32,x,y,xs,ys,C32CYAN);
//	outtextxyc32(B32,x+(xs>>1),y+(ys>>1),C32WHITE,texts.c_str());
	shape::draw(); // children
}
#endif
void text::draw3d() const
{
	if (!vis)
		return;
//	cliprect32(B32,x,y,xs,ys,C32CYAN);
	video_sprite_draw(backing,F32CYAN,(float)x,(float)y,(float)xs,(float)ys,0,0,0,SPT_U0,SPT_V0,.2f-SPT_U0,.2f-SPT_V0);
//	outtextxyc32(B32,x+(xs>>1),y+(ys>>1),C32BLACK,texts.c_str());
	res3dfont->setleftmarg(0);
	res3dfont->settopmarg(0);
	res3dfont->print((float)x,(float)y,(float)xs,(float)ys,F32WHITE,"%s",texts.c_str());
//	logger("text print %d %d %d %d '%s'\n",x,y,xs,ys,texts.c_str());
	res3dfont->setleftmarg(0);
	res3dfont->settopmarg(0);
	shape::draw3d(); // children
}

/////////// edit derived class
//#define EDITXSTEP 16
void edit::show() const
{
	logger("edit : ");
	text::show();
}

// clean this up
S32 edit::pos2mx() const
{
	S32 len=texts.length();
	return x+(xs>>1)-(len*5)+(pos*10);
//	return x+(xs>>1)-(len<<3)+(pos<<4);
}

// clean this up
void edit::mx2pos(S32 mx)
{
	S32 len=texts.length();
	pos=((mx<<1)-(x<<1)-xs+(len*10)+10)/20;
//	pos=((mx<<1)-(x<<1)-xs+(len<<4)+16)>>5;
	checkpos();
}

void edit::checkpos()
{
	S32 len=texts.length();
	pos=range(0,pos,len);
}

#if 0
void edit::draw() const
{
	if (!vis)
		return;
//	C32 c=active ? focuscol : col;
//	cliprect32(B32,x,y,xs,ys,c);
/*	if (blink>=8) {
		S32 mx=pos2mx();
		clipline32(B32,mx,y+2,mx,y+ys-1-2,C32WHITE);
	} */
//	outtextxyc32(B32,x+(xs>>1),y+(ys>>1),C32WHITE,texts.c_str());
/*	if (!active)
		blink=7;
	active=0; */
	shape::draw(); // children
}
#endif
void edit::draw3d() const
{
	if (!vis)
		return;
	C32 c=active ? focuscol : col;
	pointf3 fc;
	fc=C32topointf3(c);
	video_sprite_draw(backing,fc,(float)x,(float)y,(float)xs,(float)ys,0,0,0,SPT_U0,SPT_V0,BACKUV-SPT_U0,BACKUV-SPT_V0);
//	cliprect32(B32,x,y,xs,ys,c);
	if (blink>=8) {
		S32 mx=pos2mx();
//		clipline32(B32,mx,y+2,mx,y+ys-1-2,C32BLACK);
		video_sprite_draw(wt,pointf3x(.8f,1,.9f,1),(float)mx-1,(float)y/*+ys/4*/,2,(float)ys/*/2*/,0,0,0,SPT_U0,SPT_V0,.2f-SPT_U0,.2f-SPT_V0);
	}
//	outtextxyc32(B32,x+(xs>>1),y+(ys>>1),C32BLACK,texts.c_str());
	res3dfont->setfixedwidth(16);
	res3dfont->setcharspace(0);
	res3dfont->setleftmarg(5);
	res3dfont->settopmarg(5);
	res3dfont->print((float)x,(float)y,(float)xs,(float)ys,F32WHITE,"%s",texts.c_str());
	res3dfont->setleftmarg(0);
	res3dfont->settopmarg(0);
	res3dfont->setfixedwidth(0);
	res3dfont->setcharspace(2); // default hmm.
	if (!active)
		blink=7;
	active=0; 
	shape::draw3d(); // children 
}

//	video_sprite_draw(backing,F32WHITE,(float)x,(float)y,(float)xs,(float)ys,0,0,0,SPT_U0,SPT_V0,.2f-SPT_U0,.2f-SPT_V0);

void edit::settname(const C8* tnamea)
{ 
	if (!tnamea)
		return;
	texts=tnamea;
	pos=texts.length();
}

S32 edit::procfocus()
{
	S32 ret=-1;
	if (isinside() && wininfo.mleftclicks) {
		mx2pos((S32)VMX);
//		active=1;
	}
	if (isinside() && wininfo.mrightclicks) {
		pos=0;
		texts.clear();
//		active=1;
	}
//	if (!active) {
//		blink=0;
		active=1;
//	}
//	if (active) {
		++blink;
		if (KEY) {
			S32 len=texts.length();
			if (KEY>=32 && KEY<127) {
				texts = texts.substr(0,pos) + (C8)KEY + texts.substr(pos,len-pos);
//				ret = 1;
				++pos;
			} else if (KEY==K_BACKSPACE) {
				if (pos>0) {
					texts = texts.substr(0,pos-1) + texts.substr(pos,len-pos);
//					ret = 1;
					--pos;
				}
			} else if (KEY==K_DELETE) {
				if (pos<len) {
					texts = texts.substr(0,pos) + texts.substr(pos+1,len-pos);
//					ret = 1;
				}
			} else if (KEY==K_RIGHT) {
				++pos;
				checkpos();
			} else if (KEY==K_LEFT) {
				--pos;
				checkpos();
			} else if (KEY==K_RETURN)
				ret=KEY;
			KEY=0;
		}
//	}
	if (blink>16)
		blink=0;
	return ret;
}
void edit::procover()
{
	col=drift_col(col,brightcol,.25f);
}


void edit::procnotover()
{
	col=drift_col(col,darkcol,.05f);
}

/*
void edit::deactivate()
{
	active=0;
	blink=0;
}
*/

const C32 edit::darkcol=C32(128,128,128);
const C32 edit::brightcol=C32(192,192,192);
const C32 edit::focuscol=C32BLACK;

// pbut derived class
void pbut::show() const
	{
		logger("pbut : ");
		text::show();
	}

#if 0
void pbut::draw() const
{
	if (!vis)
		return;
//	cliprect32(B32,x,y,xs,ys,col);
//	outtextxyc32(B32,x+(xs>>1),y+(ys>>1),C32WHITE,texts.c_str());
	shape::draw(); // children
}
#endif
#if 1
void pbut::draw3d() const
{
	if (!vis)
		return;
//	cliprect32(B32,x,y,xs,ys,C32CYAN);
	pointf3 fc;
	fc=C32topointf3(col);
	video_sprite_draw(backing,fc,(float)x,(float)y,(float)xs,(float)ys,0,0,0,SPT_U0,SPT_V0,.2f-SPT_U0,.2f-SPT_V0);
//	outtextxyc32(B32,x+(xs>>1),y+(ys>>1),C32BLACK,texts.c_str());
	res3dfont->setleftmarg(5);
	res3dfont->settopmarg(5);
	res3dfont->print((float)x,(float)y,(float)xs,(float)ys,F32WHITE,"%s",texts.c_str());
	res3dfont->setleftmarg(0);
	res3dfont->settopmarg(0);
	shape::draw3d(); // children
}
#endif
S32 pbut::procfocus()
{
	if (!isinside()) {
		down=0;
		return -1;
	} else if (MBUT&1) {
		down=1;
	} else
		down=0;
	if (down)
		col = drift_col(col,focuscol,.55f);
	return (!(MBUT&1) && (LMBUT&1))-1;
}

void pbut::procover()
{
	if (!down)
		col=drift_col(col,brightcol,.25f);
}


void pbut::procnotover()
{
	col=drift_col(col,darkcol,.05f);
}

const C32 pbut::darkcol=C32(64,0,0);
const C32 pbut::brightcol=C32(255,0,0);
const C32 pbut::focuscol=C32(192,192,192);

/////////// vscroll derived class
vscroll::vscroll(script& sc) : back(sc),idx(0),numidx(1),offset(0),cnt(0),pagedir(0),indrag(0)
{
}

vscroll::vscroll(const C8* namea,S32 xa,S32 ya,S32 xsa,S32 ysa) : back(namea,xa,ya,xsa,ysa),idx(0),numidx(1),offset(0),cnt(0),pagedir(0),indrag(0)
{
}

vscroll::~vscroll()
{
//	logger("deleting vscroll\n");
}

void vscroll::show() const
{
	logger("vscroll : ");
	back::show();
}

#if 0
void vscroll::draw() const
{
	if (ys<=3*xs) 
		errorexit("bad vscl: too wide '%s'",name.c_str());
	if (!vis)
		return;
	if (numidx<=1)
		return;
	S32 vdmax=ys-3*xs;
	S32 soff=vdmax*idx/(numidx-1)+xs;
// whole thing
//	cliprect32(B32,x,y,xs,ys,C32(200,40,120));
/*
// top
	cliprect32(B32,x,y,xs,xs,C32(200,240,220));
	cliprecto32(B32,x,y,xs,xs,C32BLACK);
	clipline32(B32,x+xs/2,y+xs/6,x+  xs/6,y+5*xs/6,C32BLACK);
	clipline32(B32,x+xs/2,y+xs/6,x+5*xs/6,y+5*xs/6,C32BLACK);
// middle
	cliprect32(B32,x,y+soff,xs,xs,C32(200,240,220));
	cliprecto32(B32,x,y+soff,xs,xs,C32BLACK);
//	outtextxyf32(B32,x,y+soff,C32BLACK,"%d",idx);
//	outtextxyf32(B32,x,y+soff+8,C32BLACK,"%d",numidx);
	clipcircleo32(B32,x+xs/2,y+xs/2+soff,xs/3,C32BLACK);
// bottom
	cliprect32(B32,x,y+ys-xs,xs,xs,C32(200,240,220)); 
	cliprecto32(B32,x,y+ys-xs,xs,xs,C32BLACK); 
	clipline32(B32,x+xs/2,y+ys-xs/6,x+  xs/6,y+ys-5*xs/6,C32BLACK);
	clipline32(B32,x+xs/2,y+ys-xs/6,x+5*xs/6,y+ys-5*xs/6,C32BLACK);
*/
	shape::draw(); // children
}
#endif
void vscroll::draw3d() const
{
	if (ys<=3*xs) 
		errorexit("bad vscl: too wide '%s'",name.c_str());
	if (!vis)
		return;
	if (numidx<=1)
		return;
	S32 vdmax=ys-3*xs;
	S32 soff=vdmax*idx/(numidx-1)+xs;
// whole thing
//	cliprect32(B32,x,y,xs,ys,C32(200,40,120));
	video_sprite_draw(backing,C32topointf3(C32(200,40,120)),(float)x,(float)y,(float)xs,(float)ys,0,0,0,SPT_U0,SPT_V0,BACKUV-SPT_U0,BACKUV-SPT_V0);
// top
//	cliprect32(B32,x,y,xs,xs,C32(200,240,220));
	video_sprite_draw(backing,C32topointf3(C32(200,240,220)),(float)x,(float)y,(float)xs,(float)xs,0,0,0,SPT_U0,SPT_V0,BACKUV-SPT_U0,BACKUV-SPT_V0);
// middle
//	cliprect32(B32,x,y+soff,xs,xs,C32(200,240,220));
	video_sprite_draw(backing,C32topointf3(C32(0,0,0)),(float)x,(float)y+soff,(float)xs,(float)xs,0,0,0,SPT_U0,SPT_V0,BACKUV-SPT_U0,BACKUV-SPT_V0);
// bottom
//	cliprect32(B32,x,y+ys-xs,xs,xs,C32(200,240,220)); 
	video_sprite_draw(backing,C32topointf3(C32(200,240,220)),(float)x,(float)y+ys-xs,(float)xs,(float)xs,0,0,0,SPT_U0,SPT_V0,BACKUV-SPT_U0,BACKUV-SPT_V0);
	shape::draw3d(); // children
}


S32 vscroll::procfocus()
{
	if (numidx<=1)
		return -1;
	if (DMZ) {
		idx-=5*DMZ;
		setidx(idx);
		return idx+offset;
	}
	switch(KEY) {
		case K_DOWN:
		case K_NUMDOWN:
			++idx;
			setidxi(idx);
			return idx+offset;
			break;
		case K_PAGEDOWN:
			idx+=5;
			setidxi(idx);
			return idx+offset;
			break;
		case K_UP:
		case K_NUMUP:
			--idx;
			setidxi(idx);
			return idx+offset;
			break;
		case K_PAGEUP:
			idx-=5;
			setidxi(idx);
			return idx+offset;
			break;
	}
	if ((MBUT&1) && !(LMBUT&1)) {
		cnt=0;
		pagedir=0;
		indrag=0;
	}
	if (MBUT&1) {
//		if (DMX || DMY)
//			pagedir=0;
		if (!indrag && (cnt==0 || (cnt>=10 && (cnt%2==0)))) {
			S32 vdmax=ys-3*xs;
			S32 soff=vdmax*idx/(numidx-1)+xs;
			if (VMY>=y && VMY<y+xs) { // up
				--idx;
			} else if (VMY>=y+ys-xs && VMY<y+ys) { // down
				++idx;
			} else if (VMY>=y+xs && VMY<y+soff) { // page up
				if (pagedir!=1) {
					idx-=5;
					pagedir=-1;
				}
			} else if (VMY>=y+soff+xs && VMY<y+ys-xs) { // page down
				if (pagedir!=-1) {
					idx+=5;
					pagedir=1;
				}
			} else if (VMY>=y+soff && VMY<y+soff+xs) { // drag
//				if (!(LMBUT&1))
					indrag=1;
			}
			setidxi(idx);
		}
		if (indrag) {
			S32 mlo=y+xs+(xs>>1);
			S32 mhi=y+ys-xs-(xs>>1);
			idx=(S32)(((VMY-mlo)*(numidx-1)+((mhi-mlo)>>1))/(mhi-mlo));
			setidxi(idx);
		}
		++cnt;
		return idx+offset;
	} 
	return -1;
}

void vscroll::setidxi(S32 idxa)
{
	idx=range(0,idxa,numidx-1);
	if (parent) {
		listbox* lp=dynamic_cast<listbox*>(parent);
		if (lp) {
			lp->curoffset=idx;
		}
		listboxn* lpn=dynamic_cast<listboxn*>(parent);
		if (lpn) {
			lpn->curoffset=idx;
		}
	}
}

/////////// hscroll derived class
hscroll::hscroll(script& sc) : vscroll(sc)
{
}

hscroll::~hscroll()
{
//	logger("deleting hscroll\n");
}

void hscroll::show() const
{
	logger("hscroll : ");
	back::show();
}

#if 0
void hscroll::draw() const
{
	if (xs<=3*ys) 
		errorexit("bad hscl: too tall '%s'",name.c_str());
	if (!vis)
		return;
	if (numidx<=1)
		return;
	S32 vdmax=xs-3*ys;
	S32 soff=vdmax*idx/(numidx-1)+ys;
/*
// whole thing
//	cliprect32(B32,x,y,xs,ys,C32(200,40,120));
// top
	cliprect32(B32,x,y,ys,ys,C32(200,240,220));
	cliprecto32(B32,x,y,ys,ys,C32BLACK);
	clipline32(B32,x+ys/6,y+ys/2,x+5*ys/6,y+  ys/6,C32BLACK);
	clipline32(B32,x+ys/6,y+ys/2,x+5*ys/6,y+5*ys/6,C32BLACK);
// middle
	cliprect32(B32,x+soff,y,ys,ys,C32(200,240,220));
	cliprecto32(B32,x+soff,y,ys,ys,C32BLACK);
//	outtextxyf32(B32,x,y+soff,C32BLACK,"%d",idx);
//	outtextxyf32(B32,x,y+soff+8,C32BLACK,"%d",numidx);
	clipcircleo32(B32,x+ys/2+soff,y+ys/2,ys/3,C32BLACK);
// bottom
	cliprect32(B32,x+xs-ys,y,ys,ys,C32(200,240,220)); 
	cliprecto32(B32,x+xs-ys,y,ys,ys,C32BLACK); 
	clipline32(B32,x+xs-ys/6,y+ys/2,x+xs-5*ys/6,y+  ys/6,C32BLACK);
	clipline32(B32,x+xs-ys/6,y+ys/2,x+xs-5*ys/6,y+5*ys/6,C32BLACK);
*/
	shape::draw(); // children
}
#endif
void hscroll::draw3d() const
{
	if (xs<=3*ys) 
		errorexit("bad hscl: too tall '%s'",name.c_str());
	if (!vis)
		return;
	if (numidx<=1)
		return;
	S32 vdmax=xs-3*ys;
	S32 soff=vdmax*idx/(numidx-1)+ys;
// whole thing
//	cliprect32(B32,x,y,xs,ys,C32(200,40,120));
	video_sprite_draw(backing,C32topointf3(C32(255,255,255),2),(float)x,(float)y,(float)xs,(float)ys,0,0,0,SPT_U0,SPT_V0,BACKUV-SPT_U0,BACKUV-SPT_V0);
// top
//	cliprect32(B32,x,y,ys,ys,C32(200,240,220));
	video_sprite_draw(backing,C32topointf3(C32(200,240,220)),(float)x,(float)y,(float)ys,(float)ys,0,0,0,SPT_U0,SPT_V0,BACKUV-SPT_U0,BACKUV-SPT_V0);
// middle
//	cliprect32(B32,x+soff,y,ys,ys,C32(200,240,220));
	video_sprite_draw(backing,C32topointf3(C32(0,0,0)),(float)x+soff,(float)y,(float)ys,(float)ys,0,0,0,SPT_U0,SPT_V0,BACKUV-SPT_U0,BACKUV-SPT_V0);
// bottom
//	cliprect32(B32,x+xs-ys,y,ys,ys,C32(200,240,220)); 
	video_sprite_draw(backing,C32topointf3(C32(200,240,220)),(float)x+xs-ys,(float)y,(float)ys,(float)ys,0,0,0,SPT_U0,SPT_V0,BACKUV-SPT_U0,BACKUV-SPT_V0);
	shape::draw3d(); // children
}

S32 hscroll::procfocus()
{
	S32 oldidx=idx;
	if (numidx<=1)
		return -1;
	if (DMZ) {
		idx-=5*DMZ;
		setidxi(idx);
		return oldidx!=idx ? 1 : -1;
	}
	switch(KEY) {
		case K_RIGHT:
		case K_NUMRIGHT:
			++idx;
			setidxi(idx);
//			return idx+offset;
			return oldidx!=idx ? 1 : -1;
			break;
		case K_PAGEDOWN:
			idx+=5;
			setidxi(idx);
//			return idx+offset;
			return oldidx!=idx ? 1 : -1;
			break;
		case K_LEFT:
		case K_NUMLEFT:
			--idx;
			setidxi(idx);
//			return idx+offset;
			return oldidx!=idx ? 1 : -1;
			break;
		case K_PAGEUP:
			idx-=5;
			setidxi(idx);
//			return idx+offset;
			return oldidx!=idx ? 1 : -1;
			break;
	}
	if ((MBUT&1) && !(LMBUT&1)) {
		cnt=0;
		pagedir=0;
		indrag=0;
	}
	if (MBUT&1) {
//		if (DMX || DMY)
//			pagedir=0;
		if (!indrag && (cnt==0 || (cnt>=10 && (cnt%2==0)))) {
			S32 vdmax=xs-3*ys;
			S32 soff=vdmax*idx/(numidx-1)+ys;
			if (VMX>=x && VMX<x+ys) { // up
				--idx;
			} else if (VMX>=x+xs-ys && VMX<x+xs) { // down
				++idx;
			} else if (VMX>=x+ys && VMX<x+soff) { // page up
				if (pagedir!=1) {
					idx-=5;
					pagedir=-1;
				}
			} else if (VMX>=x+soff+ys && VMX<x+xs-ys) { // page down
				if (pagedir!=-1) {
					idx+=5;
					pagedir=1;
				}
			} else if (VMX>=x+soff && VMX<x+soff+ys) { // drag
//				if (!(LMBUT&1))
					indrag=1;
			}
			setidxi(idx);
		}
		if (indrag) {
			S32 mlo=x+ys+(ys>>1);
			S32 mhi=x+xs-ys-(ys>>1);
			idx=(S32)(((VMX-mlo)*(numidx-1)+((mhi-mlo)>>1))/(mhi-mlo));
			setidxi(idx);
		}
		++cnt;
//		return idx+offset;
		return oldidx!=idx ? 1 : -1;
	} 
	return -1;
}

/////////// listbox derived class
#define LISTBOXYSTEP 16
static bool sortips(const string& a,const string& b)
{
	U32 ipa=str2ip(a.c_str());
	U32 ipb=str2ip(b.c_str());
	return ipa<ipb;
}

static bool sortint(const string& a,const string& b)
{
	U32 ia=atoi(a.c_str());
	U32 ib=atoi(b.c_str());
	return ia<ib;
}

void listbox::procover()
{
	col=drift_col(col,brightcol,.25f);
}

void listbox::procnotover()
{
	col=drift_col(col,darkcol,.05f);
}

listbox::~listbox()
{
//	logger("deleting listbox\n");
}

void listbox::show() const
{
	logger("listbox : ");
	back::show();
}

#if 0
void listbox::draw() const
{
	if (!vis)
		return;
//	recti2 cliprectsave=B32->cliprect;
//	setcliprect32(B32,x,y,xs,ys);
//	cliprect32(B32,x,y,xs,ys,col);
	S32 i,n=thelist.size(),nv=ys/LISTBOXYSTEP;
	for (i=0;i<nv;++i) {
		S32 j=i+curoffset;
		if (j<0)
			continue;
		if (j>=n)
			continue;
//		if (j==idx)
//			cliprect32(B32,x,y+(i<<3),xs,8,C32GREEN);
//		outtextxy32(B32,x,y+(i<<3),C32YELLOW,thelist[j].c_str());
	}
//	resetcliprect32(B32);
//	B32->cliprect=cliprectsave;
	shape::draw(); // children
}
#endif
void listbox::draw3d() const
{
	if (!vis)
		return;
	pointf3 fc;
	fc=C32topointf3(col);
	video_sprite_draw(backing,fc,(float)x,(float)y,(float)xs,(float)ys,0,0,0,SPT_U0,SPT_V0,BACKUV-SPT_U0,BACKUV-SPT_V0);
	S32 i,n=thelist.size(),nv=ys/LISTBOXYSTEP;
	for (i=0;i<nv;++i) {
		S32 j=i+curoffset;
		if (j<0)
			continue;
		if (j>=n)
			continue;
		if (j==idx) {
//			cliprect32(B32,x,y+(i<<3),xs,8,C32GREEN);
			pointf3 ga=F32GREEN; // leave green alone, no BACKUV
			ga.w=.5f;
			video_sprite_draw(wt,ga,(float)x,(float)y+i*LISTBOXYSTEP,(float)xs,(float)LISTBOXYSTEP,0,0,0,SPT_U0,SPT_V0,.2f-SPT_U0,.2f-SPT_V0);
		}
//		outtextxy32(B32,x,y+(i<<3),C32YELLOW,thelist[j].c_str());
		res3dfont->setcenterx(false);
		res3dfont->settopmarg(2);
		float txs=(float)xs;
		if (children.size()) { // take into account the scrollbar
			vscroll* vs=dynamic_cast<vscroll*>(children[0]);
			if (vs)
				txs-=(float)vs->xs;
		}
		res3dfont->print((float)x,(float)y+i*LISTBOXYSTEP,txs,(float)LISTBOXYSTEP,F32YELLOW,"%s",thelist[j].c_str());
		res3dfont->setcenterx(true);
		res3dfont->settopmarg(0);
	}
	shape::draw3d(); // children
}

S32 listbox::procfocus()
{
	if (!getnumidx()) {
		idx=-1;
		return -1;
	}
	if (idx>=0) {
		switch(KEY) {
			case K_DOWN:
			case K_NUMDOWN:
				++idx;
				setidxv(idx);
				return idx;
				break;
			case K_PAGEDOWN:
				idx+=5;
				setidxv(idx);
				return idx;
				break;
			case K_UP:
			case K_NUMUP:
				--idx;
				if (idx<0)
					idx=0;
				setidxv(idx);
				return idx;
				break;
			case K_PAGEUP:
				idx-=5;
				if (idx<0)
					idx=0;
				setidxv(idx);
				return idx;
				break;
		}
	}
	if (DMZ && children.size()==1) { // has a scroll bar
		vscroll* vs=dynamic_cast<vscroll*>(children[0]);
		if (vs) {
			S32 i=vs->getidx();
			i-=5*DMZ;
			vs->setidx(i);
		}
	}
	if (!isinside())
		return -1;
	if ((MBUT&1) && !(LMBUT&1)) {
		S32 i;
		i=(S32)(((VMY-y)/LISTBOXYSTEP)+curoffset);
		if (i<(S32)thelist.size()) {
			setidxv(i);
			return idx;
		}
	}
	return -1;
}

S32 listbox::sort(enum listsortkind lsk)
{
	string ss;
	if (idx>=0)
		ss=thelist[idx];
	switch(lsk) {
		case SK_NORMAL:
			::sort(thelist.begin(),thelist.end());
			break;
		case SK_REVERSE:
			::sort(thelist.begin(),thelist.end(),greater<string>());
			break;
		case SK_IPS:
			::sort(thelist.begin(),thelist.end(),sortips);
			break;
		case SK_INTVAL:
			::sort(thelist.begin(),thelist.end(),sortint);
			break;
	}
	if (idx>=0)
		idx=findstring(ss.c_str());
	return idx;
}

void listbox::addstring(const C8* str)
{
	thelist.push_back(str);
	S32 n=thelist.size();
	idx=n-1;
	if (mx>=0 && n>mx) {
		thelist.erase(thelist.begin());
		n=thelist.size();
		--idx;
	}
	numoffsets=n-(ys/LISTBOXYSTEP)+1;
	if (numoffsets<1)
		numoffsets=1;
	++curoffset;
	setidxc();
	setscroll();
//	sort();
}

void listbox::addstringstay(const C8* str)
{
	S32 n=thelist.size();
	if (idx==n-1)
		return addstring(str);
	thelist.push_back(str);
	n=thelist.size();
	if (mx>=0 && n>mx) {
		thelist.erase(thelist.begin());
		n=thelist.size();
		--idx;
		if (idx<0)
			idx=0;
		--curoffset;
		if (curoffset<0)
			curoffset=0;
	}
	numoffsets=n-(ys/LISTBOXYSTEP)+1;
	if (numoffsets<1)
		numoffsets=1;
	setscroll();
}

void listbox::printf(const C8* fmt,...) // add at the end, idx untouched, just like addstringstay but with formmated text
{
	C8 str[2048];
	va_list arglist;
	va_start(arglist,fmt);
	vsprintf(str,fmt,arglist);
	addstringstay(str);
//	logger("lb printf: '%s'\n",str);
	va_end(arglist);
}

void listbox::addstring(const C8* str,S32 where)
{
//	thelist.push_back(str);
	thelist.insert(thelist.begin()+where,str);
	S32 n=thelist.size();
	idx=where;
	if (mx>=0 && n>mx) {
		thelist.erase(thelist.begin());
		n=thelist.size();
	}
	numoffsets=n-(ys/LISTBOXYSTEP)+1;
	if (numoffsets<1)
		numoffsets=1;
	++curoffset;
	setidxc();
	setscroll();
//	sort();
}

void listbox::changestring(const C8* str,S32 where)
{
	if (where>=0 && where<(S32)thelist.size())
		thelist[where]=str;
}

/*void listbox::addstringn(const C8* str,S32 mx)
{
	thelist.push_back(str);
	S32 n=thelist.size();
	numoffsets=n-(ys>>3)+1;
	if (numoffsets<1)
		numoffsets=1;
	++curoffset;
	setscroll();
//	sort();
}
*/
void listbox::setidxc(S32 idxa)
{ 
	S32 n=thelist.size();
	if (n<=0) {
		idx=-1;
		curoffset=0;
		numoffsets=1;
		return;
	}
	if (idxa<0)
		idx=-1;
	else if (idxa>=n)
		idx=n-1;
	else
		idx=idxa;
	S32 nvislines=ys/LISTBOXYSTEP;
	curoffset=idx-(((nvislines+1)>>1)-1);
	rangeoffset();
	setscroll();
}

void listbox::setidxv(S32 idxa)
{ 
	S32 n=thelist.size();
	if (n<=0) {
		idx=-1;
		curoffset=0;
		numoffsets=1;
		return;
	}
	if (idxa<0)
		idx=-1;
	else if (idxa>=n)
		idx=n-1;
	else
		idx=idxa;
	S32 nvislines=ys/LISTBOXYSTEP;
	S32 locuroffset=idx-(nvislines-1);
	if (curoffset<locuroffset)
		curoffset=locuroffset;
	S32 hicuroffset=idx;
	if (curoffset>hicuroffset)
		curoffset=hicuroffset;
	rangeoffset();
	setscroll();
}

/*
void listbox::setcuroffset(S32 offa)
{
	curoffset=offa;
	rangeoffset();
}
*/

void listbox::setscroll()
{
	if (curoffset>=numoffsets)
		curoffset=numoffsets-1;
	if (numoffsets>1/* && idx>=0*/) {
		if (!children.size()) {
			vscroll* vs=new vscroll("lbh",x+xs-12,y,12,ys);
			addchild(vs);
		}
		vscroll* vsc=dynamic_cast<vscroll*>(children[0]);
		vsc->setnumidx(numoffsets);
		vsc->setidx(curoffset);
	}
	if ((numoffsets<=1/* || idx<0*/)&& children.size()==1) {
		children[0]->parent=0;
		delete children[0];
		children.clear();
	}
}

void listbox::rangeoffset()
{
	if (curoffset<0)
		curoffset=0;
	if (curoffset>=numoffsets)
		curoffset=numoffsets-1;
}

void listbox::removeidx(S32 i)
{
	if (i>=(S32)thelist.size() || i<0)
		return;
	vector<string>::iterator it=thelist.begin()+i;
	thelist.erase(it);
	if (idx>=(S32)thelist.size())
		--idx;
	S32 n=thelist.size();
	numoffsets=n-(ys/LISTBOXYSTEP)+1;
	if (numoffsets<1)
		numoffsets=1;
	setscroll();
}

void listbox::removeidx()
{
	removeidx(idx);
}

const C8* listbox::getidxname() const
{ 
	if (idx<0)
		return 0;
	return thelist[idx].c_str();
}

const C8* listbox::getidxname(S32 ind) const
{ 
	if (ind<0 && ind>=(S32)thelist.size())
		return 0;
	return thelist[ind].c_str();
}

S32 listbox::findstring(const C8* fs) const
{
	S32 i,n=thelist.size();
	for (i=0;i<n;++i)
		if (!strcmp(fs,thelist[i].c_str()))
			return i;
	return -1;
}

const C32 listbox::darkcol=C32(10,40,120);
const C32 listbox::brightcol=C32(110,40,120);

// multi column list box
/////////// listboxn derived class
void listboxn::procover()
{
	col=drift_col(col,brightcol,.25f);
}

void listboxn::procnotover()
{
	col=drift_col(col,darkcol,.05f);
}

listboxn::~listboxn()
{
//	logger("deleting listboxn\n");
}

void listboxn::show() const
{
	logger("listboxn : ");
	back::show();
}

#if 0
void listboxn::draw() const
{
	if (!vis)
		return;
//	recti2 cliprectsave=B32->cliprect;
//	cliprect32(B32,x,y,xs,ys,col);
	S32 j,n=thelist.size(),nv=ys/LISTBOXYSTEP;
	for (j=0;j<nv;++j) {
		S32 jo=j+curoffset;
		if (jo<0)
			continue;
		if (jo>=n)
			continue;
/*		if (jo==idx) {
			setcliprect32(B32,x,y,xs,ys);
			cliprect32(B32,x,y+j*LISTBOXYSTEP,xs,8,C32GREEN);
			resetcliprect32(B32);
		} */
		S32 nc=thelist[jo].size();
		if (nc>0) {
			S32 step=xs/nc;
			S32 i,ix;
			for (i=0,ix=x;i<nc;++i,ix+=step) {
				S32 ixc=ix;
				if (i<(S32)columnsx.size())
					ixc=x+columnsx[i];
//				if (i!=0)
//					clipvline32(B32,ixc-2,y,y+ys-1,C32WHITE);
				setcliprect32(B32,ixc,y,step,ys);
//				outtextxy32(B32,ixc,y+j*LISTBOXYSTEP,C32YELLOW,thelist[jo][i].c_str());
				resetcliprect32(B32);
			}
		}
	}
//	B32->cliprect=cliprectsave;
	shape::draw(); // children
}
#endif
void listboxn::draw3d() const
{
	if (!vis)
		return;
	pointf3 fc;
	fc=C32topointf3(col);
//	recti2 cliprectsave=B32->cliprect;
//	cliprect32(B32,x,y,xs,ys,col);
	video_sprite_draw(backing,fc,(float)x,(float)y,(float)xs,(float)ys,0,0,0,SPT_U0,SPT_V0,.2f-SPT_U0,.2f-SPT_V0);
	S32 j,n=thelist.size(),nv=ys/LISTBOXYSTEP;
	for (j=0;j<nv;++j) {
		S32 jo=j+curoffset;
		if (jo<0)
			continue;
		if (jo>=n)
			continue;
		if (jo==idx) {
//			setcliprect32(B32,x,y,xs,ys);
//			cliprect32(B32,x,y+j*LISTBOXYSTEP,xs,8,C32GREEN);
			pointf3 ga=F32GREEN;
			ga.w=.5f;
			video_sprite_draw(wt,ga,(float)x,(float)y+j*LISTBOXYSTEP,(float)xs,(float)LISTBOXYSTEP,0,0,0,SPT_U0,SPT_V0,.2f-SPT_U0,.2f-SPT_V0);
//			resetcliprect32(B32);
		}
		S32 nc=thelist[jo].size();
		if (nc>0) {
			S32 step=xs/nc;
			S32 i,ix;
			for (i=0,ix=x;i<nc;++i,ix+=step) {
				S32 ixc=ix;
				if (i<(S32)columnsx.size())
					ixc=x+columnsx[i];
				if (i!=0)
//					clipvline32(B32,ixc-2,y,y+ys-1,C32WHITE);
					video_sprite_draw(wt,F32WHITE,(float)ixc-2,(float)y,(float)2,(float)ys,0,0,0,SPT_U0,SPT_V0,.2f-SPT_U0,.2f-SPT_V0);
//				setcliprect32(B32,ixc,y,step,ys);
//				outtextxy32(B32,ixc,y+j*LISTBOXYSTEP,C32YELLOW,thelist[jo][i].c_str());
//				resetcliprect32(B32);
				res3dfont->setcenterx(false);
				res3dfont->settopmarg(2);
				float txs=(float)step;
				if (i==nc-1) {
					if (children.size()) { // take into account the scrollbar
						vscroll* vs=dynamic_cast<vscroll*>(children[0]);
						if (vs)
							txs-=(float)vs->xs;
					}
				} else 
					txs-=3;
				res3dfont->print((float)ixc,(float)y+j*LISTBOXYSTEP,txs,(float)LISTBOXYSTEP,F32YELLOW,"%s",thelist[jo][i].c_str());
				res3dfont->setcenterx(true);
				res3dfont->settopmarg(0);
			}
		}
	}
//	B32->cliprect=cliprectsave;
	shape::draw3d(); // children
}

S32 listboxn::procfocus()
{
	if (!getnumidx()) {
		idx=-1;
		return -1;
	}
	if (idx>=0) {
		switch(KEY) {
			case K_DOWN:
			case K_NUMDOWN:
				++idx;
				setidxv(idx);
				return idx;
				break;
			case K_PAGEDOWN:
				idx+=5;
				setidxv(idx);
				return idx;
				break;
			case K_UP:
			case K_NUMUP:
				--idx;
				if (idx<0)
					idx=0;
				setidxv(idx);
				return idx;
				break;
			case K_PAGEUP:
				idx-=5;
				if (idx<0)
					idx=0;
				setidxv(idx);
				return idx;
				break;
		}
	}
	if (!isinside())
		return -1;
	if ((MBUT&1) && !(LMBUT&1)) {
		if (VMY>=y && VMY<y+ys) {
			S32 i;
			i=(S32)(((VMY-y)/LISTBOXYSTEP)+curoffset);
			if (i<(S32)thelist.size()) {
				setidxv(i);
				return idx;
			}
		}
	}
	if (DMZ && children.size()==1) { // has a scroll bar
		vscroll* vs=dynamic_cast<vscroll*>(children[0]);
		if (vs) {
			S32 i=vs->getidx();
			i-=5*DMZ;
			vs->setidx(i);
		}
	}
	return -1;
}

void listboxn::sort()
{
	string ss;
	if (idx>=0 && thelist[idx].size())
		ss=thelist[idx][0];
	::sort(thelist.begin(),thelist.end());
	if (idx>=0 && thelist[idx].size())
		idx=findstring(ss.c_str());
}

void listboxn::addstring(const vector<string> & str) // at the end, idx = the new size()-1
{
	thelist.push_back(str);
	S32 n=thelist.size();
	idx=n-1;
	if (mx>=0 && n>mx) {
		thelist.erase(thelist.begin());
		n=thelist.size();
		--idx;
	}
	numoffsets=n-(ys/LISTBOXYSTEP)+1;
	if (numoffsets<1)
		numoffsets=1;
	++curoffset;
	setidxc();
	setscroll();
//	sort();
}

void listboxn::addstringstay(const vector<string> & str) // add at the end, idx untouched
{
	thelist.push_back(str);
	S32 n=thelist.size();
	numoffsets=n-(ys/LISTBOXYSTEP)+1;
	if (numoffsets<1)
		numoffsets=1;
	setscroll();
}

void listboxn::addstring(const vector<string> & str,S32 where)
{
//	thelist.push_back(str);
	thelist.insert(thelist.begin()+where,str);
	S32 n=thelist.size();
	idx=where;
	if (mx>=0 && n>mx) {
		thelist.erase(thelist.begin());
		n=thelist.size();
	}
	numoffsets=n-(ys/LISTBOXYSTEP)+1;
	if (numoffsets<1)
		numoffsets=1;
	++curoffset;
	setidxc();
	setscroll();
//	sort();
}

void listboxn::changestring(const vector<string> & str,S32 where)
{
	if (where>=0 && where<(S32)thelist.size())
		thelist[where]=str;
}

void listboxn::setidxc(S32 idxa)
{ 
	S32 n=thelist.size();
	if (n<=0) {
		idx=-1;
		curoffset=0;
		numoffsets=1;
		return;
	}
	if (idxa<0)
		idx=-1;
	else if (idxa>=n)
		idx=n-1;
	else
		idx=idxa;
	S32 nvislines=ys/LISTBOXYSTEP;
	curoffset=idx-(((nvislines+1)>>1)-1);
	rangeoffset();
	setscroll();
}

void listboxn::setidxv(S32 idxa)
{ 
	S32 n=thelist.size();
	if (n<=0) {
		idx=-1;
		curoffset=0;
		numoffsets=1;
		return;
	}
	if (idxa<0)
		idx=-1;
	else if (idxa>=n)
		idx=n-1;
	else
		idx=idxa;
	S32 nvislines=ys/LISTBOXYSTEP;
	S32 locuroffset=idx-(nvislines-1);
	if (curoffset<locuroffset)
		curoffset=locuroffset;
	S32 hicuroffset=idx;
	if (curoffset>hicuroffset)
		curoffset=hicuroffset;
	rangeoffset();
	setscroll();
}

void listboxn::setscroll()
{
	if (curoffset>=numoffsets)
		curoffset=numoffsets-1;
	if (numoffsets>1/* && idx>=0*/) {
		if (!children.size()) {
			vscroll* vs=new vscroll("lbh",x+xs-12,y,12,ys);
			addchild(vs);
		}
		vscroll* vsc=dynamic_cast<vscroll*>(children[0]);
		vsc->setnumidx(numoffsets);
		vsc->setidx(curoffset);
	}
	if ((numoffsets<=1/* || idx<0*/)&& children.size()==1) {
		children[0]->parent=0;
		delete children[0];
		children.clear();
	}
}

void listboxn::rangeoffset()
{
	if (curoffset<0)
		curoffset=0;
	if (curoffset>=numoffsets)
		curoffset=numoffsets-1;
}

void listboxn::removeidx(S32 i)
{
	if (i>=(S32)thelist.size() || i<0)
		return;
	vector<vector<string> >::iterator it=thelist.begin()+i;
	thelist.erase(it);
	if (idx>=(S32)thelist.size())
		--idx;
	S32 n=thelist.size();
	numoffsets=n-(ys/LISTBOXYSTEP)+1;
	if (numoffsets<1)
		numoffsets=1;
	setscroll();
}

void listboxn::removeidx()
{
	removeidx(idx);
}

vector<string> listboxn::getidxname() const
{ 
	if (idx<0) {
		vector<string> vs;
		return vs; // null
	}
	return thelist[idx];
}

vector<string> listboxn::getidxname(S32 ind) const
{ 
	if (ind<0 && ind>=(S32)thelist.size()) {
		vector<string> vs;
		return vs; // null
	}
	return thelist[ind];
}

S32 listboxn::findstring(const C8* fs) const
{
	S32 i,n=thelist.size();
	for (i=0;i<n;++i)
		if (thelist[i].size())
			if (!strcmp(fs,thelist[i][0].c_str()))
				return i;
	return -1;
}

void listboxn::setcol(S32 col,const C8* cs)
{
	S32 nlist=thelist.size();
	S32 j;
	for (j=0;j<nlist;++j) {
		S32 nele=thelist[j].size();
		if (col<nele) {
			thelist[j][col]=cs;
		}
	}
}

const C32 listboxn::darkcol=C32(10,40,120);
const C32 listboxn::brightcol=C32(110,40,120);

}

// specialized shape class family (res) factory constructor
template <>
factory2<shape>::factory2()
{
	funmaph["shape"]=(FPH)shape::createh;
	funmaph["back"]=(FPH)back::createh;
	funmaph["text"]=(FPH)text::createh; 
	funmaph["pbut"]=(FPH)pbut::createh;
	funmaph["vscroll"]=(FPH)vscroll::createh; 
	funmaph["hscroll"]=(FPH)hscroll::createh; 
	funmaph["listbox"]=(FPH)listbox::createh; 
	funmaph["listboxn"]=(FPH)listboxn::createh; 
	funmaph["edit"]=(FPH)edit::createh; 
/*
	funmaph["rbut"]=(FPH)rbut::createh;
	funmaph["cbut"]=(FPH)cbut::createh;
	funmaph["hslider"]=(FPH)hslider::createh;
	funmaph["vslider"]=(FPH)vslider::createh;
	funmaph["hscroll"]=(FPH)hscroll::createh; 
*/
}

// helpers

shape* res_loadfile(const C8* str)
{
	factory2<shape> fact;
	script sc=script(str);
	shape* rls=fact.newclass_from_handle(sc);
	if (!rls)
		errorexit("can't find shaperoot for '%s'",str);
	return rls;
}

C32 drift_col(const C32 src,const C32 dst,float t) // t from 0 to 1, 0 stop, 1 instant
{
	S32 r,g,b;
	r=dst.r-src.r;
	g=dst.g-src.g;
	b=dst.b-src.b;
	r=S32(r*t);
	g=S32(g*t);
	b=S32(b*t);
	return C32(src.r+r,src.g+g,src.b+b);
}

namespace res3d {
	
void init_res3d(float vxa,float vya)
{
	vx=vxa;
	vy=vya;
	wt=texture_create("white.pcx");
	if (wt->rc.getrc(wt)==1)
		wt->load();
	pushandsetdir("res3d");
//	if (false) {
//	if (true) {
	if (videoinfo.video_maindriver!=VIDEO_GDI && videoinfo.video_maindriver!=VIDEO_DDRAW) {
//		backing=texture_create("maptestnck.tga");
//#define OLDTEX
#ifdef OLDTEX
		backing=texture_create("stones.jpg"); // load a backing texture if 3d hardware, else sprite draw will use 'font' draw in software
		if (backing->rc.getrc(backing)==1)
			backing->load();
#else
		pushandsetdir("gfxtest");
		backing=texture_create("stone_wall.jpg"); // load a backing texture if 3d hardware, else sprite draw will use 'font' draw in software
		//backing=texture_create("white.pcx"); // load a backing texture if 3d hardware, else sprite draw will use 'font' draw in software
		if (backing->rc.getrc(backing)==1)
			backing->load();
		popdir();
#endif
	}
	popdir();
	pushandsetdir("fonts");
	res3dfont=new fontq("med");
	res3dfont->setcenter(true);
//	res3dfont->setfixedwidth(20);
//	res3dfont->setspace(20);
	res3dfont->setscale(.65f);
	popdir();
}

void exit_res3d()
{
	textureb::rc.deleterc(backing);
	backing=0;
	textureb::rc.deleterc(wt);
	wt=0;
	delete res3dfont;
	res3dfont=0;
}

}
