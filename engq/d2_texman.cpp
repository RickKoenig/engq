#define D2_3D
#include <m_eng.h>
#include <m_perf.h>

referencecount<textureb> textureb::rc; // and add this

bitmap32 dummyb = {
	{0,0},
	{{0,0},{0,0}},
	0,
};

textureb::textureb(string namea) :
	islocked(0),
	texformat(0),
	hasalpha(false),
	name(namea),
	refidx(-1)
{
	tsize.x=tsize.y=0;
	origsize.x=origsize.y=0;
}

bool textureb::checkname(string instr,string& outstr,bool& iscub,bool& issix)
{
	issix=false;
	if (instr.find("CUB_")==0) {
		instr=instr.substr(4,instr.length()-4);
		iscub=1;
	} else
		iscub=0;
	if (fileexist(instr.c_str())) {
		outstr=instr;
		return true;
	} else {
		S32 i;
		i=instr.rfind('.');
		if (i>=0) {
			string instr2=instr.substr(0,i)+".dds";
			if (fileexist(instr2.c_str())) {
				outstr=instr2;
				return true;
			} else {
				i=instr.rfind('.');
				if (i>=0) {
					string instr2=instr.substr(0,i)+".png";
					if (fileexist(instr2.c_str())) {
						outstr=instr2;
						return true;
					}
				}
			}
		}
	}
	if (pushandsetdirdown(instr.c_str())) {
		if (fileexist("negz.jpg")) {
			issix=true;
			outstr=instr;
			popdir();
			return true;
		}
	}
	popdir();
	return false;
}

//#define PATTERNS1
//#define PATTERNS2

bitmap32* textureb::gfxread32c(const C8* name,bool issix)
{
	if (issix) {
		bitmap32* cm,*b;
		pushandsetdirdown(name);

		b=gfxread32("posz.jpg");
		cm=bitmap32alloc(b->size.x*4,b->size.y*3,C32WHITE);
#ifdef PATTERNS1
		pattern1(b,0);
#endif
#ifdef PATTERNS2
		pattern2(b,0,0);
#endif
		clipblit32(b,cm,0,0,b->size.x,b->size.y,b->size.x,b->size.y);
		bitmap32free(b);

		b=gfxread32("negz.jpg");
#ifdef PATTERNS1
		pattern1(b,0);
#endif
#ifdef PATTERNS2
		pattern2(b,0,0);
#endif
		clipblit32(b,cm,0,0,3*b->size.x,b->size.y,b->size.x,b->size.y);
		bitmap32free(b);

		b=gfxread32("posx.jpg");
#ifdef PATTERNS1
		pattern1(b,1);
#endif
#ifdef PATTERNS2
		pattern2(b,1,0);
#endif
		clipblit32(b,cm,0,0,2*b->size.x,b->size.y,b->size.x,b->size.y);
		bitmap32free(b);

		b=gfxread32("negx.jpg");
#ifdef PATTERNS1
		pattern1(b,1);
#endif
#ifdef PATTERNS2
		pattern2(b,1,0);
#endif
		clipblit32(b,cm,0,0,0,b->size.y,b->size.x,b->size.y);
		bitmap32free(b);

		b=gfxread32("posy.jpg");
#ifdef PATTERNS1
		pattern1(b,1);
#endif
#ifdef PATTERNS2
		pattern2(b,0,1);
#endif
		clipblit32(b,cm,0,0,b->size.x,0,b->size.x,b->size.y);
		bitmap32free(b);

		b=gfxread32("negy.jpg");
#ifdef PATTERNS1
		pattern1(b,1);
#endif
#ifdef PATTERNS2
		pattern2(b,0,1);
#endif
		clipblit32(b,cm,0,0,b->size.x,2*b->size.y,b->size.x,b->size.y);
		bitmap32free(b);

		popdir();
		return cm;
	} else
		return gfxread32(name);
}

// uses colorkeyinfo.lasthascolorkey to determine if alpha
void textureb::build(S32 x,S32 y)
{
	bitmap32* b=bitmap32alloc(x,y,C32BLACK);
	addbitmap(b,0);
	bitmap32free(b);
}

void textureb::animtex()
{
	U32 i;
	for (i=0;i<rc.rlist.size();++i) {
		texturebifl* ta=dynamic_cast<texturebifl*>(rc.rlist[i].rdata);
		if (ta)
			ta->step();
	}
}

// texturebifl class
textureb* ifl_createtexture(const C8* n)
{
	return textureb::rc.newrc<texturebifl>(n);
}

texturebifl::texturebifl(string namea) : textureb(namea),curcnt(0),curtex(0)
{
}

void texturebifl::showline()
{
	logger("[texturebifl] nseq %d",texs.size());
//	logger("[textureb] format %2d, name '%-12s' size %4d,%4d orig size wid hit %d %d,hasalpha %d",texformat,name.c_str(),tsize.x,tsize.y,origsize.x,origsize.y,hasalpha);
	logger_indent();
	U32 i;
	for (i=0;i<texs.size();++i) {
		logger_disableindent();
		texs[i]->showline();
		if (i!=texs.size()-1)
			logger("(%d)\n",cnts[i]);
		else
			logger("(%d) ",cnts[i]);
		logger_enableindent();
	}
	logger_unindent();
}

texturebifl::~texturebifl()
{
	U32 i;
	for (i=0;i<texs.size();++i)
		textureb::rc.deleterc(texs[i]);
}

void texturebifl::load()
{
	script ts=script(name.c_str());
	S32 i;
	for (i=0;i<ts.num();++i) {
		const C8* s=ts.idx(i).c_str();
		U32 v=0;
		if (i+1<ts.num()) {
			const C8* s2=ts.idx(i+1).c_str();
			v=atoi(s2);
		}
		if (v) {
			++i;
		} else {
			v=1;
		}
		textureb* t=texture_create(s);
		t->load();
		if (t->hasalpha)
			hasalpha=t->hasalpha;
		texs.push_back(t);
		cnts.push_back(v);
	}
}

void texturebifl::addbitmap(const bitmap32* b,bool iscub)
{
}

bitmap32* texturebifl::locktexture()
{
	return 0;
}

void texturebifl::unlocktexture()
{
}

textureb* texturebifl::snapshot()
{
	if (texs.size())
		return texs[curtex];
	return 0;
}

void texturebifl::step()
{
	if (!texs.size())
		return;
	curcnt+=wininfo.framestep;
	U32 watch=0;
	while (curcnt>=cnts[curtex]) {
		curcnt-=cnts[curtex];
		++curtex;
		if (curtex==cnts.size())
			curtex=0;
		++watch;
		if (watch>10)
			break;
	}
}
