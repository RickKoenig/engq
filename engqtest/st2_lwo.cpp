#include <math.h>
#include <string.h>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>

#include "engine1.h"
#include "perf.h"

static struct model *themodel;
static int thematidx;
static struct pointf3 tsiz[MAXGROUPS],tctr[MAXGROUPS],tvel[MAXGROUPS];
static int hastvel[MAXGROUPS];
static float tfp0[MAXGROUPS],tfp1[MAXGROUPS];
static float sman[MAXGROUPS];
static int theaxis;
static struct pointf3 *newverts;
static struct face *newfaces;
static struct uv *newuvs;
static struct pointf3 *newcverts;
static int nnewvert=0;
static int nnewface=0;
static int inctex,inttex;
static int wrapwidth[MAXGROUPS],wrapheight[MAXGROUPS];

//int usecvertsfromuv;
struct uvrgb {
	float u,v,r,g,b;
};

static struct uvrgb *uvrgbs;



struct chunktab {
	char *chunkid;
	void (*chunkfunc)(int);
};

///////// sub chunks ///////////////////
static void dosubchunkcolr(int len)
{
	struct rgb24 col;
	col.r=filereadbyte();
	col.g=filereadbyte();
	col.b=filereadbyte();
//	logger("colr: (%3d,%3d,%3d)\n",col.r,col.g,col.b);
	themodel->mats[thematidx].color=rgb2float(col);
}

#define FLAG_MASK 0x7ff
#define FLAG_LUMINOUS 1
#define FLAG_OUTLINE 2
#define FLAG_SMOOTHING 4
#define FLAG_COLORHIGHLIGHTS 8
#define FLAG_COLORFILTER 16
#define FLAG_OPAQUEEDGE 32
#define FLAG_TRANSPARENTEDGE 64
#define FLAG_SHARPTERMINATOR 128
#define FLAG_DOUBLESIDED 256
#define FLAG_ADDITIVE 512
#define FLAG_SHADOWALPHA 1024
static char *flagstrs[]={
	"LUMINOUS",
	"OUTLINE",
	"SMOOTHING",
	"COLORHIGHLIGHTS",
	"COLORFILTER",
	"OPAQUEEDGE",
	"TRANSPARENTEDGE",
	"SHARPTERMINATOR",
	"DOUBLESIDED",
	"ADDITIVE",
	"SHADOWALPHA",
};
#define NUMFLAGSTRS (sizeof(flagstrs)/sizeof(flagstrs[0]))

static void dosubchunkflag(int len)
{
	int flag;
//	int i;
	flag=filereadword();
//	logger("flag: $%04x\n",flag);
//	for (i=0;i<NUMFLAGSTRS;i++)
//		if (flag&(1<<i))
//			logger("flag bits: '%s'\n",flagstrs[i]);
	themodel->mats[thematidx].msflags=flag;
	if (!(flag&FLAG_SMOOTHING)) // set no smoothing set to max smoothing...
		sman[thematidx]=PI;
	if (flag&FLAG_LUMINOUS)
		uselights=0;
}

#define TFLAG_SHIFTER 11
#define TFLAG_MASK 0x7f
#define AXIS_MASK 7
#define TFLAG_XAXIS 1
#define TFLAG_YAXIS 2
#define TFLAG_ZAXIS 4
#define TFLAG_WORLDCOORDS 8
#define TFLAG_NEGATIVEIMAGE 16
#define TFLAG_PIXELBLENDING 32
#define TFLAG_ANTIALIAS 64
static char *tflgstrs[]={
	"XAXIS",
	"YAXIS",
	"ZAXIS",
	"WORLDCOORDS",
	"NEGATIVEIMAGE",
	"PIXELBLENDING",
	"ANTIALIAS",
};
#define NUMTFLGSTRS (sizeof(tflgstrs)/sizeof(tflgstrs[0]))

static void dosubchunktflg(int len)
{
	int flag;
//	int i;
	flag=filereadword();
	if (inttex && (flag&16)) {
//		logger("negative alpha flag set\n");
		textmannegalpha=1;
	}
	if (!inctex)
		return;
//	logger("tflg: $%04x\n",flag);
//	for (i=0;i<NUMTFLGSTRS;i++)
//		if (flag&(1<<i))
//			logger("tflg bits: '%s'\n",tflgstrs[i]);
	themodel->mats[thematidx].msflags|=(flag<<TFLAG_SHIFTER);
}

static void dosubchunkvtrn(int len)
{
	float p;
	int *pp=(int *)&p;
	pp[0]=filereadlong();
	themodel->mats[thematidx].mtrans=1-p;
}

static void dosubchunkvlum(int len)
{
	uselights=0;
}

static void dosubchunkvdif(int len)
{
	float p;
	int *pp=(int *)&p;
	pp[0]=filereadlong();
//	logger("vdif: %f\n",p);
}

// doc says it's degrees but it really is radians ?!
static void dosubchunksman(int len)
{
	float p;
	int *pp=(int *)&p;
	pp[0]=filereadlong();
	if ((themodel->mats[thematidx].msflags&FLAG_SMOOTHING)) // set no smoothing set to max smoothing...
		sman[thematidx]=p;
//	sman[thematidx]=p*PIOVER180;
//	logger("sman: %f\n",sman[thematidx]);
}

static void dosubchunktfp0(int len)
{
	float p;
	int *pp=(int *)&p;
	if (!inctex)
		return;
	pp[0]=filereadlong();
	tfp0[thematidx]=p;
//	logger("tfp0: %f\n",tfp0[thematidx]);
}

