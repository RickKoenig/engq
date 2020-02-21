// revised for fast graphics!!!
#if 0
#include <windows.h>

#include <m_eng.h>

//#include <stdio.h>	// for isnewer
//#include <string.h>
//#include <stdarg.h>
#include "u_file_nh.h"

//#include "engine1.h"

char commondir[MAX_PATH];
static int scanning;

// name of handleless file thats currently open
static int curfile=-1;
#define MAXFILES 10
static char thefilename[MAXFILES][300];
static char *modestrs[3]={"rb","wb","r+b"}; // 3 modes supported for handlelessfileio
static FILE *fp[MAXFILES];	// the handle for handleless files

// lbm run length compression
static int lbmcompress;
static int lbmgetval,lbmstate,lbmcount;

// callback for slow loads
static void (*loadcallback)();

// combine and break apart z:/path/filename.ext's
// given a filename, return just the path with a '\'
void getpath(char *path,char *filename)
{
	char *fp;
	strcpy(path,filename);
	fp=path+strlen(path);
	while(fp!=path-1) {
		if (*fp=='/' || *fp=='\\')
			break;
		fp--;
	}
	fp[1]='\0';
}

// given a filename, return just the nonpath name 
void getnameext(char *nameext,char *filename)
{
	char *fp;
	fp=filename+strlen(filename);
	while(fp!=filename-1) {
		if (*fp=='/' || *fp=='\\')
			break;
		fp--;
	}
	strcpy(nameext,fp+1);
}

void getname(char *name,char *filename)
{
	char *fp;
	getnameext(name,filename);
	fp=name+strlen(name);
	while(fp!=name-1) {
		if (*fp=='.')
			break;
		fp--;
	}
	if (fp!=name-1)
		fp[0]='\0';
	else
		strcpy(name,filename); // if no '.' found!
}

void getext(char *ext,char *filename)
{
	char *fp;
	char fname[300];
	getnameext(fname,filename);
	fp=fname+strlen(fname);
	while(fp!=fname-1) {
		if (*fp=='.')
			break;
		fp--;
	}
	if (fp==fname-1)
		ext[0]='\0';	// if no '.' found!
	else {
		strcpy(ext,fp);
		if (ext[1]=='\0')
			ext[0]='\0';
	}
}

int fileext(char *filename,char *fext)
{
	char ext[300];
	getext(ext,filename);
	if (!my_stricmp(ext,fext))
		return 1;
	return 0;
}

void buildfilename(char *filename,char *path,char *name,char *ext)
{
	filename[0]='\0';
	if (path)
		strcpy(filename,path);
	if (name)
		strcat(filename,name);
	if (ext)
		strcat(filename,ext);
}

#if 0
// handy file stuff, doesn't use handleless
int fileexist(char *filename)
{
	FILE *fr;
	fr=fopen2(filename,"rb");
	if (fr==0)
		return 0;
	fclose(fr);
	return 1;
}


char *fileload(char *filename)
{
	int filelen;
	char *buffer;
	FILE *f;
	if ((f=fopen2(filename,"rb"))==NULL) {
		errorexit("can't open '%s' for loading",filename);
		return NULL;
	}
	fseek(f,0,SEEK_END);
	filelen=ftell(f);
	fseek(f,0,SEEK_SET);
	buffer=memalloc(filelen+1);
	if (buffer==NULL)
		return NULL;
	fread(buffer,1,filelen,f);
	buffer[filelen]='\0';
	fclose(f);
	return buffer;
}

int filelen(char *name)
{
	int filelen;
	FILE *f;
	if ((f=fopen2(name,"rb"))==NULL)
		return 0;
	fseek(f,0,SEEK_END);
	filelen=ftell(f);
	fclose(f);
	return filelen;
}
#endif

//////////// handleless fileio ////////////////////////////////
unsigned int getfilesize()
{
	unsigned int fileptr;
	unsigned int len;
	if (!fp[curfile])
		return 0;
	fileptr=fileskip(0L,FILE_CURRENT);
	len=fileskip(0,FILE_END);
	fileskip(fileptr,FILE_START);
	return len;
}

