#define D2_3D
#include <float.h>

#include <m_eng.h>
#include <m_perf.h>
#include "d2_software.h"
#include "d2_font.h"
#include "d2_software_font.h"
#include "d2_dx9_font.h"
//#include "d2_light.h"
//#define DOCLIPPING

//static float soft_zfront,soft_zback;
//static S32 inscene;
//#define SOFT_MAXZBUFFER (65535.0f) // nice for low res zbuffer checks
#define SOFT_MAXZBUFFER (65535.0f*100.0f) // more than 16 bit 100 times better
#define MAXXVERTS 600000 // this will increase, maybe automatic.. was 60000
#define MAXMATS 100
static pointf3 xvertsbase[MAXXVERTS+NUMEXTRA];
static pointf3* xverts=xvertsbase+NUMEXTRA;
static pointf3 projxvertsbase[MAXXVERTS+NUMEXTRA];
static pointf3* projxverts=projxvertsbase+NUMEXTRA;
static uv* xuvs;
static pointf3* xcverts;
// clipping
static U8 andorcode[MAXXVERTS*3];
static struct face clipfaces[NUMEXTRA-2];
struct linklist {
	S32 vidx;
	struct linklist *next;
};
static linklist *newlist;
static linklist thelist[40];
static S32 newcidx; // negative index
static mater2* clipmat;

static void registerproj(float offx,float offy,float xres,float yres,float zfront,bool isorth);

void software_init()
{
	video3dinfo.wbuffer3d=bitmap32alloc(WX,WY,C32BLACK);
//	video3dinfo.texavail[TEX32CK]=1;
//	video3dinfo.texavail[TEX32NCK]=1;
	video3dinfo.maxtexlogu=video3dinfo.maxtexlogv=12;
	video3dinfo.canshadow=false;
	video3dinfo.cantoon=false;
	video3dinfo.is3dhardware=false;
}

void software_uninit()
{
	bitmap32free(video3dinfo.wbuffer3d);
	video3dinfo.wbuffer3d=0;
//	memset(video3dinfo.texavail,sizeof(video3dinfo.texavail),0);
}

void software_rendertargetmainfunc()
{
}

bool software_rendertargetshadowmapfunc()
{
	return false;
}

//static S32 cantdraw;
mat4 soft_vpw2c; // special for software only

void software_beginscene()
{
//	if (!inscene) {
		video_lock();
//		inscene=1;
//	}
	registertribackdrop(B32);
	registertriwb(video3dinfo.wbuffer3d); // zbuffer for tris (optional)
}

void software_setviewport(viewport2* vp)
{
	S32 i,j;
	perf_start(BEGINSCENE);
	if (vp->xres<=0 || vp->yres<=0) {
//		cantdraw=1;
		perf_end(BEGINSCENE);
		return;
	}
// make viewproj more 'w friendly' for software
/*	mat4 scaletest;
	identmat4(&scaletest);
	scaletest.e[0][0]=scaletest.e[1][1]=scaletest.e[2][2]=scaletest.e[3][3]=
		1.0f/(vp->zfront);
	mulmat4(&video3dinfo.matViewProj,&scaletest,&video3dinfo.matViewProj); */
	if (vp->xstart<0) {
		vp->xres+=vp->xstart;
		vp->xstart=0;
	}
	if (vp->ystart<0) {
		vp->yres+=vp->ystart;
		vp->ystart=0;
	}
	if (vp->xstart+vp->xres > B32->size.x)
		vp->xres=B32->size.x-vp->xstart;
	if (vp->ystart+vp->yres > B32->size.y)
		vp->yres=B32->size.y-vp->ystart;
	if (vp->xres<=0 || vp->yres<=0) {
		perf_end(BEGINSCENE);
		return;
	}
	if (vp->flags&VP_CLEARWB) {
		cliprect32(video3dinfo.wbuffer3d,vp->xstart,vp->ystart,vp->xres,vp->yres,0);
//		cliprect32(video3dinfo.wbuffer3d,vp->xstart,vp->ystart,vp->xres,vp->yres,C32(U32(video3dinfo.clearzbuffval*65535U)));
	}
	if (vp->flags&VP_CLEARBG) {
		cliprect32(B32,vp->xstart,vp->ystart,vp->xres,vp->yres,vp->backcolor);
		
		if (vp->flags&VP_CHECKER) { // not terribly fast
			setcliprect32(B32,vp->xstart,vp->ystart,vp->xres,vp->yres);
			C32 darkcolor;
			darkcolor.r=(vp->backcolor.r*3)>>2;
			darkcolor.g=(vp->backcolor.g*3)>>2;
			darkcolor.b=(vp->backcolor.b*3)>>2;
			for (j=0;j<vp->yres;j+=8)
				for (i=0;i<vp->xres;i+=8)
					if ((i^j)&8)
						cliprect32(B32,i+vp->xstart,j+vp->ystart,8,8,darkcolor);
			resetcliprect32(B32);
		}
	}

/*	float camzoom=vp->camzoom;
	float wbscale=1.0f/(65535*vp->zfront);
	pointf3 camscl;
	camscl.x=camzoom*wbscale*vp->ysrc/vp->xsrc;
	camscl.y=camzoom*wbscale;
	camscl.z=wbscale;
	postmulscale3d(&vp->w2c,&camscl,&soft_vpw2c); */

	registerproj((float)vp->xstart,(float)vp->ystart,(float)vp->xres,(float)vp->yres,vp->zfront,vp->isortho);
//	soft_zfront=1.0f/65535.0f;
//	soft_zback=vp->zback/(vp->zfront*65535.0f);
	perf_end(BEGINSCENE);
}

