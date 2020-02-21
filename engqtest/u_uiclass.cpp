#include <m_eng.h>
#include "u_factory.h"
#include "u_uipopup_info.h"
#include "u_uiclass.h"
#include "u_uiclass2.h"
template <> // compiler sometimes complains ??!! (msvisualstudio)
factory<uitree,script>::factory();

class uitree* uitree::focus; 	/// should be on top, last place clicked, proc set if wanted
bool uitree::usedinput; 		/// input has been used by some node
pointf2 uitree::fmxy; 			/// float mouse xy
pointf2 uitree::oldfmxy;
//S32 uitree::movecount;
S32 uitree::num_popup;
class uitree* uitree::dest; 	/// where to move/copy tree 'to'
class uitree* uitree::tview; 	/// what treeview is looking at
S32 uitree::indentamount; 		/// for save pretty formatting

/// class uitree
/// protected functions
/*void uitree::logrec()
{
	S32 nc=children.size();
	S32 i;
	logger_indent();
	for (i=0;i<nc;++i)
		children[i]->log();
	logger_unindent();
}
*/
void uitree::procrec()
{
	S32 nc=children.size();
	S32 i;
	for (i=0;i<nc;++i) {
		children[i]->proc();
		nc=children.size();
	}
}

string getstringnum(string s,S32& num)
{
	S32 retn=0;
	S32 sz=s.size();
	S32 cut=0;
	while(cut<sz) {
		if (s[cut]>='0' && s[cut]<='9')
			break;
		++cut;
	}
	retn=atoi(s.c_str()+cut);
	num=retn;
	return s.substr(0,cut);
}

/// look for sssnum where next is sssleastmore or sssleastless (what?)
uitree* uitree::focusnext()
{
	S32 bn;
	string bs=getstringnum(name,bn);
	uitree* pmini=0,*pminig=0;
	S32 mini=0,minig=0;
	if (!parent)
		return 0;
//	if (name.size()<3)
//		return 0;
//	const C8* ncstr=name.c_str();
//	if (ncstr[0]!='_')
//		return 0;
//	if (ncstr[1]!='_')
//		return 0;
//	S32 sv=atoi(ncstr+2);
	S32 nc=parent->children.size();
	S32 i;
	for (i=0;i<nc;++i) {
		uitree* c=parent->children[i];
		S32 cn;
		string cs=getstringnum(c->getname(),cn);
//		if (c->name.size()<3)
//			continue;
//		const C8* ncstr2=c->name.c_str();
//		if (ncstr2[0]!='_')
//			continue;
//		if (ncstr2[1]!='_')
//			continue;
//		S32 dv=atoi(ncstr2+2);
		if (bs!=cs)
			continue;
		if (cn<bn && (!pmini || cn<mini)) { /// smallest less than (wrap)
			pmini=c;
			mini=cn;
		}
		if (cn>bn && (!pminig || cn<minig)) { /// smallest greater than
			pminig=c;
			minig=cn;
		}
	}
	if (pminig)
		return pminig;
	return pmini;
}

/// look for sssnum where next is sssmostless or sssmostmore
uitree* uitree::focusprev()
{
	S32 bn;
	string bs=getstringnum(name,bn);
	uitree* pmaxi=0,*pmaxiless=0;
	S32 maxi=0,maxiless=0;
	if (!parent)
		return 0;
//	if (name.size()<3)
//		return 0;
//	const C8* ncstr=name.c_str();
//	if (ncstr[0]!='_')
//		return 0;
//	if (ncstr[1]!='_')
//		return 0;
//	S32 sv=atoi(ncstr+2);
	S32 nc=parent->children.size();
	S32 i;
	for (i=0;i<nc;++i) {
		uitree* c=parent->children[i];
		S32 cn;
		string cs=getstringnum(c->getname(),cn);
//		if (c->name.size()<3)
//			continue;
//		const C8* ncstr2=c->name.c_str();
//		if (ncstr2[0]!='_')
//			continue;
//		if (ncstr2[1]!='_')
//			continue;
//		S32 dv=atoi(ncstr2+2);
		if (bs!=cs)
			continue;
		if (cn>bn && (!pmaxi || cn>maxi)) { /// greatest greater than (wrap)
			pmaxi=c;
			maxi=cn;
		}
		if (cn<bn && (!pmaxiless || cn>maxiless)) { /// greatest less than
			pmaxiless=c;
			maxiless=cn;
		}
	}
	if (pmaxiless)
		return pmaxiless;
	return pmaxi;
}

void uitree::saverec(FILE* fh)
{
	S32 nc=children.size();
	if (!nc) {
		fprintf(fh,"\n");
		return;
	}
	S32 i;
	fprintf(fh," {\n");
	++indentamount;
	for (i=0;i<nc;++i)
		children[i]->save(fh);
	--indentamount;
	printindent(fh);
	fprintf(fh,"}\n");
}

