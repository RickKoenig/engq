/*#include <stdio.h>
#include <string.h>

#include <engine7cpp.h>
#include <videoicpp.h>
#include <misclib7cpp.h>

#include "usefulcpp.h"
*/
#define D2_3D
#define INCLUDE_WINDOWS
#include <m_eng.h>
#include "n_deflatecpp.h"
#include "n_usefulcpp.h" 
#include "n_constructorcpp.h"
#include "n_facevertscpp.h"
#include "n_polylistcpp.h"
#include "n_meshtestcpp.h"

static viewport2 mt_viewport;

#define MTVIEWX 400
#define MTVIEWY 300
#define MTVIEWW 400
#define MTVIEWH 300
static tree2 *mt_viewport_roottree;
static modelb* dum;

// input verts
#define MAXV 50
static int nevert;
static pointf3 everts[MAXV];
static int cevert;
static int movemode;
static int inttest=1; // keep at 1

static tree2* themesh;

/*#define NGTEST 100000
static int *ps;
static int psicomp(void *p,int a,int b)
{
	int *pi=(int *)p;
	if (ps[pi[b]]>ps[pi[a]])
		return 1;
	if (ps[pi[b]]<ps[pi[a]])
		return -1;
	return pi[b]-pi[a];
}

static void goldbach()
{
	int i;
	int md=2,d,ds=(md+1)*(md+1);
	char *comp;
	int *psi;
	logger("goldbach test...........\n");
	comp=(char *)memzalloc(sizeof(char)*NGTEST);
	ps=(int *)memzalloc(sizeof(int)*NGTEST);
	psi=(int *)memzalloc(sizeof(int)*NGTEST);
	for (i=0;i<NGTEST;i++)
		psi[i]=i;
	comp[0]=comp[1]=1;
	for (i=3;i<NGTEST;i++) {
		if (i>=ds) {
			md++;
			ds=(md+1)*(md+1);
		}
		for (d=2;d<=md;d++)
			if (i%d==0) {
				comp[i]=1;
				break;
			}
	}
//	for (i=0;i<NGTEST;i++)
//		if (!comp[i])
//			logger ("%6d is prime\n",i);
	ps[4]=1;
	for (i=6;i<NGTEST;i+=2)
		for (d=3;d<=(i>>1);d+=2)
			if (!comp[d] && !comp[i-d])
				ps[i]++;
	nuqsort(psi,NGTEST,psicomp);
	for (i=0;i<NGTEST;i++)
		if (ps[psi[i]]!=0 && ps[psi[i]]<1000)
			logger("%6d has %6d prime sums\n",psi[i],ps[psi[i]]);
	memfree(comp);
	memfree(ps);
	memfree(psi);
}
*/

// build models based on piece data
// take a list of verts and make tris out of them, 2 less than the number of verts
// can be concave but no crossing edges, return pointer to allocated faces
// convert to mesh
//int (* buildfaces(pointf3 *vrts,int nvrt,int *nfcep))[3]
face* buildfaces(const pointf3 *vrts,int nvrt,int *nfcep)
{
//	int *vidx=(int *)memalloc(sizeof(int)*nvrt);
	int *vidx=new int[nvrt];
	int nvidx;
	int i,j,k;
	int nfce;
	int idxm1,idxp1;
	float a,besta;
	int bestidx;
	const pointf3 *pm1,*p0,*pp1;
//	int (*fces)[3]=(int (*)[3])memalloc(sizeof(int[3])*(nvrt-2));
	face* fces=new face[nvrt-2];
	for (i=0;i<nvrt;i++)
		vidx[i]=i;
	nvidx=nvrt;
	nfce=0;
	while(nvidx>=3) {
		bestidx=-1;
		besta=1e20f;
		for (i=0;i<nvidx;i++) {
			idxm1=(i+nvidx-1)%nvidx;
			idxp1=(i+1)%nvidx;
			pm1=&vrts[vidx[idxm1]];
			p0=&vrts[vidx[i]];
			pp1=&vrts[vidx[idxp1]];
			a=(pm1->x-p0->x)*(pp1->z-p0->z)-(pm1->z-p0->z)*(pp1->x-p0->x);
			if (a<besta && a>=0) {
				int passint=1;
				if (inttest && nvidx>=5) {
					j=(i+2)%nvidx;
					k=(i+nvidx-2)%nvidx;
					while(j!=k) {
						float t,u;
						if (intersectline2d(pm1,pp1,&vrts[vidx[j]],&vrts[vidx[(j+1)%nvidx]],NULL,&t,&u)>0) {
							if (t!=0 && t!=1 && u!=0 && u!=1) {
								passint=0; 
								break;
							}
						}
						j++;
						if (j>=nvidx)
							j=0;
					}
				}
				if (passint) {
					bestidx=i;
					besta=a;
				}
			}
		}
		if (bestidx<0) {
//			memfree(fces);
			delete[] fces;
//			memfree(vidx);
			delete[] vidx;
			return 0;
		}
		fces[nfce].vertidx[0]=vidx[(bestidx+nvidx-1)%nvidx];
		fces[nfce].vertidx[1]=vidx[bestidx];
		fces[nfce].vertidx[2]=vidx[(bestidx+1)%nvidx];
		fces[nfce].fmatidx=0;
		for (j=bestidx;j<nvidx-1;j++)
			vidx[j]=vidx[j+1];
		nfce++;
		nvidx--;
	}
//	memfree(vidx);
	delete[] vidx;
	*nfcep=nfce;
	return fces;
}

