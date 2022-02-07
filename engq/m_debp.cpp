#include <m_eng.h>
#include "m_perf.h"
#include "d2_font.h"

#define USESMALL // small font
//#define USEMEDIUM // medium font
//#define USELARGE // large font

#ifdef USESMALL
#define DEBFONT SMALLFONT
#define DEBUGNUMLINES (WY/8)
#endif
#ifdef USEMEDIUM
#define DEBFONT MEDIUMFONT
#define DEBUGNUMLINES (WY/16)
#endif
#ifdef USELARGE
#define DEBFONT LARGEFONT
#define DEBUGNUMLINES (WY/32)
#endif

#define DEFAULTIDX (DEBUGNUMLINES / 6)

// print location
#define DEBUGSTARTX	 8 //(WX/2) //8
#define DEBUGSTARTY	 0 // (WY-DEBUGNUMLINES*8-8) // (WY/2)

static S32 menupos=0;
static S32 totalvars = 0;

static S32 atox(const C8* s)
{
	C8 c;
	S32 ret=0;
	while(c=*s++) {
		if (c>='0' && c<='9') {
			ret<<=4;
			ret+=c-'0';
		} else if (c>='a' && c<='f') {
			ret<<=4;
			ret+=c-'a'+0xa;
		} else if (c>='A' && c<='F') {
			ret<<=4;
			ret+=c-'A'+0xA;
		}
	}
	return ret;
}

// read colors
// take '@lightmagenta@---- joystick input --------' and set color to lightmagenta
static void getcolorfromline(const C8* name,pointf3* col)
{
	C8 colstr[300],*colstrptr=colstr;
	if (name[0]!='@')
		return;
	strcpy(colstr,name+1);
	while(*colstrptr) {
		S32 idx;
		if (*colstrptr=='@') {
			*colstrptr='\0';
			idx=findstdcoloridx(colstr);
			if (idx>=0)
				*col=F32stdcolors[idx];
			return;
		}
		++colstrptr;
	}
	
}

// skip colors
// take '@lightmagenta@---- joystick input --------' and return '---- joystick input --------'
static const C8* getskipfromline(const C8* name)
{
	const C8* nameptr;
	if (name[0]!='@')
		return name;
	nameptr=name+1;
	while(*nameptr) {
		if (*nameptr=='@')
			return nameptr+1;
		++nameptr;
	}
	return name;
}

struct debinfo {
	string name;
	menuvar* vars;
	S32 nvars;
	S32* dum; // check for memory leaks
};

vector<debinfo> debvars;

void extradebvars(struct menuvar *dv,S32 ndv)
{
	removedebvars("__extra");
	if (dv != 0)
		adddebvars("__extra",dv,ndv);
}

void debp_init()
{
	logger("init debp\n");
	adddebvars("__sysvars",wininfovars,nwininfovars);
}

void debp_exit()
{
	logger("exit debp\n");
	removedebvars("__sysvars");
}

static S32 gettotalvars()
{
	S32 ret = 0; // total number of debvars
	vector<debinfo>::const_iterator it;
	// run through list of menuvar arrays to count total debvars
	for (it=debvars.begin();it!=debvars.end();++it)
		ret += it->nvars;
	return ret;
}

void adddebvars(string name,struct menuvar *dv,S32 ndv,bool changemenupos)
{
	removedebvars(name);
	debinfo di;
	di.name = name;
	di.vars = dv;
	di.nvars = ndv;
	pushmemname("debprint");
	di.dum = new int();
	popmemname();
	S32 oldtotalvars = gettotalvars();
	debvars.push_back(di);
	totalvars = gettotalvars();
	logger("debp, adding '%s' size %d, groups %d\n",name.c_str(),ndv,debvars.size());
	if (changemenupos) {
		if (totalvars > oldtotalvars) {
			menupos = oldtotalvars; // goto first line of new stuff
		}
		menupos=range(0,menupos,totalvars-1);
	}
}

void removedebvars(string name)
{
	for (U32 i=0;i<debvars.size();) {
		const debinfo& di = debvars[i];
		if (name == di.name) {
			delete di.dum;
			debvars.erase(debvars.begin() + i);
		} else {
			++i;
		}
	}
	logger("debp, removing '%s', groups %d\n",name.c_str(),debvars.size());
	totalvars = gettotalvars();
	menupos=range(0,menupos,totalvars-1);
}