static void dosubchunktfp1(int len)
{
	float p;
	int *pp=(int *)&p;
	if (!inctex)
		return;
	pp[0]=filereadlong();
	tfp1[thematidx]=p;
//	logger("tfp1: %f\n",tfp1[thematidx]);
}

static void dosubchunktsiz(int len)
{
	struct pointf3 p;
	int *pp=(int *)&p;
	if (!inctex)
		return;
	pp[0]=filereadlong();
	pp[1]=filereadlong();
	pp[2]=filereadlong();
//	logger("tsiz:  %9.2f %9.2f %9.2f\n",p.x,p.y,p.z);
	tsiz[thematidx]=p;
}

static void dosubchunktvel(int len)
{
	struct pointf3 p;
	int *pp=(int *)&p;
	if (!inctex)
		return;
	pp[0]=filereadlong();
	pp[1]=filereadlong();
	pp[2]=filereadlong();
//	logger("tsiz:  %9.2f %9.2f %9.2f\n",p.x,p.y,p.z);
	tvel[thematidx]=p;
	hastvel[thematidx]=1;
}

static void dosubchunktctr(int len)
{
	struct pointf3 p;
	int *pp=(int *)&p;
	if (!inctex)
		return;
	pp[0]=filereadlong();
	pp[1]=filereadlong();
	pp[2]=filereadlong();
//	logger("tctr:  %9.2f %9.2f %9.2f\n",p.x,p.y,p.z);
	tctr[thematidx]=p;
}

static char *twrpstr[4]={
	"BLACK",
	"CLAMP",
	"REPEAT", // default
	"MIRROR",
};

static void dosubchunktwrp(int len)
{
	if (!inctex)
		return;
	wrapwidth[thematidx]=filereadword();
	wrapheight[thematidx]=filereadword();
//	logger("twrp: width '%s', height '%s'\n",twrpstr[wid],twrpstr[hit]);
}

static char timgname[300];
static char talphaname[300];

static void dosubchunktimg(int len)
{
	char timgfullname[300];
	char c;
	int curstrlen=0;
	if (!inctex && !inttex)
		return;
	while(1) {
		c=filereadbyte();
		timgfullname[curstrlen++]=c;
		if (!c)
			break;
	}
	if (inctex && (strcmp("(none)",timgfullname) || !timgname[0]))
		getnameext(timgname,timgfullname);
	if (inttex && (strcmp("(none)",timgfullname) || !talphaname[0]))
		getnameext(talphaname,timgfullname);
//	logger("timg: '%s'\n",timgname);
//	themodel->mats[thematidx].thetex=loadfindtexture(timgname,NULL,0);
}

#define UV_MASK 15
#define UV_SHIFTER 18
static char *uvtypes[]={
	"Planar Image Map",
	"Cylindrical Image Map",
	"Spherical Image Map",
};
#define UVPLANAR 0
#define UVCYLINDRICAL 1
#define UVSPHERICAL 2

#define NUMUVTYPES (sizeof(uvtypes)/sizeof(uvtypes[0]))

static void dosubchunknoctex(int len)
{
	inctex=0;
	inttex=0;
}

static void dosubchunkctex(int len)
{
	char ctexname[300];
	char c;
	int i;
	int curstrlen=0;
	inctex=1;
	inttex=0;
	tctr[thematidx]=zerov;
	tsiz[thematidx]=onev;
	hastvel[thematidx]=0;
	wrapwidth[thematidx]=wrapheight[thematidx]=2;	// repeat default
	while(1) {
		c=filereadbyte();
		ctexname[curstrlen++]=c;
		if (!c)
			break;
	}
	for (i=0;i<NUMUVTYPES;i++)
		if (!strcmp(ctexname,uvtypes[i]))
			break;
	if (i==NUMUVTYPES)
		errorexit("unknown uvmapper '%s'",ctexname);
	themodel->mats[thematidx].msflags|=(i<<UV_SHIFTER);
}

static void dosubchunkttex(int len)
{
	inctex=0;
	inttex=1;
}

static struct chunktab subchunks[]={
// main subchunks
	"COLR",dosubchunkcolr,
	"FLAG",dosubchunkflag,
	"VDIF",dosubchunkvdif,
	"SMAN",dosubchunksman,
	"VTRN",dosubchunkvtrn,
	"VLUM",dosubchunkvlum,
// texture selector
	"CTEX",dosubchunkctex,
	"DTEX",dosubchunknoctex,
	"STEX",dosubchunknoctex,
	"RTEX",dosubchunknoctex,
	"TTEX",dosubchunkttex,
	"LTEX",dosubchunknoctex,
	"BTEX",dosubchunknoctex,
// texture subchunks
	"TIMG",dosubchunktimg,
	"TWRP",dosubchunktwrp,
	"TFLG",dosubchunktflg,
	"TSIZ",dosubchunktsiz,
	"TVEL",dosubchunktvel,
	"TCTR",dosubchunktctr,
	"TFP0",dosubchunktfp0,
	"TFP1",dosubchunktfp1,
};
#define NUMSUBCHUNKS (sizeof(subchunks)/sizeof(subchunks[0]))
//  "DIFF",dosubchunkdiff,
//	"TAAS",dosubchunktass,
//	"TCLR",dosubchunktclr,

//////// chunks ///////////////
static void dochunkpnts(int len)
{
	int i;
	struct pointf3 p;
	int *pp=(int *)&p;
	int npnts=len/12;
//	logger("%d points\n",npnts);
	if (npnts>MAXVERTS)
		errorexit("lwo: too many verts");
	themodel->verts=(struct pointf3 *)memalloc(sizeof(struct pointf3)*npnts);
	for (i=0;i<npnts;i++) {
		pp[0]=filereadlong();
		pp[1]=filereadlong();
		pp[2]=filereadlong();
		themodel->verts[i]=p;
//		logger("point %5d: %9.2f %9.2f %9.2f\n",i,p.x,p.y,p.z);
	}
	themodel->nvert=npnts;
}