/*
static int canbuild;
//static char modelname[100];
string modelname;
void buildmtstart(char *mn)
{
	canbuild=0;
//	mystrncpy(modelname,mn,100);
	modelname=mn;
}
*/

// check return value
tree2* buildmt(const char* modelname,const char *texname,pointf3 *v,uv *uvs,int nv)
{
//	int (*facs)[3];
	face* facs;
	int nfac;
	int i;
	uv *wuvs;
	facs=buildfaces(v,nv,&nfac);
	if (!facs)
		return 0;
	if (!uvs) {
//		wuvs=(uv *)memalloc(sizeof(uv)*nv);
		wuvs=new uv[nv];
		for (i=0;i<nv;i++) {
			wuvs[i].u=v[i].x*(1.0f/32.0f);
			wuvs[i].v=1-v[i].z*(1.0f/32.0f);
		}
	} else
		wuvs=uvs;
	modelb* m=model_create(modelname);
	if (model_getrc(m)==1) {
		pushandsetdirdown("constructed");
		m->copyverts(v,nv);
		m->copyuvs0(wuvs,nv);
		m->addmatn("tex",SMAT_HASTEX|SMAT_HASWBUFF,texname,0,30,nfac,nv);
		m->addfaces(facs,nfac,0);
		m->close();
		popdir();
	}
	tree2* ret=new tree2(modelname);
	ret->setmodel(m);
//		canbuild=1;	// done once but only after we know we have valid faces..
//	}
//	setnextmaterial("buildmtmat",texname);
//	buildmodeladduvs(v,wuvs,nv);
//	for (i=0;i<nfac;i++)
//		buildmodelface(facs[i][0],facs[i][1],facs[i][2]);
//	memfree(facs);
	delete[] facs;
	if (!uvs)
//		memfree(wuvs);
		delete[] wuvs;
	return ret;
}

