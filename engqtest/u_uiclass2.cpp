//#define INCLUDE_WINDOWS
#include <m_eng.h>
#include "u_factory.h"
#include "u_uipopup_info.h"
#include "u_uiclass.h"
#include "u_uiclass2.h"

/// class sizer, size up a parent
/// virtual functions
/*void sizer::add_popup_options(class popup* p) const
{
}
*/
void sizer::proc()
{
	if (!doproc)
		return;
	uitree2drect::proc();
	if (!parent)
		return;
	uitree2drect* pinf=dynamic_cast<uitree2drect*>(parent);
	if (!pinf)
		return;
	pointf2 psz=pinf->getsize();
	o2p.x=psz.x-size.x;
	o2p.y=psz.y-size.y;
	if (getfocus()!=this)
		return;
	if (MBUT&M_LBUTTON) {
		if (!(LMBUT&M_LBUTTON)) {
			ref.x=fmxy.x-o2p.x;
			ref.y=fmxy.y-o2p.y;
		}
		o2p.x=fmxy.x-ref.x;
		o2p.y=fmxy.y-ref.y;
		pointf2 npsz;
		npsz.x=o2p.x+size.x;
		npsz.y=o2p.y+size.y;
		pinf->setsize(npsz);
	} else {
		setfocus(parent);
	}
}

void sizer::draw()
{
	if (!dodraw)
		return;
	cliprect32(B32,(S32)o2w.x,(S32)o2w.y,(S32)size.x,(S32)size.y,C32YELLOW);
	cliprecto32(B32,(S32)o2w.x,(S32)o2w.y,(S32)size.x,(S32)size.y,C32BLACK);
	clipline32(B32,(S32)o2w.x,(S32)o2w.y,(S32)(o2w.x+size.x-1),(S32)(o2w.y+size.y-1),C32BLACK);
	uitree2d::draw();
}

void sizer::save(FILE* fh,bool usename)
{
//	if (usename) {
//		printindent(fh);
//		fprintf(fh,"sizer");
//	}
	uitree2drect::save(fh,false);
	if (usename)
		saverec(fh);
}

/// class mover, size up a parent
/// virtual functions
/*void mover::add_popup_options(class popup* p) const
{
}
*/
void mover::proc()
{
	if (!doproc)
		return;
	uitree2drect::proc();
	if (!parent)
		return;
	uitree2drect* pinf=dynamic_cast<uitree2drect*>(parent);
	if (!pinf)
		return;
	o2p.x=0;
	o2p.y=0;
	size.x=pinf->getsize().x-rbord;
	if (getfocus()==this) {
		if (MBUT&M_LBUTTON) {
			if (!(LMBUT&M_LBUTTON)) {
				pointf2 oldo2p=pinf->geto2p();
				ref.x=oldo2p.x-fmxy.x;
				ref.y=oldo2p.y-fmxy.y;
			}
			pointf2 no2p;
			no2p.x=fmxy.x+ref.x;
			no2p.y=fmxy.y+ref.y;
			pinf->seto2p(no2p);
		} else {
			setfocus(parent);
		}
	}
}

void mover::draw()
{
	if (!dodraw)
		return;
	cliprect32(B32,(S32)o2w.x,(S32)o2w.y,(S32)size.x,(S32)size.y,C32LIGHTBLUE);
	cliprecto32(B32,(S32)o2w.x,(S32)o2w.y,(S32)size.x,(S32)size.y,C32BLACK);
	if (parent)
		outtextxyfc32(B32,(S32)(o2w.x+size.x*.5f),(S32)(o2w.y+size.y*.5f),C32WHITE,
		  "%s",parent->getname().c_str()/*,waspressed*/);
	uitree2d::draw();
}

void mover::save(FILE* fh,bool usename)
{
//	if (usename) {
//		printindent(fh);
//		fprintf(fh,"mover");
//	}
//	uitree2drect::save(fh,false);
	uitree2d::save(fh,false);
	fprintf(fh," %f %f",rbord,size.y);
	if (usename)
		saverec(fh);
}

void mover::setsize(const pointf2& sizea)
{
	uitree2drect::setsize(sizea);
	if (!parent)
		return;
	uitree2drect* pinf=dynamic_cast<uitree2drect*>(parent);
	if (!pinf)
		return;
	rbord=pinf->getsize().x-size.x;
}

xscroll::xscroll(const C8* na/*,const C8* na2*/,float xa,float ya,float xsa,float ysa)
  : uitree2drect(na/*,na2*/,xa,ya,xsa,ysa)
{
}

xscroll::xscroll(script& sc)
  : uitree2drect(sc)
{
}

/*
void xscroll::add_popup_options(class popup* p) const
{
}
*/

void xscroll::proc()
{
	if (!doproc)
		return;
	uitree2drect::proc();
	if (!parent)
		return;
	uitree2drect* parentrect=dynamic_cast<uitree2drect*>(parent); // see if a rect
	if (!parentrect)
		return;
	pointf2 psz=parentrect->getsize(); /// adjust position to lower left of parent
//	o2p.y=psz.y+3;
	o2p.y=psz.y-size.y;//0;
	o2p.x=size.y;
	size.x=psz.x-2*size.y;
	if (getfocus()!=this)
		return;
	if (MBUT&M_LBUTTON) {
		float xval=parentrect->getoffsetx();
		float xlocal=fmxy.x-o2w.x;
		float xa=.5f*size.y;
		float xb=size.x-.5f*size.y;
		if (xb<=xa) {
			xval=0;
			return;
		}
		xval=(xlocal-xa)/(xb-xa);
		if (xval<0)
			xval=0;
		else if (xval>1)
			xval=1;
		parentrect->setoffsetx(xval);
	} else {
		setfocus(parent);
	}
//	if (focus==parentrect) {
//		const pointf2& coff=parentrect->getoffset();
//	}
	switch(KEY) {
	case K_RIGHT:
	case K_NUMRIGHT:
	case K_LEFT:
	case K_NUMLEFT:
	case K_DOWN:
	case K_NUMDOWN:
	case K_UP:
	case K_NUMUP:
		setfocus(parent);
		break;
	}
}

void xscroll::draw()
{
	if (!dodraw)
		return;
/// main
	cliprect32(B32,(S32)o2w.x,(S32)o2w.y,(S32)size.x,(S32)size.y,C32GREEN);
	cliprecto32(B32,(S32)o2w.x,(S32)o2w.y,(S32)size.x,(S32)size.y,C32BLACK);
/// top
//	cliprect32(B32,(S32)o2w.x,(S32)o2w.y,(S32)size.x,(S32)size.x,C32CYAN);
//	cliprecto32(B32,(S32)o2w.x,(S32)o2w.y,(S32)size.x,(S32)size.x,C32BLACK);
/// bot
//	cliprect32(B32,(S32)o2w.x,(S32)(o2w.y+size.y-size.x),(S32)size.x,(S32)size.x,C32CYAN);
//	cliprecto32(B32,(S32)o2w.x,(S32)(o2w.y+size.y-size.x),(S32)size.x,(S32)size.x,C32BLACK);
/// mid
	if (!parent)
		return;
	uitree2drect* parentrect=dynamic_cast<uitree2drect*>(parent); // see if a rect
	if (parentrect) {
		float x0=o2w.x;//+size.y;
		float x1=o2w.x+size.x-/*2**/size.y;
		float xval=parentrect->getoffsetx();
		float xc=x0+(x1-x0)*xval;
		cliprect32(B32,(S32)xc,(S32)o2w.y,(S32)size.y,(S32)size.y,C32CYAN);
		cliprecto32(B32,(S32)xc,(S32)o2w.y,(S32)size.y,(S32)size.y,C32BLACK);
//		outtextxybf32(B32,(S32)xc+8,(S32)o2w.y+22,C32BLACK,C32WHITE,"%6.3f",xval);
	}
	uitree2d::draw();
}