void fileopen(char *filename,int mode)
{
//	mde=mode;
	curfile++;
	if (curfile==MAXFILES)
		errorexit("too many files.. '%s'",filename);
	fp[curfile]=fopen2(filename,modestrs[mode]);
/*	if ((mode==RW)&&(!fp[curfile])) {
		fp[curfile]=fopen2(filename,"wb");
		if (fp[curfile])
			fclose(fp[curfile]);
		fp[curfile]=fopen2(filename,modestrs[mode]);
	} */
	if (!fp[curfile])
		errorexit("error opening file '%s' for '%s'",filename,modestrs[mode]);
	strcpy(thefilename[curfile],filename);
}

unsigned int fileread(unsigned char *buffer,unsigned int count)
{
	return fread(buffer,1,count,fp[curfile]);
}

unsigned int filewrite(void *buffer,unsigned int count)
{
	unsigned int retval;
	retval=fwrite(buffer,1,count,fp[curfile]);
	if (retval!=count)
		errorexit("error writing file\n","","");
	return retval;
}

unsigned int fileskip(int count,int method)
{
	int offset=fseek(fp[curfile],count,method);
	if (offset!=0)
		errorexit("error in fseek");
	offset=ftell(fp[curfile]);
	if (offset==-1)
		errorexit("error in ftell");
	return offset;
}

void fileclose()
{
	if (curfile==-1)
		errorexit("can't close file");
	fclose(fp[curfile]);
	thefilename[curfile][0]='\0';
	curfile--;
}

char *getfilename()
{
	return thefilename[curfile];
}

unsigned int filereadbyte()
{
	unsigned char i;
	if (fread(&i,1,1,fp[curfile])==0) {
	//	printf("EOF ");
		return(EOF);
	}
	return(i);
}

unsigned int filereadword()
{
	unsigned int i;
	i=filereadbyte();
	i=(i<<8)+filereadbyte();
	return(i);
}

/////// reads using little endian 8086 //////
unsigned int filereadword2()
{
	unsigned int retval;
	retval=filereadbyte();
	return (filereadbyte()<<8) + retval;
}

unsigned int filereadlong()
{
	unsigned int i;
	i=filereadword();
	i=(i<<16)+filereadword();
	return(i);
}

unsigned int filereadlong2()
{
	unsigned int retval;
	retval=filereadword2();
	return (filereadword2()<<16) + retval;
}

static unsigned int filereadbytecomp()	/* read compressed */
{
	while(lbmstate==0) {
		lbmgetval=filereadbyte();
		if (lbmgetval>128) {
			lbmstate=2;
			lbmcount=257-lbmgetval;
			lbmgetval=filereadbyte();
		} else if (lbmgetval<128) {
			lbmstate=1;
			lbmcount=lbmgetval+1;
		}
	}
	if (lbmstate==1) {
		lbmgetval=filereadbyte();
		lbmcount--;
		if (lbmcount==0)
			lbmstate=0;
		return(lbmgetval);
	} else {
		lbmcount--;
		if (lbmcount==0)
			lbmstate=0;
		return(lbmgetval);
	}
}

unsigned int filereadbyteq()
{
	//unsigned int i;
	if (lbmcompress)
		return(filereadbytecomp());
	return(filereadbyte());
}

unsigned int filereadwordq()
{
	unsigned int i;
	i=filereadbyteq();
	return((i<<8)+filereadbyteq());
}

void setcompress(int c)
{
	lbmcompress=c;
}


void filewritebyte(unsigned int val)
{
	unsigned char c;
	c=val;
	filewrite(&c,1);
}

void filewriteword(unsigned int val)
{
	filewritebyte(val>>8);
	filewritebyte(val);
}

void filewritelong(unsigned int val)
{
	filewriteword(val>>16);
	filewriteword(val);
}

void filewriteword2(unsigned int val)
{
	filewritebyte(val);
	filewritebyte(val>>8);
}

void filewritelong2(unsigned int val)
{
	filewriteword2(val);
	filewriteword2(val>>16);
}
#if 0
//////////////// logfile ///////////////////////////
#ifdef USELOGGER
// log file
static FILE *flogger;
void loggerinit(int second)
{
	if (second)
		pushandsetdir("logfileb");
	flogger=fopen2("logfile.txt","w");
	if (second)
		popdir();
	logger("open logfile\n");
}