void uitree::printindent(FILE* fh)
{
	S32 i;
	for (i=0;i<indentamount*4;++i)
		fprintf(fh," ");
}

 /// static functions
 void uitree::setinput()
{
	oldfmxy=fmxy;
	usedinput=false;
	fmxy.x=(float)MX;
	fmxy.y=(float)MY;
/*	if (num_popup<=0 && oldfmxy.x==fmxy.x && oldfmxy.y==fmxy.y)
		++movecount;
	else
		movecount=0;
	if (movecount>MOUSEOVERCOUNT)
		movecount=MOUSEOVERCOUNT;
*/
}

/// non virtual functions
bool uitree::killdeaddudes()
{
	if (killc) {
		delete this;
		return true;
	}
	S32 nc=children.size();
	S32 i;
	for (i=0;i<nc;++i) {
		bool ret=children[i]->killdeaddudes();
		if (ret) {
			--i;
			nc=children.size();
		}
	}
	return false;
}

bool uitree::focustofront()
{
	S32 i,nc=children.size();
	bool ena=focustofrontenabled();
	for (i=0;i<nc;++i) {
		uitree* p=children[i];
//		if (!p->focustofrontenabled())
//			return false;
		bool ret=p->focustofront();
		if (p==focus || ret) {
			if (i && ena) {
				children.erase(children.begin()+i);
				children.insert(children.begin(),p);
			}
			return true;
		}
	}
	return false;
}

void uitree::linkchild(uitree* ch)
{
	if (!ch)
		errorexit("bad linkchild");
	if (ch->parent)
		errorexit("linkchild: child already has a parent");
	ch->parent = this;
	children.push_back(ch);
}

void uitree::linkchildf(uitree* ch)
{
	if (!ch)
		errorexit("bad linkchild");
	if (ch->parent)
		errorexit("linkchild: child already has a parent");
	ch->parent = this;
//	children.push_back(ch);
	vector<uitree*>::iterator front = children.begin();
	children.insert(front,ch);
}

void uitree::copychildrenrec(uitree* n)
{
	S32 i,nc=children.size();
	for (i=0;i<nc;++i) {
//		logger("copychild '%s'\n",script2print(children[i]->getname()).c_str());
		n->linkchild(children[i]->copy());
	}
}

void uitree::loadscript2fn(string fn)
{
	logger("load script '%s' into '%s'\n",fn.c_str(),script2printc(name).c_str());
	script ns=script(fn.c_str());
	factory<uitree,script> fs; // these 2 lines, this is where it all comes together
	uitree* bm=fs.newclass_from_handle(ns);
	if (bm) {
		linkchild(bm);
		focus=bm;
	}
}

void uitree::savescript2fn(string fn)
{
	logger("save script '%s' into '%s'\n",fn.c_str(),script2printc(name).c_str());
	FILE* fw=fopen2(fn.c_str(),"w");
	save(fw);
	fclose(fw);
}

/// virtual functions
string uitree::parentgetstring() const
{
	if (!parent)
		return "";
	return parent->getname();
}

/*void uitree::parentsetstring(string s)
{
	if (!parent)
		return;
	parent->setname(s);
}
*/
uitree::~uitree()
{
	S32 nc;
	S32 i;
	if (focus==this) {
//		logger("~uitree: '%s' '%s' %p setting focus to parent\n",getclassname(),script2print(name).c_str(),this);
		focus=parent;
	}
	if (dest==this)
		dest=0;
	if (tview==this)
		tview=0;
	if (parent) {
//		logger("got a parent\n");
		nc=parent->children.size();
		for (i=0;i<nc;++i)
			if (parent->children[i]==this)
				break;
		if (i==nc)
			errorexit("child doesn't belong to parent \n");
		vector<uitree*>::iterator ti=parent->children.begin()+i;
		parent->children.erase(ti);
	}
	nc=children.size();
	for (i=0;i<nc;++i) {
		children[i]->parent=0;
		delete children[i];
	}
}

void uitree::buildo2w()
{
	S32 nc=children.size();
	S32 i;
	for (i=0;i<nc;++i)
		children[i]->buildo2w();
}

void uitree::proc()
{
	if (!doproc)
		return;
	procrec();
}

void uitree::draw()
{
	if (!dodraw)
		return;
	S32 nc=children.size();
	S32 i;
	for (i=nc-1;i>=0;--i)
		children[i]->draw();
}

bool uitree::mouseover()
{
	if (MBUT&M_LBUTTON)
		return true;
	S32 nc=children.size();
	S32 i;
	for (i=0;i<nc;++i) {
		bool ret=children[i]->mouseover();
		if (ret)
			return true;
	}
	return false;
}

void uitree::save(FILE* fh,bool userec)
{
//	if (userec) {
	printindent(fh);
	fprintf(fh,"%s",getclassname());
//	}
//	fprintf(fh," \"%s\" \"%s\" ",script2print(name).c_str(),script2print(name2).c_str());
	fprintf(fh," \"%s\" ",script2printc(name).c_str());
	if (userec)
		saverec(fh);
}