/*
void xscroll::save(FILE* fh,bool usename)
{
//	if (usename) {
//		printindent(fh);
//		fprintf(fh,"xscroll");
//	}
	uitree2drect::save(fh,false);
	if (usename)
		saverec(fh);
}
*/


yscroll::yscroll(const C8* na/*,const C8* na2*/,float xa,float ya,float xsa,float ysa)
  : uitree2drect(na/*,na2*/,xa,ya,xsa,ysa)
{
}

yscroll::yscroll(script& sc)
  : uitree2drect(sc)
{
}

/*
void yscroll::add_popup_options(class popup* p) const
{
}
*/
void yscroll::proc()
{
	if (!doproc)
		return;
	uitree2drect::proc();
	if (!parent)
		return;
	uitree2drect* parentrect=dynamic_cast<uitree2drect*>(parent); // see if a rect
	if (!parentrect)
		return;
	pointf2 psz=parentrect->getsize(); /// adjust position to lower left of parent
//	o2p.x=psz.x+3;
	o2p.x=psz.x-size.x;//0;
	o2p.y=size.x;
	size.y=psz.y-2*size.x;
	if (getfocus()!=this)
		return;
	if (MBUT&M_LBUTTON) {
		float yval=parentrect->getoffsety();
		float ylocal=fmxy.y-o2w.y;
		float ya=.5f*size.x; /// starty, right in middle of top square
		float yb=size.y-.5f*size.x; /// end y, right in middle of bottom square
		if (yb<=ya) {
			yval=0; /// nothing to scroll
			return;
		}
		yval=(ylocal-ya)/(yb-ya); /// set offset given ylocal
		if (yval<0)
			yval=0;
		else if (yval>1)
			yval=1;
		parentrect->setoffsety(yval);
	} else {
		setfocus(parent);
	}
//	if (focus==parentrect) {
//		const pointf2& coff=parentrect->getoffset();
//	}
	switch(KEY) {
	case K_RIGHT:
	case K_NUMRIGHT:
	case K_LEFT:
	case K_NUMLEFT:
	case K_DOWN:
	case K_NUMDOWN:
	case K_UP:
	case K_NUMUP:
		setfocus(parent);
		break;
	}
}

void yscroll::draw()
{
	if (!dodraw)
		return;
/// main
	cliprect32(B32,(S32)o2w.x,(S32)o2w.y,(S32)size.x,(S32)size.y,C32GREEN);
	cliprecto32(B32,(S32)o2w.x,(S32)o2w.y,(S32)size.x,(S32)size.y,C32BLACK);
/// top
//	cliprect32(B32,(S32)o2w.x,(S32)o2w.y,(S32)size.x,(S32)size.x,C32CYAN);
//	cliprecto32(B32,(S32)o2w.x,(S32)o2w.y,(S32)size.x,(S32)size.x,C32BLACK);
/// bot
//	cliprect32(B32,(S32)o2w.x,(S32)(o2w.y+size.y-size.x),(S32)size.x,(S32)size.x,C32CYAN);
//	cliprecto32(B32,(S32)o2w.x,(S32)(o2w.y+size.y-size.x),(S32)size.x,(S32)size.x,C32BLACK);
/// mid
	if (!parent)
		return;
	uitree2drect* parentrect=dynamic_cast<uitree2drect*>(parent); // see if a rect
	if (parentrect) {
		float y0=o2w.y;//+size.x;
		float y1=o2w.y+size.y-/*2**/size.x;
		float yval=parentrect->getoffsety();
		float yc=y0+(y1-y0)*yval;
		cliprect32(B32,(S32)o2w.x,(S32)yc,(S32)size.x,(S32)size.x,C32CYAN);
		cliprecto32(B32,(S32)o2w.x,(S32)yc,(S32)size.x,(S32)size.x,C32BLACK);
//		outtextxybf32(B32,(S32)o2w.x+22,(S32)yc+8,C32BLACK,C32WHITE,"%6.3f",yval);
	}
	uitree2d::draw();
}

xslider::xslider(const C8* na/*,const C8* na2*/,float xa,float ya,float xsa,float ysa,float topvala,float botvala,float vala)
  : uitree2drect(na/*,na2*/,xa,ya,xsa,ysa),topval(topvala),botval(botvala),val(e2i(vala))
{
}

xslider::xslider(script& sc)
  : uitree2drect(sc)
{
	topval=(float)atof(sc.read());
	botval=(float)atof(sc.read());
	val=(float)atof(sc.read());
	val=e2i(val);
}

void xslider::add_popup_options(class popup* p) const
{
//	logger("uitree2dbitmap::add popup options\n");
//	return ret;
	uitree2drect::add_popup_options(p);
	p->setup_popup(slider_changeleftrightval);
}

void xslider::proc()
{
	if (!doproc)
		return;
	uitree2drect::proc();
//	return;
	if (!parent)
		return;
	uitree2drect* parentrect=dynamic_cast<uitree2drect*>(parent); // see if a rect
	if (!parentrect)
		return;
//	pointf2 psz=parentrect->getsize(); /// adjust position to lower left of parent
//	o2p.y=psz.y+3;
//	o2p.y=psz.y-size.y;//0;
//	o2p.x=size.y;
//	size.x=psz.x-2*size.y;
	if (getfocus()!=this)
		return;
	if (MBUT&M_LBUTTON) {
//		float xval=parentrect->getoffsetx();
		float xlocal=fmxy.x-o2w.x;
		float xa=.5f*size.y;
		float xb=size.x-.5f*size.y;
		if (xb<=xa) {
			val=0;
			return;
		}
		val=(xlocal-xa)/(xb-xa);
		if (val<0)
			val=0;
		else if (val>1)
			val=1;
//		parentrect->setoffsetx(xval);
//	} else {
//		setfocus(parent);
	}
//	if (focus==parentrect) {
//		const pointf2& coff=parentrect->getoffset();
//	}
/*	switch(KEY) {
	case K_RIGHT:
	case K_NUMRIGHT:
	case K_LEFT:
	case K_NUMLEFT:
	case K_DOWN:
	case K_NUMDOWN:
	case K_UP:
	case K_NUMUP:
		setfocus(parent);
		break;
	} */
}