static void dochunksrfs(int len)
{
	char surfname[200];
	char c;
	int nsrfs=0;
	int curstrlen=0;
	themodel->mats=memzalloc(MAXGROUPS*sizeof(struct mat));
	while(len>0) {
		c=filereadbyte();
		len--;
		surfname[curstrlen++]=c;
		if (!c) {
			if (curstrlen&1) {
				fileskip(1,FILE_CURRENT);
				len--;
			}
			curstrlen=0;
//			logger("srfs %3d: '%s'\n",nsrfs,surfname);
			if (nsrfs>=MAXGROUPS)
				errorexit("lwo: too many material surfaces");
			mystrncpy(themodel->mats[nsrfs].name,surfname,NAMESIZE);
			nsrfs++;
		}
	}
//	logger("nsrfs = %d\n",nsrfs);
	themodel->mats=memrealloc(themodel->mats,nsrfs*sizeof(struct mat));
	themodel->nmat=nsrfs;
	themodel->groups=memalloc(nsrfs*sizeof(struct group));
//	themodel->ngroup=nsrfs;
}

static void dochunkpols(int len)
{
	int n,v0,v1,v2,m;
	int npols=0;
	themodel->faces=(struct face *)memalloc(MAXFACES*sizeof(struct face));
//	themodel->facenorms=(struct pointf3 *)memalloc(MAXFACES*sizeof(struct pointf3));
	while(len>0) {
		n=filereadword();
		if (n!=3)
			errorexit("can't handle non-triangle pols");
		v0=filereadword();
		v1=filereadword();
		v2=filereadword();
		m=filereadword();
		len-=10;
		if (m<1) 
			errorexit("can't handle detail pols");
		m--;
//		logger("pols %5d: vertidx (%4d %4d %4d), matidx %3d\n",npols,v0,v1,v2,m);
		if (npols>=MAXFACES)
			errorexit("lwo: too many faces");
		themodel->faces[npols].vertidx[0]=v0;
		themodel->faces[npols].vertidx[1]=v1;
		themodel->faces[npols].vertidx[2]=v2;
		themodel->faces[npols].matidx=m;
		npols++;
	}
//	logger("npols = %d\n",npols);
	themodel->faces=(struct face *)memrealloc(themodel->faces,npols*sizeof(struct face));
//	if (themodel->facenorms)
//		themodel->facenorms=(struct pointf3 *)memrealloc(themodel->facenorms,npols*sizeof(struct pointf3));
	themodel->nface=npols;
}

static void dochunksurf(int len)
{
	char surfname[200];
	char c;
	int i,subchunksave,subchunklen;
	static char subchunkid[5];
	int curstrlen=0;
	timgname[0]='\0';
	talphaname[0]='\0';
	while(1) {
		c=filereadbyte();
		len--;
		surfname[curstrlen++]=c;
		if (!c) {
			if (curstrlen&1) {
				fileskip(1,FILE_CURRENT);
				len--;
			}
			break;
		}
	}
	surfname[NAMESIZE-1]='\0';
//	logger("surf: '%s'\n",surfname);
	for (i=0;i<themodel->nmat;i++)
		if (!strcmp(themodel->mats[i].name,surfname))
			break;
	if (i==themodel->nmat)
		errorexit("mat '%s' not found",surfname);
	thematidx=i;
	themodel->mats[thematidx].mtrans=1;
	sman[thematidx]=0;
	inctex=0;
	while(len>0) {
		fileread(&subchunkid,4);
		subchunklen=filereadword();
		len-=6;
		len-=subchunklen;
//		logger("-- sub chunk name '%s' len %2d, subformleft %4d\n",subchunkid,subchunklen,len);
		subchunksave=fileskip(0,FILE_CURRENT);
		for (i=0;i<NUMSUBCHUNKS;i++) {
			if (!strcmp(subchunks[i].chunkid,subchunkid))
				(*subchunks[i].chunkfunc)(subchunklen);
		}
		fileskip(subchunksave+subchunklen,FILE_START);
		if (subchunklen&1) {
			fileskip(1,FILE_CURRENT);
			len--;
		}

	}
	if (timgname[0]) {
		themodel->mats[thematidx].thetex=loadfindtexture(timgname,talphaname,0);
		initifls(themodel,&themodel->mats[thematidx]);
	}
	textmannegalpha=0;
}

static struct chunktab chunks[]={
	"PNTS",dochunkpnts,
	"SRFS",dochunksrfs,
	"POLS",dochunkpols,
	"SURF",dochunksurf,
};
#define NUMCHUNKS (sizeof(chunks)/sizeof(chunks[0]))