/*tree2* buildmtend()
{
	tree2* ret;
	ret=alloctreebuildmodelend(0);
	return ret;
}
*/
#if 0
//int buildmts(char* modelname,char *texname,pointf3 **v,uv **uvs,int *pmatid,int *nv,int nnv,char *texname2)
tree2* buildmts(const char* modelname,const char *texname,const polylist& pl,const char *texname2)
{
//	int doi;
	int reti=1;
	modelb* mb=0;
	int tm;
	int nmat;
//	typedef int (*facep)[3];
//	facep *facs;
	face** facs;
	int *nfac;
	int i,k;
	uv *wuvs;
//	facs=(facep*)memzalloc(sizeof(facep)*nnv);
	S32 nnv=pl.polys.size();
	facs=new face* [nnv];
	fill(facs,facs+nnv,(face*)0);
//	nfac=(int*)memalloc(sizeof(int)*nnv);
	nfac=new int[nnv];
//	logger("nnv = %d\n",nnv);
	for (k=0;k<nnv;k++) {
//		logger("nv[%d]=%d\n",k,nv[k]);
//		facs[k]=buildfaces(v[k],nv[k],&nfac[k]);
		facs[k]=buildfaces(&pl.polys[k].verts[0],pl.polys[k].verts.size(),&nfac[k]);
//		logger("nfac[%d] = %d\n",k,nfac[k]);
		if (!facs[k]) {
			reti=0;
			goto out;
		}
	}
//	if (!canbuild) {
//		buildmodelstart(modelname);
	mb=model_create(modelname);
	if (model_getrc(mb)!=1)
		errorexit("model '%s' allready created",modelname);
/* pushandsetdirdown("constructed");
		m->copyverts(v,nv);
		m->copyuvs0(wuvs,nv);
		m->addmatn("tex",SMAT_HASTEX|SMAT_HASWBUFF,texname,0,30,nfac,nv);
		m->addfaces(facs,nfac,0);
		m->close();
popdir(); */
//		canbuild=1;	// done once but only after we know we have valid faces..
//	}
	if (texname2)
		nmat=2;
	else
		nmat=1;
	for (tm=0;tm<nmat;tm++) {
		pushandsetdir("constructed");
		if (tm==0)
			;//setnextmaterial("mat 0",texname);
		else
			;//setnextmaterial("mat 1",texname2);
		popdir();
		for (k=0;k<nnv;k++) {
//			if (pmatid[k]==tm || nmat==1) {
			if (k==tm || nmat==1) {
//				if (!uvs) {
				S32 nvk=pl.polys[k].verts.size();
				const pointf3* vk=&pl.polys[k].verts[0];
				if (!pl.hasuvs) {
//					wuvs=(uv *)memalloc(sizeof(uv)*nv[k]);
//					wuvs=new uv[nv[k]];
					wuvs=new uv[nvk];
					for (i=0;i<nvk;i++) {
//						wuvs[i].u=v[k][i].x*(1.0f/32.0f);
						wuvs[i].u=vk[i].x*(1.0f/32.0f);
//						wuvs[i].v=1-v[k][i].z*(1.0f/32.0f);
						wuvs[i].v=1-vk[i].z*(1.0f/32.0f);
					}
					mb->copyuvs0(wuvs,nvk);
				} else {
//					wuvs=uvs[k];
					mb->copyuvs0(pl.polys[k].uvs);
				}
				mb->copyverts(vk,nvk);
//				buildmodeladduvs(v[k],wuvs,nv[k]);
//				for (i=0;i<nfac[k];i++) {
//					buildmodelface(facs[k][i].vertidx[0],facs[k][i].vertidx[1],facs[k][i].vertidx[2]);
					mb->addfaces(facs[k],0,nfac[k]);
/*				logger("face [%d][%d][0] = %d, [%d][%d][1] = %d, [%d][%d][2] = %d\n",
						k,i,facs[k][i][0],k,i,facs[k][i][1],k,i,facs[k][i][2]);
		*/
//				}
//				if (!uvs)
				if (!pl.hasuvs)
//					memfree(wuvs);
					delete[] wuvs;
			}
		}
	}
out:
	for (k=0;k<nnv;k++) {
		if (facs[k])
//			memfree(facs[k]);
			delete[] facs[k];
	}
//	memfree(nfac);
	delete[] nfac;
//	memfree(facs);
	delete[] facs;
	tree2* ret=new tree2("buildmts");
	if (mb)
		ret->setmodel(mb);
	return ret;
//	return new tree2("buildmts");
}
#elif 0
tree2* buildmts(const char* modelname,const char *texname,const polylist& pl,const char *texname2)
{
	modelb* mb=0;
	face** facs;
	int *nfac;
	int k;
	S32 nmat=pl.polys.size();
	facs=new face* [nmat];
	fill(facs,facs+nmat,(face*)0);
	nfac=new int[nmat];
	vector<pointf3> combverts;
	vector<uv> combuvs;
	vector<S32> voff;
	if (texname2)
		nmat=min(nmat,2);
	else
		nmat=min(nmat,1);
	for (k=0;k<nmat;k++) {
		facs[k]=buildfaces(&pl.polys[k].verts[0],pl.polys[k].verts.size(),&nfac[k]);
		if (!facs[k]) {
			goto out;
		}
		voff.push_back(combverts.size());
		combverts.insert(combverts.end(),pl.polys[k].verts.begin(),pl.polys[k].verts.end());
		if (!pl.hasuvs) {
			vector<uv> tuvs(pl.polys[k].verts.size());
			combuvs.insert(combuvs.end(),tuvs.begin(),tuvs.end());
			U32 i;
			for (i=0;i<pl.polys[k].verts.size();++i) {
				tuvs[i].u=pl.polys[k].verts[i].x*(1.0f/32.0f);
				tuvs[i].v=1-pl.polys[k].verts[i].z*(1.0f/32.0f);
			}
		} else {
			combuvs.insert(combuvs.end(),pl.polys[k].uvs.begin(),pl.polys[k].uvs.end());
		}
	}
	voff.push_back(combverts.size());
	mb=model_create(modelname);
	if (model_getrc(mb)!=1)
		errorexit("model '%s' allready created",modelname);
pushandsetdirdown("constructed");
	mb->copyverts(combverts);
	mb->copyuvs0(combuvs);
	for (k=0;k<nmat;++k) {
		if (k==0)
			mb->addmatn("tex",SMAT_HASTEX|SMAT_HASWBUFF,texname,0,30,nfac[k],voff[k+1]-voff[k]);
		else
			mb->addmatn("tex",SMAT_HASTEX|SMAT_HASWBUFF,texname2,0,30,nfac[k],voff[k+1]-voff[k]);
		mb->addfaces(facs[k],nfac[k],voff[k]);
	}
popdir();
	mb->close();
out:
	for (k=0;k<nmat;k++)
		if (facs[k])
			delete[] facs[k];
	delete[] nfac;
	delete[] facs;
	tree2* ret=new tree2("buildmts");
	if (mb)
		ret->setmodel(mb);
	return ret;
}

