#include <m_eng.h>
#include <ddraw.h>
#include "m_perf.h"

static S32 ddsclipmap;
static const C8* curname;

static C32 color565toC32(U16 c)
{
	C32 r;
	r.r=((c>>11)&0x1f)<<3;
	r.g=((c>>5)&0x3f)<<2;
	r.b=(c&0x1f)<<3;
	r.a=0xff;
	return r;
}

static void dxt1reada(U16 *c,struct bitmap32 *m)//,struct bitmap24 *a)
{
	C32 color[4];
	S32 bits;
	S32 bmx,bmy,mx,my;
	S32 i,j,px,py,u,v;
	U32 val;
	mx=m->size.x;
	my=m->size.y;
	bmx=(mx+3)>>2;
	bmy=(my+3)>>2;
	for (j=0;j<bmy;j++)
		for (i=0;i<bmx;i++) {
			color[0]=color565toC32(c[0]);
			color[1]=color565toC32(c[1]);
			if (c[0]>c[1]) {
				color[2].r=((color[0].r<<1)+color[1].r+1)/3;
				color[2].g=((color[0].g<<1)+color[1].g+1)/3;
				color[2].b=((color[0].b<<1)+color[1].b+1)/3;
				color[2].a=0xff;
				color[3].r=(color[0].r+(color[1].r<<1)+1)/3;
				color[3].g=(color[0].g+(color[1].g<<1)+1)/3;
				color[3].b=(color[0].b+(color[1].b<<1)+1)/3;
				color[3].a=0xff;
			} else {
				color[2].r=(color[0].r+color[1].r)>>1;
				color[2].g=(color[0].g+color[1].g)>>1;
				color[2].b=(color[0].b+color[1].b)>>1;
				color[2].a=0xff;
				color[3]=C32(0,0,0,0);
			}
			val=c[2]+(c[3]<<16);
			for (v=0;v<4;v++) {
				for (u=0;u<4;u++) {
					bits=val&3;
					val>>=2;
					px=(i<<2)+u;
					py=(j<<2)+v;
					if ((px<mx) && (py<my)) {
						clipputpixel32(m,px,py,color[bits]);
						if (bits==3 && color[3].a==0)
							ddsclipmap=1;
					}
				}
			}
			c+=4;
		}
}

static void dxt3reada(U16 *c,struct bitmap32 *m)
{
	C32 color[4];
	S32 bits;
	S32 bmx,bmy,mx,my;
	S32 i,j,px,py,u,v;
	U32 val;
	mx=m->size.x;
	my=m->size.y;
	bmx=(mx+3)>>2;
	bmy=(my+3)>>2;
	for (j=0;j<bmy;j++) {
		for (i=0;i<bmx;i++) {
			for (v=0;v<4;v++) {
				val=c[v];
				for (u=0;u<4;u++) {
					bits=val&15;
					val>>=4;
					C32 alpha;
					alpha.r=alpha.g=alpha.b=0;
					alpha.a=bits<<4;
					px=(i<<2)+u;
					py=(j<<2)+v;
					if ((px<mx) && (py<my)) {
						clipputpixel32(m,px,py,alpha); 
						if (bits<15)
							ddsclipmap=4;
					}
				}
			}
			c+=4;
			color[0]=color565toC32(c[0]);
			color[1]=color565toC32(c[1]);
			color[2].r=((color[0].r<<1)+color[1].r+1)/3;
			color[2].g=((color[0].g<<1)+color[1].g+1)/3;
			color[2].b=((color[0].b<<1)+color[1].b+1)/3;
			color[3].r=(color[0].r+(color[1].r<<1)+1)/3;
			color[3].g=(color[0].g+(color[1].g<<1)+1)/3;
			color[3].b=(color[0].b+(color[1].b<<1)+1)/3;
			color[0].a=color[1].a=color[2].a=color[3].a=0;
			val=c[2]+(c[3]<<16);
			for (v=0;v<4;v++) {
				for (u=0;u<4;u++) {
					bits=val&3;
					val>>=2;
					px=(i<<2)+u;
					py=(j<<2)+v;
					if ((px<mx) && (py<my)) {
						C32 compo=clipgetpixel32(m,px,py);
						compo.c32+=color[bits].c32;
						clipputpixel32(m,px,py,compo);
					}
				}
			}
			c+=4;
		}
	}
}