void xslider::draw()
{
	if (!dodraw)
		return;
/// main
	cliprect32(B32,(S32)o2w.x,(S32)o2w.y,(S32)size.x,(S32)size.y,C32GREEN);
	cliprecto32(B32,(S32)o2w.x,(S32)o2w.y,(S32)size.x,(S32)size.y,C32BLACK);
/// top
//	cliprect32(B32,(S32)o2w.x,(S32)o2w.y,(S32)size.x,(S32)size.x,C32CYAN);
//	cliprecto32(B32,(S32)o2w.x,(S32)o2w.y,(S32)size.x,(S32)size.x,C32BLACK);
/// bot
//	cliprect32(B32,(S32)o2w.x,(S32)(o2w.y+size.y-size.x),(S32)size.x,(S32)size.x,C32CYAN);
//	cliprecto32(B32,(S32)o2w.x,(S32)(o2w.y+size.y-size.x),(S32)size.x,(S32)size.x,C32BLACK);
/// mid
//	if (!parent)
//		return;
//	uitree2drect* parentrect=dynamic_cast<uitree2drect*>(parent); // see if a rect
//	if (parentrect) {
		float x0=o2w.x;//+size.y;
		float x1=o2w.x+size.x-/*2**/size.y;
//		float xval=parentrect->getoffsetx();
		float xc=x0+(x1-x0)*val;
		cliprect32(B32,(S32)xc,(S32)o2w.y,(S32)size.y,(S32)size.y,C32CYAN);
		cliprecto32(B32,(S32)xc,(S32)o2w.y,(S32)size.y,(S32)size.y,C32BLACK);
		float eval=i2e(val);
		texter* ct=find<texter>(0);
		if (ct) {
			char str[100];
			string nstr=ct->getname();
			if (nstr.find("%")!=string::npos)
				sprintf(str,nstr.c_str(),eval);
			else
				sprintf(str,"%f",eval);
			ct->settextname(str);
		} else
			outtextxybf32(B32,(S32)xc+8,(S32)o2w.y+22,C32BLACK,C32WHITE,"%6.3f",eval);
//	}
	uitree2d::draw();
}

void xslider::save(FILE* fh,bool userec)
{
	uitree2drect::save(fh,false);
	fprintf(fh," %f %f %f",topval,botval,i2e(val));
	if (userec)
		saverec(fh);
}

yslider::yslider(const C8* na/*,const C8* na2*/,float xa,float ya,float xsa,float ysa,float topvala,float botvala,float vala)
  : xslider(na/*,na2*/,xa,ya,xsa,ysa,topvala,botvala,vala) {}

yslider::yslider(script& sc) : xslider(sc) {}

void yslider::add_popup_options(class popup* p) const
{
//	logger("uitree2dbitmap::add popup options\n");
//	return ret;
	uitree2drect::add_popup_options(p);
	p->setup_popup(slider_changetopbotval);
}

void yslider::proc()
{
	if (!doproc)
		return;
	uitree2drect::proc();
//	return;
	if (!parent)
		return;
	uitree2drect* parentrect=dynamic_cast<uitree2drect*>(parent); // see if a rect
	if (!parentrect)
		return;
//	pointf2 psz=parentrect->getsize(); /// adjust position to lower left of parent
//	o2p.y=psz.y+3;
//	o2p.y=psz.y-size.y;//0;
//	o2p.x=size.y;
//	size.x=psz.x-2*size.y;
	if (getfocus()!=this)
		return;
	if (MBUT&M_LBUTTON) {
//		float xval=parentrect->getoffsetx();
		float ylocal=fmxy.y-o2w.y;
		float ya=.5f*size.x;
		float yb=size.y-.5f*size.x;
		if (yb<=ya) {
			val=0;
			return;
		}
		val=(ylocal-ya)/(yb-ya);
		if (val<0)
			val=0;
		else if (val>1)
			val=1;
//		parentrect->setoffsetx(xval);
//	} else {
//		setfocus(parent);
	}
//	if (focus==parentrect) {
//		const pointf2& coff=parentrect->getoffset();
//	}
/*	switch(KEY) {
	case K_RIGHT:
	case K_NUMRIGHT:
	case K_LEFT:
	case K_NUMLEFT:
	case K_DOWN:
	case K_NUMDOWN:
	case K_UP:
	case K_NUMUP:
		setfocus(parent);
		break;
	} */
}

void yslider::draw()
{
	if (!dodraw)
		return;
/// main
	cliprect32(B32,(S32)o2w.x,(S32)o2w.y,(S32)size.x,(S32)size.y,C32GREEN);
	cliprecto32(B32,(S32)o2w.x,(S32)o2w.y,(S32)size.x,(S32)size.y,C32BLACK);
/// top
//	cliprect32(B32,(S32)o2w.x,(S32)o2w.y,(S32)size.x,(S32)size.x,C32CYAN);
//	cliprecto32(B32,(S32)o2w.x,(S32)o2w.y,(S32)size.x,(S32)size.x,C32BLACK);
/// bot
//	cliprect32(B32,(S32)o2w.x,(S32)(o2w.y+size.y-size.x),(S32)size.x,(S32)size.x,C32CYAN);
//	cliprecto32(B32,(S32)o2w.x,(S32)(o2w.y+size.y-size.x),(S32)size.x,(S32)size.x,C32BLACK);
/// mid
//	if (!parent)
//		return;
//	uitree2drect* parentrect=dynamic_cast<uitree2drect*>(parent); // see if a rect
//	if (parentrect) {
		float y0=o2w.y;//+size.y;
		float y1=o2w.y+size.y-/*2**/size.x;
//		float xval=parentrect->getoffsetx();
		float yc=y0+(y1-y0)*val;
		cliprect32(B32,(S32)o2w.x,(S32)yc,(S32)size.x,(S32)size.x,C32CYAN);
		cliprecto32(B32,(S32)o2w.x,(S32)yc,(S32)size.x,(S32)size.x,C32BLACK);
		float eval=i2e(val);
		texter* ct=find<texter>(0);
		if (ct) {
			char str[100];
			string nstr=ct->getname();
			if (nstr.find("%")!=string::npos)
				sprintf(str,nstr.c_str(),eval);
			else
				sprintf(str,"%f",eval);
			ct->settextname(str);
		} else
//			outtextxybf32(B32,(S32)xc+8,(S32)o2w.y+22,C32BLACK,C32WHITE,"%6.3f",eval);
			outtextxybf32(B32,(S32)o2w.x+8,(S32)yc+22,C32BLACK,C32WHITE,"%6.3f",eval);
//	}
	uitree2d::draw();
}

/*
void yscroll::save(FILE* fh,bool usename)
{
//	return;	/// don't save popups
//	if (usename) {
//		printindent(fh);
//		fprintf(fh,"yscroll");
//	}
	uitree2drect::save(fh,false);
	if (usename)
		saverec(fh);
}
*/

treeview::treeview(const C8* na,float xa,float ya,float xsa,float ysa) :
  listbox2d(na,xa,ya,xsa,ysa),blink(0),newsel(-1) {}

treeview::treeview(script& sc) : listbox2d(sc),blink(0),newsel(-1) {}

void treeview::populate(uitree* from)
{
	static S32 level;
	string ls="";
	S32 i;
	for (i=0;i<level;++i)
		ls+="  ";
	if (from==tview)
		ls+="*";
	addentry(ls+from->getname());
	if (from==tview) {
		newsel=listdata.size()-1;
		//logger("set newsel to %d\n",newsel);
	}
	tp.push_back(from);
//	setsel(0);
	S32 n=from->getchildrensize();
	for (i=0;i<n;++i) {
		uitree* sib=from->getchildrenidx(i);
		++level;
		populate(sib);
		--level;
	}
}