static void uvplanar(int matidx)
{
	int i;
	int st,end;
	float mu,bu,mv,bv;
//	logger("uvmap: uvplanar '%s' and matidx %d\n",tflgstrs[ilog2(theaxis)],matidx);
	st=themodel->groups[matidx].vertidx;
	end=st+themodel->groups[matidx].nvert;
	if (theaxis==TFLAG_XAXIS) {
		tvel[matidx].x=0;
		mu=1.0f/tsiz[matidx].z;
		bu=.5f-mu*tctr[matidx].z;
		mv=-1.0f/tsiz[matidx].y;
		bv=.5f-mv*tctr[matidx].y;
		for (i=st;i<end;i++) {
			themodel->uvs[0][i].u=mu*newverts[i].z+bu;
			themodel->uvs[0][i].v=mv*newverts[i].y+bv;
//			themodel->uvs[0][i].u=mu*themodel->verts[i].z+bu;
//			themodel->uvs[0][i].v=mv*themodel->verts[i].y+bv;
		}
	} else if (theaxis==TFLAG_YAXIS) {
		tvel[matidx].y=0;
		mu=1.0f/tsiz[matidx].x;
		bu=.5f-mu*tctr[matidx].x;
		mv=-1.0f/tsiz[matidx].z;
		bv=.5f-mv*tctr[matidx].z;
		for (i=st;i<end;i++) {
			themodel->uvs[0][i].u=mu*newverts[i].x+bu;
			themodel->uvs[0][i].v=mv*newverts[i].z+bv;
//			themodel->uvs[0][i].u=mu*themodel->verts[i].x+bu;
//			themodel->uvs[0][i].v=mv*themodel->verts[i].z+bv;
		}
	} else { // z
		tvel[matidx].z=0;
		mu=1.0f/tsiz[matidx].x;
		bu=.5f-mu*tctr[matidx].x;
		mv=-1.0f/tsiz[matidx].y;
		bv=.5f-mv*tctr[matidx].y;
		for (i=st;i<end;i++) {
			themodel->uvs[0][i].u=mu*newverts[i].x+bu;
			themodel->uvs[0][i].v=mv*newverts[i].y+bv;
//			themodel->uvs[0][i].u=mu*themodel->verts[i].x+bu;
//			themodel->uvs[0][i].v=mv*themodel->verts[i].y+bv;
		}
	}
}

static void uvcylindrical(int matidx)
{
	int i;
	int st,end;
	float mv,bv;
	float wrp;
//	logger("uvmap: uvcylindrical '%s' and matidx %d\n",tflgstrs[ilog2(theaxis)],matidx);
	st=themodel->groups[matidx].vertidx;
	end=st+themodel->groups[matidx].nvert;
	wrp=1.0f/TWOPI;
	if (theaxis==TFLAG_XAXIS) {
		tvel[matidx].x=0;
		mv=-1.0f/tsiz[matidx].x;
		bv=.5f-mv*tctr[matidx].x;
		for (i=st;i<end;i++) {
			themodel->uvs[0][i].u=
				wrp*ratan2(newverts[i].z-tctr[matidx].z,-(newverts[i].y-tctr[matidx].y));
			themodel->uvs[0][i].v=mv*newverts[i].x+bv;
		}
	} else if (theaxis==TFLAG_YAXIS) {
		tvel[matidx].y=0;
		mv=-1.0f/tsiz[matidx].y;
		bv=.5f-mv*tctr[matidx].y;
		for (i=st;i<end;i++) {
			themodel->uvs[0][i].u=
				wrp*ratan2(-(newverts[i].x-tctr[matidx].x),newverts[i].z-tctr[matidx].z);
			themodel->uvs[0][i].v=mv*newverts[i].y+bv;
		}
	} else { // z
		tvel[matidx].z=0;
		mv=-1.0f/tsiz[matidx].z;
		bv=.5f-mv*tctr[matidx].z;
		for (i=st;i<end;i++) {
			themodel->uvs[0][i].u=
				wrp*ratan2(-(newverts[i].x-tctr[matidx].x),-(newverts[i].y-tctr[matidx].y));
			themodel->uvs[0][i].v=mv*newverts[i].z+bv;
		}
	}
}

static void uvspherical(int matidx)
{
	int i;
	int st,end;
	float wrp,wrp2,r;
//	logger("uvmap: uvspherical '%s' and matidx %d\n",tflgstrs[ilog2(theaxis)],matidx);
	st=themodel->groups[matidx].vertidx;
	end=st+themodel->groups[matidx].nvert;
	wrp=1.0f/TWOPI;
	wrp2=tfp1[matidx]/PI;
	if (theaxis==TFLAG_XAXIS) {
		tvel[matidx].x=0;
		for (i=st;i<end;i++) {
			themodel->uvs[0][i].u=
				wrp*ratan2(newverts[i].z-tctr[matidx].z,-(newverts[i].y-tctr[matidx].y));
			r=(newverts[i].y-tctr[matidx].y)*(newverts[i].y-tctr[matidx].y)+
				(newverts[i].z-tctr[matidx].z)*(newverts[i].z-tctr[matidx].z);
			r=(float)sqrt((double)(r));
			themodel->uvs[0][i].v=wrp2*ratan2(r,newverts[i].x-tctr[matidx].x);
		}
	} else if (theaxis==TFLAG_YAXIS) {
		tvel[matidx].y=0;
		for (i=st;i<end;i++) {
			themodel->uvs[0][i].u=
				wrp*ratan2(-(newverts[i].x-tctr[matidx].x),newverts[i].z-tctr[matidx].z);
			r=(newverts[i].x-tctr[matidx].x)*(newverts[i].x-tctr[matidx].x)+
				(newverts[i].z-tctr[matidx].z)*(newverts[i].z-tctr[matidx].z);
			r=(float)sqrt((double)(r));
			themodel->uvs[0][i].v=wrp2*ratan2(r,newverts[i].y-tctr[matidx].y);
		}
	} else { // z
		tvel[matidx].z=0;
		for (i=st;i<end;i++) {
			themodel->uvs[0][i].u=
				wrp*ratan2(-(newverts[i].x-tctr[matidx].x),-(newverts[i].y-tctr[matidx].y));
			r=(newverts[i].x-tctr[matidx].x)*(newverts[i].x-tctr[matidx].x)+
				(newverts[i].y-tctr[matidx].y)*(newverts[i].y-tctr[matidx].y);
			r=(float)sqrt((double)(r));
			themodel->uvs[0][i].v=wrp2*ratan2(r,newverts[i].z-tctr[matidx].z);
		}
	}
}