void loggerexit()
{
	logger("close logfile\n");
	if (flogger)
		fclose(flogger);
}

void logger(char *fmt,...)
{
	va_list arglist;
	va_start(arglist,fmt);
	if (flogger)
		vfprintf(flogger,fmt,arglist);
	va_end(arglist);
}

#else
// no log file
void loggerinit(int second)
{
}

void loggerexit()
{
}

#endif

///////////////////////////////////////////////// fopen2 ////////////////////////////////////////
#define MAXDIRS 80
#define MAXFLEN 256
#define MAXFLOGFILES 2800
static int usecommon,accessed;
static struct filelog {
	char filenames[MAXFLOGFILES][MAXFLEN]; // all the filenames
	int fileaccessed[MAXFLOGFILES];
	int filediridx[MAXFLOGFILES]; // all the directories files were accessed from
	int sortedfiles[MAXFLOGFILES]; 
	int filesizes[MAXFLOGFILES];
	int fileid[MAXFLOGFILES];
	int filesame[MAXFLOGFILES];
	int nfilenames;

	char dirnames[MAXDIRS][MAXFLEN];
	int dirsizes[MAXDIRS];	// sum total file sizes in dirs
	int nadirsizes[MAXDIRS];	// sum total file sizes in dirs
	int dirnfiles[MAXDIRS];
	int nadirnfiles[MAXDIRS];
	int sorteddirs[MAXDIRS];
	int ndirnames;
	int curdiridx;
} *fscan;

static int dotordotdot(char *s)
{
	if (!strcmp(s,"."))
		return 1;
	if (!strcmp(s,".."))
		return 1;
	return 0;
}

static void fileloggerfname(char *str)
{
	int i;
	if (accessed==-1)
		return;
	if (fscan->curdiridx<0)
//		return;
		errorexit("setdir not called");
	if (strlen(str)>=MAXFLEN)
		errorexit("big str");
	for (i=0;i<fscan->nfilenames;i++) {
		if (fscan->curdiridx==fscan->filediridx[i] && !my_stricmp(fscan->filenames[i],str)) {
			if (fscan->fileaccessed[i]==0 && accessed && !scanning)
				logger("new file added '%s'\n",str);
			fscan->fileaccessed[i]=accessed;
			break;
		}
	}
	if (i==MAXFLOGFILES)
		errorexit("too many files");
	if (i==fscan->nfilenames) {
		strcpy(fscan->filenames[i],str);
		fscan->filediridx[i]=fscan->curdiridx;
		fscan->fileaccessed[i]=accessed;
		fscan->nfilenames++;
	}
}

static void fileloggerdir(char *str)
{
	int i;
	if (accessed==-1)
		return;
	if (strlen(str)>=MAXFLEN)
		errorexit("big str");
	for (i=0;i<fscan->ndirnames;i++) {
		if (!my_stricmp(fscan->dirnames[i],str))
			break;
	}
	if (i==MAXDIRS)
		errorexit("too many dirs");
	if (i==fscan->ndirnames) {
		strcpy(fscan->dirnames[i],str);
		fscan->ndirnames++;
	}
	fscan->curdiridx=i;
}

static void scanfiles(char *dir)
{
	char fpn[MAX_PATH];
	HANDLE datadirs;
	WIN32_FIND_DATA fileinfo;
	;//ricklogger("setdir scanning '%s'\n",dir);
	pushandsetdir(dir);
	fileloggerdir(getdir());
	popdir();
	strcpy(fpn,dir);
	strcat(fpn,"\\*.*");
	datadirs=FindFirstFile(fpn,&fileinfo);
	if (datadirs!=INVALID_HANDLE_VALUE) {
		if (!dotordotdot(fileinfo.cFileName))
			fileloggerfname(fileinfo.cFileName);
		while (FindNextFile(datadirs,&fileinfo))
			if (!dotordotdot(fileinfo.cFileName))
				fileloggerfname(fileinfo.cFileName);
		FindClose(datadirs);
	}
}