void treeview::proc()
{
	if (!doproc)
		return;
	listbox2d::proc();
	if (pressed(0)) {
		S32 gs=getsel();
		if (gs>=0) {
			logger("getsel %d\n",gs);
			if (gs>=(S32)(tp.size()))
				errorexit("treeview:: bad gs size");
			tview=tp[gs];
		} else
			tview=0;
	}
	//	listdata.clear();
	S32 oldsel=getsel();
	clearlist();
	tp.clear();
	if (parent) {
		newsel=-1;
		populate(parent);
		setsel(newsel,newsel!=oldsel);
	}
	++blink;
	if (blink==30)
		blink=0;
}

void treeview::draw()
{
	if (!dodraw)
		return;
//	cliprect32(B32,(S32)o2w.x,(S32)o2w.y,(S32)size.x,(S32)size.y,C32(50,50,50));
//	cliprecto32(B32,(S32)o2w.x,(S32)o2w.y,(S32)size.x,(S32)size.y,C32BLACK);
////	outtextxyc32(B32,(S32)(o2w.x),(S32)(o2w.y),C32BLACK,"rect");
	listbox2d::draw();
	uitree2drect* tvr=dynamic_cast<uitree2drect*>(tview);
	if (tvr) {
		if (blink<10) {
			tvr->showoutline(2,C32(255,255,0));
			tvr->showoutline(3,C32(0,255,255));
			tvr->showoutline(4,C32(255,0,255));
		} else if (blink<20) {
			tvr->showoutline(12,C32(255,255,0));
			tvr->showoutline(13,C32(0,255,255));
			tvr->showoutline(14,C32(255,0,255));
		} else {
			tvr->showoutline(22,C32(255,255,0));
			tvr->showoutline(23,C32(0,255,255));
			tvr->showoutline(24,C32(255,0,255));
		}
	}

//	uitree2d::draw();
}
/*
	cliprect32(B32,(S32)o2w.x,(S32)o2w.y,(S32)size.x,(S32)size.y,C32(100,220,172));
//	outtextxyfc32(B32,(S32)(o2w.x+size.x*.5f),(S32)(o2w.y+size.y*.5f),C32BLACK,"off %6.3f %6.3f",offset.x,offset.y);
	S32 i;
	S32 nc=listdata.size();
	setcliprect32(B32,(S32)o2w.x,(S32)o2w.y,(S32)size.x,(S32)size.y);
	pointf2 off;
	off.y=offset.y*(size.y-nc*10-40-40);
	if (off.y>0)
		off.y=0;
	S32 oy=(S32)(o2w.y+off.y+40);
	for (i=0;i<nc;++i) {
		S32 oy2=oy+10*i;
		if (i==sel) {
			cliprect32(B32,(S32)o2w.x,oy2,(S32)size.x,10,C32BLACK);
			outtextxyf32(B32,(S32)(o2w.x+20),oy2,C32WHITE,"%s",listdata[i].c_str());
		} else
			outtextxyf32(B32,(S32)(o2w.x+20),oy2,C32BLACK,"%s",listdata[i].c_str());
	}
	resetcliprect32(B32);
	uitree2d::draw();
*/
/// class popup
/// virtual functions
/// non virtual functions
void popup::add_popup_options(class popup* p) const
{
	logger("wow, in popup::add_popup_options\n");
}

void popup::setup_popup(const popup_info* choices)
{
//	logger("setuppopup\n");
/// find end of data
	S32 nchoices=0;
		while(choices[nchoices].butname)
			++nchoices;
	S32 i;
	for (i=0;i<nchoices;++i) {
		S32 sl=strlen(choices[i].butname);
		if (sl>curwid)
			curwid=sl;
	}
	size.x=(float)(8*curwid+6+6);
	S32 j=0;
	for (i=0;i<nchoices;++i) {
		C8 n[10];
		j=i+piv.size();
		sprintf(n,"__%d",j);
		uitree* but=new button(n,3,(float)(3+13+j*(8+3+3+3)),size.x-3-3,8+3+3,choices[i].butname);
		linkchild(but);
	}
	++j;
	size.y=(float)(3+13+j*(8+3+3+3));
	if (piv.empty()) {
		deleter* del=new deleter("del",0,0,10,10,"X");
		linkchild(del);
		mover* mov=new mover("mov",0,0,10,10);
		o2p.x-=size.x/2;
		o2p.y-=10;
		linkchild(mov);
	}
	setfocus(this);
//	pi=choices;
	piv.insert(piv.end(),&choices[0],&choices[nchoices]);
	uitree2drect* parentr=dynamic_cast<uitree2drect*>(parent);
	if (!parentr)
		errorexit("bad parentr");
	const pointf2 po2p=parentr->geto2p();
	const pointf2 po2w=parentr->geto2w();
	const pointf2 psize=parentr->getsize();
	pointf2 to2p;
	to2p.x=-po2w.x;
	to2p.y=-po2w.y;
	if (o2p.x<to2p.x)
		o2p.x=to2p.x;
	if (o2p.y<to2p.y)
		o2p.y=to2p.y;
	to2p.x=(float)(WX)-po2w.x-size.x;
	to2p.y=(float)(WY)-po2w.y-size.y;
	if (o2p.x>to2p.x)
		o2p.x=to2p.x;
	if (o2p.y>to2p.y)
		o2p.y=to2p.y;
//	logger("now has %d buttons\n",piv.size());
}

popup::popup(const C8* na/*,const C8* na2*/,float xa,float ya,float xsa,float ysa)
  : uitree2drect(na/*,na2*/,xa,ya,xsa,ysa),curwid(0)
{
//	setup_popup(pia);
	setfocus(this);
	++num_popup;
}

void popup::proc()
{
	if (!doproc)
		return;
/// children first
	uitree2drect::proc();
	if (!getfocus()) {
		killc=true;
		return;
	}
/// kill if not focused
	if (getfocus()!=this) {
		uitree* p=getfocus()->getparent();
		if (p!=this)
			killc=true;
	}
/// build buttons from parent if not yet built
	if (piv.empty() && parent) {
		uitree2drect* pr=dynamic_cast<uitree2drect*>(parent);
		if (pr) {
			pr->add_popup_options(this);
		}
		if (piv.empty())
			killc=true;
	}
/*
//		const popup_info* pi=pr->get_popup_options();
		if (pi && pi[0].butname) {
			setup_popup(pi);
		} else
			killc=true;
	}
*/
/// check the buttons and call appropriate function
	S32 i,j,nc=children.size();	/// skip delete and mover
	for (i=0;i<nc;++i) {
		button* but=dynamic_cast<button*>(children[i]);
		if (but && but->pressed()) {
///			focus=0;
			j=atoi(children[i]->getname().c_str()+2);
//			logger("button %d pressed\n",j);
			if (piv[j].popup_fun)
				piv[j].popup_fun(parent,o2p);
			break;
		}
	}
}