static void dxt5reada(U16 *c,struct bitmap32 *m)
{
	C32 color[4];
	U8 alpha[8];
	S32 bits;
	S32 bmx,bmy,mx,my;
	S32 i,j,px,py,u,v;
	U32 val,val2;
	mx=m->size.x;
	my=m->size.y;
	bmx=(mx+3)>>2;
	bmy=(my+3)>>2;
	for (j=0;j<bmy;j++)
		for (i=0;i<bmx;i++) {
			alpha[0]=c[0]&0xff;
			alpha[1]=c[0]>>8;
			if (alpha[0]>alpha[1]) {
				alpha[2]=(6*alpha[0]+alpha[1]+3)/7;
				alpha[3]=(5*alpha[0]+2*alpha[1]+3)/7;
				alpha[4]=(4*alpha[0]+3*alpha[1]+3)/7;
				alpha[5]=(3*alpha[0]+4*alpha[1]+3)/7;
				alpha[6]=(2*alpha[0]+5*alpha[1]+3)/7;
				alpha[7]=(alpha[0]+6*alpha[1]+3)/7;
			} else {
				alpha[2]=(4*alpha[0]+alpha[1]+2)/5;
				alpha[3]=(3*alpha[0]+2*alpha[1]+2)/5;
				alpha[4]=(2*alpha[0]+3*alpha[1]+2)/5;
				alpha[5]=(alpha[0]+4*alpha[1]+2)/5;
				alpha[6]=0;
				alpha[7]=255;
			}
			val=c[1]+(c[2]<<16);
			val2=c[3];
			for (v=0;v<4;v++)
				for (u=0;u<4;u++) {
					bits=val&7;
					val>>=3;
					val|=((val2&7)<<(32-3));
					val2>>=3;
					px=(i<<2)+u;
					py=(j<<2)+v;
					if ((px<mx) && (py<my)) {
						C32 aa;
						aa.r=aa.g=aa.b=0;
						aa.a=alpha[bits];
						clipputpixel32(m,px,py,aa);
						if (aa.a<240)
							ddsclipmap=8;
					}
				}
			c+=4;
			color[0]=color565toC32(c[0]);
			color[1]=color565toC32(c[1]);
			color[2].r=((color[0].r<<1)+color[1].r+1)/3;
			color[2].g=((color[0].g<<1)+color[1].g+1)/3;
			color[2].b=((color[0].b<<1)+color[1].b+1)/3;
			color[3].r=(color[0].r+(color[1].r<<1)+1)/3;
			color[3].g=(color[0].g+(color[1].g<<1)+1)/3;
			color[3].b=(color[0].b+(color[1].b<<1)+1)/3;
			color[0].a=color[1].a=color[2].a=color[3].a=0;
			val=c[2]+(c[3]<<16);
			for (v=0;v<4;v++)
				for (u=0;u<4;u++) {
					bits=val&3;
					val>>=2;
					px=(i<<2)+u;
					py=(j<<2)+v;
					if ((px<mx) && (py<my)) {
						C32 compo=clipgetpixel32(m,px,py);
						compo.c32+=color[bits].c32;
						clipputpixel32(m,px,py,compo);
					}
				}
			c+=4;
		}
}

