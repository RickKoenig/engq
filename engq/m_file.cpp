#define INCLUDE_WINDOWS
#include <m_eng.h>
#include "m_perf.h"

#include <fstream>
#include <iostream>


// log file
static FILE* flogger;
static S32 indent,indentdisabled;

// cout file
fstream coutFile;


// copy to dest of sizeofdest size, copy enough chars to be able to add a null at the end of cstr
void mystrncpy(char *dest,const char *src,int sizeofdest)
{
	int nchars=sizeofdest-1;
	int pos=0;
	if (sizeofdest<=0)
		return;
	while(pos<nchars && src[pos]) {
		dest[pos]=src[pos];
		pos++;
	}
	dest[pos]='\0';
}

const C8* getloggername() // checks wininfo.isalreadyrunning
{
	if (wininfo.isalreadyrunning)
		return "logfileb.txt";
	else
		return "logfile.txt";
}

const C8* getcoutname() // checks wininfo.isalreadyrunning
{
	if (wininfo.isalreadyrunning)
		return "coutb.txt";
	else
		return "cout.txt";
}



void loggerinit()
{
	flogger=fopen2(getloggername(),"w");
	SYSTEMTIME lt;
	GetLocalTime(&lt);
	logger("open %s at LOCAL %04d_%02d_%02d_%02d_%02d_%02d\n",getloggername(),lt.wYear,lt.wMonth,lt.wDay,lt.wHour,lt.wMinute,lt.wSecond);
#if 1
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof (statex);
	GlobalMemoryStatusEx (&statex);
	logger("%ld percent of memory is in use.\n",statex.dwMemoryLoad);
	logger ("There are %lld total bytes of physical memory.\n",statex.ullTotalPhys);
	logger ("There are %lld free bytes of physical memory.\n",statex.ullAvailPhys);
	logger ("There are %lld total bytes of paging file.\n",statex.ullTotalPageFile);
	logger ("There are %lld free bytes of paging file.\n",statex.ullAvailPageFile);
	logger ("There are %lld total bytes of virtual memory.\n",statex.ullTotalVirtual);
	logger ("There are %lld free bytes of virtual memory.\n",statex.ullAvailVirtual);
	logger ("There are %lld free bytes of extended memory.\n",statex.ullAvailExtendedVirtual);
#endif

#if 1
	coutFile.open(getcoutname(), ios::out);
	string line;

	// Backup streambuffers of  cout 
	streambuf* stream_buffer_cout = cout.rdbuf();
	streambuf* stream_buffer_cin = cin.rdbuf();

	// Get the streambuffer of the coutFile 
	streambuf* stream_buffer_file = coutFile.rdbuf();

	// Redirect cout to file 
	cout.rdbuf(stream_buffer_file);

	const U32 COUTMAXHEADER = 1000;
	C8 coutHeader[COUTMAXHEADER];
	S32 ret = snprintf(coutHeader,COUTMAXHEADER,"open %s at LOCAL %04d_%02d_%02d_%02d_%02d_%02d\n", getcoutname(), lt.wYear, lt.wMonth, lt.wDay, lt.wHour, lt.wMinute, lt.wSecond);
	cout << coutHeader;
#if 0
	// Redirect cout back to screen 
	cout.rdbuf(stream_buffer_cout);
	cout << "This line is written to screen" << endl;

	coutFile.close();
#endif
#endif
}

void loggerexit()
{
	logger("close logfile, not quite\n");
	for (S32 i = 0; i < 40; ++i)
		logger("\n");
//	if (flogger)
//		fclose(flogger);
	cout << "close coutfile" << endl;
	coutFile.close();
}

void loggerclose() // for unhandled exception filter
{
	if (flogger)
		fclose(flogger);
}

void logger_indent()
{
	indent+=3;
//	logger("indent at %d\n",indent);
}

void logger_unindent()
{
	indent-=3;
	if (indent<0)
		indent=0;
//	logger("unindent at %d\n",indent);
}

static void loggerspace(S32 amount)
{
	S32 i;
	if (!indentdisabled)
		for (i=0;i<amount;++i)
			fprintf(flogger," ");
}