void popup::draw()
{
	if (!dodraw)
		return;
	cliprect32(B32,(S32)o2w.x,(S32)o2w.y,(S32)size.x,(S32)size.y,C32LIGHTGRAY);
	cliprecto32(B32,(S32)o2w.x,(S32)o2w.y,(S32)size.x,(S32)size.y,C32BLACK);
//	outtextxybf32(B32,(S32)(o2w.x-10),(S32)(o2w.y-10),C32BLACK,C32WHITE,"%d",children.size());
	uitree2d::draw();
}

void popup::save(FILE* fh,bool usename)
{
	return;	/// don't save popups
}

changename::changename(const C8* na/*,const C8* na2*/,float xa,float ya,float xsa,float ysa,const C8* cnscripta)
  : uitree2drect(na/*,na2*/,xa,ya,xsa,ysa),eb(0),ok(0),can(0)/*,gn(gna),sn(sna)*/,cnscript(cnscripta)
{
	scriptbuildcopy(cnscripta);
}

void changename::proc()
{
	if (!doproc)
		return;
	uitree2drect::proc();
	if (!parent)
		return;
	if (!eb) {
		eb=find<editbox>("eb");
		if (eb) {
//			logger("get eb set focus\n");
			setfocus(eb);
			eb->settextname(parentgetstring());
		}
	}
	if (!ok) {
		ok=find<button>("ok");
//		ok=dynamic_cast<button*>(t);
	}
	if (!can) {
		can=find<button>("cancel");
	}
	if (can && can->pressed())
		dokillc();
	if (ok && ok->pressed() || (eb && eb->enterpressed())) {
		string n=eb->gettextname();
		parentloadsavestring(n);
		dokillc();
	}
	if (getfocus()==this)
		setfocus(eb);
}

void changename::parentloadsavestring(string s)
{
	if (!parent)
		return;
	parent->setname(s);
}

string changetextname::parentgetstring() const
{
	if (!parent)
		return "";
	texter* pt=dynamic_cast<texter*>(parent);
	if (!pt)
		return "";
	return pt->gettextname();
}

void changetextname::parentloadsavestring(string s)
{
	if (!parent)
		return;
	texter* pt=dynamic_cast<texter*>(parent);
	if (!pt)
		return;
	pt->settextname(s);
}

string changebuttonname::parentgetstring() const
{
	if (!parent)
		return "";
	button* pt=dynamic_cast<button*>(parent);
	if (!pt)
		return "";
	return pt->getbuttonname();
}

void changebuttonname::parentloadsavestring(string s)
{
	if (!parent)
		return;
	button* pt=dynamic_cast<button*>(parent);
	if (!pt)
		return;
	pt->setbuttonname(s);
}

string changeslidertopval::parentgetstring() const
{
	if (!parent)
		return "0.0";
	xslider* pt=dynamic_cast<xslider*>(parent);
	if (!pt)
		return "0.0";
	C8 numstr[200];
	sprintf(numstr,"%f",pt->gettopval());
	return numstr;
}

void changeslidertopval::parentloadsavestring(string s)
{
	if (!parent)
		return;
	xslider* pt=dynamic_cast<xslider*>(parent);
	if (!pt)
		return;
//	pt->setbuttonname(s);
	pt->settopval((float)(atof(s.c_str())));
}

string changesliderbotval::parentgetstring() const
{
	if (!parent)
		return "0.0";
	xslider* pt=dynamic_cast<xslider*>(parent);
	if (!pt)
		return "0.0";
	C8 numstr[200];
	sprintf(numstr,"%f",pt->getbotval());
	return numstr;
}

void changesliderbotval::parentloadsavestring(string s)
//void changetextname::parentsetstring(string s)
{
	if (!parent)
		return;
	xslider* pt=dynamic_cast<xslider*>(parent);
	if (!pt)
		return;
//	pt->setbuttonname(s);
	pt->setbotval((float)(atof(s.c_str())));
}

string listbox2daddentry::parentgetstring() const
{
	return "";
}

void listbox2daddentry::parentloadsavestring(string s)
{
	if (!parent)
		return;
	listbox2d* pt=dynamic_cast<listbox2d*>(parent);
	if (!pt)
		return;
	pt->addentry(s);
}

loadscriptdialog::loadscriptdialog(
	  const C8* na,const C8* startdir/*,const C8* na2*/,float xa,float ya,float xsa,float ysa
	  /*mgetnamefunp gna,msetnamefunp sna,mloadsavedatafunp lsda*/,bool issavea,bool prevua,const C8** extsa,const C8* lsscripta)
  : uitree2drect(na/*,na2*/,xa,ya,xsa,ysa),
	  lb(0),ok(0),can(0)/*,gn(gna),sn(sna),lsd(lsda)*/,issave(issavea),tb(0),eb(0),tbd(0),curdir(startdir),nups(0),ndirs(0),nfiles(0),prevu(prevua),exts(extsa),lsscript(lsscripta)
{
	revertdir=curdir;
	scriptbuildcopy(lsscripta);
}

bool loadscriptdialog::checkext(string fname)
{
	if (!exts)
		return true;
	C8 ex[MAX_PATH];
	if (mgetext(fname.c_str(),ex,MAX_PATH)) {
		const C8* const * p=exts;
		while(*p) {
			if (!my_stricmp(*p,ex))
				return true;
			++p;
		}
	}
	return false;
}

static const C8* str_truncate(const C8* s,S32 cu)
{
	static C8 trun[MAX_PATH];
	S32 sl=strlen(s);
	if (sl<=cu)
		return s;
	if (sl<=3)
		return s;
	strcpy(trun,"...");
	strcat(trun,s+sl-cu+3);
	return trun;
}

static const C8* updir(const C8* s)
{
	S32 i=strlen(s);
	if (i==0) {
		return s;
	}
	static C8 up[MAX_PATH]; /// persistent little devils
	strcpy(up,s);
	--i;
	while(i>=0) {
		if (up[i]=='\\') {
			up[i]='\0';
			if ((strlen(up)==2) && up[1]==':')
				strcat(up,"\\");
			return up;
		}
		--i;
	}
	return s;
}

static const C8* downdir(const C8* s,const C8* dir)
{
	logger("in downdir with '%s' and '%s'\n",s,dir);
	S32 i=strlen(s);
	if (i==0) {
		logger("returning 1 '%s'\n",s);
		return s;
	}
	static C8 dn[MAX_PATH]; /// persistent little devils
	strcpy(dn,s);
	if (s[i-1]!='\\')
		strcat(dn,"\\");
	strcat(dn,dir+1);
	dn[strlen(dn)-1]='\0';
	return dn;
}

void loadscriptdialog::populateloadsavelist()
{
/// fill out the directories and files in the listbox
	lb->clearlist();
	nups=ndirs=nfiles=0;
	lb->addentry("[..]");
	++nups;
	scriptdir* sc = new scriptdir(1);
	sc->sort();
	S32 nc=sc->num();
	S32 i;
	for (i=0;i<nc;++i) {
		sc->idx(i)=string("[")+sc->idx(i)+"]";
		lb->addentry(sc->idx(i).c_str());
	}
	ndirs+=nc;
	delete sc;
	sc = new scriptdir(0);
	sc->sort();
	nc=sc->num();
	for (i=0;i<nc;++i) {
		string et=sc->idx(i);
//		C8 ex[MAX_PATH];
/*		if (mgetext(et,ex)) {
			const C8* const * p=exts;
			while(*p) {
				if (!my_stricmp(*p,ex)) {
					lb->addentry(et);
					break;
				}
				++p;
			}
		} */
		if (checkext(et))
			lb->addentry(et);
	}
	nfiles+=nc;
	delete sc;
	lb->setoffsety(0);
}

