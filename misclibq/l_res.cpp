#include <m_eng.h>
#include <l_misclibm.h>
namespace res2d {
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

void shape::draw() const
{
	if (!vis)
		return;
	S32 i,n=children.size();
	for (i=0;i<n;++i)
		children[i]->draw();
}

shape* shape::getfocus()
{
	S32 i,n=children.size();
	if (!vis)
		return 0;
	for (i=n-1;i>=0;--i) {
		shape* ret=children[i]->getfocus();
		if (ret)
			return ret;
	}
	if (isinside())
		return this;
	return 0;
}

S32 shape::proc()
{
	return -1;
}

/////////// back derived class
back::back(script& sc) : shape(sc),color(C32BLUE)
{
	xs=atoi(sc.read());
	ys=atoi(sc.read());
}

back::back(const C8* namea,S32 xa,S32 ya,S32 xsa,S32 ysa) : shape(namea,xa,ya),xs(xsa),ys(ysa),color(C32BLUE)
{
}

void back::show() const
{
	logger("back xs %d ys %d : ",xs,ys);
	shape::show();
}

void back::draw() const
{
	if (!vis)
		return;
	cliprect32(B32,x,y,xs,ys,color);
	shape::draw(); // children
//	vis=0; // draw back just once..
}

/////////// text derived class
text::text(script& sc) : back(sc)
{
	texts=sc.read();
	color = C32CYAN;
}

text::text(const C8* namea,S32 xa,S32 ya,S32 xsa,S32 ysa,const C8* thetexta) : back(namea,xa,ya,xsa,ysa),texts(thetexta)
{
	color = C32CYAN;
}

void text::show() const
{
	logger("text '%s' : ",texts.c_str());
	back::show();
}

void text::draw() const
{
	if (!vis)
		return;
	cliprect32(B32,x,y,xs,ys,color);
	outtextxyc32(B32,x+(xs>>1),y+(ys>>1),C32BLACK,texts.c_str());
	shape::draw(); // children
}

/////////// edit derived class
void edit::show() const
{
	logger("edit : ");
	text::show();
}

S32 edit::pos2mx() const
{
	S32 len=texts.length();
	return x+(xs>>1)-(len<<2)+(pos<<3);
}

void edit::mx2pos(S32 mx)
{
	S32 len=texts.length();
	pos=((mx<<1)-(x<<1)-xs+(len<<3)+8)>>4;
	checkpos();
}

void edit::checkpos()
{
	S32 len=texts.length();
	pos=range(0,pos,len);
}

void edit::draw() const
{
	if (!vis)
		return;
	C32 c=active ? C32YELLOW : C32WHITE;
	cliprect32(B32,x,y,xs,ys,c);
	if (blink>=8) {
		S32 mx=pos2mx();
		clipline32(B32,mx,y+2,mx,y+ys-1-2,C32BLACK);
	}
	outtextxyc32(B32,x+(xs>>1),y+(ys>>1),C32BLACK,texts.c_str());
	if (!active)
		blink=7;
	active=0;
	shape::draw(); // children
}

void edit::settname(const C8* tnamea)
{ 
	//logger("edit::settname to '%s'\n",tnamea);
	texts=tnamea;
	pos=texts.length();
}

S32 edit::proc()
{
	S32 ret=-1;
	if (isinside() && wininfo.mleftclicks) {
		mx2pos(MX);
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
			} else if (KEY==K_TAB || KEY==K_RETURN)
				ret=KEY;
		}
//	}
	if (blink>16)
		blink=0;
	return ret;
}
/*
void edit::deactivate()
{
	active=0;
	blink=0;
}
*/
// pbut derived class
pbut::pbut(script& sc) : text(sc),down(0)
{
	color = C32RED;
}
pbut::pbut(const C8* namea,S32 xa,S32 ya,S32 xsa,S32 ysa,const C8* thetexta) : text(namea,xa,ya,xsa,ysa,thetexta),down(0)
{
	color = C32RED;
}
void pbut::show() const
	{
		logger("pbut : ");
		text::show();
	}

void pbut::draw() const
{
	if (!vis)
		return;
	C32 c=color;
	if (down) {
		c.r+=50;
		c.g+=50;
		c.b+=50;
	}
	cliprect32(B32,x,y,xs,ys,c);
	outtextxyc32(B32,x+(xs>>1),y+(ys>>1),C32WHITE,texts.c_str());
	shape::draw(); // children
}

/*S32 pbut::proc()
{
	if (!isinside()) {
		down=0;
		return 0;
	} else if (MBUT&1) {
		down=1;
	} else
		down=0;
	return !(MBUT&1) && (LMBUT&1);
}
*/
S32 pbut::proc()
{
	if (!isinside()) {
		down=0;
		return 0;
	} else if (MBUT) {
		down=1;
	} else
		down=0;
	return ~MBUT & LMBUT;
}