#else
tree2* buildmts(const char* modelname,const char *texname,const polylist& pl,const char *texname2)
{
	modelb* mb=0;
	face** facs;
	int *nfac;
	int k,pp;
	S32 nmat;
	S32 npoly=pl.polys.size();
	facs=new face* [npoly];
	fill(facs,facs+npoly,(face*)0);
	nfac=new int[npoly];
	vector<pointf3> combverts;
	vector<uv> combuvs;
	vector<S32> voff;
	if (texname2)
		nmat=min(npoly,2);
	else
		nmat=min(npoly,1);
	vector<face> buckface[2];
	vector<pointf3> buckvert[2];
	vector<uv> buckuv[2];
	for (pp=0;pp<npoly;pp++) {
		facs[pp]=buildfaces(&pl.polys[pp].verts[0],pl.polys[pp].verts.size(),&nfac[pp]);
		if (!facs[pp]) {
			goto out;
		}
	}
	for (pp=0;pp<(S32)pl.polys.size();++pp) {
		for (k=0;k<nmat;k++) {
			S32 mid=pl.polys[pp].matid;
			if (mid>=nmat)
				mid=nmat-1;
			if (k==mid) {
				S32 el;
				for (el=0;el<nfac[pp];++el) {
					face f=facs[pp][el];
					S32 mm;
					S32 off=buckvert[k].size();
					for (mm=0;mm<3;++mm) {
						f.vertidx[mm]+=off;
					}
//					buckface[k].insert(buckface[k].end(),facs[pp],facs[pp]+nfac[pp]);
					buckface[k].insert(buckface[k].end(),f);
				}
				buckvert[k].insert(buckvert[k].end(),pl.polys[pp].verts.begin(),pl.polys[pp].verts.end());
				if (pl.hasuvs)
					buckuv[k].insert(buckuv[k].end(),pl.polys[pp].uvs.begin(),pl.polys[pp].uvs.end());
			}
		}
	}
	for (k=0;k<nmat;k++) {
		voff.push_back(combverts.size());
//		combverts.insert(combverts.end(),pl.polys[k].verts.begin(),pl.polys[k].verts.end());
		combverts.insert(combverts.end(),buckvert[k].begin(),buckvert[k].end());
		if (!pl.hasuvs) {
			vector<uv> tuvs(buckvert[k].size());
			U32 i;
			for (i=0;i<buckvert[k].size();++i) {
				tuvs[i].u=buckvert[k][i].x*(1.0f/32.0f);
				tuvs[i].v=1-buckvert[k][i].z*(1.0f/32.0f);
			}
			combuvs.insert(combuvs.end(),tuvs.begin(),tuvs.end());
		} else {
			combuvs.insert(combuvs.end(),buckuv[k].begin(),buckuv[k].end());
		}
	}
	voff.push_back(combverts.size());
	mb=model_create(modelname);
	if (model_getrc(mb)!=1)
		errorexit("model '%s' allready created",modelname);
pushandsetdirdown("constructed");
	mb->copyverts(combverts);
	mb->copyuvs0(combuvs);
	for (k=0;k<nmat;++k) {
		if (k==0)
			mb->addmatn("tex",SMAT_HASTEX|SMAT_HASWBUFF,texname,0,30,buckface[k].size(),voff[k+1]-voff[k]);
		else
			mb->addmatn("tex",SMAT_HASTEX|SMAT_HASWBUFF,texname2,0,30,buckface[k].size(),voff[k+1]-voff[k]);
		mb->addfaces(&buckface[k][0],buckface[k].size(),voff[k]);
	}
popdir();
	mb->close();
out:
	for (k=0;k<npoly;k++)
		if (facs[k])
			delete[] facs[k];
	delete[] nfac;
	delete[] facs;
	tree2* ret=new tree2("buildmts");
	if (mb)
		ret->setmodel(mb);
	return ret;
}