void loadscriptdialog::proc()
{
	if (!doproc)
		return;
/// proc children first
	uitree2drect::proc();
/// no point if no parent to work on
	if (!parent)
		return;
/// find listbox etc.
	if (!tbd) {
		tbd=find<texter>("curdir");
//		tbd=dynamic_cast<texter*>(t);
		if (tbd) {
			uitree2dbitmap* bm=dynamic_cast<uitree2dbitmap*>(parent);
			if (bm) {
				curdir=bm->getbitmapdir();
				revertdir=curdir;
			}
			tbd->settextname(str_truncate(absdir2reldirdisplay(curdir).c_str(),(S32)(tbd->getsizex()/8)));
		}
	}
	if (!lb) {
		lb=find<listbox2d>("lb");
//		lb=dynamic_cast<listbox2d*>(t);
		if (lb) {
			setfocus(lb);
			pushandsetdir(curdir.c_str());
			populateloadsavelist();
			popdir();
		} else {
			errorexit("can't find lb in '%s'",script2printc(getname()).c_str());
		}
	}
	if (!ok) {
		ok=find<button>("ok");
//		ok=dynamic_cast<button*>(t);
	}
	if (!can) {
		can=find<button>("cancel");
//		can=dynamic_cast<button*>(t);
	}
	if (issave) {
		if (!eb) {
			eb=find<editbox>("eb");
//			eb=dynamic_cast<editbox*>(t);
		}
	} else {
		if (!tb) {
			tb=find<texter>("tb");
//			tb=dynamic_cast<texter*>(t);
		}
	}
/// get original name and save in revert and show in text/editbox
	if (revert.empty() && !(parentgetstring().empty())) {
		revert=parentgetstring(); // member function pointer, get name
		logger("setting revert to '%s'\n",revert.c_str());
		if (tb)
			tb->settextname(revert.c_str());
		if (eb)
			eb->settextname(revert.c_str());
	}
/// listbox selection
	bool pm;
	if (lb && lb->pressed(&pm)) {
		S32 sl=lb->getsel();
/// updir
		if (sl==0 && nups==1) {
			if (pm) {
				curdir=updir(curdir.c_str());
				if (tbd)
					tbd->settextname(str_truncate(absdir2reldirdisplay(curdir).c_str(),(S32)(tbd->getsizex()/8)));
				pushandsetdir(curdir.c_str());
				populateloadsavelist();
				popdir();
			}
/// downdir
		} else if (sl>=nups && sl<nups+ndirs) {
			if (pm) {
				string n=lb->getselstr();
				curdir=downdir(curdir.c_str(),n.c_str());
				if (tbd)
					tbd->settextname(str_truncate(absdir2reldirdisplay(curdir).c_str(),(S32)(tbd->getsizex()/8)));
				pushandsetdir(curdir.c_str());
				populateloadsavelist();
				popdir();
			}
/// files
		} else {
			string n=lb->getselstr();
//			if (sn)
//				(parent->*sn)(n); // member function pointer, set name
			if (!issave && prevu) { // preview if wanted
				pushandsetdir(curdir.c_str());
				parentloadsavestring(n);
//				(parent->*lsd)(n); // member function pointer, load/save script/data
				popdir();
			}
			if (tb)
				tb->settextname(n);
			if (eb)
				eb->settextname(n);
		}
	}
/// ok button selection
	if (ok && ok->pressed()) {
		string n;
		if (tb)
			n=tb->gettextname();
		if (eb)
			n=eb->gettextname();
//		if (sn)
//			(parent->*sn)(n); // member function pointer, set name
		pushandsetdir(curdir.c_str());
		parentloadsavestring(n);
//		(parent->*lsd)(n); // member function pointer, load/save script/data
		popdir();
		dokillc();
	}
/// cancel button selection
	if (can && can->pressed()) {
		logger("reverting back to '%s'\n",revert.c_str());
//		if (sn)
//			(parent->*sn)(revert); // member function pointer, set name
		if (!issave && prevu) {
			pushandsetdir(revertdir.c_str());
//			(parent->*lsd)(revert); // member function pointer, load/save script/data
			parentloadsavestring(revert);
			popdir();
		}
		dokillc();
	}
/// edit box selection (enter pressed)
	if (eb && eb->enterpressed()) {
		string n;
		if (eb)
			n=eb->gettextname();
//		if (sn)
//			(parent->*sn)(n); // member function pointer, set name
		pushandsetdir(curdir.c_str());
//		(parent->*lsd)(n); // member function pointer, load/save script/data
		parentloadsavestring(n);
		popdir();
		dokillc();
	}
/// keep focus on listbox
	if (getfocus()==this)
		setfocus(lb);
}

void loadscriptdialog::parentloadsavestring(string s)
{
	if (!parent)
		return;
	parent->loadscript2fn(s);
}

void savescriptdialog::parentloadsavestring(string s)
{
	if (!parent)
		return;
/*	C8 ex[MAX_PATH];
	if (mgetext(s.c_str(),ex)) {
		const C8* const * p=exts;
		while(*p) {
			if (!my_stricmp(*p,ex)) {
				parent->savescript2fn(s);
				logger("saving '%s'\n",s.c_str());
				return;
			}
			++p;
		}
		logger("failed to find an ext for '%s'\n",s.c_str());
	} */
	if (checkext(s))
		parent->savescript2fn(s);
	else
		logger("failed to find an ext for '%s'\n",s.c_str());
}

string loadbitmapdialog::parentgetstring() const
{
	if (!parent)
		return "";
	uitree2dbitmap* pt=dynamic_cast<uitree2dbitmap*>(parent);
	if (!pt)
		return "";
	return pt->getbitmapname();
}

void loadbitmapdialog::parentloadsavestring(string s)
{
	if (!parent)
		return;
	uitree2dbitmap* pt=dynamic_cast<uitree2dbitmap*>(parent);
	if (!pt)
		return;
	return pt->loaddata2fn(s);
}

string savebitmapdialog::parentgetstring() const
{
	if (!parent)
		return "";
	uitree2dbitmap* pt=dynamic_cast<uitree2dbitmap*>(parent);
	if (!pt)
		return "";
	return pt->getbitmapname();
}

void savebitmapdialog::parentloadsavestring(string s)
{
	if (!parent)
		return;
	uitree2dbitmap* pt=dynamic_cast<uitree2dbitmap*>(parent);
	if (!pt)
		return;
/*	C8 ex[MAX_PATH];
	if (mgetext(s.c_str(),ex)) {
		const C8* const * p=exts;
		while(*p) {
			if (!my_stricmp(*p,ex)) {
				pt->savedata2fn(s);
				logger("saving '%s'\n",s.c_str());
				return;
			}
			++p;
		}
		logger("failed to find an ext for '%s'\n",s.c_str());
	} */
	if (checkext(s)) {
		pt->savedata2fn(s);
		logger("saving '%s'\n",s.c_str());
	} else
		logger("failed to find an ext for '%s'\n",s.c_str());
}