static void rgb24reada(U8 *c,struct bitmap32 *m,U32 rm,U32 gm,U32 bm)
{
	S32 i,j;
	C32* p=m->data;
	if (rm==0xffU && gm==0xff00U && bm==0xff0000U) {
		for (j=0;j<m->size.x;j++) {
			for (i=0;i<m->size.y;i++) {
				p->r=*c++;
				p->g=*c++;
				p->b=*c++;
				++p;
			}
		}
	} else if (bm==0xff && gm==0xff00 && rm==0xff0000) {
		for (j=0;j<m->size.x;j++) {
			for (i=0;i<m->size.y;i++) {
				p->b=*c++;
				p->g=*c++;
				p->r=*c++;
				++p;
			}
		}
	} else
		errorexit("can't work out ddsrgb24 bitmask");
}

static void rgb32reada(U8 *c,struct bitmap32 *m,U32 rm,U32 gm,U32 bm,U32 am)
{
	S32 i,j;
	C32* p=m->data;
/// argb
	if (bm==0xffU && gm==0xff00U && rm==0xff0000U && am==0xff000000U) {
		ddsclipmap=8;
		for (j=0;j<m->size.x;j++) {
			for (i=0;i<m->size.y;i++) {
				p->b=*c++;
				p->g=*c++;
				p->r=*c++;
				p->a=*c++;
				++p;
			}
		}
// xrgb
	} else if (bm==0xff && gm==0xff00 && rm==0xff0000 && am==0) {
		for (j=0;j<m->size.x;j++) {
			for (i=0;i<m->size.y;i++) {
				p->b=*c++;
				p->g=*c++;
				p->r=*c++;
				p->a=0xff;
				++c;
				++p;
			}
		}
/// bgra
	} else if (am==0xff && rm==0xff00 && gm==0xff0000 && bm==0xff000000) {
		ddsclipmap=8;
		for (j=0;j<m->size.x;j++) {
			for (i=0;i<m->size.y;i++) {
				p->a=*c++;
				p->r=*c++;
				p->g=*c++;
				p->b=*c++;
				++p;
			}
		}
/// bgrx
	} else if (am==0 && rm==0xff00 && gm==0xff0000 && bm==0xff000000) {
		for (j=0;j<m->size.x;j++) {
			for (i=0;i<m->size.y;i++) {
				p->a=0xff;
				++c;
				p->r=*c++;
				p->g=*c++;
				p->b=*c++;
				++p;
			}
		}
/// abgr
	}else if (bm==0xff0000U && gm==0xff00U && rm==0xff && am==0xff000000U) {
		ddsclipmap=8;
		for (j=0;j<m->size.x;j++) {
			for (i=0;i<m->size.y;i++) {
				p->r=*c++;
				p->g=*c++;
				p->b=*c++;
				p->a=*c++;
				++p;
			}
		}
// xbgr
	} else if (bm==0xff0000 && gm==0xff00 && rm==0xff && am==0) {
		for (j=0;j<m->size.x;j++) {
			for (i=0;i<m->size.y;i++) {
				p->r=*c++;
				p->g=*c++;
				p->b=*c++;
				p->a=0xff;
				++c;
				++p;
			}
		}
/// rgba
	} else if (am==0xff && rm==0xff000000 && gm==0xff0000 && bm==0xff00) {
		ddsclipmap=8;
		for (j=0;j<m->size.x;j++) {
			for (i=0;i<m->size.y;i++) {
				p->a=*c++;
				p->b=*c++;
				p->g=*c++;
				p->r=*c++;
				++p;
			}
		}
/// rgbx
	} else if (am==0 && rm==0xff000000 && gm==0xff0000 && bm==0xff00) {
		for (j=0;j<m->size.x;j++) {
			for (i=0;i<m->size.y;i++) {
				p->a=0xff;
				++c;
				p->b=*c++;
				p->g=*c++;
				p->r=*c++;
				++p;
			}
		}
	} else
		errorexit("can't work out '%s' ddsrgb32 bitmask argb %x %x %x %x",curname,am,rm,gm,bm);
}