// find menuvar pointer line given global position
static menuvar* getmenuvarfrompos(S32 pos)
{
	S32 cnt = pos;
	vector<debinfo>::const_iterator it;
	for (it=debvars.begin();it!=debvars.end();++it) {
		S32 lv = it->nvars; // number of vars in one group
		if (pos < lv) {
			return &it->vars[pos];
		}
		pos -= lv; // maybe now pos will be set for next block
	}
	errorexit("getmenuvarfrompos pos too big %d",pos);
	return 0;
}

void debprocesskey()
{
	//if (KEY=='`')
	//	wininfo.indebprint^=1;
	S32 d=0,z=0;		// delta for changing a variable
	S32 v;		// value of variable
	float fv;
	double dbv;
	void *p;
	//struct menuvar *dv=wininfovars;
	//S32 ndv=nwininfovars+nedv;
	if (!wininfo.enabledebprint)
		return;
	if (KEY=='`')
		wininfo.indebprint^=1;
	if (!wininfo.indebprint)
		return;
	if (wininfo.usedirectinput) {
		if (KEY==K_NUM5)
			z=1;
		if (KEY==K_NUMLEFT)
			d=-1;
		if (KEY==K_NUMRIGHT)
			d=1;
		if (KEY==K_NUMUP)
			menupos--;
		if (KEY==K_NUMDOWN)
			menupos++;
		if (KEY==K_NUMPAGEUP)
			menupos-=9;
		if (KEY==K_NUMPAGEDOWN)
			menupos+=9;
	} else {
		if (KEY==K_NUM5)
			z=1;
		if (KEY==K_LEFT)
			d=-1;
		if (KEY==K_RIGHT)
			d=1;
		if (KEY==K_UP)
			menupos--;
		if (KEY==K_DOWN)
			menupos++;
		if (KEY==K_PAGEUP)
			menupos-=9;
		if (KEY==K_PAGEDOWN)
			menupos+=9;
	}
	menupos=range(0,menupos,totalvars - 1);
	menuvar* dv = getmenuvarfrompos(menupos);
	p=dv->ptr;
	if (dv->speed == 0) {
		switch (dv->bytesize) {
		case D_FLOAT:
		case D_FLOATEXP:
		case D_DOUBLE:
		case D_DOUBLEEXP:
			dv->speed = FLOATUP;
			break;
		default:
			dv->speed = 1;
		}
	}
	if (KEY=='+') {
		if (dv->speed < (1<<30))
			dv->speed*=2;
	}
	if (KEY==K_NUMMINUS) {
		if (dv->speed > 1)
			dv->speed/=2;
	}
	d*=dv->speed;
	if (!wininfo.indebprint)
		d=0;
	switch(dv->bytesize) {
	case D_CHAR:
		v=*(U8*)p;
		v+=d;
		if (z)
			v=0;
		*(U8*)p=(U8)v;
		break;
	case D_SHORT:
		v=*(S16*)p;
		v+=d;
		if (z)
			v=0;
		*(S16*)p=(S16)v;
		break;
	case D_INT:
	case D_HEX:	
		v=*(S32*)p;
		v+=d;
		if (z)
			v=0;
		*(S32*)p=v;
		break;
	case D_FLOAT:	
	case D_FLOATEXP:	
		fv=*(float *)p;
		fv+=(float)(d/(float)FLOATUP);
		if (z)
			fv=0;
		*(float *)p=fv;
		break;
	case D_DOUBLE:
	case D_DOUBLEEXP:
		dbv = *(double *)p;
		dbv += (double)(d/(double)FLOATUP);
		if (z)
			dbv = 0;
		*(double *)p = dbv;
		break;
	}
}