/*
void software_drawscene(tree2* t)
{
	perf_start(DRAWTREE);
// recursively draw the tree..
	treeinfo.ndrawtree=0;
	video_buildtreelist(t); // builds up t->o2w
//	buildworldmats(t);
	dolights(t);	//	logmat4(&curvp->w2c,"w2c");
	video_drawtreelist();
	perf_end(DRAWTREE);
}
*/
void software_endscene(bool dotextdump)
{
//	if (dotextdump)
//		dx9_drawtextque_do();
	perf_start(ENDSCENE);
//	if (inscene && !cantdraw) {
//		if (last)
//			software_drawtextque_do();
		video_unlock();
//		inscene=0;
//	}
//	cantdraw=0;
	perf_end(ENDSCENE);
}
//void projvec(pointf3* in,pointf3 *out);
static float proj_zoomx,proj_zoomy,proj_offsetx,proj_offsety,proj_zfront,proj_inortho;//,proj_zfrontclip;
#if 0
static struct linklist *software_cliptriright(struct linklist *ll)
{
	float t,tnp;
	struct pointf3 *p0,*p1,pdel;
	struct pointf3 *c0,*c1,cdel;
	struct uv *uv0,*uv1,uvdel;
	struct linklist *lln,*llstart,*newliststart;
	llstart=ll;
	newliststart=newlist;
//	logger("cliptriright\n");
	do {
//		logger("vidx %d\n",ll->vidx);
		p0=&xverts[ll->vidx];
		lln=ll->next;
		p1=&xverts[lln->vidx];
		if (p0->x<=p0->z) {
			if (p1->x<=p1->z) { // both inside, 1 old point
				newlist->vidx=lln->vidx;
				newlist->next=newlist+1;
				newlist++;
			} else { // goin out, 1 new point
				pdel.x=p1->x-p0->x;
				pdel.y=p1->y-p0->y;
				pdel.z=p1->z-p0->z;
				t=(p0->z-p0->x)/(pdel.x-pdel.z);
				xverts[newcidx].x=p0->x+pdel.x*t;
				xverts[newcidx].y=p0->y+pdel.y*t;
				xverts[newcidx].z=xverts[newcidx].x;
				projvec(&xverts[newcidx],&projxverts[newcidx]);
				tnp=p1->z*(p0->z-p0->x)/(p0->z*p1->x-p0->x*p1->z);
				if (!video3dinfo.perscorrect)
					t=tnp;
				if (clipmat->msflags&SMAT_HASTEX) {
					uv0=&xuvs[ll->vidx];
					uv1=&xuvs[lln->vidx];
					uvdel.u=uv1->u-uv0->u;
					uvdel.v=uv1->v-uv0->v;
					xuvs[newcidx].u=uv0->u+uvdel.u*t;
					xuvs[newcidx].v=uv0->v+uvdel.v*t;
				}
				if (clipmat->msflags&SMAT_HASSHADE) {
					c0=&xcverts[ll->vidx];
					c1=&xcverts[lln->vidx];
					cdel.x=c1->x-c0->x;
					cdel.y=c1->y-c0->y;
					cdel.z=c1->z-c0->z;
					xcverts[newcidx].x=c0->x+cdel.x*tnp;
					xcverts[newcidx].y=c0->y+cdel.y*tnp;
					xcverts[newcidx].z=c0->z+cdel.z*tnp;
				}
				newlist->vidx=newcidx;
				newlist->next=newlist+1;
				newlist++;
				newcidx++;
//				newv++;
			}
		} else {
			if (p1->x<=p1->z) { // goin in, 2 points, 1 new and 1 old
				//new
				pdel.x=p0->x-p1->x;
				pdel.y=p0->y-p1->y;
				pdel.z=p0->z-p1->z;
				t=(p1->z-p1->x)/(pdel.x-pdel.z);
				xverts[newcidx].x=p1->x+pdel.x*t;
				xverts[newcidx].y=p1->y+pdel.y*t;
				xverts[newcidx].z=xverts[newcidx].x;
				projvec(&xverts[newcidx],&projxverts[newcidx]);
				tnp=p0->z*(p1->z-p1->x)/(p1->z*p0->x-p1->x*p0->z);
				if (!video3dinfo.perscorrect)
					t=tnp;
				if (clipmat->msflags&SMAT_HASTEX) {
					uv0=&xuvs[ll->vidx];
					uv1=&xuvs[lln->vidx];
					uvdel.u=uv0->u-uv1->u;
					uvdel.v=uv0->v-uv1->v;
					xuvs[newcidx].u=uv1->u+uvdel.u*t;
					xuvs[newcidx].v=uv1->v+uvdel.v*t;
				}
				if (clipmat->msflags&SMAT_HASSHADE) {
					c0=&xcverts[ll->vidx];
					c1=&xcverts[lln->vidx];
					cdel.x=c0->x-c1->x;
					cdel.y=c0->y-c1->y;
					cdel.z=c0->z-c1->z;
					xcverts[newcidx].x=c1->x+cdel.x*tnp;
					xcverts[newcidx].y=c1->y+cdel.y*tnp;
					xcverts[newcidx].z=c1->z+cdel.z*tnp;
				}
				newlist->vidx=newcidx;
				newlist->next=newlist+1;
				newlist++;
				//old
				newlist->vidx=lln->vidx;
				newlist->next=newlist+1;
				newlist++;
				newcidx++;
//				newv++;
			} else { // both outside, 0 points
			}
		}
		ll=lln;
	} while (ll!=llstart);
	if (newlist==newliststart)
		return 0;
	newlist--;
	newlist->next=newliststart;
	newlist++;
	return newliststart;
}
#endif
static struct linklist *software_cliptriright(struct linklist *ll)
{
	float t,tnp=0;
	struct pointf3 *p0,*p1,pdel;
	struct pointf3 *c0,*c1,cdel;
	struct uv *uv0,*uv1,uvdel;
	struct linklist *lln,*llstart,*newliststart;
	llstart=ll;
	newliststart=newlist;
//	logger("cliptriright\n");
	do {
//		logger("vidx %d\n",ll->vidx);
		p0=&xverts[ll->vidx];
		lln=ll->next;
		p1=&xverts[lln->vidx];
		if (p0->x<=p0->w) {
			if (p1->x<=p1->w) { // both inside, 1 old point
				newlist->vidx=lln->vidx;
				newlist->next=newlist+1;
				newlist++;
			} else { // goin out, 1 new point
				pdel.x=p1->x-p0->x;
				pdel.y=p1->y-p0->y;
				pdel.z=p1->z-p0->z;
				pdel.w=p1->w-p0->w;
				t=(p0->x-p0->w)/(pdel.w-pdel.x);
				xverts[newcidx].x=p0->x+pdel.x*t;
				xverts[newcidx].y=p0->y+pdel.y*t;
				xverts[newcidx].z=p0->z+pdel.z*t;
				xverts[newcidx].w=xverts[newcidx].x;
//				projvec(&xverts[newcidx],&projxverts[newcidx]);
				if (!video3dinfo.perscorrect || (clipmat->msflags&SMAT_HASSHADE)) {
					tnp=t*(p0->w+pdel.w)/(p0->w+t*pdel.w);
					if (tnp<0) tnp=0; if (tnp>1) tnp=1;
				}
//				p1->z*(p0->z-p0->x)/(p0->z*p1->x-p0->x*p1->z);
				if (!video3dinfo.perscorrect)
					t=tnp; 
				if (clipmat->msflags&SMAT_HASTEX) {
					uv0=&xuvs[ll->vidx];
					uv1=&xuvs[lln->vidx];
					uvdel.u=uv1->u-uv0->u;
					uvdel.v=uv1->v-uv0->v;
					xuvs[newcidx].u=uv0->u+uvdel.u*t;
					xuvs[newcidx].v=uv0->v+uvdel.v*t;
				}
				if (clipmat->msflags&SMAT_HASSHADE) {
					c0=&xcverts[ll->vidx];
					c1=&xcverts[lln->vidx];
					cdel.x=c1->x-c0->x;
					cdel.y=c1->y-c0->y;
					cdel.z=c1->z-c0->z;
					xcverts[newcidx].x=c0->x+cdel.x*tnp;
					xcverts[newcidx].y=c0->y+cdel.y*tnp;
					xcverts[newcidx].z=c0->z+cdel.z*tnp;
				} 
				newlist->vidx=newcidx;
				newlist->next=newlist+1;
				newlist++;
				newcidx++;
//				newv++;
			}
		} else {
			if (p1->x<=p1->w) { // goin in, 2 points, 1 new and 1 old
				//new
				pdel.x=p0->x-p1->x;
				pdel.y=p0->y-p1->y;
				pdel.z=p0->z-p1->z;
				pdel.w=p0->w-p1->w;
				t=(p1->x-p1->w)/(pdel.w-pdel.x);
				xverts[newcidx].x=p1->x+pdel.x*t;
				xverts[newcidx].y=p1->y+pdel.y*t;
				xverts[newcidx].z=p1->z+pdel.z*t;
				xverts[newcidx].w=xverts[newcidx].x;
//				projvec(&xverts[newcidx],&projxverts[newcidx]);
				if (!video3dinfo.perscorrect || (clipmat->msflags&SMAT_HASSHADE)) {
					tnp=t*(p1->w+pdel.w)/(p1->w+t*pdel.w);
					if (tnp<0) tnp=0; if (tnp>1) tnp=1;
				}
//				tnp=p0->z*(p1->z-p1->x)/(p1->z*p0->x-p1->x*p0->z);
				if (!video3dinfo.perscorrect)
					t=tnp; 
				if (clipmat->msflags&SMAT_HASTEX) {
					uv0=&xuvs[ll->vidx];
					uv1=&xuvs[lln->vidx];
					uvdel.u=uv0->u-uv1->u;
					uvdel.v=uv0->v-uv1->v;
					xuvs[newcidx].u=uv1->u+uvdel.u*t;
					xuvs[newcidx].v=uv1->v+uvdel.v*t;
				}
				if (clipmat->msflags&SMAT_HASSHADE) {
					c0=&xcverts[ll->vidx];
					c1=&xcverts[lln->vidx];
					cdel.x=c0->x-c1->x;
					cdel.y=c0->y-c1->y;
					cdel.z=c0->z-c1->z;
					xcverts[newcidx].x=c1->x+cdel.x*tnp;
					xcverts[newcidx].y=c1->y+cdel.y*tnp;
					xcverts[newcidx].z=c1->z+cdel.z*tnp;
				} 
				newlist->vidx=newcidx;
				newlist->next=newlist+1;
				newlist++;
				newcidx++;
				//old
				newlist->vidx=lln->vidx;
				newlist->next=newlist+1;
				newlist++;
//				newcidx++;
//				newv++;
			} else { // both outside, 0 points
			}
		}
		ll=lln;
	} while (ll!=llstart);
	if (newlist==newliststart)
		return 0;
	newlist--;
	newlist->next=newliststart;
	newlist++;
	return newliststart;
}
static struct linklist *software_cliptrileft(struct linklist *ll)
{
	float t,tnp=0;
	struct pointf3 *p0,*p1,pdel;
	struct pointf3 *c0,*c1,cdel;
	struct uv *uv0,*uv1,uvdel;
	struct linklist *lln,*llstart,*newliststart;
	llstart=ll;
	newliststart=newlist;
//	logger("cliptriright\n");
	do {
//		logger("vidx %d\n",ll->vidx);
		p0=&xverts[ll->vidx];
		lln=ll->next;
		p1=&xverts[lln->vidx];
		if (-p0->x<=p0->w) {
			if (-p1->x<=p1->w) { // both inside, 1 old point
				newlist->vidx=lln->vidx;
				newlist->next=newlist+1;
				newlist++;
			} else { // goin out, 1 new point
				pdel.x=p1->x-p0->x;
				pdel.y=p1->y-p0->y;
				pdel.z=p1->z-p0->z;
				pdel.w=p1->w-p0->w;
				t=(-p0->x-p0->w)/(pdel.w+pdel.x);
				xverts[newcidx].x=p0->x+pdel.x*t;
				xverts[newcidx].y=p0->y+pdel.y*t;
				xverts[newcidx].z=p0->z+pdel.z*t;
				xverts[newcidx].w=-xverts[newcidx].x;
//				projvec(&xverts[newcidx],&projxverts[newcidx]);
				if (!video3dinfo.perscorrect || (clipmat->msflags&SMAT_HASSHADE)) {
					tnp=t*(p0->w+pdel.w)/(p0->w+t*pdel.w);
					if (tnp<0) tnp=0; if (tnp>1) tnp=1;
				}
//				p1->z*(p0->z-p0->x)/(p0->z*p1->x-p0->x*p1->z);
				if (!video3dinfo.perscorrect)
					t=tnp; 
				if (clipmat->msflags&SMAT_HASTEX) {
					uv0=&xuvs[ll->vidx];
					uv1=&xuvs[lln->vidx];
					uvdel.u=uv1->u-uv0->u;
					uvdel.v=uv1->v-uv0->v;
					xuvs[newcidx].u=uv0->u+uvdel.u*t;
					xuvs[newcidx].v=uv0->v+uvdel.v*t;
				}
				if (clipmat->msflags&SMAT_HASSHADE) {
					c0=&xcverts[ll->vidx];
					c1=&xcverts[lln->vidx];
					cdel.x=c1->x-c0->x;
					cdel.y=c1->y-c0->y;
					cdel.z=c1->z-c0->z;
					xcverts[newcidx].x=c0->x+cdel.x*tnp;
					xcverts[newcidx].y=c0->y+cdel.y*tnp;
					xcverts[newcidx].z=c0->z+cdel.z*tnp;
				} 
				newlist->vidx=newcidx;
				newlist->next=newlist+1;
				newlist++;
				newcidx++;
//				newv++;
			}
		} else {
			if (-p1->x<=p1->w) { // goin in, 2 points, 1 new and 1 old
				//new
				pdel.x=p0->x-p1->x;
				pdel.y=p0->y-p1->y;
				pdel.z=p0->z-p1->z;
				pdel.w=p0->w-p1->w;
				t=(-p1->x-p1->w)/(pdel.w+pdel.x);
				xverts[newcidx].x=p1->x+pdel.x*t;
				xverts[newcidx].y=p1->y+pdel.y*t;
				xverts[newcidx].z=p1->z+pdel.z*t;
				xverts[newcidx].w=-xverts[newcidx].x;
//				projvec(&xverts[newcidx],&projxverts[newcidx]);
				if (!video3dinfo.perscorrect || (clipmat->msflags&SMAT_HASSHADE)) {
					tnp=t*(p1->w+pdel.w)/(p1->w+t*pdel.w);
					if (tnp<0) tnp=0; if (tnp>1) tnp=1;
				}
//				tnp=p0->z*(p1->z-p1->x)/(p1->z*p0->x-p1->x*p0->z);
				if (!video3dinfo.perscorrect)
					t=tnp; 
				if (clipmat->msflags&SMAT_HASTEX) {
					uv0=&xuvs[ll->vidx];
					uv1=&xuvs[lln->vidx];
					uvdel.u=uv0->u-uv1->u;
					uvdel.v=uv0->v-uv1->v;
					xuvs[newcidx].u=uv1->u+uvdel.u*t;
					xuvs[newcidx].v=uv1->v+uvdel.v*t;
				}
				if (clipmat->msflags&SMAT_HASSHADE) {
					c0=&xcverts[ll->vidx];
					c1=&xcverts[lln->vidx];
					cdel.x=c0->x-c1->x;
					cdel.y=c0->y-c1->y;
					cdel.z=c0->z-c1->z;
					xcverts[newcidx].x=c1->x+cdel.x*tnp;
					xcverts[newcidx].y=c1->y+cdel.y*tnp;
					xcverts[newcidx].z=c1->z+cdel.z*tnp;
				} 
				newlist->vidx=newcidx;
				newlist->next=newlist+1;
				newlist++;
				newcidx++;
				//old
				newlist->vidx=lln->vidx;
				newlist->next=newlist+1;
				newlist++;
//				newcidx++;
//				newv++;
			} else { // both outside, 0 points
			}
		}
		ll=lln;
	} while (ll!=llstart);
	if (newlist==newliststart)
		return 0;
	newlist--;
	newlist->next=newliststart;
	newlist++;
	return newliststart;
}
static struct linklist *software_cliptritop(struct linklist *ll)
{
	float t,tnp=0;
	struct pointf3 *p0,*p1,pdel;
	struct pointf3 *c0,*c1,cdel;
	struct uv *uv0,*uv1,uvdel;
	struct linklist *lln,*llstart,*newliststart;
	llstart=ll;
	newliststart=newlist;
//	logger("cliptriright\n");
	do {
//		logger("vidx %d\n",ll->vidx);
		p0=&xverts[ll->vidx];
		lln=ll->next;
		p1=&xverts[lln->vidx];
		if (p0->y<=p0->w) {
			if (p1->y<=p1->w) { // both inside, 1 old point
				newlist->vidx=lln->vidx;
				newlist->next=newlist+1;
				newlist++;
			} else { // goin out, 1 new point
				pdel.x=p1->x-p0->x;
				pdel.y=p1->y-p0->y;
				pdel.z=p1->z-p0->z;
				pdel.w=p1->w-p0->w;
				t=(p0->y-p0->w)/(pdel.w-pdel.y);
				xverts[newcidx].x=p0->x+pdel.x*t;
				xverts[newcidx].y=p0->y+pdel.y*t;
				xverts[newcidx].z=p0->z+pdel.z*t;
				xverts[newcidx].w=xverts[newcidx].y;
//				projvec(&xverts[newcidx],&projxverts[newcidx]);
				if (!video3dinfo.perscorrect || (clipmat->msflags&SMAT_HASSHADE)) {
					tnp=t*(p0->w+pdel.w)/(p0->w+t*pdel.w);
					if (tnp<0) tnp=0; if (tnp>1) tnp=1;
				}
//				p1->z*(p0->z-p0->x)/(p0->z*p1->x-p0->x*p1->z);
				if (!video3dinfo.perscorrect)
					t=tnp; 
				if (clipmat->msflags&SMAT_HASTEX) {
					uv0=&xuvs[ll->vidx];
					uv1=&xuvs[lln->vidx];
					uvdel.u=uv1->u-uv0->u;
					uvdel.v=uv1->v-uv0->v;
					xuvs[newcidx].u=uv0->u+uvdel.u*t;
					xuvs[newcidx].v=uv0->v+uvdel.v*t;
				}
				if (clipmat->msflags&SMAT_HASSHADE) {
					c0=&xcverts[ll->vidx];
					c1=&xcverts[lln->vidx];
					cdel.x=c1->x-c0->x;
					cdel.y=c1->y-c0->y;
					cdel.z=c1->z-c0->z;
					xcverts[newcidx].x=c0->x+cdel.x*tnp;
					xcverts[newcidx].y=c0->y+cdel.y*tnp;
					xcverts[newcidx].z=c0->z+cdel.z*tnp;
				} 
				newlist->vidx=newcidx;
				newlist->next=newlist+1;
				newlist++;
				newcidx++;
//				newv++;
			}
		} else {
			if (p1->y<=p1->w) { // goin in, 2 points, 1 new and 1 old
				//new
				pdel.x=p0->x-p1->x;
				pdel.y=p0->y-p1->y;
				pdel.z=p0->z-p1->z;
				pdel.w=p0->w-p1->w;
				t=(p1->y-p1->w)/(pdel.w-pdel.y);
				xverts[newcidx].x=p1->x+pdel.x*t;
				xverts[newcidx].y=p1->y+pdel.y*t;
				xverts[newcidx].z=p1->z+pdel.z*t;
				xverts[newcidx].w=xverts[newcidx].y;
//				projvec(&xverts[newcidx],&projxverts[newcidx]);
				if (!video3dinfo.perscorrect || (clipmat->msflags&SMAT_HASSHADE)) {
					tnp=t*(p1->w+pdel.w)/(p1->w+t*pdel.w);
					if (tnp<0) tnp=0; if (tnp>1) tnp=1;
				}
//				tnp=p0->z*(p1->z-p1->x)/(p1->z*p0->x-p1->x*p0->z);
				if (!video3dinfo.perscorrect)
					t=tnp; 
				if (clipmat->msflags&SMAT_HASTEX) {
					uv0=&xuvs[ll->vidx];
					uv1=&xuvs[lln->vidx];
					uvdel.u=uv0->u-uv1->u;
					uvdel.v=uv0->v-uv1->v;
					xuvs[newcidx].u=uv1->u+uvdel.u*t;
					xuvs[newcidx].v=uv1->v+uvdel.v*t;
				}
				if (clipmat->msflags&SMAT_HASSHADE) {
					c0=&xcverts[ll->vidx];
					c1=&xcverts[lln->vidx];
					cdel.x=c0->x-c1->x;
					cdel.y=c0->y-c1->y;
					cdel.z=c0->z-c1->z;
					xcverts[newcidx].x=c1->x+cdel.x*tnp;
					xcverts[newcidx].y=c1->y+cdel.y*tnp;
					xcverts[newcidx].z=c1->z+cdel.z*tnp;
				} 
				newlist->vidx=newcidx;
				newlist->next=newlist+1;
				newlist++;
				newcidx++;
				//old
				newlist->vidx=lln->vidx;
				newlist->next=newlist+1;
				newlist++;
//				newcidx++;
//				newv++;
			} else { // both outside, 0 points
			}
		}
		ll=lln;
	} while (ll!=llstart);
	if (newlist==newliststart)
		return 0;
	newlist--;
	newlist->next=newliststart;
	newlist++;
	return newliststart;
}
static struct linklist *software_cliptribot(struct linklist *ll)
{
	float t,tnp=0;
	struct pointf3 *p0,*p1,pdel;
	struct pointf3 *c0,*c1,cdel;
	struct uv *uv0,*uv1,uvdel;
	struct linklist *lln,*llstart,*newliststart;
	llstart=ll;
	newliststart=newlist;
//	logger("cliptriright\n");
	do {
//		logger("vidx %d\n",ll->vidx);
		p0=&xverts[ll->vidx];
		lln=ll->next;
		p1=&xverts[lln->vidx];
		if (-p0->y<=p0->w) {
			if (-p1->y<=p1->w) { // both inside, 1 old point
				newlist->vidx=lln->vidx;
				newlist->next=newlist+1;
				newlist++;
			} else { // goin out, 1 new point
				pdel.x=p1->x-p0->x;
				pdel.y=p1->y-p0->y;
				pdel.z=p1->z-p0->z;
				pdel.w=p1->w-p0->w;
				t=(-p0->y-p0->w)/(pdel.w+pdel.y);
				xverts[newcidx].x=p0->x+pdel.x*t;
				xverts[newcidx].y=p0->y+pdel.y*t;
				xverts[newcidx].z=p0->z+pdel.z*t;
				xverts[newcidx].w=-xverts[newcidx].y;
//				projvec(&xverts[newcidx],&projxverts[newcidx]);
				if (!video3dinfo.perscorrect || (clipmat->msflags&SMAT_HASSHADE)) {
					tnp=t*(p0->w+pdel.w)/(p0->w+t*pdel.w);
					if (tnp<0) tnp=0; if (tnp>1) tnp=1;
				}
//				p1->z*(p0->z-p0->x)/(p0->z*p1->x-p0->x*p1->z);
				if (!video3dinfo.perscorrect)
					t=tnp; 
				if (clipmat->msflags&SMAT_HASTEX) {
					uv0=&xuvs[ll->vidx];
					uv1=&xuvs[lln->vidx];
					uvdel.u=uv1->u-uv0->u;
					uvdel.v=uv1->v-uv0->v;
					xuvs[newcidx].u=uv0->u+uvdel.u*t;
					xuvs[newcidx].v=uv0->v+uvdel.v*t;
				}
				if (clipmat->msflags&SMAT_HASSHADE) {
					c0=&xcverts[ll->vidx];
					c1=&xcverts[lln->vidx];
					cdel.x=c1->x-c0->x;
					cdel.y=c1->y-c0->y;
					cdel.z=c1->z-c0->z;
					xcverts[newcidx].x=c0->x+cdel.x*tnp;
					xcverts[newcidx].y=c0->y+cdel.y*tnp;
					xcverts[newcidx].z=c0->z+cdel.z*tnp;
				} 
				newlist->vidx=newcidx;
				newlist->next=newlist+1;
				newlist++;
				newcidx++;
//				newv++;
			}
		} else {
			if (-p1->y<=p1->w) { // goin in, 2 points, 1 new and 1 old
				//new
				pdel.x=p0->x-p1->x;
				pdel.y=p0->y-p1->y;
				pdel.z=p0->z-p1->z;
				pdel.w=p0->w-p1->w;
				t=(-p1->y-p1->w)/(pdel.w+pdel.y);
				xverts[newcidx].x=p1->x+pdel.x*t;
				xverts[newcidx].y=p1->y+pdel.y*t;
				xverts[newcidx].z=p1->z+pdel.z*t;
				xverts[newcidx].w=-xverts[newcidx].y;
//				projvec(&xverts[newcidx],&projxverts[newcidx]);
				if (!video3dinfo.perscorrect || (clipmat->msflags&SMAT_HASSHADE)) {
					tnp=t*(p1->w+pdel.w)/(p1->w+t*pdel.w);
					if (tnp<0) tnp=0; if (tnp>1) tnp=1;
				}
//				tnp=p0->z*(p1->z-p1->x)/(p1->z*p0->x-p1->x*p0->z);
				if (!video3dinfo.perscorrect)
					t=tnp; 
				if (clipmat->msflags&SMAT_HASTEX) {
					uv0=&xuvs[ll->vidx];
					uv1=&xuvs[lln->vidx];
					uvdel.u=uv0->u-uv1->u;
					uvdel.v=uv0->v-uv1->v;
					xuvs[newcidx].u=uv1->u+uvdel.u*t;
					xuvs[newcidx].v=uv1->v+uvdel.v*t;
				}
				if (clipmat->msflags&SMAT_HASSHADE) {
					c0=&xcverts[ll->vidx];
					c1=&xcverts[lln->vidx];
					cdel.x=c0->x-c1->x;
					cdel.y=c0->y-c1->y;
					cdel.z=c0->z-c1->z;
					xcverts[newcidx].x=c1->x+cdel.x*tnp;
					xcverts[newcidx].y=c1->y+cdel.y*tnp;
					xcverts[newcidx].z=c1->z+cdel.z*tnp;
				} 
				newlist->vidx=newcidx;
				newlist->next=newlist+1;
				newlist++;
				newcidx++;
				//old
				newlist->vidx=lln->vidx;
				newlist->next=newlist+1;
				newlist++;
//				newcidx++;
//				newv++;
			} else { // both outside, 0 points
			}
		}
		ll=lln;
	} while (ll!=llstart);
	if (newlist==newliststart)
		return 0;
	newlist--;
	newlist->next=newliststart;
	newlist++;
	return newliststart;
}