// fixes wrap problem with cyl or sph uv mapping
static void fixuvwrap(struct model *m,int matidx)
{
	float wrp;
#define UV_NORMAL 1
#define UV_CROSS 2
	unsigned char *crossverts;
	unsigned char *crossfaces;
	int *newvidx;
	int i,k,vi;
	int stv,endv,stf,endf;
	float minu,maxu,u;
	stv=themodel->groups[matidx].vertidx;
	endv=stv+themodel->groups[matidx].nvert;
	stf=themodel->groups[matidx].faceidx;
	endf=stf+themodel->groups[matidx].nface;
	wrp=tfp0[matidx];
	crossverts=memzalloc(2*(endv-stv));
	crossfaces=memzalloc(2*(endf-stf));
	newvidx=(int *)memalloc(sizeof(int)*(endv-stv));
	for (i=stf;i<endf;i++) {
		vi=newfaces[i].vertidx[0];
		minu=maxu=m->uvs[0][vi].u;
		for (k=1;k<3;k++) {
			vi=newfaces[i].vertidx[k];
			u=m->uvs[0][vi].u;
			if (u>maxu)
				maxu=u;
			if (u<minu)
				minu=u;
		}
		if (minu+.5f<maxu) { // find faces with wrap problem
			crossfaces[i-stf]=UV_CROSS; // mark them
			for (k=0;k<3;k++) {
				vi=newfaces[i].vertidx[k];
				u=m->uvs[0][vi].u;
				if (u<.5f)
					crossverts[vi-stv]|=UV_CROSS; // mark verts with wrap prob
				else
					crossverts[vi-stv]|=UV_NORMAL;
			}
		} else {
			crossfaces[i-stf]=UV_NORMAL;
			for (k=0;k<3;k++) {
				vi=newfaces[i].vertidx[k];
				crossverts[vi-stv]|=UV_NORMAL;
			}
		}
	}
	for (i=stv;i<endv;i++) {
		if (crossverts[i-stv]==(UV_CROSS|UV_NORMAL)) { // if a vert need it both ways, make a new vert
			if (nnewvert>=MAXVERTS)
				errorexit("uvmap: too many verts");
			newverts[nnewvert]=newverts[i];
			m->uvs[0][nnewvert].u=m->uvs[0][i].u+1.0f; // fixed up u
			m->uvs[0][nnewvert].v=m->uvs[0][i].v;
			newvidx[i-stv]=nnewvert; // remember how to get to the new vert
			nnewvert++;
		} else if (crossverts[i-stv]==UV_CROSS) {
			m->uvs[0][i].u+=1.0f; // fixed up u
			newvidx[i-stv]=i; // remember how to get to the new vert
		} else
			newvidx[i-stv]=i;
	}
	for (i=stf;i<endf;i++) {
		if (crossfaces[i-stf]==UV_CROSS) { // fixup faces with cross set in them
			for (k=0;k<3;k++) {
				vi=newfaces[i].vertidx[k];
				u=m->uvs[0][vi].u;
				if (u<.5f)
					newfaces[i].vertidx[k]=newvidx[vi-stv];
			}
		}
	}
	for (i=stv;i<nnewvert;i++)
		m->uvs[0][i].u*=wrp; // finally add wrap amount
	memfree(crossfaces);
	memfree(crossverts);
	memfree(newvidx);
}

static void builduvs(struct model *m,int i)
{
	int f;
	if (m->mats[i].thetex) {
		f=UV_MASK&(m->mats[i].msflags>>UV_SHIFTER);
		theaxis=AXIS_MASK&(m->mats[i].msflags>>TFLAG_SHIFTER);
		switch(f) {
		case UVPLANAR:
			uvplanar(i);
			break;
		case UVCYLINDRICAL:
			uvcylindrical(i);
			fixuvwrap(m,i);
			break;
		case UVSPHERICAL:
			uvspherical(i);
			fixuvwrap(m,i);
			break;
		}
	}
}