/// class uitree2d
/// virtual functions
void uitree2d::buildo2w()
{
	static vector<pointf2> matstack;
	if (matstack.empty()) {
		o2w=o2p;
	} else {
		const pointf2& tp=matstack.back();
		o2w.x=o2p.x+tp.x;
		o2w.y=o2p.y+tp.y;
	}
	matstack.push_back(o2w);
	uitree::buildo2w();
	matstack.pop_back();
}

void uitree2d::save(FILE* fh,bool userec)
{
//	if (userec) {
//		printindent(fh);
//		fprintf(fh,"uitree2d");
//	}
	uitree::save(fh,false);
	fprintf(fh," %f %f",o2p.x,o2p.y);
	if (userec)
		saverec(fh);
}

/// new non virtual functions for 2d on up
bool uitree2d::clickfocus()
{
	if (usedinput)
		return false;
	if ((MBUT&M_LBUTTON) && !(LMBUT&M_LBUTTON)) {
		if (isinside(fmxy)) {
			setfocus(this);
//			logger("click focus to %p\n",getfocus());
			usedinput=true;
			return true;
		} else {
			setfocus(0);
//			logger("click focus to zero\n");
			return false;
		}
	} else
		return false;
}

/// class uitree2drect
/// non virtual functions
/// copy into class children from a .uis file, also adjust pos and size
void uitree2drect::scriptbuildcopy(const C8* cnscripta)
{
	script ns=script(cnscripta);
	factory<uitree,script> fs; // these 2 lines, this is where it all comes together
	uitree* bm=fs.newclass_from_handle(ns);
	uitree2drect* bmr=dynamic_cast<uitree2drect*>(bm);
	if (!bmr)
		errorexit("can't find uitree2drect for changename in '%s'",cnscripta);
	bmr->copychildrenrec(this);
	seto2p(bmr->geto2p());
	setsize(bmr->getsize());
	setoffset(bmr->getoffset());
	bmr->dokillc();
	bmr->killdeaddudes();
}

void uitree2drect::addrem_controls()
{
	S32 ne=0;
	S32 nc=children.size();
	S32 i;
	for (i=0;i<nc;++i) {
		S32 idx=children[i]->getname().find("__edit");
		if (idx==0) {
			children[i]->dokillc();
			++ne;
		}
	}
	if (ne>0)
		return;
	float b=BORDER;
	if (size.x<BORDER*2 || size.y<BORDER*2)
		b=min(size.x,size.y)/2;
	if (parent) {
		linkchildf(new deleter("__edit_deleter",0,0,b,b,"X"));
		linkchildf(new cloner("__edit_cloner",0,0,b,b,"C"));
	}
	linkchildf(new mover("__edit_mover",0,0,b,b));
	linkchildf(new sizer("__edit_sizer",0,0,b,b));
}

void uitree2drect::addrem_scroll()
{
	S32 ne=0;
	S32 nc=children.size();
	S32 i;
	for (i=0;i<nc;++i) {
		S32 idx=children[i]->getname().find("__scroll");
		if (idx==0) {
			children[i]->dokillc();
			++ne;
		}
	}
	if (ne>0)
		return;
	float b=BORDER;
	linkchildf(new xscroll("__scroll_x",0,0,0,b));
	linkchildf(new yscroll("__scroll_y",0,0,b,0));
}

/// virtual functions
void uitree2drect::showoutline(S32 grow,C32 color)
{
	cliprecto32(B32,(S32)o2w.x-grow,(S32)o2w.y-grow,(S32)size.x+2*grow,(S32)size.y+2*grow,color);
}

void uitree2drect::add_popup_options(class popup* p) const
{
//	logger("uitree2drect::add popup options\n");
	p->setup_popup(rect_new_class);
}

void uitree2drect::proc()
{
	if (!doproc)
		return;
	uitree2d::proc();
	clickfocus();
}

void uitree2drect::draw()
{
	if (!dodraw)
		return;
	cliprect32(B32,(S32)o2w.x,(S32)o2w.y,(S32)size.x,(S32)size.y,C32WHITE);
	cliprecto32(B32,(S32)o2w.x,(S32)o2w.y,(S32)size.x,(S32)size.y,C32BLACK);
//	outtextxyc32(B32,(S32)(o2w.x),(S32)(o2w.y),C32BLACK,"rect");
	uitree2d::draw();
}