struct bitmap32* gfxread32_dds(const C8* name)
{
	curname=name;
	colorkeyinfo.lasthascolorkey=0;
	FILE *fh;
	perf_start(DDSREADA);
	S32 usesrgb=0;
	U16 *c=0;
	S32 nc;
	DDSURFACEDESC2 surfdesc;
	struct bitmap32 *m;//,*a;
	C8 magic[5];
	ddsclipmap=0;
	fh=fopen2(name,"rb");
	fread(magic,1,4,fh);
	magic[4]='\0';
	if (strcmp("DDS ",magic))
		errorexit("bad magic '%s'",magic);
	fread(&surfdesc,1,sizeof(surfdesc),fh);
	if (surfdesc.dwSize!=sizeof(surfdesc))
		errorexit("bad size for DDSURFACEDESC2 dds reader %d",surfdesc.dwSize);
	if (surfdesc.ddpfPixelFormat.dwFlags&DDPF_RGB)
		usesrgb=1;
	if (!usesrgb) {
		if (!(surfdesc.ddpfPixelFormat.dwFlags&DDPF_FOURCC)) {
			m=bitmap32alloc(surfdesc.dwWidth,surfdesc.dwHeight,C32LIGHTGREEN);
			perf_end(DDSREADA);
			fclose(fh);
			return m;
		} else
			memcpy(magic,&surfdesc.ddpfPixelFormat.dwFourCC,4);
	}
	m=bitmap32alloc(surfdesc.dwWidth,surfdesc.dwHeight,C32BLACK);
	if (!usesrgb) {
		if (!strcmp(magic,"DXT1")) {
			nc=((m->size.x+3)>>2)*((m->size.y+3)>>2)*8;
			c=new U16[nc];
			fread(c,1,nc,fh);
			dxt1reada(c,m);
		} else if (!strcmp(magic,"DXT3")) {
			nc=((m->size.x+3)>>2)*((m->size.y+3)>>2)*16;
			c=new U16[nc];
			fread(c,1,nc,fh);
			dxt3reada(c,m);
		} else if (!strcmp(magic,"DXT5")) {
			nc=((m->size.x+3)>>2)*((m->size.y+3)>>2)*16;
			c=new U16[nc];
			fread(c,1,nc,fh);
			dxt5reada(c,m);
		} else
			errorexit("no dxt1 or dxt1 or dxt5 '%s'",magic);
	} else {
		if (surfdesc.ddpfPixelFormat.dwRGBBitCount==24) {
			nc=m->size.x*m->size.y*3;
			c=new U16[nc];
			fread(c,1,nc,fh);
			rgb24reada((U8 *)c,m,surfdesc.ddpfPixelFormat.dwRBitMask,
				surfdesc.ddpfPixelFormat.dwGBitMask,surfdesc.ddpfPixelFormat.dwBBitMask);
		} else if (surfdesc.ddpfPixelFormat.dwRGBBitCount==32) {
			nc=m->size.x*m->size.y*4;
			c=new U16[nc];
			fread(c,1,nc,fh);
			rgb32reada((U8 *)c,m,surfdesc.ddpfPixelFormat.dwRBitMask,
				surfdesc.ddpfPixelFormat.dwGBitMask,surfdesc.ddpfPixelFormat.dwBBitMask,
				surfdesc.ddpfPixelFormat.dwRGBAlphaBitMask);
		} else
			errorexit("not 24 or 32 bit dds '%d'",surfdesc.ddpfPixelFormat.dwRGBBitCount);
	}
	delete[] c;
	perf_end(DDSREADA);
	fclose(fh);
	if (ddsclipmap>0)
		colorkeyinfo.lasthascolorkey=colorkeyinfo.usecolorkey;
	return m;
}

////////////////////// the following is a study of .dds files
#if 0
#define STRICT
#define WINDOWS_LEAN_AND_MEAN
#include <windows.h>
#include <ddraw.h>
#include <d3d.h>
#include <math.h>
#include <string.h>
#include <stdio.h>

#include <engine2cpp.h>

static DDSURFACEDESC2 surfdesc;
static S32 ndds;

static void tablevel(S32 l)
{
	while(l--)
		logger("  ");
}