void logger_disableindent()
{
	loggerspace(indent);
	indentdisabled=1;
}

void logger_enableindent()
{
	indentdisabled=0;
}

void logger(const C8* fmt,...)
{
	//return;
	va_list arglist;
	va_start(arglist,fmt);
//	perf_start(LOGGER);
	if (flogger) {
		loggerspace(indent);
		vfprintf(flogger,fmt,arglist);
#define LOGCON // also log to console
#ifdef LOGCON
		const U32 maxsize = 10000;
		char str[maxsize];
		vsnprintf(str,maxsize,fmt,arglist);
		str[maxsize-1] = '\0';
		OutputDebugString(str);
#endif
	}
	va_end(arglist);
//	perf_end(LOGGER);
}

void loggervs(const C8* fmt,va_list arglist)
{
	if (flogger) {
		loggerspace(indent);
		vfprintf(flogger,fmt,arglist);
	}
}

// alternate fopen uses same old fclose
FILE* fopen2(const C8* name,const C8* mode)
{
	C8 curdir[MAX_PATH];
	FILE* ret;
	ret=fopen(name,mode);
	if (!ret) {
		pushandsetdir("common");
		ret=fopen(name,mode);
		popdir();
		if (!ret) {
			logger("going to sleep, hope file '%s' can be accessed later!\n",name); // if process is highpriority
			Sleep(100);
			ret=fopen(name,mode);
			if (!ret) {
				pushandsetdir("common");
				ret=fopen(name,mode);
				popdir();
				if (!ret) {
					GetCurrentDirectory(MAX_PATH,curdir);
					errorexit("can't open file '%s' errno %d, mode '%s' curdir '%s'",name,errno,mode,curdir);
				}
			}
		}
	}
	return ret;
}

C8* fileload(const C8* name,S32 extra) // fileunload with delete[]
{
	if (fileexist(name)) {
		FILE* fp=fopen2(name,"rb");
		S32 size=getfilesize(fp);
		C8* ret=new C8[size+1+extra];
		U32 rd=fread(ret,1,size,fp);
		ret[size]='\0';
		fclose(fp);
		return ret;
	} else {
		C8* ret=new C8[1+extra];
		ret[0]='\0';
		return ret;
	}
}

vector<U8> fileload_bin(const C8* name)
{
	vector<U8> bin;
	if (fileexist(name)) {
		FILE* fp=fopen2(name,"rb");
		S32 size=getfilesize(fp);
		bin.assign(size,0);
		U32 rd=fread(&bin[0],1,size,fp);
		fclose(fp);
	}
	return bin;
}

bool filesave_bin(const C8* name, const vector<U8>& bin)
{
	FILE *fw = fopen2(name,"wb");
	if (!fw)
		return false;
	filewrite(fw,&bin[0],bin.size());
	fclose(fw);
	return true;
}

string fileload_string(const C8* name)
{
	if (fileexist(name)) {
		U32 rd;
		C8* buf;
		FILE* fp=fopen2(name,"rb");
		S32 size=getfilesize(fp);
		buf=new C8[size+1];
		rd=fread(buf,1,size,fp);
		buf[size]='\0';
		fclose(fp);
		string ret = buf;
		delete[] buf;
		return ret;
	} else {
		return "";
	}
}

void filesave(const C8* fname,const C8* data) // fileunload with delete[]
{
	U32 len=strlen(data);
	FILE* fp=fopen2(fname,"wb");
	fwrite(data,1,len,fp);
	fclose(fp);
}

void filesave_string(const C8* fname,string data) // fileunload with delete[]
{
	U32 len=data.length();
	FILE* fp=fopen2(fname,"wb");
	fwrite(data.c_str(),1,len,fp);
	fclose(fp);
}

S32 fileexist(const C8* name)
{
	if (strlen(name) > 400)
		return 0;
	FILE* fp;
	fp=fopen(name,"rb");
	if (fp) {
		fclose(fp);
		return 1;
	}
	C8 cname[500];
	strcpy(cname,"common/");
	strcat(cname,name);
//	pushandsetdir("common");
	fp=fopen(cname,"rb");
//	popdir();
	if (fp) {
		fclose(fp);
		return 1;
	}
	return 0;
}