#ifdef DOCLIPPING

static struct linklist *software_cliptrileft(struct linklist *ll)
{
	float t,tnp;
	struct pointf3 *p0,*p1,pdel;
	struct pointf3 *c0,*c1,cdel;
	struct uv *uv0,*uv1,uvdel;
	struct linklist *lln,*llstart,*newliststart;
	llstart=ll;
	newliststart=newlist;
//	logger("cliptriright\n");
	do {
//		logger("vidx %d\n",ll->vidx);
		p0=&xverts[ll->vidx];
		lln=ll->next;
		p1=&xverts[lln->vidx];
		if (-p0->x<=p0->z) {
			if (-p1->x<=p1->z) { // both inside, 1 old point
				newlist->vidx=lln->vidx;
				newlist->next=newlist+1;
				newlist++;
			} else { // goin out, 1 new point
				pdel.x=p1->x-p0->x;
				pdel.y=p1->y-p0->y;
				pdel.z=p1->z-p0->z;
				t=-(p0->z+p0->x)/(pdel.x+pdel.z);
				xverts[newcidx].x=p0->x+pdel.x*t;
				xverts[newcidx].y=p0->y+pdel.y*t;
				xverts[newcidx].z=-xverts[newcidx].x;
				projvec(&xverts[newcidx],&projxverts[newcidx]);
				tnp=-p1->z*(p0->z+p0->x)/(p0->z*p1->x-p0->x*p1->z);
				if (!video3dinfo.perscorrect)
					t=tnp;
				if (clipmat->msflags&SMAT_HASTEX) {
					uv0=&xuvs[ll->vidx];
					uv1=&xuvs[lln->vidx];
					uvdel.u=uv1->u-uv0->u;
					uvdel.v=uv1->v-uv0->v;
					xuvs[newcidx].u=uv0->u+uvdel.u*t;
					xuvs[newcidx].v=uv0->v+uvdel.v*t;
				}
				if (clipmat->msflags&SMAT_HASSHADE) {
					c0=&xcverts[ll->vidx];
					c1=&xcverts[lln->vidx];
					cdel.x=c1->x-c0->x;
					cdel.y=c1->y-c0->y;
					cdel.z=c1->z-c0->z;
					xcverts[newcidx].x=c0->x+cdel.x*tnp;
					xcverts[newcidx].y=c0->y+cdel.y*tnp;
					xcverts[newcidx].z=c0->z+cdel.z*tnp;
				}
				newlist->vidx=newcidx;
				newlist->next=newlist+1;
				newlist++;
				newcidx++;
//				newv++;
			}
		} else {
			if (-p1->x<=p1->z) { // goin in, 2 points, 1 new and 1 old
				//new
				pdel.x=p0->x-p1->x;
				pdel.y=p0->y-p1->y;
				pdel.z=p0->z-p1->z;
				t=-(p1->z+p1->x)/(pdel.x+pdel.z);
				xverts[newcidx].x=p1->x+pdel.x*t;
				xverts[newcidx].y=p1->y+pdel.y*t;
				xverts[newcidx].z=-xverts[newcidx].x;
				projvec(&xverts[newcidx],&projxverts[newcidx]);
				tnp=-p0->z*(p1->z+p1->x)/(p1->z*p0->x-p1->x*p0->z);
				if (!video3dinfo.perscorrect)
					t=tnp;
				if (clipmat->msflags&SMAT_HASTEX) {
					uv0=&xuvs[ll->vidx];
					uv1=&xuvs[lln->vidx];
					uvdel.u=uv0->u-uv1->u;
					uvdel.v=uv0->v-uv1->v;
					xuvs[newcidx].u=uv1->u+uvdel.u*t;
					xuvs[newcidx].v=uv1->v+uvdel.v*t;
				}
				if (clipmat->msflags&SMAT_HASSHADE) {
					c0=&xcverts[ll->vidx];
					c1=&xcverts[lln->vidx];
					cdel.x=c0->x-c1->x;
					cdel.y=c0->y-c1->y;
					cdel.z=c0->z-c1->z;
					xcverts[newcidx].x=c1->x+cdel.x*tnp;
					xcverts[newcidx].y=c1->y+cdel.y*tnp;
					xcverts[newcidx].z=c1->z+cdel.z*tnp;
				}
				newlist->vidx=newcidx;
				newlist->next=newlist+1;
				newlist++;
				//old
				newlist->vidx=lln->vidx;
				newlist->next=newlist+1;
				newlist++;
				newcidx++;
//				newv++;
			} else { // both outside, 0 points
			}
		}
		ll=lln;
	} while (ll!=llstart);
	if (newlist==newliststart)
		return 0;
	newlist--;
	newlist->next=newliststart;
	newlist++;
	return newliststart;
}

