#define D2_3D
#include <m_eng.h>
#include <l_misclibm.h>

#include "carenalobby.h"
//#include "carenalobbyres.h"
#include "jrmcars.h"
#include "n_loadweapcpp.h"
#include "enums.h"
#include "utils/json.h"
#include "hash.h"
#include "constructor.h"
#include "newconstructor.h"
#include "system/u_states.h"
#include "carclass.h"
#include "utils/modelutil.h"
#include "n_marchingdata.h"

typedef struct {
   pointf3 p[3];
} TRIANGLE;

typedef struct {
   pointf3 p[8];
   float val[8];
} GRIDCELL;

/*
   Given a grid cell and an isolevel, calculate the triangular
   facets required to represent the surf through the cell.
   Return the number of triangular facets, the array "triangles"
   will be loaded up with the vertices at most 5 triangular facets.
	0 will be returned if the grid cell is either totally above
   of totally below the isolevel.
*/
/*
   Linearly interpolate the position where an surf cuts
   an edge between two vertices, each with their own scalar value
*/
static pointf3 VertexInterp(float isolevel,pointf3 p1,pointf3 p2,float valp1,float valp2) {
   float mu;
   pointf3 p;
   (mu);

   if (fabs(isolevel-valp1) < 0.00001f)
      return(p1);
   if (fabs(isolevel-valp2) < 0.00001f)
      return(p2);
   if (fabs(valp1-valp2) < 0.00001f)
      return(p1);
   mu = (isolevel - valp1) / (valp2 - valp1);
   p.x = p1.x + mu * (p2.x - p1.x);
   p.y = p1.y + mu * (p2.y - p1.y);
   p.z = p1.z + mu * (p2.z - p1.z);
   return(p);
}

static int Polygonise(GRIDCELL grid,float isolevel,TRIANGLE *triangles)
{
   int i,ntriang;
   int cubeindex;
   pointf3 vertlist[12];


   /*
      Determine the index into the edge table which
      tells us which vertices are inside of the surface
   */
   cubeindex = 0;
   if (grid.val[0] < isolevel) cubeindex |= 1;
   if (grid.val[1] < isolevel) cubeindex |= 2;
   if (grid.val[2] < isolevel) cubeindex |= 4;
   if (grid.val[3] < isolevel) cubeindex |= 8;
   if (grid.val[4] < isolevel) cubeindex |= 16;
   if (grid.val[5] < isolevel) cubeindex |= 32;
   if (grid.val[6] < isolevel) cubeindex |= 64;
   if (grid.val[7] < isolevel) cubeindex |= 128;

   /* Cube is entirely in/out of the surface */
   if (edgeTable[cubeindex] == 0)
      return(0);
   if (edgeTable[cubeindex] == 256-1)
      return(0);

   /* Find the vertices where the surface intersects the cube */
   if (edgeTable[cubeindex] & 1)
      vertlist[0] =
         VertexInterp(isolevel,grid.p[0],grid.p[1],grid.val[0],grid.val[1]);
   if (edgeTable[cubeindex] & 2)
      vertlist[1] =
         VertexInterp(isolevel,grid.p[1],grid.p[2],grid.val[1],grid.val[2]);
   if (edgeTable[cubeindex] & 4)
      vertlist[2] =
         VertexInterp(isolevel,grid.p[2],grid.p[3],grid.val[2],grid.val[3]);
   if (edgeTable[cubeindex] & 8)
      vertlist[3] =
         VertexInterp(isolevel,grid.p[3],grid.p[0],grid.val[3],grid.val[0]);
   if (edgeTable[cubeindex] & 16)
      vertlist[4] =
         VertexInterp(isolevel,grid.p[4],grid.p[5],grid.val[4],grid.val[5]);
   if (edgeTable[cubeindex] & 32)
      vertlist[5] =
         VertexInterp(isolevel,grid.p[5],grid.p[6],grid.val[5],grid.val[6]);
   if (edgeTable[cubeindex] & 64)
      vertlist[6] =
         VertexInterp(isolevel,grid.p[6],grid.p[7],grid.val[6],grid.val[7]);
   if (edgeTable[cubeindex] & 128)
      vertlist[7] =
         VertexInterp(isolevel,grid.p[7],grid.p[4],grid.val[7],grid.val[4]);
   if (edgeTable[cubeindex] & 256)
      vertlist[8] =
         VertexInterp(isolevel,grid.p[0],grid.p[4],grid.val[0],grid.val[4]);
   if (edgeTable[cubeindex] & 512)
      vertlist[9] =
         VertexInterp(isolevel,grid.p[1],grid.p[5],grid.val[1],grid.val[5]);
   if (edgeTable[cubeindex] & 1024)
      vertlist[10] =
         VertexInterp(isolevel,grid.p[2],grid.p[6],grid.val[2],grid.val[6]);
   if (edgeTable[cubeindex] & 2048)
      vertlist[11] =
         VertexInterp(isolevel,grid.p[3],grid.p[7],grid.val[3],grid.val[7]);

   /* Create the triangle */
   ntriang = 0;
   for (i=0;triTable[cubeindex][i]!=-1;i+=3) {
      triangles[ntriang].p[0] = vertlist[triTable[cubeindex][i  ]];
      triangles[ntriang].p[1] = vertlist[triTable[cubeindex][i+1]];
      triangles[ntriang].p[2] = vertlist[triTable[cubeindex][i+2]];
      ntriang++;
   }

   return(ntriang);
}

