#define INCLUDE_WINDOWS
#include <m_eng.h>

///////////// directory stuff
static C8 currentdir[MAX_PATH]; 
#define MAXDIRSTACK 40
static C8 dirstack[MAXDIRSTACK][MAX_PATH];
static S32 dirstackp;

static void strtolower(C8* s)
{
	while(*s) {
		*s=tolower(*s);
		++s;
	}
}

static void strtolower(string& s)
{
	S32 i=0;
	while(s[i]) {
		s[i]=tolower(s[i]);
		++i;
	}
}

U32 setdir(const C8 *s) // returns true if setdir succeeded
{
//	logger("setdir(%d) to '%s'\n",dirstackp,s);
	U32 r;
	C8 temp[MAX_PATH];
	if (s[0]=='\0')
		r=SetCurrentDirectory(wininfo.datadir); // no path, just use datadir
	else if (s[0]=='\\' || s[0]=='/' || s[1]==':')
		r=SetCurrentDirectory(s);	// absolute path
	else {
		strcpy(temp,wininfo.datadir);		// relative to datadir
		strcat(temp,"\\");
		strcat(temp,s);
		r=SetCurrentDirectory(temp);	// get all data from data subdirectory
	}
	GetCurrentDirectory(MAX_PATH,currentdir); 
	strtolower(currentdir);
	return r; 
}

void pushdir()
{
//	logger("in pushandsetdir with '%s'\n",s);
	if (dirstackp>=MAXDIRSTACK)
		errorexit("pushandsetdir: dirstack too big");
	else
		strcpy(dirstack[dirstackp++],currentdir);
}

U32 pushandsetdir(const C8 *s)
{
	pushdir();
	return setdir(s);
}

U32 popdir()
{
//	logger("in popdir\n");
	if (dirstackp<=0)
		errorexit("popdir: dirstack empty");
	return setdir(dirstack[--dirstackp]);
}

U32 setdirdown(const C8* dn)
{
	string s=currentdir;
	s+='\\';
	s+=dn;
	return setdir(s.c_str());
}

U32 setdirup()
{
	S32 len=strlen(currentdir);
	if (len<=3)
		return 0;
	S32 i;
	for (i=len-1;i>=0;--i)
		if (currentdir[i]=='/' || currentdir[i]=='\\')
			break;
	if (i>=0) {
		C8 nd[300];
		strcpy(nd,currentdir);
		if (i<=2)
			++i;
		nd[i]='\0';
		return setdir(nd);
	}
	return 0;
}

U32 pushandsetdirdown(const C8* s)
{
	pushdir();
	return setdirdown(s);
}

U32 pushandsetdirup()
{
	pushdir();
	return setdirup();
}

const C8* getcurdir()
{
	return currentdir;
}

// look for space if no quotes
static C8* removequotes(C8* s)
{
	U32 i;
	if (s[0]=='\"') {
		i=1;
		while(s[i]) {
			if (s[i]=='\"') {
				s[i]='\0';
				return s+1;
			}
			++i;
		}
	}
	i=1;
	while(s[i]) {
		if (s[i]==' ' || s[i]=='\t') {
			s[i]=0;
			break;
		}
		++i;
	}
	return s;
}

static C8 exepath[MAX_PATH];
void getexepath()
{
// try GetCommandLine first, fails if running in a console window
// try GetCurrentDirecty next, fails if running in an IDE or drag and drop
	C8* cl=GetCommandLine();
	C8 cmdline[MAX_PATH];
	strcpy(cmdline,cl);
	C8* clp=removequotes(cmdline);
	C8 path[MAX_PATH];
	if (mgetpath(clp,path)) {
		strcpy(exepath,path);
//		error("commandline exepath '%s'",exepath);
		strtolower(exepath);
	} else {
		GetCurrentDirectory(MAX_PATH,exepath);
//		error("currentdirectory exepath '%s'",exepath);
		strtolower(exepath);
	}
}

void dirinit()
{
	getexepath();
	strcpy(wininfo.datadir,exepath);
	strcat(wininfo.datadir,"\\");
	strtolower(winmain_datadir);
	strcat(wininfo.datadir,winmain_datadir);
//	error("wininfo.datadir is '%s'",wininfo.datadir);

//	GetCurrentDirectory(MAX_PATH,path);
//	error("path '%s'",path);
}
void direxit()
{
	if (dirstackp)
		error("dirstack still at %d\n",dirstackp);
}

string reldir2absdir(string rd)
{
	return string(wininfo.datadir)+"\\"+rd;
}

string absdir2reldir(string ad)
{
	strtolower(ad);
	strtolower(wininfo.datadir);
	S32 r=ad.find(wininfo.datadir);
	logger("finding '%s' in '%s' returns a %d\n",wininfo.datadir,ad.c_str(),r);
	if (r>=0) {
		S32 len=strlen(wininfo.datadir);
		if (len<(S32)ad.size()) /// skip the backslash
			++len;
//		C8 low[MAX_PATH];
		ad=ad.substr(len,ad.size()-len); 
//		strcpy(low,ad.c_str());
//		strtolower(low);
//		return low;
		return ad.c_str();
	} else
		return ad;
}

string absdir2reldirdisplay(string ad)
{
	strtolower(ad);
	strtolower(wininfo.datadir);
	S32 r=ad.find(wininfo.datadir);
	logger("findingdisplay '%s' in '%s' returns a %d\n",wininfo.datadir,ad.c_str(),r);
	if (r>=0) {
		S32 len=strlen(wininfo.datadir);
		if (len<(S32)ad.size()) /// skip the backslash
			++len;
		return string("<")+winmain_datadir+">"+ad.substr(len,ad.size()-len); 
	} else
		return ad;
}