/////////// vscroll derived class
vscroll::vscroll(script& sc) : back(sc),idx(0),numidx(1),offset(0),cnt(0),pagedir(0),indrag(0)
{
	setminmaxval(0,9);
}

vscroll::vscroll(const C8* namea,S32 xa,S32 ya,S32 xsa,S32 ysa) : back(namea,xa,ya,xsa,ysa),idx(0),numidx(1),offset(0),cnt(0),pagedir(0),indrag(0)
{
	setminmaxval(0,9);
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
	cliprect32(B32,x,y,xs,ys,C32(200,40,120));
// top
	cliprect32(B32,x,y,xs,xs,C32(150,240,120));
	cliprecto32(B32,x,y,xs,xs,C32BLACK);
	clipline32(B32,x+xs/2,y+xs/6,x+  xs/6,y+5*xs/6,C32BLACK);
	clipline32(B32,x+xs/2,y+xs/6,x+5*xs/6,y+5*xs/6,C32BLACK);
// middle
	cliprect32(B32,x,y+soff,xs,xs,C32(255,240,220));
	cliprecto32(B32,x,y+soff,xs,xs,C32BLACK);
//	outtextxyf32(B32,x,y+soff,C32BLACK,"%d",idx);
//	outtextxyf32(B32,x,y+soff+8,C32BLACK,"%d",numidx);
	clipcircleo32(B32,x+xs/2,y+xs/2+soff,xs/3,C32BLACK);
// bottom
	cliprect32(B32,x,y+ys-xs,xs,xs,C32(150,240,120)); 
	cliprecto32(B32,x,y+ys-xs,xs,xs,C32BLACK); 
	clipline32(B32,x+xs/2,y+ys-xs/6,x+  xs/6,y+ys-5*xs/6,C32BLACK);
	clipline32(B32,x+xs/2,y+ys-xs/6,x+5*xs/6,y+ys-5*xs/6,C32BLACK);
	shape::draw(); // children
}

S32 vscroll::proc()
{
	if (numidx<=1)
		return idx+offset;
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
			if (MY>=y && MY<y+xs) { // up
				--idx;
			} else if (MY>=y+ys-xs && MY<y+ys) { // down
				++idx;
			} else if (MY>=y+xs && MY<y+soff) { // page up
				if (pagedir!=1) {
					idx-=5;
					pagedir=-1;
				}
			} else if (MY>=y+soff+xs && MY<y+ys-xs) { // page down
				if (pagedir!=-1) {
					idx+=5;
					pagedir=1;
				}
			} else if (MY>=y+soff && MY<y+soff+xs) { // drag
//				if (!(LMBUT&1))
					indrag=1;
			}
			setidxi(idx);
		}
		if (indrag) {
			S32 mlo=y+xs+(xs>>1);
			S32 mhi=y+ys-xs-(xs>>1);
			idx=((MY-mlo)*(numidx-1)+((mhi-mlo)>>1))/(mhi-mlo);
			setidxi(idx);
		}
		++cnt;
		return idx+offset;
	} 
	return idx+offset;
}