/// class cloner, clone parent
/// virtual functions
/*void cloner::add_popup_options(class popup* p) const
{
}
*/
void cloner::proc()
{
	if (!doproc)
		return;
	button::proc();		/// i'm a button, get pressed
	if (!parent)
		return;
	uitree2drect* parentrect=dynamic_cast<uitree2drect*>(parent); // see if a rect
	if (parentrect) {
		pointf2 psz=parentrect->getsize(); // adjust position to lower left of parent
		o2p.x=0;
		o2p.y=psz.y-size.y; // 0;
	}
	if (!pressed())
		return;
	uitree* parentparent=parent->getparent(); // get parent's parent
	if (!parentparent)
		return;
	uitree* parentcopy=parent->copy(); // copy parent
	uitree2drect* parentrectcopy=dynamic_cast<uitree2drect*>(parentcopy);
	if (parentrectcopy) { // adjust pos parentcopy if a rect
		pointf2 pp=parentrectcopy->geto2p();
		pp.x+=3;
		pp.y+=3;
		parentrectcopy->seto2p(pp);
	}
	parentparent->linkchild(parentcopy);
	uitree::setfocus(parentcopy);
}

void cloner::draw()
{
	if (!dodraw)
		return;
	C32 c = C32LIGHTGRAY;
	cliprect32(B32,(S32)o2w.x,(S32)o2w.y,(S32)size.x,(S32)size.y,c);
	cliprecto32(B32,(S32)o2w.x,(S32)o2w.y,(S32)size.x,(S32)size.y,C32BLACK);
	outtextxyfc32(B32,(S32)(o2w.x+size.x*.5f),(S32)(o2w.y+size.y*.5f),C32BLACK,
	  "%s",bname.c_str()/*,waspressed*/);
	uitree2d::draw();
}

void cloner::save(FILE* fh,bool usename)
{
//	if (usename) {
//		printindent(fh);
//		fprintf(fh,"cloner");
//	}
	button::save(fh,false);
	if (usename)
		saverec(fh);
}

/// class deleter, delete parent
/// virtual functions
/*void deleter::add_popup_options(class popup* p) const
{
}
*/
void deleter::proc()
{
	if (!doproc)
		return;
	button::proc();		/// i'm a button, get pressed
	if (!parent)
		return;
	uitree2drect* parentrect=dynamic_cast<uitree2drect*>(parent); // see if a rect
	if (parentrect) {
		pointf2 psz=parentrect->getsize(); /// adjust position to lower left of parent
		o2p.x=psz.x-size.x;//0;
		o2p.y=0;
	}
	if (!pressed())
		return;
	uitree* parentparent=parent->getparent(); /// get parent's parent
	if (!parentparent) /// must not delete the root, (safety)
		return;
	parent->dokillc();
}

void deleter::draw()
{
	if (!dodraw)
		return;
	C32 c = C32RED;
	cliprect32(B32,(S32)o2w.x,(S32)o2w.y,(S32)size.x,(S32)size.y,c);
	cliprecto32(B32,(S32)o2w.x,(S32)o2w.y,(S32)size.x,(S32)size.y,C32BLACK);
	outtextxyfc32(B32,(S32)(o2w.x+size.x*.5f),(S32)(o2w.y+size.y*.5f),C32WHITE,
	  "%s",bname.c_str()/*,waspressed*/);
	uitree2d::draw();
}

void deleter::save(FILE* fh,bool usename)
{
//	if (usename) {
//		printindent(fh);
//		fprintf(fh,"deleter");
//	}
	button::save(fh,false);
	if (usename)
		saverec(fh);
}

/// class derived button
/// virtual functions
void buttest0::proc()
{
	if (!doproc)
		return;
	button::proc();
	if (!pressed())
		return;
	uitree2d* pinf=dynamic_cast<uitree2d*>(parent);
	if (!pinf)
		return;
	pointf2 p=pinf->geto2p();
	p.x+=10;
	pinf->seto2p(p);
}

/*void buttest0::save(FILE* fh,bool usename)
{
//	if (usename) {
//		printindent(fh);
//		fprintf(fh,"buttest0");
//	}
	button::save(fh,false);
	if (usename)
		saverec(fh);
}
*/
/// class derived button
/// virtual functions
void buttest1::proc()
{
	if (!doproc)
		return;
	button::proc();
	if (!pressed())
		return;
	uitree2d* pinf=dynamic_cast<uitree2d*>(parent);
	if (!pinf)
		return;
	pointf2 p=pinf->geto2p();
	p.x-=10;
	pinf->seto2p(p);
}

/*void buttest1::save(FILE* fh,bool usename)
{
//	if (usename) {
//		printindent(fh);
//		fprintf(fh,"buttest1");
//	}
	button::save(fh,false);
	if (usename)
		saverec(fh);
}
*/
/// class button
/// virtual functions
void checkbox::proc()
{
	if (!doproc)
		return;
/*	uitree2drect::proc();
	if (getfocus()==this) {
		bool isinsiderect=isinside(fmxy);
		if ((MBUT&M_LBUTTON) && !(LMBUT&M_LBUTTON) && isinsiderect) {
			setcheck(!getcheck());
		}
	}
*/
	uitree2drect::proc();
	if (getfocus()==this) {
		bool isinsiderect=isinside(fmxy);
		if (MBUT&M_LBUTTON) {
			down=isinsiderect;
		} else {
			if (down) {
				if (isinsiderect) {
					setcheck(!getcheck());
				}
				down=false;
			}
		}
//		usedinput=true;
	}
}

void checkbox::draw()
{
	if (!dodraw)
		return;
	C32 c2 = down ? C32LIGHTGRAY : C32WHITE;
	C32 c1(220,210,200);
//	C32 c2 = C32WHITE;
	C32 c3 = C32BLACK;
	float cx=o2w.x+(size.x-size.y)*.5f;
	float cy=o2w.y+size.y*.5f;
	cliprect32(B32,(S32)o2w.x,(S32)o2w.y,(S32)size.x,(S32)size.y,c1);
	float stx=o2w.x+size.x-size.y*.95f;
	float sty=o2w.y+size.y*.05f;
	float sz=size.y*.9f;
	cliprect32(B32,(S32)(stx),(S32)(sty),(S32)(sz),(S32)(sz),c2);
//	cliprecto32(B32,(S32)o2w.x+3,(S32)o2w.y+3,(S32)size.x-6,(S32)size.y-6,c2);
//	cliprecto32(B32,(S32)o2w.x,(S32)o2w.y,(S32)size.x,(S32)size.y,c2);
	outtextxyfc32(B32,(S32)(cx),(S32)(cy),c3,"%s",tname.c_str());
	if (checked) {
		float li0x=stx+.75f*sz;
		float li0y=sty+.25f*sz;
		float li1x=stx+.50f*sz;
		float li1y=sty+.75f*sz;
		float li2x=stx+.25f*sz;
		float li2y=sty+.50f*sz;
		clipline32(B32,(S32)(li0x),(S32)(li0y),(S32)(li1x),(S32)(li1y),c3);
		clipline32(B32,(S32)(li1x),(S32)(li1y),(S32)(li2x),(S32)(li2y),c3);
		clipline32(B32,(S32)(li0x+1),(S32)(li0y),(S32)(li1x+1),(S32)(li1y),c3);
		clipline32(B32,(S32)(li1x+1),(S32)(li1y),(S32)(li2x+1),(S32)(li2y),c3);
	}
	uitree2d::draw();
}