// at this point you've got only verts, and faces with matidx set...
// gonna build uvs and groups
static void optimizelwomodel(struct model *m)
{
	int i,j,k,vi,mt,sc;
	perf_start(LWOOPT);
	nnewvert=0;
	nnewface=0;
	newfaces=(struct face *)memalloc(sizeof(*newfaces)*2*m->nface); // allow for double sided
	newverts=(struct pointf3 *)memalloc(sizeof(*newverts)*6*m->nface); // allow for cylindrical
// see if we've got any textures in the model, if so we need uvs
	for (i=0;i<m->nmat;i++)
		if (m->mats[i].thetex) {
			m->uvs[0]=(struct uv *)memzalloc(m->nface*6*sizeof(struct uv));
			break;
		}
// do each material
		for (i=0;i<m->nmat;i++) {
//		m->groups[i].matidx=i;
		m->groups[i].vertidx=nnewvert;
		m->groups[i].faceidx=nnewface;
		for (j=0;j<m->nface;j++) {
			mt=m->faces[j].matidx;
			if (mt==i) {
// found a face that matches our material
				for (k=0;k<3;k++) {
					vi=m->faces[j].vertidx[k];
// look for a matching vert in the same group up to the end of newvertlist
					for (sc=m->groups[i].vertidx;sc<nnewvert;sc++)
						if (newverts[sc].x==m->verts[vi].x &&
						 newverts[sc].y==m->verts[vi].y &&
						 newverts[sc].z==m->verts[vi].z)
							break;
					if (sc==nnewvert) { // new one
						newverts[sc]=m->verts[vi]; // copy it over
						newverts[sc].mi=i;
						nnewvert++;
					}
					newfaces[nnewface].vertidx[k]=sc;	// newface and newvert now set
				}
				newfaces[nnewface].matidx=i; // newface matidx now set
				nnewface++;
				if (themodel->mats[i].msflags&FLAG_DOUBLESIDED) {
					newfaces[nnewface].matidx=i;
					newfaces[nnewface].vertidx[0]=newfaces[nnewface-1].vertidx[0];
					newfaces[nnewface].vertidx[1]=newfaces[nnewface-1].vertidx[2];
					newfaces[nnewface].vertidx[2]=newfaces[nnewface-1].vertidx[1];
					nnewface++; // an extra face
				}
			}
		}
		m->groups[i].nface=nnewface-m->groups[i].faceidx; // update group
		m->groups[i].nvert=nnewvert-m->groups[i].vertidx;
		builduvs(themodel,i); // modifies nnewvert as a side effect..
		m->groups[i].nvert=nnewvert-m->groups[i].vertidx; // after adjusting for uv wrapping (cyl sph)
	}
// free old stuff, copy pointers of new stuff
	memfree(m->faces);
	memfree(m->verts);
	m->faces=memrealloc(newfaces,sizeof(*newfaces)*nnewface);
	m->verts=memrealloc(newverts,sizeof(*newverts)*nnewvert);
	if (m->uvs[0])
		m->uvs[0]=memrealloc(m->uvs[0],sizeof(struct uv)*nnewvert);
	m->nface=nnewface;
	m->nvert=nnewvert;
//	logger("optimized model\n");
//	logger("%d verts\n",m->nvert);
//	for (i=0;i<m->nvert;i++)
//		logger("  vert %5d: %9.2f %9.2f %9.2f\n",i,m->verts[i].x,m->verts[i].y,m->verts[i].z);
//	logger("%d faces\n",m->nface);
//	for (i=0;i<m->nface;i++)
//		logger("  face %5d: %4d %4d %4d matidx %4d\n",
//		i,m->faces[i].vert[0],m->faces[i].vert[1],m->faces[i].vert[2],m->faces[i].matidx);
//	logger("%d mats\n",m->nmat);
//	for (i=0;i<m->nmat;i++)
//		logger("  mat %5d: '%s'\n",i,m->mats[i].name);
//	logger("%d groups\n",m->ngroup);
//	for (i=0;i<m->nmat;i++)
//		logger("  group %5d: vertidx %4d, nverts %4d, faceidx %4d, nfaces %4d, matidx %3d\n",
//			i,m->groups[i].vertidx,m->groups[i].nvert,m->groups[i].faceidx,m->groups[i].nface,
//			m->groups[i].matidx);
	perf_end(LWOOPT);
}
// at this point you've got only verts, and faces with matidx set..., also some uvrgbs from .uv
// gonna build uvs and groups
static void optimizelwomodelwithuvs(struct model *m)
{
	int i,j,k,vi,mt,sc;
	perf_start(LWOOPTUV);
	nnewvert=0;
	nnewface=0;
	newfaces=(struct face *)memalloc(sizeof(*newfaces)*2*m->nface); // allow for double sided
	newverts=(struct pointf3 *)memalloc(sizeof(*newverts)*6*m->nface); // allow for .uv
// see if we've got any textures in the model, if so we need uvs
	for (i=0;i<m->nmat;i++)
		if (m->mats[i].thetex) {
			m->uvs[0]=(struct uv *)memzalloc(m->nface*6*sizeof(struct uv));
			break;
		}
// see if we've got any cverts in the model, if so we need cverts
//	if (usecvertsfromuv)
//		m->cverts=(struct pointf3 *)memzalloc(m->nface*6*sizeof(struct pointf3));
// build independent vertex model
	j=0;
	for (i=0;i<m->nface;i++) {
		for (k=0;k<3;k++) {
			newfaces[j].vertidx[k]=j*3+k;
			newverts[j*3+k]=m->verts[m->faces[i].vertidx[k]];
			if (m->uvs[0]) {
				m->uvs[0][j*3+k].u=uvrgbs[i*3+k].u;
				m->uvs[0][j*3+k].v=uvrgbs[i*3+k].v;
			}
//			if (usecvertsfromuv) {
//				m->cverts[j*3+k].x=uvrgbs[i*3+k].r;
//				m->cverts[j*3+k].y=uvrgbs[i*3+k].g;
//				m->cverts[j*3+k].z=uvrgbs[i*3+k].b;
//			}
		}
		newfaces[j].matidx=m->faces[i].matidx;
		j++;
		if (themodel->mats[m->faces[i].matidx].msflags&FLAG_DOUBLESIDED) {
			for (k=0;k<3;k++) {
				newfaces[j].vertidx[k]=j*3+k;
				newverts[j*3+k]=m->verts[m->faces[i].vertidx[2-k]];
				if (m->uvs[0]) {
					m->uvs[0][j*3+k].u=uvrgbs[i*3+2-k].u;
					m->uvs[0][j*3+k].v=uvrgbs[i*3+2-k].v;
				}
//				if (usecvertsfromuv) {
//					m->cverts[j*3+k].x=uvrgbs[i*3+2-k].r;
//					m->cverts[j*3+k].y=uvrgbs[i*3+2-k].g;
//					m->cverts[j*3+k].z=uvrgbs[i*3+2-k].b;
//				}
			}
			newfaces[j].matidx=m->faces[i].matidx;
			j++;
		}
	}
	nnewface=j;
	nnewvert=j*3;
	memfree(m->faces);
	memfree(m->verts);
	m->faces=memrealloc(newfaces,sizeof(*newfaces)*nnewface);
	m->verts=memrealloc(newverts,sizeof(*newverts)*nnewvert);
	if (m->uvs[0])
		m->uvs[0]=memrealloc(m->uvs[0],sizeof(struct uv)*nnewvert);
	if (m->cverts)
		m->cverts=memrealloc(m->cverts,sizeof(struct pointf3)*nnewvert);
	m->nface=nnewface;
	m->nvert=nnewvert;
// done..
	nnewface=0;
	nnewvert=0;
	newfaces=(struct face *)memalloc(sizeof(*newfaces)*m->nface); // allow for double sided
	newverts=(struct pointf3 *)memalloc(sizeof(*newverts)*3*m->nface); // allow for .uv
	if (m->uvs[0])
		newuvs=(struct uv *)memalloc(sizeof(*newuvs)*3*m->nface);
	if (m->cverts)
		newcverts=(struct pointf3 *)memalloc(sizeof(*newcverts)*3*m->nface);
// do each material
	for (i=0;i<m->nmat;i++) {
//		m->groups[i].matidx=i;
		m->groups[i].vertidx=nnewvert;
		m->groups[i].faceidx=nnewface;
		for (j=0;j<m->nface;j++) {
			mt=m->faces[j].matidx;
			if (mt==i) {
// found a face that matches our material
				for (k=0;k<3;k++) {
					vi=m->faces[j].vertidx[k];
// look for a matching vert in the same group up to the end of newvertlist
					for (sc=m->groups[i].vertidx;sc<nnewvert;sc++) {
						if (newverts[sc].x!=m->verts[vi].x ||
						 newverts[sc].y!=m->verts[vi].y ||
						 newverts[sc].z!=m->verts[vi].z)
							continue;
						if (m->uvs[0])
							if (newuvs[sc].u!=m->uvs[0][vi].u || newuvs[sc].v!=m->uvs[0][vi].v)
								continue;
						if (m->cverts)
							if (newcverts[sc].x!=m->cverts[vi].x || 
								newcverts[sc].y!=m->cverts[vi].y ||
								newcverts[sc].z!=m->cverts[vi].z)
								continue;
						break;
					}
					if (sc==nnewvert) { // new one
						newverts[sc]=m->verts[vi]; // copy it over
						if (m->cverts)
							newcverts[sc]=m->cverts[vi];
						if (m->uvs[0])
							newuvs[sc]=m->uvs[0][vi];
						newverts[sc].mi=i;
						nnewvert++;
					}
					newfaces[nnewface].vertidx[k]=sc;	// newface and newvert now set
				}
				newfaces[nnewface].matidx=i; // newface matidx now set
				nnewface++;
			}
		}
		m->groups[i].nface=nnewface-m->groups[i].faceidx; // update group
		m->groups[i].nvert=nnewvert-m->groups[i].vertidx;
	}
// free old stuff, copy pointers of new stuff
	memfree(m->faces);
	memfree(m->verts);
	m->faces=memrealloc(newfaces,sizeof(*newfaces)*nnewface);
	m->verts=memrealloc(newverts,sizeof(*newverts)*nnewvert);
	if (m->uvs[0]) {
		memfree(m->uvs[0]);
		m->uvs[0]=memrealloc(newuvs,sizeof(struct uv)*nnewvert);
	}
	if (m->cverts) {
		memfree(m->cverts);
		m->cverts=memrealloc(newcverts,sizeof(struct pointf3)*nnewvert);
	}
	m->nface=nnewface;
	m->nvert=nnewvert;
	perf_end(LWOOPTUV);
}