#endif
// local for vert editor
static void initeverts()
{
	nevert=4;
	cevert=0;
	everts[0].x=10;
	everts[0].z=20;
	everts[1].x=20;
	everts[1].z=20;
	everts[2].x=20;
	everts[2].z=10;
	everts[3].x=10;
	everts[3].z=10;
	movemode=0;
}

static int p2sx(float x)
{
	return int(x*(MTVIEWW/32.0f));
}

static int p2sy(float y)
{
	return int((32-y)*MTVIEWH/32.0f);
}

static float s2px(int x)
{
	return (32.0f/MTVIEWW)*x;
}

static float s2pz(int y)
{
	return 32-(32.0f/MTVIEWH)*y;
}

static void draweverts()
{
	int x0,y0,x1,y1;
	int i;
	for (i=0;i<nevert;i++) {
		x0=p2sx(everts[i].x);
		y0=p2sy(everts[i].z);
		clipcircleo32(B32,x0,y0,3,i==cevert ? C32WHITE : C32LIGHTGRAY);
	}
	for (i=0;i<nevert-1;i++) {
		x0=p2sx(everts[i].x);
		y0=p2sy(everts[i].z);
		x1=p2sx(everts[i+1].x);
		y1=p2sy(everts[i+1].z);
		clipline32(B32,x0,y0,x1,y1,i==cevert ? C32WHITE : C32LIGHTGRAY);
	}
	x0=p2sx(everts[nevert-1].x);
	y0=p2sy(everts[nevert-1].z);
	x1=p2sx(everts[0].x);
	y1=p2sy(everts[0].z);
	clipline32(B32,x0,y0,x1,y1,cevert==nevert-1 ? C32LIGHTRED : C32RED);
	if (nevert>=4) {
		pointf3 intsect;
		if (intersectline2d(&everts[0],&everts[1],&everts[2],&everts[3],&intsect,NULL,NULL)>0) {
			x0=p2sx(intsect.x);
			y0=p2sy(intsect.z);
			clipcircle32(B32,x0,y0,3,C32GREEN);
		}
	}
}