void checkbox::save(FILE* fh,bool userec)
{
	texter::save(fh,false);
	fprintf(fh," %d",checked);
	if (userec)
		saverec(fh);
}

void radiobutton::setcheck(bool v)
{
	if (parent) {
		if (v) { /// clear (checks/radio buttons) for all other siblings that match name
			S32 basenum;
			string basestr;
			basestr=getstringnum(getname(),basenum);
			S32 i,n=parent->getchildrensize();
			for (i=0;i<n;++i) {
				uitree* sib=parent->getchildrenidx(i);
				radiobutton* rbs=dynamic_cast<radiobutton*>(sib);
				if (rbs) {
//					logger("childname is '%s'\n",script2print(rbs->getname()).c_str());
					S32 sibnum;
					string sibstr;
					sibstr=getstringnum(sib->getname(),sibnum);
					if (sibstr==basestr)
						rbs->setcheck(false);
				}
			}
		}
	}
	checkbox::setcheck(v);
}

void radiobutton::draw()
{
	if (!dodraw)
		return;
	C32 c2 = down ? C32LIGHTGRAY : C32WHITE;
	C32 c1(210,220,200);
//	C32 c2 = C32WHITE;
	C32 c3 = C32BLACK;
	float cx=o2w.x+(size.x-size.y)*.5f;
	float cy=o2w.y+size.y*.5f;
	cliprect32(B32,(S32)o2w.x,(S32)o2w.y,(S32)size.x,(S32)size.y,c1);
	float stx=o2w.x+size.x-size.y*.95f;
	float sty=o2w.y+size.y*.05f;
	float sz=size.y*.9f;
	float li0x=stx+.5f*sz;
	float li0y=sty+.5f*sz;
	clipcircle32(B32,(S32)(li0x),(S32)(li0y),(S32)(sz*.45f),c2);
//	cliprect32(B32,(S32)(stx),(S32)(sty),(S32)(sz),(S32)(sz),c2);
//	cliprecto32(B32,(S32)o2w.x+3,(S32)o2w.y+3,(S32)size.x-6,(S32)size.y-6,c2);
//	cliprecto32(B32,(S32)o2w.x,(S32)o2w.y,(S32)size.x,(S32)size.y,c2);
	outtextxyfc32(B32,(S32)(cx),(S32)(cy),c3,"%s",tname.c_str());
	if (getcheck()) {
		clipcircle32(B32,(S32)(li0x),(S32)(li0y),(S32)(sz*.25f),c3);
	}
	uitree2d::draw();
}

void tabs::proc()
{
	if (!doproc)
		return;
/// select children to proc
S32 i;
	S32 nc=children.size();
	S32 ne=-1;
	for (i=0;i<nc;++i) {
		string nam=children[i]->getname();
		S32 fidx=nam.find("__edit");
		if (fidx!=0) {
			ne=i;
//			logger("ne = %d\n",ne);
			break;
		}
	}
	if (ne>=0 && idx<ne)
		idx=ne;
	if (idx>=nc)
		idx=nc-1;
	for (i=0;i<nc;++i) {
		string nam=children[i]->getname();
		S32 fidx=nam.find("__edit");
		bool dop=i==idx || fidx==0 || dynamic_cast<popup*>(children[i]);
		children[i]->setdoproc(dop);
		children[i]->setdodraw(dop);
	}
	for (i=0;i<nc;++i) {
		string nam=children[i]->getname();
		S32 fidx=nam.find("__edit");
		if (fidx!=0) {
			uitree2drect* cr=dynamic_cast<uitree2drect*>(children[i]);
			if (cr && !dynamic_cast<popup*>(cr) && !dynamic_cast<changename*>(cr)) {
				pointf2 p;
				p.x=size.x-0;
				p.y=size.y-0-35;
				cr->setsize(p);
				p.x=0;
				p.y=35+0;
				cr->seto2p(p);
			}
		}
	}
/// proc children first
	uitree2drect::proc();
//	clickfocus();
/// click on the tabs
	if (getfocus()!=this)
		return;
	if (!isinside(fmxy))
		return;
	if ((!(MBUT&M_LBUTTON)||(LMBUT&M_LBUTTON)) && idx>=0)
		return;
	if (nc<=0)
		return;
	S32 xo=4;
//	S32 oy=(S32)(o2w.y+36);
	for (i=0;i<nc;++i) {
		string nam=children[i]->getname();
		S32 fidx=nam.find("__edit");
		if (fidx!=0 && !dynamic_cast<popup*>(children[i])) {
			string spnam=nam;
//			string spnam=script2print(nam);
			S32 xn=xo+16+spnam.size()*8;
			S32 x0=(S32)o2w.x+xo;
			S32 y0=(S32)o2w.y+20;
			S32 xs=xn-xo-8;
			S32 ys=16;
			if (idx<0)
				idx=i;
			if (fmxy.x>=x0 && fmxy.x<x0+xs && fmxy.y>=y0 && fmxy.y<y0+ys) {
				idx=i;
				setfocus(children[idx]);
			}
			xo=xn;
		}
	}
}

void tabs::draw()
{
	if (!dodraw)
		return;
	showoutline(1,C32(255,200,150));
	showoutline(2,C32(200,200,150));
	showoutline(3,C32(150,200,150));
	S32 nc=children.size();
	S32 i,xo=4;
	cliprect32(B32,(S32)o2w.x,(S32)o2w.y,(S32)size.x,35,C32WHITE);
	S32 oy=(S32)(o2w.y+36);
	clipline32(B32,(S32)(o2w.x),oy,(S32)(o2w.x+size.x),oy,C32BLACK);
	if (nc<=0)
		return;
	if (idx<0)
		idx=0;
	if (idx>=nc)
		idx=nc-1;
	for (i=0;i<nc;++i) {
		string nam=children[i]->getname();
		S32 fidx=nam.find("__edit");
		if (fidx!=0 && !dynamic_cast<popup*>(children[i])) {
			string spnam=nam;
//			string spnam=script2print(nam);
			S32 xn=xo+16+spnam.size()*8;
			S32 x0=(S32)o2w.x+xo;
			S32 y0=(S32)o2w.y+20;
			S32 xs=xn-xo-8;
			S32 ys=16;
			cliprecto32(B32,x0,y0,xs,ys,C32BLACK);
			if (i==idx) {
				cliprect32(B32,x0+1,y0+3,xs-2,ys-2,C32WHITE);
				outtextxyf32(B32,(S32)(o2w.x+xo+4),(S32)(o2w.y+22),C32BLACK,spnam.c_str());
			} else {
				cliprect32(B32,x0+1,y0+1,xs-2,ys-2,C32WHITE);
				outtextxyf32(B32,(S32)(o2w.x+xo+4),(S32)(o2w.y+26),C32BLACK,spnam.c_str());
			}
			xo=xn;
		}
	}
	uitree2d::draw();
}