void vscroll::setidxi(S32 idxa)
{
	idx=range(0,idxa,numidx-1);
	if (parent) {
		listbox* lp=dynamic_cast<listbox*>(parent);
		if (lp) {
			lp->curoffset=idx;
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
// whole thing
	cliprect32(B32,x,y,xs,ys,C32(200,40,120));
// top
	cliprect32(B32,x,y,ys,ys,C32(150,240,120));
	cliprecto32(B32,x,y,ys,ys,C32BLACK);
	clipline32(B32,x+ys/6,y+ys/2,x+5*ys/6,y+  ys/6,C32BLACK);
	clipline32(B32,x+ys/6,y+ys/2,x+5*ys/6,y+5*ys/6,C32BLACK);
// middle
	cliprect32(B32,x+soff,y,ys,ys,C32(255,240,220));
	cliprecto32(B32,x+soff,y,ys,ys,C32BLACK);
//	outtextxyf32(B32,x,y+soff,C32BLACK,"%d",idx);
//	outtextxyf32(B32,x,y+soff+8,C32BLACK,"%d",numidx);
	clipcircleo32(B32,x+ys/2+soff,y+ys/2,ys/3,C32BLACK);
// bottom
	cliprect32(B32,x+xs-ys,y,ys,ys,C32(150,240,120)); 
	cliprecto32(B32,x+xs-ys,y,ys,ys,C32BLACK); 
	clipline32(B32,x+xs-ys/6,y+ys/2,x+xs-5*ys/6,y+  ys/6,C32BLACK);
	clipline32(B32,x+xs-ys/6,y+ys/2,x+xs-5*ys/6,y+5*ys/6,C32BLACK);
	shape::draw(); // children
}

S32 hscroll::proc()
{
	if (numidx<=1)
		return idx+offset;
	if (DMZ) {
		idx-=5*DMZ;
		setidxi(idx);
		return idx+offset;
	}
	switch(KEY) {
		case K_RIGHT:
		case K_NUMRIGHT:
			++idx;
			setidxi(idx);
			return idx+offset;
			break;
		case K_PAGEDOWN:
			idx+=5;
			setidxi(idx);
			return idx+offset;
			break;
		case K_LEFT:
		case K_NUMLEFT:
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
			S32 vdmax=xs-3*ys;
			S32 soff=vdmax*idx/(numidx-1)+ys;
			if (MX>=x && MX<x+ys) { // up
				--idx;
			} else if (MX>=x+xs-ys && MX<x+xs) { // down
				++idx;
			} else if (MX>=x+ys && MX<x+soff) { // page up
				if (pagedir!=1) {
					idx-=5;
					pagedir=-1;
				}
			} else if (MX>=x+soff+ys && MX<x+xs-ys) { // page down
				if (pagedir!=-1) {
					idx+=5;
					pagedir=1;
				}
			} else if (MX>=x+soff && MX<x+soff+ys) { // drag
//				if (!(LMBUT&1))
					indrag=1;
			}
			setidxi(idx);
		}
		if (indrag) {
			S32 mlo=x+ys+(ys>>1);
			S32 mhi=x+xs-ys-(ys>>1);
			idx=((MX-mlo)*(numidx-1)+((mhi-mlo)>>1))/(mhi-mlo);
			setidxi(idx);
		}
		++cnt;
		return idx+offset;
	} 
	return idx+offset;
}

/////////// listbox derived class
listbox::~listbox()
{
//	logger("deleting listbox\n");
}

void listbox::show() const
{
	logger("listbox : ");
	back::show();
}

void listbox::draw() const
{
	if (!vis)
		return;
//	recti2 cliprectsave=B32->cliprect;
	setcliprect32(B32,x,y,xs,ys);
	cliprect32(B32,x,y,xs,ys,C32(10,40,120));
	S32 i,n=thelist.size(),nv=(ys>>3);
	for (i=0;i<nv;++i) {
		S32 j=i+curoffset;
		if (j<0)
			continue;
		if (j>=n)
			continue;
		if (j==idx)
			cliprect32(B32,x,y+(i<<3),xs,8,C32GREEN);
		outtextxy32(B32,x,y+(i<<3),C32YELLOW,thelist[j].c_str());
	}
	resetcliprect32(B32);
//	B32->cliprect=cliprectsave;
	shape::draw(); // children
}

S32 listbox::proc()
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
		i=((MY-y)>>3)+curoffset;
		if (i<(S32)thelist.size()) {
			setidxv(i);
			return idx;
		}
	}
	return -1;
}

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

void listbox::sort(enum listsortkind lsk)
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
	numoffsets=n-(ys>>3)+1;
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
	numoffsets=n-(ys>>3)+1;
	if (numoffsets<1)
		numoffsets=1;
	setscroll();
}

void listbox::printf(const C8* fmt,...) // add at the end, idx untouched, just like addstringstay but with formmated text
{
	static C8 str[2048000];
	va_list arglist;
	va_start(arglist,fmt);
	vsprintf(str,fmt,arglist);
	addstringstay(str);
	va_end(arglist);
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
	S32 nvislines=ys>>3;
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
	S32 nvislines=ys>>3;
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
	numoffsets=n-(ys>>3)+1;
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
//	if (idx < 0)
//		return "neg idx";
	return thelist[idx].c_str();
}

const C8* listbox::getidxname(S32 ind) const
{ 
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
	funmaph["edit"]=(FPH)edit::createh; 
/*
	funmaph["rbut"]=(FPH)rbut::createh;
	funmaph["cbut"]=(FPH)cbut::createh;
	funmaph["hslider"]=(FPH)hslider::createh;
	funmaph["vslider"]=(FPH)vslider::createh;
	funmaph["hscroll"]=(FPH)hscroll::createh; 
*/
}

using namespace res2d;
shape* res2d::res_loadfile(const C8* str)
{
	factory2<shape> fact;
	script sc=script(str);
	shape* rls=fact.newclass_from_handle(sc);
	if (!rls)
		errorexit("can't find shaperoot for '%s'",str);
	return rls;
}