bool uitree2drect::mouseover()
{
	static S32 movecount;		/// for mouseover
	const S32 MOUSEOVERCOUNT=30;
	if (!doproc)
		return false;
	if (uitree::mouseover()) /// children first
		return true;
	if (isinside(fmxy)) {
		if ((MBUT&M_RBUTTON)&&!(LMBUT&M_RBUTTON)) {
			uitree2drect* pop=new popup("pop",fmxy.x-o2w.x,fmxy.y-o2w.y,0,0);
			linkchildf(pop);
		}
		if (num_popup) {
//			if (!parent)
//				return true;
			if (!dynamic_cast<popup*>(parent))
				return true;
		}
		cliprecto32(B32,(S32)o2w.x,(S32)o2w.y,(S32)size.x,(S32)size.y,C32YELLOW);
		cliprecto32(B32,(S32)o2w.x+1,(S32)o2w.y+1,(S32)size.x-2,(S32)size.y-2,C32MAGENTA);
		if (num_popup<=0 && oldfmxy.x==fmxy.x && oldfmxy.y==fmxy.y)
			++movecount;
		else
			movecount=0;
		if (movecount>MOUSEOVERCOUNT)
			movecount=MOUSEOVERCOUNT;
		if (movecount<MOUSEOVERCOUNT)
			return true;
		outtextxybf32(B32,(S32)fmxy.x+16,(S32)fmxy.y+32,C32WHITE,C32BLACK,"%s:%s",getclassname(),name.c_str());
		return true;
	}
	return false;
}

void uitree2drect::save(FILE* fh,bool userec)
{
//	if (userec) {
//		printindent(fh);
//		fprintf(fh,"uitree2drect");
//	}
	uitree2d::save(fh,false);
	fprintf(fh," %f %f",size.x,size.y);
	if (userec)
		saverec(fh);
}

//bool uitree2drect::isinside(const pointf2& p,pointf2* w2o) const
bool uitree2drect::isinside(const pointf2& p) const
{
	pointf2 localpnt;
	localpnt.x=p.x-o2w.x;
	localpnt.y=p.y-o2w.y;
	if (localpnt.x<0 || localpnt.y<0 || localpnt.x>=size.x || localpnt.y>=size.y)
		return false;
	/*if (w2o)
		*w2o=localpnt;*/
	return true;
}

void uitree2drect::keyoffset(pointf2 step)
{
	switch(KEY) {
	case K_RIGHT:
	case K_NUMRIGHT:
		if (step.x>0)
			step.x=1.0f/step.x;
		else {
			offset.x=0;
			step.x=0;
		}
		offset.x+=step.x;
		if (offset.x>1.0f)
			offset.x=1.0f;
		KEY=0;
		break;
	case K_LEFT:
	case K_NUMLEFT:
		if (step.x>0)
			step.x=1.0f/step.x;
		else {
			offset.x=0;
			step.x=0;
		}
		offset.x-=step.x;
		if (offset.x<0)
			offset.x=0;
		KEY=0;
		break;
	case K_DOWN:
	case K_NUMDOWN:
		if (step.y>0)
			step.y=1.0f/step.y;
		else {
			offset.y=0;
			step.y=0;
		}
		offset.y+=step.y;
		if (offset.y>1.0f)
			offset.y=1.0f;
		KEY=0;
		break;
	case K_UP:
	case K_NUMUP:
		if (step.y>0)
			step.y=1.0f/step.y;
		else {
			offset.y=0;
			step.y=0;
		} offset.y-=step.y;
		if (offset.y<0)
			offset.y=0;
		KEY=0;
		break;
	}
}

/// class uitree2dbitmap
/// popup edit functions (optional)
/// non virtual functions
uitree2dbitmap::uitree2dbitmap(const C8* na,float xa,float ya,float xsa,float ysa,float offsetxa,float offsetya,const C8* dira,const C8* fna) :
  uitree2drect(na,xa,ya,xsa,ysa),bdata(0)
{
	offset.x=offsetxa;
	offset.y=offsetya;
	pushandsetdir(dira);
	loaddata2fn(fna);
	popdir();
	if (!bdata) {
		S32 xsi=(S32)xsa;
		S32 ysi=(S32)ysa;
		if (xsi>0 && ysi>0)
			bdata=bitmap32alloc(xsi,ysi,C32GREEN);
	}
}

uitree2dbitmap::uitree2dbitmap(script& sc) : uitree2drect(sc),bdata(0)
{
	offset.x=(float)atof(sc.read());
	offset.y=(float)atof(sc.read());
	string d=sc.read();
	pushandsetdir(d.c_str());
	loaddata2fn(sc.read());
	popdir();
	if (!bdata) {
		S32 xsi=(S32)size.x;
		S32 ysi=(S32)size.y;
		if (xsi>0 && ysi>0)
			bdata=bitmap32alloc(xsi,ysi,C32GREEN);
	}
}

/// virtual functions
void uitree2dbitmap::add_popup_options(class popup* p) const
{
//	logger("uitree2dbitmap::add popup options\n");
//	return ret;
	uitree2drect::add_popup_options(p);
	p->setup_popup(bitmap_change_color1);
}