// build a sphere of radius 1.5 meters
tree2* march_test1()
{
	tree2* r=new tree2("march_test");
	modelb* modb=model_create("march_test");
	if (model_getrc(modb)==1) {
		pushandsetdir("gfxtest");
//	    buildpatch_tan(modb,12,6,12,6,spheref_surf_tan(5.0f),"bark.tga","ts18_2crop.jpg",matname);
//	    buildpatch_tan(modb,24,12,6,3,spheref_surf_tan(5.0f),"maptest.tga","243-normal.jpg",matname);
		buildpatch(modb,24,12,6,3,spheref_surf(1.5f),"maptestnck.tga","tex");
		popdir();
	}
	r->setmodel(modb);
	return r;
}

tree2* march_test2(float val,int res)
{
	tree2* r=new tree2("march_test");
	modelb* mod=model_create("march_test");
	if (model_getrc(mod)==1) {
		pushandsetdir("gfxtest");
		mod->copyverts(prismverts,nprismverts);
		mod->copynorms(prismnorms,nprismverts);
		mod->copyuvs0(prismuvs,nprismverts);
// create a texture
		textureb* texmat0;
		texmat0=texture_create("maptestnck.tga");
		if (texture_getrc(texmat0)==1) {
			texmat0->load();
		} 
		mod->addmat("tex",SMAT_HASWBUFF|SMAT_HASTEX,texmat0,0,50,nprismfaces,nprismverts);
		mod->addfaces(prismfaces,nprismfaces,0);
		mod->close();
		popdir();
	}
	r->setmodel(mod);
	return r;
}

static vector<pointf3>* mcverts;
static vector<pointf3>* mcnorms;
static vector<uv>* mcuvs;
static vector<face>* mcfaces;

static void buildmarch(float val,int res,const pointf3& boxmin,const pointf3& boxmax,float (*surf)(float x,float y,float z))
{
	int i,j,k;
	GRIDCELL g;
	int facecount=0;
	for (k=0;k<res;++k) {
		float z0=boxmin.z+(boxmax.z-boxmin.z)*k/res;
		float z1=boxmin.z+(boxmax.z-boxmin.z)*(k+1)/res;
		g.p[0].z=z1;
		g.p[1].z=z1;
		g.p[2].z=z0;
		g.p[3].z=z0;
		g.p[4].z=z1;
		g.p[5].z=z1;
		g.p[6].z=z0;
		g.p[7].z=z0;
		for (j=0;j<res;++j) {
			float y0=boxmin.y+(boxmax.y-boxmin.y)*j/res;
			float y1=boxmin.y+(boxmax.y-boxmin.y)*(j+1)/res;
			g.p[0].y=y0;
			g.p[1].y=y0;
			g.p[2].y=y0;
			g.p[3].y=y0;
			g.p[4].y=y1;
			g.p[5].y=y1;
			g.p[6].y=y1;
			g.p[7].y=y1;
			for (i=0;i<res;++i) {
				float x0=boxmin.x+(boxmax.x-boxmin.x)*i/res;
				float x1=boxmin.x+(boxmax.x-boxmin.x)*(i+1)/res;
				g.p[0].x=x0;
				g.p[1].x=x1;
				g.p[2].x=x1;
				g.p[3].x=x0;
				g.p[4].x=x0;
				g.p[5].x=x1;
				g.p[6].x=x1;
				g.p[7].x=x0;
				TRIANGLE tris[5];
				g.val[0]=surf(x0,y0,z1);
				g.val[1]=surf(x1,y0,z1);
				g.val[2]=surf(x1,y0,z0);
				g.val[3]=surf(x0,y0,z0);
				g.val[4]=surf(x0,y1,z1);
				g.val[5]=surf(x1,y1,z1);
				g.val[6]=surf(x1,y1,z0);
				g.val[7]=surf(x0,y1,z0);
				int ret=Polygonise(g,val,tris);
				if (ret) {
					int el;
					for (el=0;el<ret;++el) {
						static uv auvs[3]={{0,0},{1,0},{0,1}};
						mcverts->push_back(tris[el].p[0]);
						mcverts->push_back(tris[el].p[1]);
						mcverts->push_back(tris[el].p[2]);
						mcuvs->push_back(auvs[0]);
						mcuvs->push_back(auvs[1]);
						mcuvs->push_back(auvs[2]);
						face aface={{facecount,facecount+2,facecount+1},0};
						mcfaces->push_back(aface);
						facecount+=3;
					}
				}
			}
		}
	}
}

tree2* march_test(float val,int res,const pointf3& boxmin,const pointf3& boxmax,float (*surf)(float x,float y,float z))
{
	tree2* r=new tree2("march_test");
	modelb* mod=model_create("march_test");
	if (model_getrc(mod)==1) {
		mcverts = new vector<pointf3>;
		mcnorms = new vector<pointf3>;
		mcuvs = new vector<uv>;
		mcfaces = new vector<face>;
		buildmarch(val,res,boxmin,boxmax,surf);
		mod->copyverts(*mcverts);
		mod->copynorms(*mcnorms);
		mod->copyuvs0(*mcuvs);
// create a texture
		textureb* texmat0;
		texmat0=texture_create("maptestnck.tga");
		if (texture_getrc(texmat0)==1) {
pushandsetdir("gfxtest");
			texmat0->load();
popdir();
		} 
		mod->addmat("tex",SMAT_HASWBUFF|SMAT_HASTEX,texmat0,0,50,mcfaces->size(),mcverts->size());
		mod->addfaces(*mcfaces,0);
		mod->close();
		delete mcverts;
		delete mcnorms;
		delete mcuvs;
		delete mcfaces;
		mcverts=0;
		mcnorms=0;
		mcuvs=0;
		mcfaces=0;
	}
	r->setmodel(mod);
	return r;
}