void drawdebline(const menuvar* dv,S32 curmenupos,S32 y,pointf3& pcolor)
{
	C8 str[1000]; // line to print
	const C8* n; // name of variable after @...@ removed
	U32 v=0;		// value of variable
	float fv=0.0f;
	double dbv = 0.0;
	const C8* sp; // string cstr pointer
	void *p = dv->ptr; // generic variable pointer
	if (dv->name[0]=='@')
		getcolorfromline(dv->name,&pcolor);
	if (curmenupos == menupos)
		drawtextque_string_foreback(DEBFONT,DEBUGSTARTX,y,pcolor,F32BLACK,">");
//			outtextxyb32(B32,DEBUGSTARTX,y,pcolor,C32BLACK,">");
	else
		drawtextque_string_foreback(DEBFONT,DEBUGSTARTX,y,pcolor,F32BLACK," ");
//			outtextxyb32(B32,DEBUGSTARTX,y,pcolor,C32BLACK," ");
	switch(dv->bytesize&~D_RDONLY) {
	case D_CHAR:
		v=*(U8*)p;
		break;
	case D_SHORT:
		v=*(S16*)p;
		break;
	case D_INT:
	case D_HEX:
//		case D_ENUM:
		v=*(S32*)p;
		break;
	case D_FLOAT:
	case D_FLOATEXP:
		fv=*(float *)p;
		break;
	case D_DOUBLE:
	case D_DOUBLEEXP:
		dbv=*(double *)p;
		break;
	case D_STRING:
		sp = (*(string*)p).c_str();
/*		case D_DOUBLE:
		fv=(float)(*(double *)p);
		break; */
	} 
	n=getskipfromline(dv->name);
	S32 spd = dv->speed;
	if (!spd) {
		switch (dv->bytesize) {
		case D_FLOAT:
		case D_FLOATEXP:
		case D_DOUBLE:
		case D_DOUBLEEXP:
			spd = FLOATUP;
			break;
		default:
			spd = 1;
		}
	}
	switch(dv->bytesize) {
/*		case D_STR:
	case D_STR|D_RDONLY:
		sprintf(str,"%s %s",n,(U8*)dv[i].ptr);
		break; */
	case D_CHAR:
	case D_SHORT:
	case D_INT:
		sprintf(str,"%s %5d [%d]",n,v,spd);
		break;
	case D_CHAR|D_RDONLY:
	case D_SHORT|D_RDONLY:
	case D_INT|D_RDONLY:
		sprintf(str,"%s %5d",n,v);
		break;
	case D_HEX:
		sprintf(str,"%s $%08x [%08x]",n,v,spd);
		break;
	case D_HEX|D_RDONLY:
		sprintf(str,"%s $%08x",n,v);
		break;
/*		case D_ENUM:
	case D_ENUM|D_RDONLY:
		if (dv->enumstr[v])
			sprintf(str,"%s %s",n,dv->enumstr[v]);
		else
			sprintf(str,"%s ----",n);
		break; */
	case D_FLOAT:
//		case D_DOUBLE:
		sprintf(str,"%s %f [%f]",n,fv,spd/(float)FLOATUP);
		break;
	case D_DOUBLE:
		sprintf(str,"%s %f [%f]",n,dbv,spd/(float)FLOATUP);
		break;
	case D_FLOATEXP:
		sprintf(str, "%s %g [%f]", n, fv, spd / (float)FLOATUP);
		break;
	case D_DOUBLEEXP:
		sprintf(str, "%s %g [%f]", n, dbv, spd / (float)FLOATUP);
		break;
	case D_FLOAT|D_RDONLY:
//		case D_DOUBLE|D_RDONLY:
		sprintf(str,"%s %f",n,fv);
		break;
	case D_DOUBLE|D_RDONLY:
		sprintf(str,"%s %f",n,dbv);
		break;
	case D_FLOATEXP | D_RDONLY:
		sprintf(str, "%s %g", n, fv);
		break;
	case D_DOUBLEEXP | D_RDONLY:
		sprintf(str, "%s %g", n, dbv);
		break;
	case D_VOID:
	case D_VOID|D_RDONLY:
		sprintf(str,"%s",n);
		break;
	case D_STRING:
		sprintf(str,"%s '%s'",n,sp);
		//logger("string debprint draw = '%s %s'\n",n,sp);
		break;
	} 
	drawtextque_string_foreback(DEBFONT,DEBUGSTARTX+DEBFONT->gx,y,pcolor,F32BLACK,str);
//		outtextxyb32(B32,DEBUGSTARTX+8,y,pcolor,C32BLACK,str);
}