// ok, change flags from .lwo format to our format, also alloc cverts(nvert) if needed
int halftrans;
int getsurfprefixflags(char *s,int inf)
{
	int f=inf;
	int more;
	halftrans=0;
	more=1;
	while(more && *s) {
		switch(*s) {
		case '$':
			f&=~SMAT_HASWBUFF;	// $ turns off wbuffer
			break;
		case '@':
			f|=SMAT_CALCENVMAP; // @ turns on enviorment mapper
			break;
//		case '#':
//			f|=SMAT_ISSKY; // ! low priority sky dome, put up first for drawing
//			break; // commented out because messes up guy in water..
		case '!':
			halftrans=1;
			break;
		default:
			more=0;
			break;
		}
		s++;
	}
	return f;
}

static void fixupflags(struct model *m)
{
	int i;
	int f;
	for (i=0;i<themodel->nmat;i++) {
		if (themodel->mats[i].thetex)
			f=SMAT_HASTEX;
		else
			f=0;
		if (wrapwidth[i]==2)
			f|=SMAT_WRAPU;
		if (wrapheight[i]==2)
			f|=SMAT_WRAPV;
		if (uselights) {
			if (!themodel->cverts)
				f|=SMAT_CALCLIGHTS;  // if you don't explicity set colored verts
			f|=SMAT_HASSHADE;
		}
		f|=SMAT_HASWBUFF;
		f=getsurfprefixflags(themodel->mats[i].name,f);
		if (themodel->mats[i].thetex) {
			f=getsurfprefixflags(themodel->mats[i].thetex->name,f);
		}
		if (hastvel[i]) {
			f|=SMAT_TEXVELS;
			m->hastexvels|=1;
			if (tvel[i].x==0) {
				m->mats[i].texvel.u=-tvel[i].z/tsiz[i].z;
				m->mats[i].texvel.v=tvel[i].y/tsiz[i].y;
			} else if (tvel[i].y==0) {
				m->mats[i].texvel.u=-tvel[i].x/tsiz[i].x;
				m->mats[i].texvel.v=tvel[i].z/tsiz[i].z;
			} else {
				m->mats[i].texvel.u=-tvel[i].x/tsiz[i].x;
				m->mats[i].texvel.v=tvel[i].y/tsiz[i].y;
			}
		}
		if (halftrans)
			themodel->mats[i].mtrans=.5f;
		themodel->mats[i].msflags=f;
	}
	if (!themodel->cverts && uselights)
		themodel->cverts=(struct pointf3 *)memzalloc(sizeof(struct pointf3)*themodel->nvert);
}