U32 getfilesize(FILE* fh)
{
	U32 save=ftell(fh);
	U32 ret;
	fseek(fh,0,SEEK_END);
	ret=ftell(fh);
	fseek(fh,save,SEEK_SET);
	return ret;
}

U32 fileread(FILE* fh,void* dest,U32 len)
{
	return fread(dest,1,len,fh);
}

U32 filereadU8(FILE* fh)
{
	U8 i;
	if (fread(&i,1,1,fh)==0) {
	//	printf("EOF ");
		return(~0);
	}
	return i;
}

U32 filereadU16BE(FILE* fh)
{
	U32 i;
	i=filereadU8(fh);
	i=(i<<8)|filereadU8(fh);
	return i;
}

/////// reads using little endian 8086 //////
U32 filereadU16LE(FILE* fh)
{
	U32 i;
	i=filereadU8(fh);
	return (filereadU8(fh)<<8) | i;
}

U32 filereadU32BE(FILE* fh)
{
	U32 i;
	i=filereadU16BE(fh);
	i=(i<<16)|filereadU16BE(fh);
	return i;
}

U32 filereadU32LE(FILE* fh)
{
	U32 i;
	i=filereadU16LE(fh);
	return (filereadU16LE(fh)<<16) | i;
}

void filewrite(FILE* fh,const void* src,U32 len)
{
	fwrite(src,1,len,fh);
}

void filewriteU8(FILE* fh,U32 d)
{
	U8 i = d;
	fwrite(&i,1,1,fh);
}

void filewriteU16BE(FILE* fh,U32 d)
{
	filewriteU8(fh,d>>8);
	filewriteU8(fh,d);
}

void filewriteU16LE(FILE* fh,U32 d) // writes using little endian 8086
{
	filewriteU8(fh,d);
	filewriteU8(fh,d>>8);
}

void filewriteU32BE(FILE* fh,U32 d)
{
	filewriteU16BE(fh,d>>16);
	filewriteU16BE(fh,d);
}

void filewriteU32LE(FILE* fh,U32 d)
{
	filewriteU16LE(fh,d);
	filewriteU16LE(fh,d>>16);
}


// split up and combine absolute [drive:]/path/name.ext into /path name ext
// [] means optional
static const C8* skipdriveletter(const C8* p)
{
	if (strlen(p)<2)
		return p;
	if (p[1]==':')
		return p+2;
	return p;
}
static const C8* getdriveletter(const C8* p)
{
	static C8 drv[2];
	drv[0]='\0';
	if (strlen(p)<2)
		return drv;
	if (p[1]==':') {
		drv[0]=p[0];
		drv[1]='\0';
		return drv;
	}
	return drv;
}
// /path/name.ext becomes /path
// /name.ext becomes '' # no path, zero len string
// new 1/12/2009
// path/name.ext becomes path
U32 mgetpath(const C8* filename,C8* path)
{
//	error("finding path in '%s'",filename);
	const C8* dl=getdriveletter(filename);
	filename=skipdriveletter(filename);
	S32 len=strlen(filename);
	S32 fp=len-1;
	if (len<1)
		return 0;
//	if (filename[0]!='/' && filename[0]!='\\')
//		return 0; // 1/12/2009
	while(fp>=0) {
		if (filename[fp]=='/' || filename[fp]=='\\')
			break;
		fp--;
	}
	if (fp<0)
		return 0;
	if (fp==0) {
//		strcpy(path,"/");
		if (!dl[0]) {
			path[0]='\0';
			return 1;
		} else {
			path[0]=dl[0];
			path[1]=':';
			path[2]='\0';
			return 1;
		}
	}
	if (!dl[0]) {
		memcpy(path,filename,fp);
		path[fp]='\0';
	} else {
		path[0]=dl[0];
		path[1]=':';
		memcpy(path+2,filename,fp);
		path[fp+2]='\0';
	}
	return 1;
}