void drawdebprint()
{
	if (!wininfo.indebprint)
		return;
	S32 ndv = gettotalvars(); // total number of debvars
	perf_start(DRAWDEBPRINT);
	S32 startidx;			// start index of debug display, first line to print
	//U32 v=0;		// value of variable
	S32 idx; // index into whole debprint
	S32 locidx; // index into current menuvar list
	S32 ypos = DEBUGSTARTY;
	//float fv=0.0f;
	//const C8* sp; // string cstr pointer
	pointf3 pcolor=F32WHITE;
	//C8 str[100];
	//S32 ndv=nwininfovars+nedv;
//	video_lock();
	//menupos=range(0,menupos,ndv-1);
	startidx=range(0,menupos-DEFAULTIDX,max(0,ndv-DEBUGNUMLINES));


	// start from index 0 up to where printing is about to begin, looking for color changes
	vector<debinfo>::const_iterator it = debvars.begin();
	locidx = 0;
	for (idx=0;idx<startidx;idx++) {
		// convert
		const menuvar& dv = it->vars[locidx];
		const C8* name = dv.name;
		if (name[0] == '@')
			getcolorfromline(name,&pcolor);
		// now increment
		++locidx;
		ypos += DEBFONT->gy;
		if (locidx == it->nvars) {
			++it;
			locidx = 0;
		}
	}
	// continue from first printed line to the end of printing or debvars
	ypos = DEBUGSTARTY;

	for (idx=startidx,ypos=DEBUGSTARTY;idx<startidx+DEBUGNUMLINES && idx<ndv;idx++,ypos+=DEBFONT->gy) {
#if 1
		// convert
		const menuvar* dv = &it->vars[locidx];
		drawdebline(dv,idx,ypos,pcolor);
// now increment
		++locidx;
		if (locidx == it->nvars) {
			++it;
			locidx = 0;
		}
#endif
	}
//	video_unlock();
	perf_end(DRAWDEBPRINT);
}

string gscriptfile;

void doscriptline(string first,string second)
{
	if (first.length() && first[0] == '-') { // find and remove dashes
		first = first.substr(1);
		//errorexit("var name '%s' starts with '-', just checking",first.c_str());
	}
#if 1
	float *fptr;
	double *dptr;
	S32 *iptr;
	C8 *cptr;
	string* sptr;
	vector<debinfo>::const_iterator it;
	// run through list of menuvar arrays
	for (it=debvars.begin();it!=debvars.end();++it) {
		const menuvar* dv = it->vars;
		const S32 ndv = it->nvars;
		// each menuvar array
		S32 j;
		for (j=0;j<ndv;j++) {
#if 1
			if (first == dv[j].name) {
				switch(dv[j].bytesize&~D_RDONLY) {
				case D_HEX:
					iptr=(S32*)dv[j].ptr;
					*iptr=atox(second.c_str());
					break;
				case D_INT:
//					case D_ENUM:
					iptr=(S32*)dv[j].ptr;
					*iptr=atoi(second.c_str());
					break;
				case D_CHAR:
					cptr=(C8*)dv[j].ptr;
					*cptr=atoi(second.c_str());
					break;
				case D_FLOAT:
				case D_FLOATEXP:
					fptr=(float*)dv[j].ptr;
					*fptr=(float)atof(second.c_str());
					break;
				case D_DOUBLE:
				case D_DOUBLEEXP:
					dptr=(double*)dv[j].ptr;
					*dptr=atof(second.c_str());
					break;
				case D_STRING:
					sptr = (string*)dv[j].ptr;
					*sptr = second.c_str();
					break;
				}
				break; // wrote to debprint menuvar, done
			}
#endif
		}
		if (j!=ndv)
			break;
	}
	if (it == debvars.end())
		logger("WARNING: entry '%s' in file '%s' IS NOT in the debvar list\n",first.c_str(),gscriptfile.c_str());
#endif
}

void loadconfigscript(const script& s,menuvar* dev,S32 nedv)
{
	adddebvars("__loadconfigscript",dev,nedv,false); // don't update UI position in debprint
	S32 nscript = s.num();
	if (nscript&1) // odd number of strings
		errorexit("'%s' not right, try lines like: state 7",gscriptfile.c_str());
	for (S32 i=0;i<nscript;i+=2) {
		doscriptline(s.idx(i),s.idx(i+1));
	}
	removedebvars("__loadconfigscript");
//	freescript(script,nscript<<1);
}

void loadconfigfile(const C8* scriptfile,menuvar* dv,S32 ndv)
{
	gscriptfile = scriptfile;
//	script=loadscript(scriptfile,&nscript);
	script s(scriptfile);
	logger("loadconfigfile '%s', input chars %d, num tokens %d, num chars %d\n",scriptfile,s.inchars(),s.num(),s.numchars());
	loadconfigscript(s,dv,ndv);
	gscriptfile = "---";
}