static int loaduv(char *uvname)
{
//	int i;
	FILE *fp;
	struct uvrgb *p,*pe;
	char line[256];
	int vert=0;//,poly=-3;
	int nface3=themodel->nface*3;
	char *tok;
	fp=fopen2(uvname,"r");
	if (!fp)
		return 0;
	perf_start(LOADUV);
	uvrgbs=(struct uvrgb *)memalloc(sizeof(struct uvrgb)*themodel->nface*3);
	p=uvrgbs-3;
	pe=uvrgbs+nface3;
//	logger("uvname '%s' found\n",uvname);
	while(1) {
		if (p>=pe)
			break;
		if (!fgets(line,256,fp))
			break;
		tok=strtok(line," \r\n\t");
		if (!tok)
			break;
		if (!strcmp(tok,"POLY")) {
			p+=3;
		} else if (!strcmp(tok,"VERT")) {
			vert=atoi(strtok(NULL," \r\n\t"));
		} else if (!strcmp(tok,"VERT_UV")) {
			p[vert].u=(float)atof(strtok(NULL," \r\n\t"));
			p[vert].v=(float)atof(strtok(NULL," \r\n\t"));
		} else if (!strcmp(tok,"VERT_CUV")) {
			gettok();
			p[vert].u=(float)atof(strtok(NULL," \r\n\t"));
			p[vert].v=(float)atof(strtok(NULL," \r\n\t"));
		} else if (!strcmp(tok,"VERT_RGB")) {
			p[vert].r=(float)atof(strtok(NULL," \r\n\t"));
			p[vert].g=(float)atof(strtok(NULL," \r\n\t"));
			p[vert].b=(float)atof(strtok(NULL," \r\n\t"));
		}
	}
	fclose(fp);
//	for (i=0;i<themodel->nface*3;i++)
//		logger(".uv %4d, uv %6.3f %6.3f rgb %6.3f %6.3f %6.3f\n",
//		i,uvrgbs[i].u,uvrgbs[i].v,uvrgbs[i].r,uvrgbs[i].g,uvrgbs[i].b); 
	perf_end(LOADUV);
	return 1;
}

struct model *loadlwomodel(char *fname)
{
	int i;
	int uselightssave=uselights;
	char mn[NAMESIZE];
	char nameext[256];
	char uvname[256];
	int formlen,chunklen,chunksave;
	int filelength;
	static char chunkid[5];
//	logger(">>>>>>>>>> in loadlwomodel with '%s' <<<<<<<<<<<<\n",fname);

	//	m->mhflags=when 16 bit comes online;		// hardware flags
	perf_start(LOADLWO);
	fileopen(fname,READ);
	fileread(&chunkid,4);
	if (strcmp("FORM",chunkid))
		errorexit("loadlwomodel: fname is not a 'FORM'");
	filelength=getfilesize();
	formlen=filereadlong();
//	logger("filelen %d, formlen %d\n",filelength,formlen);
	fileread(&chunkid,4);
	formlen-=4;
	if (strcmp("LWOB",chunkid))
		errorexit("loadlwomodel: fname has no 'LWOB'");
	themodel=memzalloc(sizeof(*themodel));
	getnameext(nameext,fname);
	mystrncpy(mn,nameext,NAMESIZE);
	strcpy(themodel->name,mn);
	
	themodel->refcount=1;
	themodel->index=nmodels;
	if (nmodels>=MAXMODELS)
		errorexit("too many models");
	modellist[nmodels++]=themodel;

//	timgname[0]='\0';
//	talphaname[0]='\0';

	perf_start(LWOLOOP);
	while(formlen>0) {
		fileread(&chunkid,4);
		chunklen=filereadlong();
		formlen-=8;
		formlen-=chunklen;
//		logger("---------- chunk name '%s' len %d, formleft %d\n",chunkid,chunklen,formlen);
		chunksave=fileskip(0,FILE_CURRENT);
		for (i=0;i<NUMCHUNKS;i++) {
			if (!strcmp(chunks[i].chunkid,chunkid))
				(*chunks[i].chunkfunc)(chunklen);
		}
		fileskip(chunksave+chunklen,FILE_START);
		if (chunklen&1) {
			fileskip(1,FILE_CURRENT);
			formlen--;
		}
	}
	fileclose();
	perf_end(LWOLOOP);
// now lets sort the faces, points, and materials
	getname(uvname,fname);
	strcat(uvname,".uv");
	if (loaduv(uvname)) {
		optimizelwomodelwithuvs(themodel);
		memfree(uvrgbs);
	}
	else
		optimizelwomodel(themodel);
//	builduvs(themodel);
	perf_start(FIXUPFLAGS);
	fixupflags(themodel);
	perf_end(FIXUPFLAGS);
	perf_start(SETBBOX);
	setmodelbbox(themodel);
	perf_end(SETBBOX);
	setmodelnormsang(themodel,0,sman);
	perf_start(DRAWORDER);
	setmodeldraworder(themodel);
	perf_end(DRAWORDER);
	perf_start(SETFORCLIP);
	setmodelforclip(themodel);
	whitentexmaterial(themodel);
	perf_end(SETFORCLIP);
//	logger(">>>>>>>>>> done loadlwomodel\n",fname);
	uselights=uselightssave;
	perf_end(LOADLWO);
	return themodel;
}