static C8 *ddsd_caps_info[32]={
	"DDSD_CAPS",		"DDSD_HEIGHT",			"DDSD_WIDTH",			"DDSD_PITCH",
	"",					"DDSD_BACKBUFFERCOUNT",	"DDSD_ZBUFFERBITDEPTH",	"DDSD_ALPHABITDEPTH",
	"",					"",						"",						"DDSD_LPSURFACE",
	"DDSD_PIXELFORMAT",	"DDSD_CKDESTOVERLAY",	"DDSD_CKDESTBLT",		"DDSD_CKSRCOVERLAY",
	"DDSD_CKSRCBLT",	"DDSD_MIPMAPCOUNT",		"DDSD_REFRESHRATE",		"DDSD_LINEARSIZE",
	"DDSD_TEXTURESTAGE","DDSD_FVF",				"DDSD_SRCVBHANDLE",		"",
	"",					"",						"",						"",
	"",					"",						"",						"",
};
static C8 *ddpf_info[32]={
	"DDPF_ALPHAPIXELS",			"DDPF_ALPHA",			"DDPF_FOURCC",			"DDPF_PALETTEINDEXED4",
	"DDPF_PALETTEINDEXEDTO8",	"DDPF_PALETTEINDEXED8",	"DDPF_RGB",				"DDPF_COMPRESSED",
	"DDPF_RGBTOYUV",			"DDPF_YUV",				"DDPF_ZBUFFER",			"DDPF_PALETTEINDEXED1",
	"DDPF_PALETTEINDEXED2",		"DDPF_ZPIXELS",			"DDPF_STENCILBUFFER",	"DDPF_ALPHAPREMULT",
	"",							"DDPF_LUMINANCE",		"DDPF_BUMPLUMINANCE",	"DDPF_BUMPDUDV",
	"",							"",						"",						"",
	"",							"",						"",						"",
	"",							"",						"",						"",
};
static C8 *ddscaps_info[32]={
	"DDSCAPS_RESERVED1",	"DDSCAPS_ALPHA",			"DDSCAPS_BACKBUFFER",		"DDSCAPS_COMPLEX",
	"DDSCAPS_FLIP",			"DDSCAPS_FRONTBUFFER",		"DDSCAPS_OFFSCREENPLAIN",	"DDSCAPS_OVERLAY",
	"DDSCAPS_PALETTE",		"DDSCAPS_PRIMARYSURFACE",	"DDSCAPS_RESERVED3",		"DDSCAPS_SYSTEMMEMORY",
	"DDSCAPS_TEXTURE",		"DDSCAPS_3DDEVICE",			"DDSCAPS_VIDEOMEMORY",		"DDSCAPS_VISIBLE",
	"DDSCAPS_WRITEONLY",	"DDSCAPS_ZBUFFER",			"DDSCAPS_OWNDC",			"DDSCAPS_LIVEVIDEO",
	"DDSCAPS_HWCODEC",		"DDSCAPS_MODEX",			"DDSCAPS_MIPMAP",			"DDSCAPS_RESERVED2",
	"",						"",							"DDSCAPS_ALLOCONLOAD",		"DDSCAPS_VIDEOPORT",
	"DDSCAPS_LOCALVIDMEM",	"DDSCAPS_NONLOCALVIDMEM",	"DDSCAPS_STANDARDVGAMODE",	"DDSCAPS_OPTIMIZED",
};
static C8 *ddscaps2_info[32]={
	"",								"DDSCAPS2_HARDWAREDEINTERLACE",	"DDSCAPS2_HINTDYNAMIC",		"DDSCAPS2_HINTSTATIC",
	"DDSCAPS2_TEXTUREMANAGE",		"DDSCAPS2_RESERVED1",			"DDSCAPS2_RESERVED2",		"DDSCAPS2_OPAQUE",
	"DDSCAPS2_HINTANTIALIASING",	"DDSCAPS2_CUBEMAP",				"DDSCAPS2_CUBEMAP_POSITIVEX","DDSCAPS2_CUBEMAP_NEGATIVEX",
	"DDSCAPS2_CUBEMAP_POSITIVEY",	"DDSCAPS2_CUBEMAP_NEGATIVEY",	"DDSCAPS2_CUBEMAP_POSITIVEZ","DDSCAPS2_CUBEMAP_NEGATIVEZ",
	"DDSCAPS2_MIPMAPSUBLEVEL",		"DDSCAPS2_D3DTEXTUREMANAGE",	"DDSCAPS2_DONOTPERSIST",	"DDSCAPS2_STEREOSURFACELEFT",
	"",								"",								"",							"",
	"",								"",								"",							"",
	"",								"",								"",							"",
};