static struct linklist *software_cliptritop(struct linklist *ll)
{
	float t,tnp;
	struct pointf3 *p0,*p1,pdel;
	struct pointf3 *c0,*c1,cdel;
	struct uv *uv0,*uv1,uvdel;
	struct linklist *lln,*llstart,*newliststart;
	llstart=ll;
	newliststart=newlist;
//	logger("cliptriright\n");
	do {
//		logger("vidx %d\n",ll->vidx);
		p0=&xverts[ll->vidx];
		lln=ll->next;
		p1=&xverts[lln->vidx];
		if (p0->y<=p0->z) {
			if (p1->y<=p1->z) { // both inside, 1 old point
				newlist->vidx=lln->vidx;
				newlist->next=newlist+1;
				newlist++;
			} else { // goin out, 1 new point
				pdel.x=p1->x-p0->x;
				pdel.y=p1->y-p0->y;
				pdel.z=p1->z-p0->z;
				t=(p0->z-p0->y)/(pdel.y-pdel.z);
				xverts[newcidx].x=p0->x+pdel.x*t;
				xverts[newcidx].y=p0->y+pdel.y*t;
				xverts[newcidx].z=xverts[newcidx].y;
				projvec(&xverts[newcidx],&projxverts[newcidx]);
				tnp=p1->z*(p0->z-p0->y)/(p0->z*p1->y-p0->y*p1->z);
				if (!video3dinfo.perscorrect)
					t=tnp;
				if (clipmat->msflags&SMAT_HASTEX) {
					uv0=&xuvs[ll->vidx];
					uv1=&xuvs[lln->vidx];
					uvdel.u=uv1->u-uv0->u;
					uvdel.v=uv1->v-uv0->v;
					xuvs[newcidx].u=uv0->u+uvdel.u*t;
					xuvs[newcidx].v=uv0->v+uvdel.v*t;
				}
				if (clipmat->msflags&SMAT_HASSHADE) {
					c0=&xcverts[ll->vidx];
					c1=&xcverts[lln->vidx];
					cdel.x=c1->x-c0->x;
					cdel.y=c1->y-c0->y;
					cdel.z=c1->z-c0->z;
					xcverts[newcidx].x=c0->x+cdel.x*tnp;
					xcverts[newcidx].y=c0->y+cdel.y*tnp;
					xcverts[newcidx].z=c0->z+cdel.z*tnp;
				}
				newlist->vidx=newcidx;
				newlist->next=newlist+1;
				newlist++;
				newcidx++;
//				newv++;
			}
		} else {
			if (p1->y<=p1->z) { // goin in, 2 points, 1 new and 1 old
				//new
				pdel.x=p0->x-p1->x;
				pdel.y=p0->y-p1->y;
				pdel.z=p0->z-p1->z;
				t=(p1->z-p1->y)/(pdel.y-pdel.z);
				xverts[newcidx].x=p1->x+pdel.x*t;
				xverts[newcidx].y=p1->y+pdel.y*t;
				xverts[newcidx].z=xverts[newcidx].y;
				projvec(&xverts[newcidx],&projxverts[newcidx]);
				tnp=p0->z*(p1->z-p1->y)/(p1->z*p0->y-p1->y*p0->z);
				if (!video3dinfo.perscorrect)
					t=tnp;
				if (clipmat->msflags&SMAT_HASTEX) {
					uv0=&xuvs[ll->vidx];
					uv1=&xuvs[lln->vidx];
					uvdel.u=uv0->u-uv1->u;
					uvdel.v=uv0->v-uv1->v;
					xuvs[newcidx].u=uv1->u+uvdel.u*t;
					xuvs[newcidx].v=uv1->v+uvdel.v*t;
				}
				if (clipmat->msflags&SMAT_HASSHADE) {
					c0=&xcverts[ll->vidx];
					c1=&xcverts[lln->vidx];
					cdel.x=c0->x-c1->x;
					cdel.y=c0->y-c1->y;
					cdel.z=c0->z-c1->z;
					xcverts[newcidx].x=c1->x+cdel.x*tnp;
					xcverts[newcidx].y=c1->y+cdel.y*tnp;
					xcverts[newcidx].z=c1->z+cdel.z*tnp;
				}
				newlist->vidx=newcidx;
				newlist->next=newlist+1;
				newlist++;
				//old
				newlist->vidx=lln->vidx;
				newlist->next=newlist+1;
				newlist++;
				newcidx++;
//				newv++;
			} else { // both outside, 0 points
			}
		}
		ll=lln;
	} while (ll!=llstart);
	if (newlist==newliststart)
		return 0;
	newlist--;
	newlist->next=newliststart;
	newlist++;
	return newliststart;
}