/*// a simple puzzle
#define PUZZX 3
#define PUZZY 3
static int puzza[PUZZY][PUZZX];
#define PUZZSTX 500
#define PUZZSTY 10
#define PUZZSTEP 30
#define PUZZSIZE 20
static void initpuzz()
{
	memset(puzza,0,sizeof(puzza));
}
static void checkpuzz()
{
	static int offx[5]={ 0,-1, 0, 1, 0};
	static int offy[5]={-1, 0, 0, 0, 1};
	int i,j,k,a,b;
	int io,jo;
	if (!wininfo.mleftclicks)
		return;
	i=MX-PUZZSTX;
	if (i<0)
		return;
	j=MY-PUZZSTY;
	if (j<0)
		return;
	io=i%PUZZSTEP;
	if (io>=PUZZSIZE)
		return;
	jo=j%PUZZSTEP;
	if (jo>=PUZZSIZE)
		return;
	i/=PUZZSTEP;
	j/=PUZZSTEP;
	if (i>=PUZZX)
		return;
	if (j>=PUZZY)
		return;
	for (k=0;k<5;k++) {
		a=offx[k]+i;
		b=offy[k]+j;
		if (a>=0 && a<PUZZX && b>=0 && b<PUZZY)
			puzza[b][a]^=1;
	}
}
static void drawpuzz()
{
	int i,j,c;
	for (j=0;j<PUZZY;j++)
		for (i=0;i<PUZZX;i++) {
			c=puzza[j][i]?hiwhite:hidarkgray;
			cliprect16(B16,
				PUZZSTX+PUZZSTEP*i,PUZZSTY+PUZZSTEP*j,
				PUZZSTX+PUZZSTEP*i+PUZZSIZE-1,PUZZSTY+PUZZSTEP*j+PUZZSIZE-1,c);
		}
}
#define PUZZX2 100
static int puzza2cnt;
static int puzza2[PUZZX2+1];
#define PUZZSTX2 200
#define PUZZSTY2 100
#define PUZZSTEP2 5
#define PUZZSIZE2 3
static void initpuzz2()
{
	memset(puzza2,0,sizeof(puzza2));
	puzza2cnt=1;
}
static void checkpuzz2()
{
	static int offx[5]={ 0,-1, 0, 1, 0};
	static int offy[5]={-1, 0, 0, 0, 1};
	int i;
	if (!wininfo.mleftclicks)
		return;
	if (puzza2cnt>100)
		return;
	for (i=puzza2cnt;i<=PUZZX2;i+=puzza2cnt)
		puzza2[i]^=1;
	puzza2cnt++;
}
static void drawpuzz2()
{
	int i,c;
	for (i=1;i<=PUZZX2;i++) {
			c=puzza2[i]?hiwhite:hidarkgray;
			cliprect16(B16,
				PUZZSTX2+PUZZSTEP2*(i-1),PUZZSTY2,PUZZSTX2+PUZZSTEP2*(i-1)+PUZZSIZE2-1,PUZZSTY2+PUZZSIZE2-1,c);
		}
}
// end a simple puzzle
*/
void meshtestinit()
{
	pushandsetdir("engine7testdata");
	logger("---------------------- meshtest init -------------------------------\n");
// video
	video_setupwindow(800,600);//,565);
/*	if (video_maindriver==VIDEO_MAINDRIVER_D3D) {
		uselights=1;
		usescnlights=1;
		dodefaultlights=1;
	} else {
		uselights=0;
		usescnlights=0;
	} */
// setup viewport
	mt_viewport.flags=VP_CLEARWB|VP_CHECKER|VP_CLEARBG;
//	rt=alloctree(10000,0);
//	mystrncpy(rt->name,"roottree",NAMESIZE);
//	mt_viewport.roottree=rt;
	mt_viewport_roottree=new tree2("roottree");
	mt_viewport.xstart=MTVIEWX;
	mt_viewport.ystart=MTVIEWY;
	mt_viewport.xres=MTVIEWW;
	mt_viewport.yres=MTVIEWH;
	mt_viewport.zfront=.1f;
	mt_viewport.zback=2000;
	mt_viewport.camzoom=2;
	mt_viewport.camtrans.x=16;
	mt_viewport.camtrans.z=16;
	mt_viewport.camtrans.y=32;
	mt_viewport.camrot.x=PI/2;
//	mt_viewport.camattach=0;//getlastcam();
//	setviewportsrc(&mt_viewport); // user calls this
	mt_viewport.xsrc=4;mt_viewport.ysrc=3;
	mt_viewport.useattachcam=false;
//	mt_viewport.lookat=0;
	themesh=0;
	initeverts();
	pushandsetdirdown("constructed");
	dum=model_create("dum");
	dum->addmatn("tex",0,"dum",0,30,0,0);
	dum->close();
	popdir();
//	initpuzz();
//	initpuzz2();
}