static C8 *showflags(U32 flag,C8 *info[])
{
	static C8 flagstr[1024];
	C8 unkbitstr[20];
	S32 i,j;
	flagstr[0]='\0';
	for (i=0,j=1;i<32;i++,j<<=1)
		if (j&flag) {
			if (info[i][0])
				strcat(flagstr,info[i]); 
			else {
				sprintf(unkbitstr,"UNKBIT%d",i);
				strcat(flagstr,unkbitstr);
			}
			strcat(flagstr," ");
		}
	return flagstr;
}

static C8 *showfourcc(U32 fourcc)
{
	static C8 fourccstr[5];
	S32 *fp=(S32 *)fourccstr;
	*fp=fourcc;
	return fourccstr;
}

static void printsurfdesc()
{
	logger("surfdesc.dwSize %d\n",surfdesc.dwSize);
	logger("surfdesc.dwFlags '%s'\n",showflags(surfdesc.dwFlags,ddsd_caps_info));
	logger("surfdesc.dwHeight %d\n",surfdesc.dwHeight);
	logger("surfdesc.dwWidth %d\n",surfdesc.dwWidth);
	logger("surfdesc.lPitch %d\n",surfdesc.lPitch);
	logger("surfdesc.dwBackBufferCount %d\n",surfdesc.dwBackBufferCount);
	logger("surfdesc.dwMipMapCount %d\n",surfdesc.dwMipMapCount);
	logger("surfdesc.dwAlphaBitDepth %d\n",surfdesc.dwAlphaBitDepth);
	logger("surfdesc.dwReserved %d\n",surfdesc.dwReserved);
	logger("surfdesc.lpSurface $%08X\n",surfdesc.lpSurface);
    logger("surfdesc.ddckCKDestOverlay.dwColorSpaceLowValue $%08X\n",surfdesc.ddckCKDestOverlay.dwColorSpaceLowValue);
	logger("surfdesc.ddckCKDestOverlay.dwColorSpaceHighValue $%08X\n",surfdesc.ddckCKDestOverlay.dwColorSpaceHighValue);
	logger("surfdesc.ddckCKDestBlt.dwColorSpaceLowValue $%08X\n",surfdesc.ddckCKDestBlt.dwColorSpaceLowValue);
	logger("surfdesc.ddckCKDestBlt.dwColorSpaceHighValue $%08X\n",surfdesc.ddckCKDestBlt.dwColorSpaceHighValue);
	logger("surfdesc.ddckCKSrcOverlay.dwColorSpaceLowValue $%08X\n",surfdesc.ddckCKSrcOverlay.dwColorSpaceLowValue);
	logger("surfdesc.ddckCKSrcOverlay.dwColorSpaceHighValue $%08X\n",surfdesc.ddckCKSrcOverlay.dwColorSpaceHighValue);
	logger("surfdesc.ddckCKSrcBlt.dwColorSpaceLowValue $%08X\n",surfdesc.ddckCKSrcBlt.dwColorSpaceLowValue);
	logger("surfdesc.ddckCKSrcBlt.dwColorSpaceHighValue $%08X\n",surfdesc.ddckCKSrcBlt.dwColorSpaceHighValue);
	logger("surfdesc.ddpfPixelFormat.dwSize %d\n",surfdesc.ddpfPixelFormat.dwSize);
	logger("surfdesc.ddpfPixelFormat.dwFlags '%s'\n",showflags(surfdesc.ddpfPixelFormat.dwFlags,ddpf_info));
	logger("surfdesc.ddpfPixelFormat.dwFourCC '%s'\n",showfourcc(surfdesc.ddpfPixelFormat.dwFourCC));
	logger("surfdesc.ddpfPixelFormat.dwRGBBitCount %d\n",surfdesc.ddpfPixelFormat.dwRGBBitCount);
	logger("surfdesc.ddpfPixelFormat.dwRBitMask $%08X\n",surfdesc.ddpfPixelFormat.dwRBitMask);
	logger("surfdesc.ddpfPixelFormat.dwGBitMask $%08X\n",surfdesc.ddpfPixelFormat.dwGBitMask);
	logger("surfdesc.ddpfPixelFormat.dwBBitMask $%08X\n",surfdesc.ddpfPixelFormat.dwBBitMask);
	logger("surfdesc.ddpfPixelFormat.dwRGBAlphaBitMask $%08X\n",surfdesc.ddpfPixelFormat.dwRGBAlphaBitMask);
	logger("surfdesc.ddsCaps.dwCaps '%s'\n",showflags(surfdesc.ddsCaps.dwCaps,ddscaps_info));
	logger("surfdesc.ddsCaps.dwCaps2 '%s'\n",showflags(surfdesc.ddsCaps.dwCaps2,ddscaps2_info));
	logger("surfdesc.ddsCaps.dwCaps3 $%08X\n",surfdesc.ddsCaps.dwCaps3);
	logger("surfdesc.ddsCaps.dwCaps4 $%08X\n",surfdesc.ddsCaps.dwCaps4);
	logger("surfdesc.dwTextureStage %d\n",surfdesc.dwTextureStage);
}

