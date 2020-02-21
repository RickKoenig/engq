#include <m_eng.h>

C32 pointf3toC32(const struct pointf3* f)
{
	U32 r,g,b,a;
	r=(U32)(f->x*255.0f);
	g=(U32)(f->y*255.0f);
	b=(U32)(f->z*255.0f);
	a=(U32)(f->w*255.0f);
	return C32(r,g,b,a);
}

pointf3 C32topointf3(C32 c,float m)
{
	pointf3 fc;
	fc.x=c.r*(1/255.0f)*m;
	fc.y=c.g*(1/255.0f)*m;
	fc.z=c.b*(1/255.0f)*m;
	fc.w=c.a*(1/255.0f)*m;
	return fc;
}

cki colorkeyinfo;

C32 C32stdcolors[CI_NUM]={
	C32BLACK,
	C32BLUE,
	C32GREEN,
	C32CYAN,
	
	C32RED,
	C32MAGENTA,
	C32BROWN,
	C32LIGHTGRAY,
	
	C32DARKGRAY,
	C32LIGHTBLUE,
	C32LIGHTGREEN,
	C32LIGHTCYAN,
	
	C32LIGHTRED,
	C32LIGHTMAGENTA,
	C32YELLOW,
	C32WHITE,
};

struct pointf3 F32stdcolors[CI_NUM]={
	{  0.0f/256.0f,  0.0f/256.0f,  0.0f/256.0f,{1}},
	{  0.0f/256.0f,  0.0f/256.0f,170.0f/256.0f,{1}},
	{  0.0f/256.0f,170.0f/256.0f,  0.0f/256.0f,{1}},
	{  0.0f/256.0f,170.0f/256.0f,170.0f/256.0f,{1}},
												
	{170.0f/256.0f,  0.0f/256.0f,  0.0f/256.0f,{1}},
	{170.0f/256.0f,  0.0f/256.0f,170.0f/256.0f,{1}},
	{170.0f/256.0f, 85.0f/256.0f,  0.0f/256.0f,{1}},
	{170.0f/256.0f,170.0f/256.0f,170.0f/256.0f,{1}},
												
	{ 85.0f/256.0f, 85.0f/256.0f, 85.0f/256.0f,{1}},
	{ 85.0f/256.0f, 85.0f/256.0f,255.0f/256.0f,{1}},
	{ 85.0f/256.0f,255.0f/256.0f, 85.0f/256.0f,{1}},
	{ 85.0f/256.0f,255.0f/256.0f,255.0f/256.0f,{1}},
												
	{255.0f/256.0f, 85.0f/256.0f, 85.0f/256.0f,{1}},
	{255.0f/256.0f, 85.0f/256.0f,255.0f/256.0f,{1}},
	{255.0f/256.0f,255.0f/256.0f, 85.0f/256.0f,{1}},
	{255.0f/256.0f,255.0f/256.0f,255.0f/256.0f,{1}},
};

C8* stdcolornames[CI_NUM]={
	"black",
	"blue",
	"green",
	"cyan",
	
	"red",
	"magenta",
	"brown",
	"lightgray",
	
	"darkgray",
	"lightblue",
	"lightgreen",
	"lightcyan",
	
	"lightred",
	"lightmagenta",
	"yellow",
	"white"
};

S32 findstdcoloridx(const C8* name)
{
	U32 i;
	for (i=0;i<CI_NUM;++i)
		if (!strcmp(name,stdcolornames[i]))
			return i;
	return -1;
}

struct bitmap32* convert8to32(struct bitmap8* b8,C32* dacs)
{
	struct bitmap32* ret32 = bitmap32alloc(b8->size.x,b8->size.y,C32BLACK);
	convert8to32(b8,dacs,ret32);
	return ret32;
}

void convert8to32(struct bitmap8* b8,C32* dacs,struct bitmap32* b32)
{
	S32 i;
	C32* p32;
	U8* p8;
	S32 prod=b8->size.x*b8->size.y;
	p8=b8->data;
	p32=b32->data;
	colorkeyinfo.lasthascolorkey=0;
	if (colorkeyinfo.usecolorkey) {
		C32 colorkeyval=C32(0,0,0);
		for (i=0;i<prod;++i) {
			U8 pi=p8[i];
			p32[i]=dacs[pi];
//			if (pi==0) { // palette 0 is colorkey
			if (p32[i].c32==colorkeyval.c32) { // color 0 is colorkey
				p32[i].a=0; // black is transparent
				//p32[i].a = 255; // whole image is opaque
				colorkeyinfo.lasthascolorkey=1;
//				p32[i].a=0;
			}
		}
	} else {
		for (i=0;i<prod;++i)
			p32[i]=dacs[p8[i]];
	}
}