void uitree2dbitmap::proc()
{
	if (!doproc)
		return;
	uitree2drect::proc();
	if (getfocus()==this) {
		if (bdata){
			pointf2 step;
			step.x=(bdata->size.x-(size.x-BORDER))/5;	/// step by 5 pixels
			step.y=(bdata->size.y-(size.y-BORDER))/5;
			keyoffset(step);
		}
	}
}

void uitree2dbitmap::draw()
{
	if (!dodraw)
		return;
	if (bdata) {
		pointf2 src;
		pointf2 sh;
		sh.x=bdata->size.x-size.x+BORDER; /// width of scroll
		sh.y=bdata->size.y-size.y+BORDER;
		if (sh.x<0)
			sh.x=0;
		if (sh.y<0)
			sh.y=0;
		src.x=offset.x*sh.x;
		src.y=offset.y*sh.y;
		clipblit32(bdata,B32,(S32)src.x,(S32)src.y,(S32)o2w.x,(S32)o2w.y,(S32)size.x,(S32)size.y);
	}
//	cliprecto32(B32,(S32)o2w.x,(S32)o2w.y,(S32)size.x,(S32)size.y,C32BLACK);
//	outtextxyfc32(B32,(S32)(o2w.x+size.x*.5f),(S32)(o2w.y+size.y*.5f),C32BLACK,"off %6.3f %6.3f",offset.x,offset.y);
	uitree2d::draw();
}

void uitree2dbitmap::save(FILE* fh,bool userec)
{
//	if (userec) {
//		printindent(fh);
//		fprintf(fh,"uitree2dbitmap");
//	}
	uitree2drect::save(fh,false);
	fprintf(fh," %f %f \"%s\" \"%s\"",offset.x,offset.y,absdir2reldir(dirname).c_str(),fname.c_str());
	if (userec)
		saverec(fh);
}

/// new virtual functions
void uitree2dbitmap::loaddata2fn(string fn)
{
//	setname2(fn);
//	logger("in loaddata2fn bitmap with '%s'\n",fn.c_str());
	dirname=getcurdir();
	fname=fn;
	if (fn.empty())
		return;
	pushandsetdir(dirname.c_str());
	if (!fileexist(fn.c_str())) {
		popdir();
		return;
	}
	if (bdata)
		bitmap32free(bdata);
	bdata=gfxread32(fn.c_str());
	popdir();
	if (size.x<=0 || size.y<=0) {
		size.x=(float)(bdata->size.x);
		size.y=(float)(bdata->size.y);
	}
//	offset.x=offset.y=0;
}

void uitree2dbitmap::savedata2fn(string fn)
{
//	logger("saving '%s'\n",fn.c_str());
//	setname2(fn);
	dirname=getcurdir();
	fname=fn;
	pushandsetdir(dirname.c_str());
	gfxwrite32(fname.c_str(),bdata);
	popdir();
}

/// 2d bitmap with file name, stretch blit
/// non virtual functions
uitree2dbitmapscale::uitree2dbitmapscale(const C8* na,float xa,float ya,float sizexa,float sizeya,float offsetxa,float offsetya,const C8* dira,const C8* fna) :
	uitree2dbitmap(na,xa,ya,sizexa,sizeya,offsetxa,offsetya,dira,fna),bout(0),update(true)
{
//	if (bdata) {
//		S32 xsi=(S32)size.x;
//		S32 ysi=(S32)size.y;
//		if (xsi>0 && ysi>0)
//			bout=bitmap32alloc(xsi,ysi,C32BLACK);
//		clipscaleblit32(bdata,bout);
//	}
}

uitree2dbitmapscale::uitree2dbitmapscale(script& sc) : uitree2dbitmap(sc),bout(0),update(true)
{
/*	if (bdata) {
		S32 xsi=(S32)size.x;
		S32 ysi=(S32)size.y;
		if (xsi>0 && ysi>0) {
			bout=bitmap32alloc(xsi,ysi,C32BLACK);
			clipscaleblit32(bdata,bout);
		}
	} */
}

/// virtual functions
void uitree2dbitmapscale::draw()
{
	if (!dodraw)
		return;
	if (update) {
		if (bout) {
			bitmap32free(bout);
			bout=0;
		}
		if (bdata) {
			S32 xsi=(S32)size.x;
			S32 ysi=(S32)size.y;
			if (xsi>0 && ysi>0) {
				bout=bitmap32alloc(xsi,ysi,C32BLACK);
				clipscaleblit32(bdata,bout);
			}
		}
		update=false;
	}
	if (bout)
		clipblit32(bout,B32,0,0,(S32)o2w.x,(S32)o2w.y,(S32)size.x,(S32)size.y);
	uitree2d::draw();
}