static struct linklist *software_cliptribot(struct linklist *ll)
{
	float t,tnp;
	struct pointf3 *p0,*p1,pdel;
	struct pointf3 *c0,*c1,cdel;
	struct uv *uv0,*uv1,uvdel;
	struct linklist *lln,*llstart,*newliststart;
	llstart=ll;
	newliststart=newlist;
//	logger("cliptriright\n");
	do {
//		logger("vidx %d\n",ll->vidx);
//		if (newcidx>=MAXXVERTS)
//			errorexit("clip, too many xverts");
		p0=&xverts[ll->vidx];
		lln=ll->next;
		p1=&xverts[lln->vidx];
		if (-p0->y<=p0->z) {
			if (-p1->y<=p1->z) { // both inside, 1 old point
				newlist->vidx=lln->vidx;
				newlist->next=newlist+1;
				newlist++;
			} else { // goin out, 1 new point
				pdel.x=p1->x-p0->x;
				pdel.y=p1->y-p0->y;
				pdel.z=p1->z-p0->z;
				t=-(p0->z+p0->y)/(pdel.y+pdel.z);
				xverts[newcidx].x=p0->x+pdel.x*t;
				xverts[newcidx].y=p0->y+pdel.y*t;
				xverts[newcidx].z=-xverts[newcidx].y;
				projvec(&xverts[newcidx],&projxverts[newcidx]);
				tnp=-p1->z*(p0->z+p0->y)/(p0->z*p1->y-p0->y*p1->z);
				if (!video3dinfo.perscorrect)
					t=tnp;
				if (clipmat->msflags&SMAT_HASTEX) {
					uv0=&xuvs[ll->vidx];
					uv1=&xuvs[lln->vidx];
					uvdel.u=uv1->u-uv0->u;
					uvdel.v=uv1->v-uv0->v;
					xuvs[newcidx].u=uv0->u+uvdel.u*t;
					xuvs[newcidx].v=uv0->v+uvdel.v*t;
				}
				if (clipmat->msflags&SMAT_HASSHADE) {
					c0=&xcverts[ll->vidx];
					c1=&xcverts[lln->vidx];
					cdel.x=c1->x-c0->x;
					cdel.y=c1->y-c0->y;
					cdel.z=c1->z-c0->z;
					xcverts[newcidx].x=c0->x+cdel.x*tnp;
					xcverts[newcidx].y=c0->y+cdel.y*tnp;
					xcverts[newcidx].z=c0->z+cdel.z*tnp;
				}
				newlist->vidx=newcidx;
				newlist->next=newlist+1;
				newlist++;
				newcidx++;
//				newv++;
			}
		} else {
			if (-p1->y<=p1->z) { // goin in, 2 points, 1 new and 1 old
				//new
				pdel.x=p0->x-p1->x;
				pdel.y=p0->y-p1->y;
				pdel.z=p0->z-p1->z;
				t=-(p1->z+p1->y)/(pdel.y+pdel.z);
				xverts[newcidx].x=p1->x+pdel.x*t;
				xverts[newcidx].y=p1->y+pdel.y*t;
				xverts[newcidx].z=-xverts[newcidx].y;
				projvec(&xverts[newcidx],&projxverts[newcidx]);
				tnp=-p0->z*(p1->z+p1->y)/(p1->z*p0->y-p1->y*p0->z);
				if (!video3dinfo.perscorrect)
					t=tnp;
				if (clipmat->msflags&SMAT_HASTEX) {
					uv0=&xuvs[ll->vidx];
					uv1=&xuvs[lln->vidx];
					uvdel.u=uv0->u-uv1->u;
					uvdel.v=uv0->v-uv1->v;
					xuvs[newcidx].u=uv1->u+uvdel.u*t;
					xuvs[newcidx].v=uv1->v+uvdel.v*t;
				}
				if (clipmat->msflags&SMAT_HASSHADE) {
					c0=&xcverts[ll->vidx];
					c1=&xcverts[lln->vidx];
					cdel.x=c0->x-c1->x;
					cdel.y=c0->y-c1->y;
					cdel.z=c0->z-c1->z;
					xcverts[newcidx].x=c1->x+cdel.x*tnp;
					xcverts[newcidx].y=c1->y+cdel.y*tnp;
					xcverts[newcidx].z=c1->z+cdel.z*tnp;
				}
				newlist->vidx=newcidx;
				newlist->next=newlist+1;
				newlist++;
				//old
				newlist->vidx=lln->vidx;
				newlist->next=newlist+1;
				newlist++;
				newcidx++;
//				newv++;
			} else { // both outside, 0 points
			}
		}
		ll=lln;
	} while (ll!=llstart);
	if (newlist==newliststart)
		return 0;
	newlist--;
	newlist->next=newliststart;
	newlist++;
	return newliststart;
}
#endif
static struct linklist *software_cliptrifront(struct linklist *ll)
{
	float t,tnp=0;
	struct pointf3 *p0,*p1,pdel;
	struct pointf3 *c0,*c1,cdel;
	struct uv *uv0,*uv1,uvdel;
	struct linklist *lln,*llstart,*newliststart;
	llstart=ll;
	newliststart=newlist;
//	logger("cliptriright\n");
	do {
//		logger("vidx %d\n",ll->vidx);
		p0=&xverts[ll->vidx];
		lln=ll->next;
		p1=&xverts[lln->vidx];
		if (p0->z>=0) {
			if (p1->z>=0) { // both inside, 1 old point
				newlist->vidx=lln->vidx;
				newlist->next=newlist+1;
				newlist++;
			} else { // goin out, 1 new point
				pdel.x=p1->x-p0->x;
				pdel.y=p1->y-p0->y;
				pdel.z=p1->z-p0->z;
				pdel.w=p1->w-p0->w;
				t=-p0->z/pdel.z;
				xverts[newcidx].x=p0->x+pdel.x*t;
				xverts[newcidx].y=p0->y+pdel.y*t;
				xverts[newcidx].z=0;//p0->y+pdel.z*t;
				xverts[newcidx].w=p0->w+pdel.w*t;
//				xverts[newcidx].w=proj_zfrontclip;
//				projvec(&xverts[newcidx],&projxverts[newcidx]);
				if (!video3dinfo.perscorrect || (clipmat->msflags&SMAT_HASSHADE)) {
					tnp=t*(p0->w+pdel.w)/(p0->w+t*pdel.w);
					if (tnp<0) tnp=0; if (tnp>1) tnp=1;
				}
//				p1->z*(p0->z-p0->x)/(p0->z*p1->x-p0->x*p1->z);
				if (!video3dinfo.perscorrect)
					t=tnp; 
				if (clipmat->msflags&SMAT_HASTEX) {
					uv0=&xuvs[ll->vidx];
					uv1=&xuvs[lln->vidx];
					uvdel.u=uv1->u-uv0->u;
					uvdel.v=uv1->v-uv0->v;
					xuvs[newcidx].u=uv0->u+uvdel.u*t;
					xuvs[newcidx].v=uv0->v+uvdel.v*t;
				}
				if (clipmat->msflags&SMAT_HASSHADE) {
					c0=&xcverts[ll->vidx];
					c1=&xcverts[lln->vidx];
					cdel.x=c1->x-c0->x;
					cdel.y=c1->y-c0->y;
					cdel.z=c1->z-c0->z;
					xcverts[newcidx].x=c0->x+cdel.x*tnp;
					xcverts[newcidx].y=c0->y+cdel.y*tnp;
					xcverts[newcidx].z=c0->z+cdel.z*tnp;
				} 
				newlist->vidx=newcidx;
				newlist->next=newlist+1;
				newlist++;
				newcidx++;
//				newv++;
			}
		} else {
			if (p1->z>=0) { // goin in, 2 points, 1 new and 1 old
				//new
				pdel.x=p0->x-p1->x;
				pdel.y=p0->y-p1->y;
				pdel.z=p0->z-p1->z;
				pdel.w=p0->w-p1->w;
//				t=(proj_zfrontclip-p1->w)/pdel.w;
				t=-p1->z/pdel.z;
				xverts[newcidx].x=p1->x+pdel.x*t;
				xverts[newcidx].y=p1->y+pdel.y*t;
				xverts[newcidx].z=0;//p1->z+pdel.z*t;
				xverts[newcidx].w=p1->w+pdel.w*t;
//				xverts[newcidx].w=proj_zfrontclip;
//				projvec(&xverts[newcidx],&projxverts[newcidx]);
				if (!video3dinfo.perscorrect || (clipmat->msflags&SMAT_HASSHADE)) {
					tnp=t*(p1->w+pdel.w)/(p1->w+t*pdel.w);
					if (tnp<0) tnp=0; if (tnp>1) tnp=1;
				}
//				tnp=p0->z*(p1->z-p1->x)/(p1->z*p0->x-p1->x*p0->z);
				if (!video3dinfo.perscorrect)
					t=tnp; 
				if (clipmat->msflags&SMAT_HASTEX) {
					uv0=&xuvs[ll->vidx];
					uv1=&xuvs[lln->vidx];
					uvdel.u=uv0->u-uv1->u;
					uvdel.v=uv0->v-uv1->v;
					xuvs[newcidx].u=uv1->u+uvdel.u*t;
					xuvs[newcidx].v=uv1->v+uvdel.v*t;
				}
				if (clipmat->msflags&SMAT_HASSHADE) {
					c0=&xcverts[ll->vidx];
					c1=&xcverts[lln->vidx];
					cdel.x=c0->x-c1->x;
					cdel.y=c0->y-c1->y;
					cdel.z=c0->z-c1->z;
					xcverts[newcidx].x=c1->x+cdel.x*tnp;
					xcverts[newcidx].y=c1->y+cdel.y*tnp;
					xcverts[newcidx].z=c1->z+cdel.z*tnp;
				} 
				newlist->vidx=newcidx;
				newlist->next=newlist+1;
				newlist++;
				newcidx++;
				//old
				newlist->vidx=lln->vidx;
				newlist->next=newlist+1;
				newlist++;
//				newcidx++;
//				newv++;
			} else { // both outside, 0 points
			}
		}
		ll=lln;
	} while (ll!=llstart);
	if (newlist==newliststart)
		return 0;
	newlist--;
	newlist->next=newliststart;
	newlist++;
	return newliststart;
}
#if 0
static struct linklist *software_cliptrifront(struct linklist *ll)
{
	float t,tnp;
	struct pointf3 *p0,*p1,pdel;
	struct pointf3 *c0,*c1,cdel;
	struct uv *uv0,*uv1,uvdel;
	struct linklist *lln,*llstart,*newliststart;
	llstart=ll;
	newliststart=newlist;
//	logger("cliptriright\n");
	do {
//		logger("vidx %d\n",ll->vidx);
		p0=&xverts[ll->vidx];
		lln=ll->next;
		p1=&xverts[lln->vidx];
		if (p0->w>=proj_zfrontclip) {
			if (p1->w>=proj_zfrontclip) { // both inside, 1 old point
				newlist->vidx=lln->vidx;
				newlist->next=newlist+1;
				newlist++;
			} else { // goin out, 1 new point
				pdel.x=p1->x-p0->x;
				pdel.y=p1->y-p0->y;
				pdel.w=p1->w-p0->w;
				t=(proj_zfrontclip-p0->w)/pdel.w;
				xverts[newcidx].x=p0->x+pdel.x*t;
				xverts[newcidx].y=p0->y+pdel.y*t;
				xverts[newcidx].w=proj_zfrontclip;
				projvec(&xverts[newcidx],&projxverts[newcidx]);
				tnp=p1->w*(proj_zfrontclip-p0->w)/(proj_zfrontclip*(p1->w-p0->w));
				if (tnp<0)
					tnp=0;
				if (!video3dinfo.perscorrect)
					t=tnp;
				if (clipmat->msflags&SMAT_HASTEX) {
					uv0=&xuvs[ll->vidx];
					uv1=&xuvs[lln->vidx];
					uvdel.u=uv1->u-uv0->u;
					uvdel.v=uv1->v-uv0->v;
					xuvs[newcidx].u=uv0->u+uvdel.u*t;
					xuvs[newcidx].v=uv0->v+uvdel.v*t;
				}
				if (clipmat->msflags&SMAT_HASSHADE) {
					c0=&xcverts[ll->vidx];
					c1=&xcverts[lln->vidx];
					cdel.x=c1->x-c0->x;
					cdel.y=c1->y-c0->y;
					cdel.z=c1->z-c0->z;
					xcverts[newcidx].x=c0->x+cdel.x*tnp;
					xcverts[newcidx].y=c0->y+cdel.y*tnp;
					xcverts[newcidx].z=c0->z+cdel.z*tnp;
				}
				newlist->vidx=newcidx;
				newlist->next=newlist+1;
				newlist++;
				newcidx++;
//				newv++;
			}
		} else {
			if (p1->w>=proj_zfrontclip) { // goin in, 2 points, 1 new and 1 old
				//new
				pdel.x=p0->x-p1->x;
				pdel.y=p0->y-p1->y;
				pdel.w=p0->w-p1->w;
				t=(proj_zfrontclip-p1->w)/pdel.w;
				xverts[newcidx].x=p1->x+pdel.x*t;
				xverts[newcidx].y=p1->y+pdel.y*t;
				xverts[newcidx].w=proj_zfrontclip;
				projvec(&xverts[newcidx],&projxverts[newcidx]);
				tnp=p0->z*(proj_zfrontclip-p1->w)/(proj_zfrontclip*(p0->w-p1->w));
				if (tnp<0)
					tnp=0;
				if (!video3dinfo.perscorrect)
					t=tnp;
				if (clipmat->msflags&SMAT_HASTEX) {
					uv0=&xuvs[ll->vidx];
					uv1=&xuvs[lln->vidx];
					uvdel.u=uv0->u-uv1->u;
					uvdel.v=uv0->v-uv1->v;
					xuvs[newcidx].u=uv1->u+uvdel.u*t;
					xuvs[newcidx].v=uv1->v+uvdel.v*t;
				}
				if (clipmat->msflags&SMAT_HASSHADE) {
					c0=&xcverts[ll->vidx];
					c1=&xcverts[lln->vidx];
					cdel.x=c0->x-c1->x;
					cdel.y=c0->y-c1->y;
					cdel.z=c0->z-c1->z;
					xcverts[newcidx].x=c1->x+cdel.x*tnp;
					xcverts[newcidx].y=c1->y+cdel.y*tnp;
					xcverts[newcidx].z=c1->z+cdel.z*tnp;
				}
				newlist->vidx=newcidx;
				newlist->next=newlist+1;
				newcidx++;
				newlist++;
				//old
				newlist->vidx=lln->vidx;
				newlist->next=newlist+1;
				newlist++;
				newcidx++;
//				newv++;
			} else { // both outside, 0 points
			}
		}
		ll=lln;
	} while (ll!=llstart);
	if (newlist==newliststart)
		return 0;
	newlist--;
	newlist->next=newliststart;
	newlist++;
	return newliststart;
}
#endif
static void projvecs(struct pointf3 *vi,struct pointf3 *vo,S32 npoints);
static S32 software_cliptri(struct face *f,S32 orcode)
{
	S32 xnface=0;
	struct linklist *ll,*ll1,*ll2;
	struct pointf3 norm,del1,del2;
// check for clockwise, make sure is right, with homo and all!
	del1.x=xverts[f->vertidx[1]].x-xverts[f->vertidx[0]].x;
	del1.y=xverts[f->vertidx[1]].y-xverts[f->vertidx[0]].y;
	del1.z=xverts[f->vertidx[1]].w-xverts[f->vertidx[0]].w;
	del2.x=xverts[f->vertidx[2]].x-xverts[f->vertidx[0]].x;
	del2.y=xverts[f->vertidx[2]].y-xverts[f->vertidx[0]].y;
	del2.z=xverts[f->vertidx[2]].w-xverts[f->vertidx[0]].w;
	cross3d(&del1,&del2,&norm);
	pointf3 vip;
	vip.x=xverts[f->vertidx[0]].x;
	vip.y=xverts[f->vertidx[0]].y;
	vip.z=xverts[f->vertidx[0]].w;
	video3dinfo.clipdot=dot3d(&norm,&vip);
	if (video3dinfo.clipdot>=0)
		return 0;
// convert face to link list
	ll=&thelist[0];
	thelist[0].vidx=f->vertidx[0];
	thelist[1].vidx=f->vertidx[1];
	thelist[2].vidx=f->vertidx[2];
	thelist[0].next=&thelist[1];
	thelist[1].next=&thelist[2];
	thelist[2].next=&thelist[0];
	newlist=thelist+3;
	newcidx=-NUMEXTRA;
// clip each side
	if (orcode&CODE_NEAR)
		ll=software_cliptrifront(ll);
	if (!ll)
		return 0;
	if (orcode&CODE_RIGHT)
		ll=software_cliptriright(ll);
	if (!ll)
		return 0;
	if (orcode&CODE_LEFT)
		ll=software_cliptrileft(ll);
	if (!ll)
		return 0;
	if (orcode&CODE_TOP)
		ll=software_cliptritop(ll);
	if (!ll)
		return 0;
	if (orcode&CODE_BOT)
		ll=software_cliptribot(ll);
	if (!ll)
		return 0;
	projvecs(&xverts[-NUMEXTRA],&projxverts[-NUMEXTRA],newcidx+NUMEXTRA);
// convert list back to faces
	ll1=ll->next;
	ll2=ll1->next;
	while(ll2!=ll) {
		clipfaces[xnface].vertidx[0]=ll->vidx;
		clipfaces[xnface].vertidx[1]=ll1->vidx;
		clipfaces[xnface].vertidx[2]=ll2->vidx;
		ll1=ll2;
		ll2=ll2->next;
//		newf++;
		xnface++;
	}
	return xnface;
}

