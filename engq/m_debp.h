// print types
enum {D_CHAR,D_SHORT,D_INT,D_HEX,/*D_ENUM,*/D_FLOAT,D_DOUBLE,D_VOID,D_FLOATEXP,D_DOUBLEEXP/*,D_STR*/,D_STRING};
#define D_RDONLY 0x80
struct menuvar {
	const C8* name;
	void *ptr;
	S32 bytesize,speed;
//	U8** enumstr;
//	U8 *str;
};

void debprocesskey();
void drawdebprint();
//void loadconfigfile(U8*);
void loadconfigscript(const script&,menuvar* dv,S32 ndv);
void loadconfigfile(const C8* fname,struct menuvar *dv = 0,S32 ndv = 0);
void extradebvars(struct menuvar *dv,S32 ndv); // pass in 0,0 to remove extradebvars
// new extradebvars
void adddebvars(string name,struct menuvar *dv,S32 ndv,bool changemenupos = true);
void removedebvars(string name);

#define FLOATUP 65536

// supplied by app
//extern struct menuvar debvars[];
//extern U32 ndebvars;

// globals
//extern U32 enabledebprint;

void debp_init();
void debp_exit();