/*void uitree2dbitmapscale::save(FILE* fh,bool userec)
{
//	if (userec) {
//		printindent(fh);
//		fprintf(fh,"uitree2dbitmapscale");
//	}
	uitree2dbitmap::save(fh,false);
//	fprintf(fh," \"%s\"",name2.c_str());
	if (userec)
		saverec(fh);
}
*/
void uitree2dbitmapscale::setsize(const pointf2& sizea)
{
	pointf2 oldsize=getsize();
	if (oldsize.x!=sizea.x || oldsize.y!=sizea.y) {
		uitree2drect::setsize(sizea);
		update=true;
	}
}

/// class texter
/// static functions
/// virtual functions
void texter::draw()
{
	if (!dodraw)
		return;
	cliprect32(B32,(S32)o2w.x,(S32)o2w.y,(S32)size.x,(S32)size.y,C32WHITE);
	cliprecto32(B32,(S32)o2w.x,(S32)o2w.y,(S32)size.x,(S32)size.y,C32BLACK);
	outtextxyc32(B32,(S32)(o2w.x+size.x*.5f),(S32)(o2w.y+size.y*.5f),C32BLACK,tname.c_str());
	uitree2d::draw();
}

void texter::save(FILE* fh,bool userec)
{
//	if (userec) {
//		printindent(fh);
//		fprintf(fh,"texter");
//	}
	uitree2drect::save(fh,false);
	fprintf(fh," \"%s\"",script2printc(tname).c_str());
	if (userec)
		saverec(fh);
}

void texter::add_popup_options(class popup* p) const
{
//	logger("texter::add popup options\n");
//	return ret;
	uitree2drect::add_popup_options(p);
	p->setup_popup(texter_changename);
}

/// class texter
/// static functions
void editbox::proc()
{
	if (!doproc)
		return;
	uitree* f;
	texter::proc();
	if (getfocus()==this) {
		if ((MBUT&M_LBUTTON) && !(LMBUT&M_LBUTTON)) {
			S32 px=(S32)(o2w.x+size.x*.5f)-4-tname.size()*4;
			S32 py=(S32)(o2w.y+size.y*.5f)-4-4;
			S32 sx=tname.size()*8+8;
			S32 sy=16;
			S32 mx=(S32)fmxy.x;
			S32 my=(S32)fmxy.y;
			if (mx>=px && my>=py && mx<px+sx && my<py+sy) {
				cursorpos=(mx-px)/8;
				if (cursorpos<0)
					cursorpos=0;
				if (cursorpos>(S32)tname.size())
					cursorpos=tname.size();
				blink=0;
			}
		}
		if (KEY>=' ' && KEY<='~') {
//			if (KEY!='\"') { // for now, don't allow double quote, see scripter.cpp if add \" and \\ then enable double quote
				tname=tname.substr(0,cursorpos)+(C8)KEY+tname.substr(cursorpos,tname.size()-cursorpos);
				++cursorpos;
				blink=0;
//			}
		} else switch(KEY) {
		case K_RIGHT:
		case K_NUMRIGHT:
			if (cursorpos<(S32)tname.size())
				++cursorpos;
			blink=0;
			break;
		case K_LEFT:
		case K_NUMLEFT:
			if (cursorpos>0)
				--cursorpos;
			blink=0;
			break;
		case K_DELETE:
//			logger("del pressed\n");
			if (cursorpos<(S32)tname.size())
				tname=tname.substr(0,cursorpos)+tname.substr(cursorpos+1,tname.size()-cursorpos-1);
			blink=0;
			break;
		case K_BACKSPACE:
//			logger("bs pressed\n");
			if (cursorpos>0) {
				tname=tname.substr(0,cursorpos-1)+tname.substr(cursorpos,tname.size()-cursorpos);
				--cursorpos;
			}
			blink=0;
			break;
		case '\t':
//			logger("tab pressed\n");
			tbpressed=true;
			blink=0;
			if (wininfo.keystate[K_LEFTSHIFT]|wininfo.keystate[K_RIGHTSHIFT])
				f=focusprev();
			else
				f=focusnext();
			if (f)
				setfocus(f);
//			{
//				if (parent && parent->children.size())
//					;
//				focus=0;
//			}
			break;
		case '\r':
//			logger("enter pressed\n");
			crpressed=true;
			blink=0;
			break;
		}
		KEY=0;
		++blink;
		if (blink>=BLINKPER)
			blink=0;
	} else
		blink=0;
}

void editbox::draw()
{
	if (!dodraw)
		return;
	cliprect32(B32,(S32)o2w.x,(S32)o2w.y,(S32)size.x,(S32)size.y,C32(240,220,200));
	cliprecto32(B32,(S32)o2w.x,(S32)o2w.y,(S32)size.x,(S32)size.y,C32BLACK);
//	cliprect32(B32,(S32)(o2w.x),(S32)(o2w.y),10,10,C32LIGHTMAGENTA);
	S32 sx=(S32)(o2w.x+size.x*.5f);
	S32 sy=(S32)(o2w.y+size.y*.5f);
	outtextxyc32(B32,sx,sy,C32BLACK,tname.c_str());
	S32 len=tname.size();
	sx-=len*4;
	cliprecto32(B32,sx-4,sy-4-4,len*8+8,16,C32BLACK);
	if (getfocus()==this && blink<BLINKPER/2) {
		sx+=cursorpos*8;
		clipline32(B32,sx,sy-5-4,sx,sy+5+8-4,C32BLACK);
	}
	uitree2d::draw();
}