static void software_drawmat(mater2* m,vector<face>& tfs,S32 foffset,S32 nfaces)
{
//	S32 i;
	float tran;
//	logger("drawmat: foffset %d, nfaces %d\n",foffset,nfaces);
	tran=m->color.w;
	if (tran>=.75f)
		m->msflags&=~SMAT_HASXLU;
	else if (tran>=.25f)
		m->msflags|=SMAT_HASXLU;
	else {
		return;
	}
	registermat(m);	// material to draw tris with (required)
	drawtris(tfs,foffset,nfaces);
}
//#ifdef DOCLIPPING

static void software_drawmatclip(mater2 *m,vector<face>& tfs,S32 foffset,S32 ntris)
{
	S32 i,k,*vp,andc,orc,code,ncliptris;
	float tran;

	tran=m->color.w;
	if (tran>=.75f)
		m->msflags&=~SMAT_HASXLU;
	else if (tran>=.25f)
		m->msflags|=SMAT_HASXLU;
	else
		return;
	registermat(m);	// material to draw tris with (required)
	clipmat=m;
	for (i=0;i<ntris;i++) {
		face* f=&(tfs[i+foffset]);
		vp=f->vertidx;
		andc=0xff;
		orc=0;
		for (k=0;k<3;k++) {
			code=andorcode[vp[k]];
			andc&=code;
			orc|=code;
		}
		if (!andc &&  ((orc&CODE_FAR)==0) ) { // part or all of triangle viewable
			if (orc) { // some of triangle viewable, clip
				ncliptris=software_cliptri(f,orc);
				if (ncliptris)
					drawtris(clipfaces,ncliptris);
			} else { // all of triangle viewable
//				logger(" f 0 %d, 1 %d, 2 %d\n",f->vertidx[0],f->vertidx[1],f->vertidx[2]);
				drawtris(f,1);
			}
		}
	}
}
//#endif

clipresult model2::checkandor()
{
	S32 i;
	S32 code,andc=0xff,orc=0;
	S32 nvert=verts.size();
	for (i=0;i<nvert;i++) {
//		if (xverts[i].w>=0) {
			if (xverts[i].z>xverts[i].w /*&& !(mats[0].msflags&SMAT_ISSKY)*/)
				code=CODE_FAR;
			else {
				code=0;
				if (xverts[i].z<0.0f)
					code|=CODE_NEAR;
				if (xverts[i].x>xverts[i].w)
					code|=CODE_RIGHT;
				if (-xverts[i].x>xverts[i].w)
					code|=CODE_LEFT;
				if (xverts[i].y>xverts[i].w)
					code|=CODE_TOP;
				if (-xverts[i].y>xverts[i].w)
					code|=CODE_BOT;
			}
/*		} else {
			code=CODE_NEAR;
			if (xverts[i].x<xverts[i].w)
				code|=CODE_RIGHT;
			if (-xverts[i].x<xverts[i].w)
				code|=CODE_LEFT;
			if (xverts[i].y<xverts[i].w)
				code|=CODE_TOP;
			if (-xverts[i].y<xverts[i].w)
				code|=CODE_BOT;
		} */
		andc&=code;
		orc|=code;
		andorcode[i]=code;
	}
	if (andc)
		return CLIP_OUT;
	if (!orc)
		return CLIP_IN;
	return CLIP_CLIP; 
}

// stuff just for 3d

static void registerproj(float offx,float offy,float xres,float yres,float zfront,bool isorto)
{
	proj_zoomx=xres*.5f;
	proj_zoomy=yres*.5f;
	proj_offsetx=offx+proj_zoomx;
	proj_offsety=offy+proj_zoomy;
	proj_zfront=zfront*SOFT_MAXZBUFFER;
	proj_inortho=isorto;
//	proj_zfrontclip=zfront;
	video3dinfo.zbuffmin=1e20f;
	video3dinfo.zbuffmax=-1e20f;
}

/*
void invprojvec(struct pointf3 *vi,struct pointf3 *vo)
{
	vo->x = mouse3doffx*((vi->x - proj_offsetx)/proj_zoomx);
	vo->y = mouse3doffy*((vi->y - proj_offsety)/-proj_zoomy);
	vo->z = mouse3doffz*(1.0f-proj_zoff);
}
*/