// /path/name.ext becomes name.ext OR name.ext becomes name.ext
U32 mgetnameext(const C8* filename,C8* nameext)
{
	filename=skipdriveletter(filename);
	S32 len=strlen(filename);
	S32 fp=len-1;
	if (len<1)
		return 0;
	if (filename[fp]=='/' || filename[fp]=='\\')
		return 0;
	--fp;
	while(fp>=0) {
		if (filename[fp]=='/' || filename[fp]=='\\')
			break;
		fp--;
	}
	strcpy(nameext,&filename[fp+1]);
	return 1;
}

// /path/name.ext becomes name OR name.ext becomes name OR name becomes name
U32 mgetname(const C8* filename,C8* name)
{
	C8 nameext[MAX_PATH];
	if (!mgetnameext(filename,nameext))
		return 0;
	S32 len=strlen(nameext);
	S32 fp=len-1;
	if (len<1)
		return 0;
	while(fp>=0) {
		if (nameext[fp]=='.')
			break;
		fp--;
	}
	if (fp<0) {
		strcpy(name,nameext);
		return 1;
	}
	if (fp==0)
		return 0;
	memcpy(name,nameext,fp);
	name[fp]='\0';
	return 1;
}

// name.ext becomes ext
U32 mgetext(const C8* filename,C8* ext,S32 maxextsize)
{
	filename=skipdriveletter(filename);
	// limit ext to 50 chars

	S32 len=strlen(filename);
	if (len<2)
		return 0;
	S32 fp=len-1; // point at last U8
	if (filename[fp]=='.') {
		ext[0] = '\0';
		return 0;
	}
	--fp;
	while(fp>=0) {
		if (filename[fp]=='/' || filename[fp]=='\\')
			return 0;
		if (filename[fp]=='.')
			break;
		--fp;
	}
	if (fp<0) {
		ext[0] = '\0';
		return 0;
	}
	mystrncpy(ext,&filename[fp+1],maxextsize);
	return 1;
}

bool isfileext(const C8* filename,const C8* cext)
{
	C8 ext[50];
	if (mgetext(filename,ext,50)) {
		return !my_stricmp(ext,cext);
	}
	return false;
}

// /path name ext becomes /path/name.ext
void mbuildfilename(const C8* path,const C8* name,const C8* ext,C8* filename)
{
	filename[0]='\0';
	if (path) {
		strcpy(filename,path);
		strcat(filename,"/");
	}
	if (name)
		strcat(filename,name);
	if (ext) {
		strcat(filename,".");
		strcat(filename,ext);
	}
}

void mbuildfilenameext(const C8* path,const C8* nameext,C8* filename)
{
	filename[0]='\0';
	if (path) {
		strcpy(filename,path);
		strcat(filename,"/");
	}
	if (nameext)
		strcat(filename,nameext);
}

const C8* unique()
{
	static U32 u;
	static C8 ret[20];
	sprintf(ret,"unique%u",u++);
	return ret;
}

int my_stricmp(const char* s1,const char* s2)
{
    while (tolower((unsigned char) *s1) == tolower((unsigned char) *s2)) {
        if (*s1 == '\0')
        return 0;
        s1++; s2++;
    }
    return (int) tolower((unsigned char) *s1) -
      (int) tolower((unsigned char) *s2);
}

int my_strnicmp(const char *s1, const char *s2, int len)
{
    if(!len)
        return 0;
    do {
        if(!(*s1) || !(*s2))
            break;
        if(*s1 == *s2)
            continue;
        if(tolower(*s1) != (tolower(*s2)))
            break;
    } while(len-- && *s1++ && *s2++);
    return (int)(*s1) - (int)(*s2);
}

void my_strlwr(char* a)
{
    while(*a) {
        *a = tolower((unsigned char) *a);
        ++a;
    }
}

void my_strupr(char* a)
{
    while(*a) {
        *a = toupper((unsigned char) *a);
        ++a;
    }
}

char* my_itoa(int val,char* str,int base)
{
    if (base != 10)
        errorexit("my_itoa base not 10");
    sprintf(str,"%d",val);
    return str;
}