/*void editbox::save(FILE* fh,bool userec)
{
//	if (userec) {
//		printindent(fh);
//		fprintf(fh,"editbox");
//	}
	texter::save(fh,false);
	if (userec)
		saverec(fh);
}
*/
/// class button
/// virtual functions
void button::proc()
{
	if (!doproc)
		return;
	uitree2drect::proc();
	if (getfocus()==this) {
		bool isinsiderect=isinside(fmxy);
		if (MBUT&M_LBUTTON) {
			down=isinsiderect;
		} else {
			if (down) {
				if (isinsiderect) {
					waspressed=true;
				}
				down=false;
			}
		}
//		usedinput=true;
	}
}

void button::draw()
{
	if (!dodraw)
		return;
	C32 c1 = down ? C32LIGHTGRAY : C32(200,220,240);
	C32 c2 = C32BLACK;
	cliprect32(B32,(S32)o2w.x,(S32)o2w.y,(S32)size.x,(S32)size.y,c1);
	cliprecto32(B32,(S32)o2w.x+3,(S32)o2w.y+3,(S32)size.x-6,(S32)size.y-6,c2);
	cliprecto32(B32,(S32)o2w.x,(S32)o2w.y,(S32)size.x,(S32)size.y,c2);
	outtextxyfc32(B32,(S32)(o2w.x+size.x*.5f),(S32)(o2w.y+size.y*.5f),c2,
	  "%s",bname.c_str());
	uitree2d::draw();
}

void button::save(FILE* fh,bool userec)
{
//	if (userec) {
//		printindent(fh);
//		fprintf(fh,"button");
//	}
	uitree2drect::save(fh,false);
	fprintf(fh," \"%s\"",script2printc(bname).c_str());
	if (userec)
		saverec(fh);
}

void button::add_popup_options(class popup* p) const
{
//	logger("texter::add popup options\n");
//	return ret;
	uitree2drect::add_popup_options(p);
	p->setup_popup(button_changename);
}

/// class listbox2d
/// popup edit functions (optional)
/// non virtual functions
listbox2d::listbox2d(const C8* na/*,const C8* na2*/,float xa,float ya,float xsa,float ysa) :
  uitree2drect(na/*,na2*/,xa,ya,xsa,ysa),sel(-1),wassel(false),maxx(0)
{
}

listbox2d::listbox2d(script& sc) :
  uitree2drect(sc),sel(-1),wassel(false),maxx(0)
{
}

/// virtual functions
void listbox2d::add_popup_options(class popup* p) const
{
	uitree2drect::add_popup_options(p);
	p->setup_popup(listbox2d_addstring);
}

void listbox2d::addentry(string s)
{
	listdata.push_back(s);
	S32 n=s.size();
	if (n>maxx)
		maxx=n;
}

void listbox2d::proc()
{
	if (!doproc)
		return;
	uitree2drect::proc();
	S32 nc=listdata.size();
	if (getfocus()==this) {
		switch(KEY) {
		case K_UP:
		case K_NUMUP:
			if (getsel()<0)
				setsel(nc-1,true);
			else
				setsel(sel-1,true);
			wassel=true;
			wasselmouse=false;
			break;
		case K_DOWN:
		case K_NUMDOWN:
			if (getsel()>=nc-1)
				setsel(-1,true);
			else
				setsel(sel+1,true);
			wassel=true;
			wasselmouse=false;
			break;
		}
/*		pointf2 step;
		step.y=-(size.y-listdata.size()*10-40-40)/10;	/// step by 10 pixels or 1 line
		step.x=-(size.x-maxx*8-40-40)/8;
//		logger("step %f %f\n",step.x,step.y);
		keyoffset(step); */
		if ((MBUT&M_LBUTTON)&&!(LMBUT&M_LBUTTON)) {
			float sel0=((fmxy.y-o2w.y)-40)/10;
			float sel1=(((fmxy.y-o2w.y-size.y)+10*nc)+40)/10;
			float self;
			S32 selc;
			if (sel1<sel0)
				self=sel0;
			else
				self=sel0+offset.y*(sel1-sel0);
			wassel=true;
			wasselmouse=true;
			selc=(S32)self;
//			logger("self = %f\n",self);
			if (self<0)
				selc=-1;
			if (self>=nc)
				selc=-1;
			setsel(selc,false);
		}
	}
/*	if (sel>=nc) {
		sel=-1;
		offset.y=0;
	}
//		sel=-1;
	if (sel<-1) {
		sel=nc-1;
//		sel=-1;
		offset.y=1;
	} */
}

