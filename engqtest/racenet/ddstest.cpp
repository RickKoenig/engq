#define STRICT
#define WINDOWS_LEAN_AND_MEAN
#include <windows.h>
#include <ddraw.h>
#include <d3d.h>
#include <math.h>
#include <string.h>
#include <stdio.h>

#include <engine1.h>

static DDSURFACEDESC2 surfdesc;
static int ndds;

static void tablevel(int l)
{
	while(l--)
		logger("  ");
}

static char *ddsd_caps_info[32]={
	"DDSD_CAPS",		"DDSD_HEIGHT",			"DDSD_WIDTH",			"DDSD_PITCH",
	"",					"DDSD_BACKBUFFERCOUNT",	"DDSD_ZBUFFERBITDEPTH",	"DDSD_ALPHABITDEPTH",
	"",					"",						"",						"DDSD_LPSURFACE",
	"DDSD_PIXELFORMAT",	"DDSD_CKDESTOVERLAY",	"DDSD_CKDESTBLT",		"DDSD_CKSRCOVERLAY",
	"DDSD_CKSRCBLT",	"DDSD_MIPMAPCOUNT",		"DDSD_REFRESHRATE",		"DDSD_LINEARSIZE",
	"DDSD_TEXTURESTAGE","DDSD_FVF",				"DDSD_SRCVBHANDLE",		"",
	"",					"",						"",						"",
	"",					"",						"",						"",
};
static char *ddpf_info[32]={
	"DDPF_ALPHAPIXELS",			"DDPF_ALPHA",			"DDPF_FOURCC",			"DDPF_PALETTEINDEXED4",
	"DDPF_PALETTEINDEXEDTO8",	"DDPF_PALETTEINDEXED8",	"DDPF_RGB",				"DDPF_COMPRESSED",
	"DDPF_RGBTOYUV",			"DDPF_YUV",				"DDPF_ZBUFFER",			"DDPF_PALETTEINDEXED1",
	"DDPF_PALETTEINDEXED2",		"DDPF_ZPIXELS",			"DDPF_STENCILBUFFER",	"DDPF_ALPHAPREMULT",
	"",							"DDPF_LUMINANCE",		"DDPF_BUMPLUMINANCE",	"DDPF_BUMPDUDV",
	"",							"",						"",						"",
	"",							"",						"",						"",
	"",							"",						"",						"",
};
static char *ddscaps_info[32]={
	"DDSCAPS_RESERVED1",	"DDSCAPS_ALPHA",			"DDSCAPS_BACKBUFFER",		"DDSCAPS_COMPLEX",
	"DDSCAPS_FLIP",			"DDSCAPS_FRONTBUFFER",		"DDSCAPS_OFFSCREENPLAIN",	"DDSCAPS_OVERLAY",
	"DDSCAPS_PALETTE",		"DDSCAPS_PRIMARYSURFACE",	"DDSCAPS_RESERVED3",		"DDSCAPS_SYSTEMMEMORY",
	"DDSCAPS_TEXTURE",		"DDSCAPS_3DDEVICE",			"DDSCAPS_VIDEOMEMORY",		"DDSCAPS_VISIBLE",
	"DDSCAPS_WRITEONLY",	"DDSCAPS_ZBUFFER",			"DDSCAPS_OWNDC",			"DDSCAPS_LIVEVIDEO",
	"DDSCAPS_HWCODEC",		"DDSCAPS_MODEX",			"DDSCAPS_MIPMAP",			"DDSCAPS_RESERVED2",
	"",						"",							"DDSCAPS_ALLOCONLOAD",		"DDSCAPS_VIDEOPORT",
	"DDSCAPS_LOCALVIDMEM",	"DDSCAPS_NONLOCALVIDMEM",	"DDSCAPS_STANDARDVGAMODE",	"DDSCAPS_OPTIMIZED",
};
static char *ddscaps2_info[32]={
	"",								"DDSCAPS2_HARDWAREDEINTERLACE",	"DDSCAPS2_HINTDYNAMIC",		"DDSCAPS2_HINTSTATIC",
	"DDSCAPS2_TEXTUREMANAGE",		"DDSCAPS2_RESERVED1",			"DDSCAPS2_RESERVED2",		"DDSCAPS2_OPAQUE",
	"DDSCAPS2_HINTANTIALIASING",	"DDSCAPS2_CUBEMAP",				"DDSCAPS2_CUBEMAP_POSITIVEX","DDSCAPS2_CUBEMAP_NEGATIVEX",
	"DDSCAPS2_CUBEMAP_POSITIVEY",	"DDSCAPS2_CUBEMAP_NEGATIVEY",	"DDSCAPS2_CUBEMAP_POSITIVEZ","DDSCAPS2_CUBEMAP_NEGATIVEZ",
	"DDSCAPS2_MIPMAPSUBLEVEL",		"DDSCAPS2_D3DTEXTUREMANAGE",	"DDSCAPS2_DONOTPERSIST",	"DDSCAPS2_STEREOSURFACELEFT",
	"",								"",								"",							"",
	"",								"",								"",							"",
	"",								"",								"",							"",
};

static char *showflags(unsigned int flag,char *info[])
{
	static char flagstr[1024];
	char unkbitstr[20];
	int i,j;
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

static char *showfourcc(unsigned int fourcc)
{
	static char fourccstr[5];
	int *fp=(int *)fourccstr;
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

void readdds(char *name,int level)
{
	char magic[5];
	int size;
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

static void recursedir(char *dir)
{
	char cdir[250];
	char ext[200];
	static int level;
	int i;
	char **sc;
	int nsc;
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