void meshtestproc()
{
// process input
	int i;
	switch(KEY) {
	case K_INSERT:
	case 'a':		// add point to line
		movemode=0;
		if (nevert<MAXV) {
			for (i=nevert-1;i>cevert;i--)
				everts[i+1]=everts[i];
			nevert++;
			cevert++;
			everts[cevert].x=.5f*(everts[(cevert-1+nevert)%nevert].x+everts[(cevert+1)%nevert].x);
			everts[cevert].z=.5f*(everts[(cevert-1+nevert)%nevert].z+everts[(cevert+1)%nevert].z);
		}
		break;
	case K_DELETE:
	case 'd':		// delete point
		movemode=0;
		if (nevert>3) {
			for (i=cevert;i<nevert-1;i++)
				everts[i]=everts[i+1];
			nevert--;
			if (cevert>=nevert)
				cevert=nevert-1;
		}
		break;
	case 'm':		// toggle  mouse move mode
	case K_RIGHTCTRL:
	case K_LEFTCTRL:
		movemode^=1;
		break;
	case 'i':
		inttest^=1;
		break;
	case K_RIGHT:
	case K_NUMRIGHT: // next
		movemode=0;
		cevert++;
		if (cevert>=nevert)
			cevert=nevert-1;
		break;
	case K_LEFT:	// prev
	case K_NUMLEFT:
		movemode=0;
		cevert--;
		if (cevert<0)
			cevert=0;
		break;
	}
	if (movemode) {
		everts[cevert].x=s2px(MX);
		everts[cevert].z=s2pz(MY);
		everts[cevert].x=range(0.0f,everts[cevert].x,32.0f);
		everts[cevert].z=range(0.0f,everts[cevert].z,32.0f);
	}
//	checkpuzz();
//	checkpuzz2();
// update mesh
	if (themesh) {
//		unhooktree(themesh);	// needed because of 'model already built'
//		freetree(themesh);		// now frees right away because it has no parent
		delete themesh;
	}
	themesh=buildmt("testmesh","lusrocks.tga",everts,0,nevert);
	if (themesh)
//			linkchildtoparent(themesh,rt);
		mt_viewport_roottree->linkchild(themesh);
	else
		themesh=0;
// update display
	doflycam(&mt_viewport);
//	doanims(rt);
	mt_viewport_roottree->proc();	
//	buildtreematrices(rt);
	video_buildworldmats(mt_viewport_roottree);
}

void meshtestdraw2d()
{
//	video_lock();
//	cliprect16(B16,0,0,WX-1,WY-1,hiblack);
//	outtextxyf16(B16,150,400,hiwhite,"%d, %d",MX,MY);
	draweverts();
//	drawpuzz();
//	drawpuzz2();
//	video_unlock();
}

void meshtestdraw3d()
{
//	video_beginscene(&mt_viewport);
//	video_drawscene(&mt_viewport);
//	video_endscene(&mt_viewport);
	mt_viewport.xres=WX;
	mt_viewport.yres=WY;
	mt_viewport.xstart=0;
	mt_viewport.ystart=0;
	mt_viewport.flags=VP_CLEARWB|VP_CHECKER|VP_CLEARBG;
	mt_viewport.backcolor=C32(0,85,0);//frgbgreen;
	video_setviewport(&mt_viewport);
	mt_viewport.xres=MTVIEWW;
	mt_viewport.yres=MTVIEWH;
	mt_viewport.xstart=MTVIEWX;
	mt_viewport.ystart=MTVIEWY;
	mt_viewport.flags=VP_CHECKER|VP_CLEARBG;
	mt_viewport.backcolor=C32(0,185,0);//frgbgreen;
	video_setviewport(&mt_viewport);
	video_drawscene(mt_viewport_roottree);
}

void meshtestexit()
{
	logger("---------------------- meshtest exit -------------------------------\n");
//	logviewport(&mt_viewport,OPT_SOME);
	logrc();
// free everything
//	freetree(mt_viewport_roottree);
	delete mt_viewport_roottree;
//	goldbach();
	popdir();
	modelb::rc.deleterc(dum);
}