void listbox2d::draw()
{
	if (!dodraw)
		return;
	cliprect32(B32,(S32)o2w.x,(S32)o2w.y,(S32)size.x,(S32)size.y,C32(100,220,172));
//	outtextxyfc32(B32,(S32)(o2w.x+size.x*.5f),(S32)(o2w.y+size.y*.5f),C32BLACK,"off %6.3f %6.3f",offset.x,offset.y);
	S32 i;
	S32 nc=listdata.size();
	setcliprect32(B32,(S32)o2w.x,(S32)o2w.y,(S32)size.x,(S32)size.y);
	pointf2 off;
	off.x=offset.x*(size.x-maxx*8-40-40);
	off.y=offset.y*(size.y-nc*10-40-40);
	if (off.x>0)
		off.x=0;
	if (off.y>0)
		off.y=0;
	S32 ox=(S32)(o2w.x+off.x+40);
	S32 oy=(S32)(o2w.y+off.y+40);
	for (i=0;i<nc;++i) {
		S32 oy2=oy+10*i;
		if (i==sel) {
			cliprect32(B32,(S32)o2w.x,oy2,(S32)size.x,10,C32BLACK);
			outtextxyf32(B32,ox,oy2,C32WHITE,"%s",listdata[i].c_str());
		} else
			outtextxyf32(B32,ox,oy2,C32BLACK,"%s",listdata[i].c_str());
	}
	resetcliprect32(B32);
	uitree2d::draw();
}

void listbox2d::setsel(S32 sela,bool doscrolly)
{
	if (sela<0) {
		sel=-1;
		return;
	}
	S32 nlist=listdata.size();
	if (nlist==0) {
		sel=-1;
		offset.y=0;
		return;
	}
	sel=range(0,sela,nlist-1);
	if (!doscrolly)
		return;
	S32 adj=(S32)size.y-nlist*10-40-40;
	if (adj>=0) {
		offset.y=0;
		return;
	}
	float oy2=offset.y*adj+40+10*sel;
	bool docalc=false;
	logger("oy2 %f, sizey %f\n",oy2,size.y);
	if (oy2<.25f*size.y) {
		oy2=.25f*size.y;
		docalc=true;
	} else if (oy2>.75f*size.y) {
		oy2=.75f*size.y;
		docalc=true;
	}
	if (docalc) {
		offset.y=(oy2-40-10*sel)/adj;
		offset.y=range(0.0f,offset.y,1.0f);
	}
}

/// this one template ties uitree all together
/// specialize factory template constructor function
/// commented out funmap2's are built by hand, not the factory
#if 1
template <> // compiler sometimes complains ??!! (msvisualstudio)
factory<uitree,script>::factory()
{
/// class.h
	funmap2["uitree"]=uitree::create;
		funmap2["uitree2d"]=uitree2d::create;
			funmap2["uitree2drect"]=uitree2drect::create;
				funmap2["uitree2dbitmap"]=uitree2dbitmap::create;
					funmap2["uitree2dbitmapscale"]=uitree2dbitmapscale::create;
				funmap2["texter"]=texter::create;
						funmap2["editbox"]=editbox::create;
				funmap2["button"]=button::create;
				funmap2["listbox2d"]=listbox2d::create;
/// class2.h
/// from 	funmap2["uitree2drect"]=uitree2drect::create; // class1.h
				funmap2["sizer"]=sizer::create;
				funmap2["mover"]=mover::create;
				funmap2["xscroll"]=xscroll::create;
				funmap2["yscroll"]=yscroll::create;
				funmap2["xslider"]=xslider::create;
					funmap2["yslider"]=yslider::create;
				funmap2["tabs"]=tabs::create;
///				funmap2["popup"]=popup::create;
///				funmap2["changename"]=changename::create;
///					funmap2["changetextname"]=changetextname::create;
///					funmap2["changebuttonname"]=changebuttonname::create;
///					funmap2["listbox2daddentry"]=listbox2daddentry::create;
///				funmap2["loadscriptdialog"]=loadscriptdialog::create;
///					funmap2["savescriptdialog"]=savescriptdialog::create;
///					funmap2["loadbitmapdialog"]=loadbitmapdialog::create;
///					funmap2["savebitmapdialog"]=savebitmapdialog::create;
/// from 		funmap2["button"]=button::create; // class1.h
					funmap2["cloner"]=cloner::create;
					funmap2["deleter"]=deleter::create;
					funmap2["buttest0"]=buttest0::create;
					funmap2["buttest1"]=buttest1::create;
/// from 		funmap2["texter"]=texter::create; // class1.h
					funmap2["checkbox"]=checkbox::create;
						funmap2["radiobutton"]=radiobutton::create;
/// from 		funmap2["listbox2d"]=listbox2d::create; // class1.h
					funmap2["treeview"]=treeview::create;
}
#endif