static char *getsingquotename(char *str)
{
	int idx1,idx2,i;
	static char name[MAXFLEN];
	i=0;
	// find left quote
	while (str[i]!='\0') {
		if (str[i]=='\'') {
			idx1=i;
			break;
		}
		i++;
	}
	if (str[i]=='\0')
		return NULL;
	// find right quote
	i++;
	while (str[i]!='\0') {
		if (str[i]=='\'') {
			idx2=i;
			break;
		}
		i++;
	}
	if (str[i]=='\0')
		return NULL;
	if (idx2-idx1>MAXFLEN)
		return NULL;
	memcpy(name,str+idx1+1,idx2-idx1-1);
	name[idx2-idx1-1]='\0';
	return name;
}

void startfilelogger()
{
	char *tok;
	HANDLE datadirs;
	WIN32_FIND_DATA fileinfo;
	fscan=(struct filelog *)memzalloc(sizeof(*fscan));
	fscan->curdiridx=-1;
	pushandsetdir("");
	accessed=0;// files that are now 'in the dir' are put in list are marked as NOT accessed
	scanfiles(".");
	datadirs=FindFirstFile("*.*",&fileinfo);
	if (datadirs!=INVALID_HANDLE_VALUE) {
		if (fileinfo.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
			if (!dotordotdot(fileinfo.cFileName))
				scanfiles(fileinfo.cFileName);
		while (FindNextFile(datadirs,&fileinfo))
			if (fileinfo.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
				if (!dotordotdot(fileinfo.cFileName))
					scanfiles(fileinfo.cFileName);
		FindClose(datadirs);
	}
	accessed=-1; // files that are now fopen2'ed are NOT put in list 'files.txt'
	if (fileexist("files.txt")) {
		opentokfile("files.txt");
		accessed=1;	// files that are now fopen2'ed are put in list are marked as accessed
		while(tok=gettok()) {
			if (!strcmp(tok,"dir")) {
				tok=getsingquotename(getrestline());
				if (tok)
					fileloggerdir(tok);
			} else if (!strcmp(tok,"fname")) {
				tok=getsingquotename(getrestline());
				if (tok)
					fileloggerfname(tok);
			} else {
				getnextline();
			}
		}
		closetokfile();
	}
	accessed=1;	// files that are now fopen2'ed are put in list are marked as accessed
	popdir();
}

static int stricmpext(char *str1,char *str2)
{
	int ret;
	char *sp1=str1;
	char *sp2=str2;
	while(*sp1) {
		if (*sp1=='.')
			break;
		sp1++;
	}
	while(*sp2) {
		if (*sp2=='.')
			break;
		sp2++;
	}
	ret=my_stricmp(sp1,sp2);
	if (ret)
		return ret;
	return my_stricmp(str1,str2);
}

static void logfilelogger()
{
	FILE *filestatfile;
	int curfileid=0;
	int grandtotal=0,grandnfiles=0,nagrandtotal=0,nagrandnfiles=0;
	int i,j,k,t,m,d;
	// get filesizes
	accessed=-1;
	for (i=0;i<fscan->ndirnames;i++) {
		pushandsetdir(fscan->dirnames[i]);
		for (j=0;j<fscan->nfilenames;j++)
			if (fscan->filediridx[j]==i) {
				int len;
				filestatfile=fopen(fscan->filenames[j],"rb");
				if (filestatfile==NULL) {
//					errorexit("Error opening filelog file '%s'",fscan->filenames[j]);
					len=0;
				} else {
					fseek(filestatfile,0,SEEK_END);
					len=ftell(filestatfile);
					fclose(filestatfile);
				}
				fscan->filesizes[j]=len;
				if (fscan->fileaccessed[j]) {
					fscan->dirsizes[i]+=len;
					grandtotal+=len;
					fscan->dirnfiles[i]++;
					grandnfiles++;
				} else {
					fscan->nadirsizes[i]+=len;
					nagrandtotal+=len;
					fscan->nadirnfiles[i]++;
					nagrandnfiles++;
				}
			}
		popdir();
	}
// okay give each unique file a unique number
	for (j=0;j<fscan->nfilenames;j++) {
		int caninc=1;
		if (!fscan->fileaccessed[j])
			continue;
		for (i=0;i<j;i++) {
			if (!fscan->fileaccessed[i])
				continue;
			if (fscan->filesizes[i]==fscan->filesizes[j]) {
				char *ptr1,*ptr2;
				pushandsetdir(fscan->dirnames[fscan->filediridx[i]]);
				ptr1=fileload(fscan->filenames[i]);
				popdir();
				pushandsetdir(fscan->dirnames[fscan->filediridx[j]]);
				ptr2=fileload(fscan->filenames[j]);
				popdir();
				if (!memcmp(ptr1,ptr2,fscan->filesizes[i])) {
					fscan->filesame[i]++;
					fscan->filesame[j]=fscan->filesame[i];
					fscan->fileid[j]=fscan->fileid[i];
					caninc=0;
				}
				memfree(ptr1);
				memfree(ptr2);
			}
		} 
		if (caninc) {
			fscan->filesame[j]=1;
			fscan->fileid[j]=curfileid;
			curfileid+=caninc;
		}
	}
	//	setdir("bar");
	// sort the dirs
	for (i=0;i<fscan->ndirnames;i++)
		fscan->sorteddirs[i]=i;
	i=0;
	while (i<fscan->ndirnames-1) {
		if (my_stricmp(fscan->dirnames[fscan->sorteddirs[i]],fscan->dirnames[fscan->sorteddirs[i+1]])>0) {
			t=fscan->sorteddirs[i];
			fscan->sorteddirs[i]=fscan->sorteddirs[i+1];
			fscan->sorteddirs[i+1]=t;
			i=0;
		} else 
			i++;
	}
	// now sort the files
	for (i=0;i<fscan->nfilenames;i++)
		fscan->sortedfiles[i]=i;
	i=0;
	while (i<fscan->nfilenames-1) {
		d=fscan->filediridx[fscan->sortedfiles[i]]-fscan->filediridx[fscan->sortedfiles[i+1]];
		if ( d>0 || d==0 && stricmpext(fscan->filenames[fscan->sortedfiles[i]],fscan->filenames[fscan->sortedfiles[i+1]])>0) {
			t=fscan->sortedfiles[i];
			fscan->sortedfiles[i]=fscan->sortedfiles[i+1];
			fscan->sortedfiles[i+1]=t;
			i=0;
		} else 
			i++;
	}
	pushandsetdir("");
	filestatfile=fopen("files.txt","w");
	fprintf(filestatfile,"####################### the start of log of files accessed.. ##########################\n");
	for (i=0;i<fscan->ndirnames;i++) {
		k=fscan->sorteddirs[i];
		fprintf(filestatfile,"dir size %9d, nfiles %d '%s' -----------------------\n",
			fscan->dirsizes[k],fscan->dirnfiles[k],fscan->dirnames[k]);
		for (j=0;j<fscan->nfilenames;j++) {
			m=fscan->sortedfiles[j];
			if (fscan->filediridx[m]==k && fscan->fileaccessed[m])
				fprintf(filestatfile,"fname          size %9d, same %2d, id %4d, '%s'\n",
				fscan->filesizes[m],fscan->filesame[m],fscan->fileid[m],fscan->filenames[m]);
		}
		fprintf(filestatfile,"\n");
	}
	fprintf(filestatfile,"####################### the start of log of files NOT accessed.. ##########################\n");
	for (i=0;i<fscan->ndirnames;i++) {
		k=fscan->sorteddirs[i];
		fprintf(filestatfile,"nadir size %9d, nfiles %d '%s' -----------------------\n",
			fscan->nadirsizes[k],fscan->nadirnfiles[k],fscan->dirnames[k]);
		for (j=0;j<fscan->nfilenames;j++) {
			m=fscan->sortedfiles[j];
			if (fscan->filediridx[m]==k && !fscan->fileaccessed[m])
				fprintf(filestatfile,"nafname          size %9d, same %2d, id %4d, '%s'\n",
				fscan->filesizes[m],fscan->filesame[m],fscan->fileid[m],fscan->filenames[m]);
		}
		fprintf(filestatfile,"\n");
	}
	// ok lets list dupes a different way
	// now sort the files by id
	for (i=0;i<fscan->nfilenames;i++)
		fscan->sortedfiles[i]=i;
	i=0;
	while (i<fscan->nfilenames-1) {
		d=fscan->filesame[fscan->sortedfiles[i]]-fscan->filesame[fscan->sortedfiles[i+1]];
		if (d<0 || d==0 && fscan->fileid[fscan->sortedfiles[i]]-fscan->fileid[fscan->sortedfiles[i+1]]>0) {
			t=fscan->sortedfiles[i];
			fscan->sortedfiles[i]=fscan->sortedfiles[i+1];
			fscan->sortedfiles[i+1]=t;
			i=0;
		} else 
			i++;
	}
	fprintf(filestatfile,"############################### dupes ########################\n");
	k=fscan->sortedfiles[0];
	for (i=0;i<fscan->nfilenames;i++) {
		if (!fscan->fileaccessed[i])
			continue;
		j=fscan->sortedfiles[i];
		if (fscan->fileid[j]!=fscan->fileid[k] && fscan->filesame[j]>1)
			fprintf(filestatfile,"\n");
		k=j;
		if (fscan->filesame[j]>1)
			fprintf(filestatfile,"filesize %9d, same %2d, id %4d, dir '%9s', name '%s'\n",
				fscan->filesizes[j],fscan->filesame[j],fscan->fileid[j],fscan->dirnames[fscan->filediridx[j]],fscan->filenames[j]);
	}
	fprintf(filestatfile,"\n");
	fprintf(filestatfile,"############################### grand totals ########################\n");
	fprintf(filestatfile,"---------- grand total bytes of files accessed     %10d, nfiles %4d\n",
		grandtotal,grandnfiles);
	fprintf(filestatfile,"---------- grand total bytes of files NOT accessed %10d, nfiles %4d\n",
		nagrandtotal,nagrandnfiles);
	fprintf(filestatfile,"---------- total grand total bytes of all files    %10d, nfiles %4d\n",
		nagrandtotal+grandtotal,nagrandnfiles+grandnfiles);
	fclose(filestatfile);
	popdir();
	accessed=1;
}

void fopen2setctl(int ctl,int set)
{
	if (!set)
		errorexit("fopen2setctl can't reset (yet)");
	switch(ctl) {
	case FOPEN2_COMMON:
		usecommon=1;
		break;
	case FOPEN2_STARTMONITOR:
		scanning=1;
		if (!fscan)
			startfilelogger();
		scanning=0;
		break;
	case FOPEN2_LOGMONITOR:
		if (fscan)
			logfilelogger();
		break;
	case FOPEN2_RESETMONITOR:
		if (fscan) {
			memfree(fscan);
			fscan=NULL;
		}
		break;
	default:
		errorexit("fopen2setctl unknown mode");
	}
}

char *getmanifestvaluefromkey(char *key);
char dswmediadir[300];
int manistat_total,manistat_absolute,manistat_openok,manistat_openfailed,manistat_keynotfound;
FILE *fopen2(char *name,char *mode)
{
	char *value;
	char temp[MAX_PATH],vname[MAX_PATH],adir[MAX_PATH];
	char commonsubdir[MAX_PATH];
	extern char newdir[8][MAX_PATH];
	extern char relativedir[8][MAX_PATH];
	extern int dirstack;
	FILE *r=NULL;
	manistat_total++;
	if (!relativedir[dirstack][0] || mode[0]!='r') {
//		logger("fopen2 with dir of '%s', name '%s', mode '%s'\n",newdir[dirstack],name,mode);
		manistat_absolute++;
	} else { // look in the manifest
		strcpy(temp,relativedir[dirstack]);
		strcat(temp,"\\");
		strcat(temp,name);
		value=getmanifestvaluefromkey(temp);
		if (value) {
			getnameext(vname,value);
			getpath(temp,value);
			strcpy(adir,dswmediadir);
			strcat(adir,temp);
			pushandsetdir(adir);
			r=fopen(name,mode);
			popdir();
			if (r)
				manistat_openok++;
			else
				manistat_openfailed++;
		} else {
//			logger("keynotfound for '%s'\n",temp);
			manistat_keynotfound++;
		}
	}
	if (!r)
		r=fopen(name,mode);
	if (mode[0]!='r')
		return r;
	if (loadcallback)
		(*loadcallback)();
	if (fscan) {
		if (r) {
			fileloggerdir(getdir());
			fileloggerfname(name);
		} else if (usecommon) {
			pushandsetdir(commondir);
//			pushandsetdir("common");
			r=fopen(name,mode);
			if (r) {
				fileloggerdir(getdir());
				fileloggerfname(name);
			}
			popdir();
			if (!r) {
				strcpy(temp,commondir);
//				strcat(temp,"\\");
				getnameext(commonsubdir,newdir[dirstack]);
				strcat(temp,commonsubdir);
				pushandsetdir(temp);
				r=fopen(name,mode);
				if (r) {
					fileloggerdir(getdir());
					fileloggerfname(name);
				}
				popdir();
			}
		}
	} else if (usecommon && !r) {
		pushandsetdir(commondir);
//		pushandsetdir("common");
		r=fopen(name,mode);
		popdir();
		if (!r) {
			strcpy(temp,commondir);
//			strcat(temp,"\\");
			getnameext(commonsubdir,newdir[dirstack]);
			strcat(temp,commonsubdir);
			pushandsetdir(temp);
			r=fopen(name,mode);
			popdir();
		}
	}
	return r;
}

void setloadcallback(void (*func)())
{
	loadcallback=func;
}

static char **manisc;
static int nmanisc;
#define MAXHASH 256 // a power of 2
struct hashnode {
	char *key;
	char *value;
	struct hashnode *next;
};

static struct hashnode *hashtable[MAXHASH];

int hashfunc(char *k)
{
	char key2[MAX_PATH];
	char *k2=key2;
	int hash=0;
	strcpy(key2,k);
	my_strlwr(key2);
	while(*k2) {
		hash+=*k2;
		k2++;
	}
	return hash&(MAXHASH-1);
}

void loadmanifest(char *name)
{
	struct hashnode *hn;
	int i;
	int v;
	if (manisc)
		return;
	pushandsetdir(dswmediadir);
	if (!fileexist(name)) {
		popdir();
		return;
	}
	manisc=loadscript(name,&nmanisc);
	logger("%d tokens found in manifest '%s'\n",nmanisc,name);
	popdir();

	for (i=0;i<nmanisc>>1;i++) {
		v=hashfunc(manisc[i<<1]);
		hn=memalloc(sizeof(struct hashnode));
		hn->key=strclone(manisc[i<<1]);
		hn->value=strclone(manisc[(i<<1)+1]);
		hn->next=hashtable[v];
		hashtable[v]=hn;
	}

	if (manisc)
		freescript(manisc,nmanisc);
	manisc=NULL;
}

void freemanifest()
{
	int i,j,k=0,mh=0;
	struct hashnode *hp,*next;
	logger("manifest hash table %d entries\n",MAXHASH);
	for (i=0;i<MAXHASH;i++) {
		j=0;
		hp=hashtable[i];
		while (hp) {
			j++;
			next=hp->next;
			memfree(hp->key);
			memfree(hp->value);
			memfree(hp);
			hp=next;
		}
		hashtable[i]=NULL;
		if (j) {
//			logger("    %3d: %d\n",i,j);
			k+=j;
			if (j>mh)
				mh=j;
		}
	}
	logger("total %d, maxhash %d\n",k,mh);
	logger("manifest stats: total %d, absolute %d, openok %d, openfailed %d, keynotfound %d\n",
		manistat_total,manistat_absolute,manistat_openok,manistat_openfailed,manistat_keynotfound);
}

char *getmanifestvaluefromkey(char *key)
{
//	static char value[MAX_PATH];
	struct hashnode *hn;
	int i;
//	if (!manisc)
//		return NULL;
	i=hashfunc(key);
	hn=hashtable[i];
	while(hn) {
		if (!my_stricmp(key,hn->key))
			return hn->value;
		hn=hn->next;
	}
	return NULL;
}
#endif
#endif