/*
// xyz version, out.z gets 1/in.z
void projvec(struct pointf3 *vi,struct pointf3 *vo)
{
	float invz,fx,fy;
	invz=1.0f/vi->z;
	fx=proj_zoomx*invz;
	fy=proj_zoomy*invz;
	vo->x=proj_offsetx+fx*vi->x;
	vo->y=proj_offsety-fy*vi->y;
	vo->z=invz+proj_zoff;
}
*/
// homogeneous version
// out.x screen x 
// out.y screen y
// out.z w // pers
// out.w 1/w // zbuffer

//static float pminz,pmaxz;
static void projvec_pers(struct pointf3 *vi,struct pointf3 *vo)
{
	float invw,fx,fy;
	invw=1.0f/vi->w;
	fx=proj_zoomx*invw;
	fy=proj_zoomy*invw;
	vo->x=proj_offsetx+fx*vi->x;
	vo->y=proj_offsety-fy*vi->y;
//	vo->w=1;//invw*vi->z*65535.0f; // not yet used
	vo->z=invw*proj_zfront;
	// or _isnan
/*	if (!_finite(proj_zfront))
		logger("proj_zfront is nan\n");
	if (!_finite(vo->z))
		logger("projvec_pers is nan\n");
	if (!_finite(video3dinfo.zbuffmax))
		logger("video3dinfo.zbuffmax is nan\n"); */
	if (vo->z<video3dinfo.zbuffmin)
		video3dinfo.zbuffmin=vo->z;
	if (vo->z>video3dinfo.zbuffmax)
		video3dinfo.zbuffmax=vo->z;
}

static void projvec_ortho(struct pointf3 *vi,struct pointf3 *vo)
{
	vo->x=proj_offsetx+proj_zoomx*vi->x;
	vo->y=proj_offsety-proj_zoomy*vi->y;
	vo->z=SOFT_MAXZBUFFER*(1-vi->z);
	if (vo->z<video3dinfo.zbuffmin)
		video3dinfo.zbuffmin=vo->z;
	if (vo->z>video3dinfo.zbuffmax)
		video3dinfo.zbuffmax=vo->z;
}

static void projvecs(struct pointf3 *vi,struct pointf3 *vo,S32 npoints)
{
	perf_start(PROJVECS);
//	S32 i;
//	S32 np=npoints;
//	struct pointf3* vos=vo;
//	logger("projvecs zoomx %f, zoomy %f, proj_offsetx %f, proj_offsety %f, proj_zoff %f\n",
//	  proj_zoomx,proj_zoomy,proj_offsetx,proj_offsety,proj_zoff);
//	logger("in\n");
//	for (i=0;i<np;++i)
//		logger("%d (%g %g %g)\n",i,vi[i].x,vi[i].y,vi[i].z);
	if (proj_inortho) {
		while(npoints) {
			projvec_ortho(vi,vo);
			vi++;
			vo++;
			npoints--;
		}
	} else {
		while(npoints) {
			projvec_pers(vi,vo);
			vi++;
			vo++;
			npoints--;
		}
	}
//	logger("min invw %f, max invw %f\n",pminz,pmaxz);
//	logger("out\n");
//	for (i=0;i<np;++i)
//		logger("%d (%g %g %g)\n",i,vos[i].x,vos[i].y,vos[i].z);
	perf_end(PROJVECS);
}


/*void projvecd3d(struct pointf3 *vi,struct tlnospec *vo)
{
	float invz,fx,fy;
	invz=1.0f/vi->z;
	fx=proj_zoomx*invz;
	fy=proj_zoomy*invz;
	vo->sx=proj_offsetx+fx*vi->x;
	vo->sy=proj_offsety-fy*vi->y;
	vo->sz=invz+proj_zoff;
	vo->rhw=invz;//.5f;
}

void projvecsd3d(struct pointf3 *vi,struct tlnospec *vo,S32 npoints)
{
	while(npoints) {
		projvecd3d(vi,vo);
		vi++;
		vo++;
		npoints--;
	}
}
*/

// object to world
// object to clip (the complete matrix, standard for vertex shaders)
// tree color
// tree texture
// tree texture offset
// noclip flag, CLIP_IN draw everything(noclip), CLIP_CLIP investigate model further, CLIP_OUT don't draw
//		hardware ignores this flag
void model2::draw(mat4* o2w,mat4* o2c,pointf3* tc,float dco,textureb* tt,pointf2* tto,clipresult noclip)
//clipresult software_drawmodel(tree2* t,viewport2* v,mat4* o2c)
//void software_drawmodel(struct model *m,struct viewport *v,struct mat4 *o2c,float dissolve,struct mat4 *envmat,float zoff)
// envmat is just for envmap
// worry about drawpass later...
// bbox, selective clipping (don't calc uvs for a shade material etc.)
// lighting, bones later,
{
	if (faces.size() == 0)
		return;
	pointf3 mcs[MAXMATS];
	textureb* tts=0;
	group2* curgroup;
//	float extraang;
	S32 i,perssave;
//	float zoff=0;//t->zoff;
//	float dissolve=t->dissolve;
//	model2* m=t->mod;
#if 0
	if (!verts.size())
		return CLIP_OUT; // nothing to draw
	if (!mats.size())
		return CLIP_OUT;
//	struct mat4* envmat=&t->o2w;
//	logger("in software draw model\n");
//	logmat4(o2c,"o2c");
#endif
	perf_start(DRAWMODEL);
// xlate to viewport coords (m->verts --> xverts)
// some quick checks
#if 0
	if (tc->w<treeinfo.dissolvecutoff) {
		perf_end(DRAWMODEL);
		return CLIP_OUT;
	}
//	mat4 o2c;
//	mulmat3d(o2w,&soft_vpw2c,&o2c);
//	mat4 o2c;
//	mulmat4(o2w,&video3dinfo.w2c,&o2c); // keep at xyz1 for now
	clipresult noclip=checkbbox(o2c);
//	bboxret=noclip;
//	if (noclip==CLIP_OUT || noclip==CLIP_CLIP) {
	if (noclip==CLIP_OUT) {
		perf_end(DRAWMODEL);
		return noclip;
	}
#endif
// bbox can be seen, check xformed verts`
	if (verts.size()>MAXXVERTS)
		errorexit("model '%s' has too many verts %d..",name.c_str(),verts.size());
	if (mats.size()>MAXMATS)
		errorexit("model '%s' has too many mats %d..",name.c_str(),mats.size());
	xformvecs(o2c,&verts[0],xverts,verts.size());
//#ifdef DOCLIPPING
	if (noclip==CLIP_CLIP) // put back later
		noclip=checkandor(); // uses xverts, put back later
//#endif
	//	noclip=CLIP_IN;
	if (noclip==CLIP_OUT) {
		perf_end(DRAWMODEL);
		return;// noclip;
	}
//	registerproj((float)v->xstart,(float)v->ystart,(float)v->xres,(float)v->yres,zoff*65535.0f);
/*	S32 n=m->mats.size();
	for (i=0;i<n;++i) {
		mater2* clipmat=&m->mats[i];
		if (clipmat->msflags&SMAT_CALCENVMAP) {
			software_doenvmap(v,envmat,m->vertnorms,m->uvs[0],m->groups[i].vertidx,m->groups[i].nvert);
		}
		if (clipmat->msflags1&SMAT_CALCENVMAP) {
//			extraang=v->camrot.y;
//			if (v->camattach && useattachcam)
//				extraang+=v->camattach->rot.y;
			software_doenvmap(v,envmat,m->vertnorms,m->uvs[1],m->groups[i].vertidx,m->groups[i].nvert);
		}
	} */
	registeruvs(&uvs0[0]); // uv's to draw tris with (optional)
	xuvs=&uvs0[0]; // for clipping
//	if (t->cverts.size()) {
//		registercverts(&t->cverts[0]);
//		xcverts=&t->cverts[0];
//	} else {
		registercverts(&cverts[0]);
		xcverts=&cverts[0]; // for clipping
//	}
	perssave=video3dinfo.perscorrect;
	if (proj_inortho)
		video3dinfo.perscorrect=0;
	S32 n=mats.size();
	for (i=0;i<n;++i) {
		mcs[i]=mats[i].color;
		mats[i].color.x*=tc->x;
		mats[i].color.y*=tc->y;
		mats[i].color.z*=tc->z;
		mats[i].color.w*=tc->w;
	}
	if (tt) {
		tts=mats[0].thetexarr[0];
		mats[0].thetexarr[0]=tt;
	}
	dolightsdestrec(o2w);
	if (noclip==CLIP_IN) {
// project triangles
		if (video3dinfo.perscorrect==2)
			video3dinfo.perscorrect=0;
		registerverts(xverts); // verts to draw tris with (required)
		projvecs(xverts,xverts,verts.size());
// draw triangles
// we'll most likely draw something now
		for (i=0;i<n;++i) {
			mater2* curmat=&mats[i];
			curgroup=&groups[i];
//			logger("group %d is fo %d, fn %d, vo %d, vs %d\n",i,curgroup->faceidx,curgroup->nface,curgroup->vertidx,curgroup->nvert);
			software_drawmat(curmat,faces,curgroup->faceidx,curgroup->nface);
//			software_drawmat(curmat,m->faces,curgroup->faceidx,dissolve,curgroup->nface);
		}
//		software_drawmod(m,xverts,m->uvs,mcverts,m->faces,m->groups,dissolve);
	} else {// CLIP_CLIP:
//#ifdef DOCLIPPING
		if (video3dinfo.perscorrect==2) {
//			curmat=&m->mats[0];
//			if (curmat->msflags&SMAT_ISSKY)
//				perscorrect=0;
//			else
				video3dinfo.perscorrect=1;
		}
//		xcverts=mcverts;
//		xuvs=&m->uvs[0];  //in software who cares about multi-textures
//		xuvs[1]=m->uvs[1];  //in software who cares about multi-textures
		registerverts(projxverts); // verts to draw tris with (required)
		projvecs(xverts,projxverts,verts.size());
		for (i=0;i<n;i++) {
			mater2* curmat=&mats[i];
			group2* curgroup=&groups[i];
			software_drawmatclip(curmat,faces,curgroup->faceidx,curgroup->nface);
		}
//#endif
	}
	video3dinfo.perscorrect=perssave;
	for (i=0;i<n;++i) {
		mats[i].color=mcs[i];
		}
	if (tt) {
		mats[0].thetexarr[0]=tts;
	}
	perf_end(DRAWMODEL);
	return;// noclip;
}

modelb* software_createmodel(const C8* n)
{
	return modelb::rc.newrc<model2>(n);
}