void readdds(C8 *name,S32 level)
{
	C8 magic[5];
	S32 size;
	magic[4]='\0';
	logger("--------------------------------- '%s' -----------------------------------------------------\n",name);
	fileopen(name,READ);
	size=fileskip(0,FILE_END);
	fileskip(0,FILE_START);
	fileread(magic,4);
	if (strcmp("DDS ",magic))
		errorexit("bad magic '%s'",magic);
	fileread(&surfdesc,sizeof(surfdesc));
	if (surfdesc.dwSize!=sizeof(surfdesc))
		errorexit("bad size for DDSURFACEDESC2 %d",surfdesc.dwSize);
	fileclose();
	printsurfdesc();
	logger("size %d\n",size);
	ndds++;
}

static void recursedir(C8 *dir)
{
	C8 cdir[250];
	C8 ext[200];
	static S32 level;
	S32 i;
	C8 **sc;
	S32 nsc;
	pushandsetdir(dir);
	sc=doadir(&nsc,1);
	for (i=0;i<nsc;i++) {
		if (sc[i][0]=='*') {
			tablevel(level);
			logger("'%s'\n",sc[i]);
			level++;
			strcpy(cdir,dir);
			strcat(cdir,"\\");
			strcat(cdir,&sc[i][1]);
			recursedir(cdir);
			level--;
		} else {
			getext(ext,sc[i]);
			if (!my_stricmp(ext,".dds")) {
				readdds(sc[i],level);
			}
		}
	}
	freescript(sc,nsc);
	popdir();
}

void ddstest_init()
{
	video_setupwindow(640,480,565);
	ndds=0;
	logger("directory of 'data'\n");
	recursedir("c:\\tempdata\\techdemo530\\data");
	logger("%d dds files found\n",ndds);
}

void ddstest_proc()
{
	if (KEY==K_ESCAPE)
		popstate();
	video_lock();
	cliprect16(B16,0,0,WX-1,WY-1,hilightblue);
	video_unlock();
}

void ddstest_exit()
{
}

#endif
