// logfile
void logger(const C8* fmt,...);
void loggervs(const C8 *fmt,va_list arglist);
void logger_indent();
void logger_unindent();
void logger_disableindent(); // for printing many things on a single line, use at start of line, will indent once
void logger_enableindent();
// internal
void loggerinit(); // for the logfile, called by winm
void loggerexit(); // tenative close of logfile at end of program, ignored, closed by OS
void loggerclose(); // for unhandled exception filter, OS might not close

// alternate fopen, use same old fclose
FILE *fopen2(const C8 *name,const C8 *mode);

U32 getfilesize(FILE* fh);
U32 fileread(FILE* fh,void* dest,U32 len);
U32 filereadU8(FILE* fh);
U32 filereadU16BE(FILE* fh);
U32 filereadU16LE(FILE* fh); // reads using little endian 8086
U32 filereadU32BE(FILE* fh);
U32 filereadU32LE(FILE* fh);
void filewrite(FILE* fh,const void* src,U32 len);
void filewriteU8(FILE* fh,U32 d);
void filewriteU16BE(FILE* fh,U32 d);
void filewriteU16LE(FILE* fh,U32 d); // writes using little endian 8086
void filewriteU32BE(FILE* fh,U32 d);
void filewriteU32LE(FILE* fh,U32 d);

S32 fileexist(const C8* name);
C8* fileload(const C8* name,S32 extra=0); // fileunload with delete[]
vector<U8> fileload_bin(const C8* name);
bool filesave_bin(const C8* name, const vector<U8>& bin);
string fileload_string(const C8* name); // returns string, used on non binary files
void filesave(const C8* name,const C8* data); // fileunload with delete[]
void filesave_string(const C8* name,string data); // fileunload with delete[]
// path has no '\' at end (but has one at the start), ext has no '.' at start
U32 mgetpath(const C8* filename,C8* path);
U32 mgetnameext(const C8* filename,C8* nameext);
U32 mgetname(const C8* filename,C8* name);
U32 mgetext(const C8*filename,C8* ext,S32 maxextsize); // above 3 should be changed too
bool isfileext(const C8* filename,const C8* ext);
void mbuildfilename(const C8* path,const C8* name,const C8* ext,C8* filename);
void mbuildfilenameext(const C8* path,const C8* nameext,C8* filename);
const C8* unique();

void mystrncpy(C8* dest,const C8* src,S32 sizeofdest);
const C8* getloggername(); // checks wininfo.isalreadyrunning

int my_stricmp(const char* a,const char* b);
int my_strnicmp(const char *s1, const char *s2, int len);
void my_strlwr(char* a);
void my_strupr(char* a);
char* my_itoa(int val,char* str,int base);
