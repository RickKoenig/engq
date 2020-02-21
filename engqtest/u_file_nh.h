#if 0
// path has a '\' at end, ext has a '.' at start
void getpath(char *path,char *filename);
void getnameext(char *nameext,char *filename);
void getname(char *name,char *filename);
void getext(char *name,char *filename);
void buildfilename(char *filename,char *path,char *name,char *ext);

int fileext(char *filename,char *fext);

// handy file stuff
int fileexist(char *filename);
char *fileload(char *filename);
int filelen(char *name);		

// handleless fileio
#define READ	0
#define WRITE	1
//#define RW	2 // not used

#define FILE_START		0
#define FILE_CURRENT	1
#define FILE_END		2

unsigned int getfilesize();		// uses the fileopen
void fileopen(char *filename,int mode);
unsigned int fileread(void *buffer,unsigned int count);
unsigned int filewrite(void *buffer,unsigned int count);
unsigned int fileskip(int count,int method);
void fileclose(void);
char *getfilename();
unsigned int filereadbyte(void);
unsigned int filereadword(void);  // big endian (mac)
unsigned int filereadword2(void); // little endian (pc)
unsigned int filereadlong(void);  // big endian (mac)
unsigned int filereadlong2(void); // little endian (pc)
void filewritebyte(unsigned int);
void filewriteword(unsigned int); // big endian (mac)
void filewriteword2(unsigned int);// little endian (pc)
void filewritelong(unsigned int); // big endian (mac)
void filewritelong2(unsigned int);// little endian (pc)
// for lbm reader rle
//unsigned int filereadbytecomp(void);
void setcompress(int);
unsigned int filereadbyteq(void);
unsigned int filereadwordq(void);

// logfile, ignore the not enough args for macro if not USELOGGER
#ifdef USELOGGER
void logger(char *fmt,...);
#else
#define logger
#endif

// internal
void loggerinit(int second); // for the logfile
void loggerexit();

/////////// fopen2 /////////////
#ifndef _INC_STDIO
#define FILE int	// for those who don't include stdio.h
#endif
FILE *fopen2(char *name,char *mode);
void fopen2setctl(int ctlmode,int set); // 0 is reset
#define FOPEN2_PASSTHRU 0			// just do fopen
#define FOPEN2_STARTMONITOR 1		// start recording file access
#define FOPEN2_READCAG 2			// read cag or compressed cag files
#define FOPEN2_BUILDCAG 4			// build cag file from monitored files
#define FOPEN2_LOGMONITOR 8			// write monitored files to logfile
#define FOPEN2_RESETMONITOR 16		// monitor now has 0 files in it's store
#define FOPEN2_COMMON 32			// if reading a file and not found, don't
									// give up, read from common dir

void setloadcallback(void (*func)());

#define COMMONDIRSIZE 260
extern char commondir[COMMONDIRSIZE];

void loadmanifest(char *name);
void freemanifest();
#endif