textureb* software_createtexture(const C8* n)
{
	return textureb::rc.newrc<texture2>(n);
}

// add cverts if use calclights
void model2::close()
{
	S32 i,n=mats.size();
	S32 nv=verts.size();
//	nverts=nv;
//	nfaces=faces.size();
//	for (i=0;i<nverts;++i)
//		verts[i].w=1.0f;
	// add colored verts if lighting is turned on
	for (i=0;i<n;++i) {
		if (mats[i].msflags&SMAT_CALCLIGHTS) {
			cverts.setsize(nv);
//			memset(&cverts[0],0,nv*sizeof(pointf3));
			fill(&cverts[0],&cverts[nv],pointf3x());
			return;
		}
	}
	updatefaces = false; // compatibility
}

void model2::update()
{
}

// software version
/*clipresult model2::checkbbox2(mat4* o2c)
{
	S32 i;
	S32 andc=0xff,orc=0;
	S32 code;
	struct pointf3 xbox[8];
	xbox[0].x=boxmin.x; xbox[0].y=boxmin.y; xbox[0].z=boxmin.z;
	xbox[1].x=boxmin.x; xbox[1].y=boxmin.y; xbox[1].z=boxmax.z;
	xbox[2].x=boxmin.x; xbox[2].y=boxmax.y; xbox[2].z=boxmin.z;
	xbox[3].x=boxmin.x; xbox[3].y=boxmax.y; xbox[3].z=boxmax.z;
	xbox[4].x=boxmax.x; xbox[4].y=boxmin.y; xbox[4].z=boxmin.z;
	xbox[5].x=boxmax.x; xbox[5].y=boxmin.y; xbox[5].z=boxmax.z;
	xbox[6].x=boxmax.x; xbox[6].y=boxmax.y; xbox[6].z=boxmin.z;
	xbox[7].x=boxmax.x; xbox[7].y=boxmax.y; xbox[7].z=boxmax.z;
	xformvecs(o2c,xbox,xbox,8);
//	logger("xbox[0].z %f, soft_zfront %f\n",xbox[0].z,soft_zfront);
	for (i=0;i<8;i++) {
		code=0;
		if (xbox[i].z>soft_zback && !(mats[0].msflags&SMAT_ISSKY))
//		if (xbox[i].z>soft_zback)
			code=CODE_FAR;
		if (xbox[i].z<soft_zfront)
			code|=CODE_NEAR;
		if (xbox[i].x>xbox[i].z)
			code|=CODE_RIGHT;
		if (-xbox[i].x>xbox[i].z)
			code|=CODE_LEFT;
		if (xbox[i].y>xbox[i].z)
			code|=CODE_TOP;
		if (-xbox[i].y>xbox[i].z)
			code|=CODE_BOT;
		andc&=code;
		orc|=code;
	}
	if (andc)
		return CLIP_OUT;
	if (orc)
		return CLIP_CLIP;
	return CLIP_IN;
}
*/
// apply lights to cverts
void model2::dolightsdestrec(mat4* o2w)
{
	struct pointf3 locallightdirs[MAXLIGHTS];
	S32 j,s,e,i,nl;
	float dotp;
	struct pointf3 cg;
	if (!lightinfo.uselights)
		return;
//	if (t->flags&TF_DONECALCLIGHTSONCE)
//		return;
	perf_start(DOLIGHTS);
//	if (t->flags&TF_CALCLIGHTSONCE)
//		t->flags|=TF_DONECALCLIGHTSONCE;
	if (norms.size() && norms.size()==(U32)cverts.size()) {
// convert lights from world to local, this should use a different matrix, (don't assume orthogonal)
		for (nl=0;nl<lightinfo.ndirlights;nl++) {
			pointf3* d=(pointf3*)(&lightinfo.light2world[nl].e[2][0]);
			xformdirinv(o2w,d,&locallightdirs[nl]);
			normalize3d(&locallightdirs[nl],&locallightdirs[nl]);
		}
		S32 n=mats.size();
		vectore<pointf3>& cv=cverts;
		for (j=0;j<n;j++) {
			if (mats[j].msflags&SMAT_CALCLIGHTS) {
				s=groups[j].vertidx;
				e=s+groups[j].nvert;
				for (i=s;i<e;i++) {
					cg=lightinfo.ambcolor;
					for (nl=0;nl<lightinfo.ndirlights;nl++) {
						//error("hey: %f %f %f w:%f %f %f",locallightdirs[nl].x,locallightdirs[nl].y,locallightdirs[nl].z,worldlightdirs[nl].x,worldlightdirs[nl].y,worldlightdirs[nl].z);
						dotp=-dot3d(&locallightdirs[nl],&norms[i]);
						if (dotp<0)
							dotp=0;
						cg.x+=dotp*lightinfo.lightcolors[nl].x;
						cg.y+=dotp*lightinfo.lightcolors[nl].y;
						cg.z+=dotp*lightinfo.lightcolors[nl].z;
					}
/*
// this is the wrong thing to do, somehow should clip the clamping (later)
// produces blockiness at the saturation point, for now, don't saturate much
					if (t->mod->mats[j].msflags&SMAT_HASTEX) {
						if(!(t->flags&TF_MIXLIGHTS))
						{
							t->cverts[i].x=min(max(.0001f,cg.x),.9999f);
							t->cverts[i].y=min(max(.0001f,cg.y),.9999f);
							t->cverts[i].z=min(max(.0001f,cg.z),.9999f);
						}
						else
						{
							//t->cverts[i].x=range(.0001f,cg.x,.9999f);
							//t->cverts[i].y=range(.0001f,cg.y,.9999f);
							//t->cverts[i].z=range(.0001f,cg.z,.9999f);
						}

					} else {
*/
					cv[i].x=min(max(.0001f,cg.x*mats[j].color.x),.9999f);
					cv[i].y=min(max(.0001f,cg.y*mats[j].color.y),.9999f);
					cv[i].z=min(max(.0001f,cg.z*mats[j].color.z),.9999f);
//					t->cverts[i].x=min(max(.0001f,cg.x*t->mod->mats[j].color.x),.9999f);
//					t->cverts[i].y=min(max(.0001f,cg.y*t->mod->mats[j].color.y),.9999f);
//					t->cverts[i].z=min(max(.0001f,cg.z*t->mod->mats[j].color.z),.9999f);
//					}
				}
/*				if (video_maindriver==VIDEO_D3D) {
					for (i=s;i<e;i++)
						t->d3dverts[i].c=D3DRGB(t->cverts[i].x,t->cverts[i].y,t->cverts[i].z);
				} */
			}
		}
//		if (checkcalclightsonce)
//			resetcalclightsonce(t);
	}
//	for (i=0;i<t->nchildren;i++)
//		dolightsdestrec(t->children[i]);
	perf_end(DOLIGHTS);
}

/*
#define D2_3D
#include <m_eng.h>
#include <m_perf.h>

referencecount<texture2> texture2::rc; // and add this
*/

void texture2::addbitmap(const bitmap32* b,bool iscube) // copy bitmap b into texture
{
	origsize=b->size;
	if (texdata)
		bitmap32free(texdata);
	if (colorkeyinfo.lasthascolorkey) {
		hasalpha=true;
		texformat=TEX32CK;
	} else {
		texformat=TEX32NCK;
	}
	logu=ilog2(b->size.x);
	logv=ilog2(b->size.y);
	if (logu>video3dinfo.maxtexlogu)
		logu=video3dinfo.maxtexlogu;
	if (logv>video3dinfo.maxtexlogv)
		logv=video3dinfo.maxtexlogv;
	if ((1<<logu)!=b->size.x || (1<<logv)!=b->size.y) {
		if (b->size.x<=1024) {
			// logger("WARNING ?, texture2::addbitmap '%s' not a power of 2, is %d,%d increased to %d,%d\n",
			//	name.c_str(),b->size.x,b->size.y,2<<logu,2<<logv);
			texdata=bitmap32alloc(2<<logu,2<<logv,C32BLACK);
			clipscaleblit32(b,texdata);
			++logu;++logv;
		} else {
			// logger("WARNING ?, texture2::addbitmap '%s' not a power of 2, is %d,%d reduced to %d,%d\n",
			//	name.c_str(),b->size.x,b->size.y,1<<logu,1<<logv);
			texdata=bitmap32alloc(1<<logu,1<<logv,C32BLACK);
			clipscaleblit32(b,texdata);
		}
	} else {
		texdata=bitmap32alloc(b->size.x,b->size.y,C32BLACK);
		clipblit32(b,texdata,0,0,0,0,b->size.x,b->size.y);
	}
	if (hasalpha) {
		S32 i,m=texdata->size.x*texdata->size.y;
		C32* p=texdata->data;
		for (i=0;i<m;++i)
			p[i].a=p[i].a>0x40 ? 255 : 0;
			
	}
	tsize=texdata->size;
}

#if 0
void texture2::load()
{
	if (fileexist(name.c_str())) {
		bitmap32* b;
		b=gfxread32(name.c_str());
/*	} else {
		pushandsetdir("common");
		b=gfxread32("maptest.tga");
		popdir();
	} */
		addbitmap(b,false);
		bitmap32free(b);
	} else {
		C8 fname[300];
		mgetname(name.c_str(),fname);
		C8 newname[300];
		mbuildfilename(0,fname,"dds",newname);
		if (fileexist(newname)) {
			bitmap32* b;
			b=gfxread32(newname);
			addbitmap(b,false);
			bitmap32free(b);
		}

/*U32 mgetnameext(const C8* filename,C8* nameext);
U32 mgetname(const C8* filename,C8* name);
U32 mgetext(const C8*filename,C8* ext);
void mbuildfilename(const C8* path,const C8* name,const C8* ext,C8* filename);
void mbuildfilenameext(const C8* path,const C8* nameext,C8* filename);
*/
	}
}
#endif
void texture2::load()
{
	string outstr;
	bool cub,six;
	bool res=checkname(name,outstr,cub,six);
	if (!res)
		return;
	bitmap32* b;
	b=gfxread32c(outstr.c_str(),six);
	addbitmap(b,false);
	bitmap32free(b);
}

texture2::~texture2()
{
	if (texdata)
		bitmap32free(texdata);
}

bitmap32* texture2::locktexture()
{
	if (!texdata)
		return &dummyb;
	if (islocked)
		errorexit("texture '%s' already locked",name.c_str());
	islocked=1;
//	logger("lock texture '%s': wid %d, hit %d, pit %d\n",t->name,sd2.dwWidth,sd2.dwHeight,sd2.lPitch);
	return texdata;
}

void texture2::unlocktexture()
{
	if (!texdata)
		return;
	if (!islocked)
		errorexit("texture '%s' already unlocked",name.c_str());
	islocked=0;

}
