#include <m_eng.h>

#include "u_modelutil.h"
#include "u_platonic.h"

// call facesNtovertfaces with this data to convert to traditional mesh type (triangles, more faces and verts)

#define USE_MESH_N // if defined, use MeshN system for all solids here, if not defined, then use the raw tables instead, unless raw tables don't exist, then go back to MeshN

#ifdef USE_MESH_N
// new face vert mesh called meshN, 4 verts, 4 faces
const pointf3 tetravertsN[] = {
	// index 0
	{-1, 1,-1},
	{ 1, 1, 1},
	{-1,-1, 1},
	{ 1,-1,-1},
	// index 4
};

const S32 tetrafacesN[] = {
// right front
	0, 1, 3,-1, // 3 sided face
// right back
	1, 2, 3,-1,
// left back
	1, 0, 2,-1,
// left front
	0, 3, 2,-1,
// done
	-1 
};

// new face vert mesh called meshN, 8 verts, 6 faces
const pointf3 hexavertsN[] = {
	// index 0
	{-1,-1,-1},
	{ 1,-1,-1},
	{-1, 1,-1},
	{ 1, 1,-1},
	{-1,-1, 1},
	{ 1,-1, 1},
	{-1, 1, 1},
	{ 1, 1, 1},
	// index 8
};

const S32 hexafacesN[] = {
// front
	2, 3, 1, 0,-1, // 4 sided face
// back
	7, 6, 4, 5,-1,
// left
	6, 2, 0, 4,-1,
// right
	3, 7, 5, 1,-1,
// bottom
	0, 1, 5, 4,-1,
// top
	6, 7, 3, 2,-1,
// done	
	-1 
};

// new face vert mesh called meshN, 6 verts, 8 faces
const pointf3 octavertsN[] = {
	// index 0
	{ 0 , 1, 0}, // top
	{ 0 ,-1, 0}, // bot
	{ 1 , 0, 0}, // right
	{-1 , 0, 0}, // left
	{ 0 , 0, 1}, // back
	{ 0 , 0,-1}, // front
	// index 6
};

const S32 octafacesN[] = {
// top front right
	0,2,5,-1, // 3 sided face
// top back right
	0,4,2,-1, // 3 sided face
// top back left
	0,3,4,-1, // 3 sided face
// top front left
	0,5,3,-1, // 3 sided face
// bot front right
	1,5,2,-1, // 3 sided face
// bot back right
	1,2,4,-1, // 3 sided face
// bot back left
	1,4,3,-1, // 3 sided face
// bot front left
	1,3,5,-1, // 3 sided face
	-1 
};
#endif

// new face vert mesh called meshN, 20 verts, 12 faces
const float phi = (1.0f + sqrtf(5.0f))*.5f; // approx 1.618
const float iphi = 1.0f / phi; // approx .618
const pointf3 dodecavertsN[] = {
	// from wikipedia (column 1)
	// type 1: cube points
	// index 0
	{-1,-1,-1},
	{ 1,-1,-1},
	{-1, 1,-1},
	{ 1, 1,-1},
	{-1,-1, 1},
	{ 1,-1, 1},
	{-1, 1, 1},
	{ 1, 1, 1},
	// type 2
	// index 8
	{0,-iphi,-phi},
	{0, iphi,-phi},
	{0,-iphi, phi},
	{0, iphi, phi},
	// type 3
	// index 12
	{-iphi,-phi,0},
	{ iphi,-phi,0},
	{-iphi, phi,0},
	{ iphi, phi,0},
	// type 4
	// index 16
	{-phi,0,-iphi},
	{ phi,0,-iphi},
	{-phi,0, iphi},
	{ phi,0, iphi},
	// index 20
};

const S32 dodecafacesN[] = {

// work on top of the 'cube'
// pretty rough to do, but there you go!
// front left
	2,9,8,0,16,-1, // 5 sided face
// front right
	3,17,1,8,9,-1, // 5 sided face

// back left
	4,10,11,6,18,-1, // 5 sided face
// back right
	5,19,7,11,10,-1, // 5 sided face

// right top
	15,7,19,17,3,-1, // 5 sided face
// right bot
	13,1,17,19,5,-1, // 5 sided face

// left top
	14,2,16,18,6,-1, // 5 sided face
// left bot
	12,4,18,16,0,-1, // 5 sided face

// top front
	14,15,3,9,2,-1, // 5 sided face
// top back
	15,14,6,11,7,-1, // 5 sided face

// bot front
	13,12,0,8,1,-1, // 5 sided face
// bot back
	12,13,5,10,4,-1, // 5 sided face

	-1 
};

// new face vert mesh called meshN, 12 verts, 20 faces
const pointf3 icosavertsN[] = {
	// from wikipedia (column 2)
	// plane YZ (purple)
	// index 0
	{0,-phi,-1},
	{0, phi,-1},
	{0,-phi, 1},
	{0, phi, 1},
	// plane XY (dark green)
	// index 4
	{-phi,-1,0},
	{ phi,-1,0},
	{-phi, 1,0},
	{ phi, 1,0},
	// plane XZ (light green)
	// index 8
	{-1,0,-phi},
	{ 1,0,-phi},
	{-1,0, phi},
	{ 1,0, phi},
	// index 12
};

const S32 icosafacesN[] = {
// easier to do, drew a diagram 3 perpendicular planes with vert index labeled
// front
	1,9,8,-1, // top 3 sided face
	8,9,0,-1, // bot 3 sided face
// back
	3,10,11,-1, // top 3 sided face
	11,10,2,-1, // bot 3 sided face
// left
	6,8,4,-1, // front 3 sided face
	4,10,6,-1, // back 3 sided face
// right
	9,7,5,-1, // front 3 sided face
	5,7,11,-1, // back 3 sided face
// bottom
	0,5,2,-1, // left 3 sided face
	0,2,4,-1, // right 3 sided face
// top
	3,1,6,-1, // left 3 sided face
	1,3,7,-1, // right 3 sided face

// front top right
	1,7,9,-1, // 3 sided face
// front top left
	1,8,6,-1, // 3 sided face
// front bot right
	9,5,0,-1, // 3 sided face
// front bot left
	8,0,4,-1, // 3 sided face
// back top right
	7,3,11,-1, // 3 sided face
// back top left
	3,6,10,-1, // 3 sided face
// back bot right
	2,5,11,-1, // 3 sided face
// back bot left
	4,2,10,-1, // 3 sided face
	-1 
};

// now for some Archimedean solids

// new face vert mesh called meshN, 24 verts, 14 faces
const float xi = sqrtf(2) - 1; // approx .414
const pointf3 truncatedhexavertsN[] = {
	// prism, short on x
	// index 0
	{-xi, -1,-1},
	{ xi, -1,-1},
	{-xi,  1,-1},
	{ xi,  1,-1},
	{-xi, -1, 1},
	{ xi, -1, 1},
	{-xi,  1, 1},
	{ xi,  1, 1},
	// prism, short on y
	// index 8
	{ -1,-xi,-1},
	{  1,-xi,-1},
	{ -1, xi,-1},
	{  1, xi,-1},
	{ -1,-xi, 1},
	{  1,-xi, 1},
	{ -1, xi, 1},
	{  1, xi, 1},
	// prism, short on z
	// index 16
	{ -1,-1,-xi},
	{  1,-1,-xi},
	{ -1, 1,-xi},
	{  1, 1,-xi},
	{ -1,-1, xi},
	{  1,-1, xi},
	{ -1, 1, xi},
	{  1, 1, xi},
	// index 24
};

const S32 truncatedhexafacesN[] = {
// pretty easy after drawing the 3 prisms on paper and labeling the verts
// front
	2,3,11,9,1,0,8,10,-1, // 8 sided face
// back
	7,6,14,12,4,5,13,15,-1, // 8 sided face
// left
	22,18,10,8,16,20,12,14,-1, // 8 sided face
// right
	19,23,15,13,21,17,9,11,-1, // 8 sided face
// bottom
	0,1,17,21,5,4,20,16,-1, // 8 sided face
// top
	3,2,18,22,6,7,23,19,-1, // 8 sided face

// front top right
	3,19,11,-1, // 3 sided face
// front top left
	2,10,18,-1, // 3 sided face
// front bot right
	9,17,1,-1, // 3 sided face
// front bot left
	8,0,16,-1, // 3 sided face
// back top right
	23,7,15,-1, // 3 sided face
// back top left
	22,14,6,-1, // 3 sided face
// back bot right
	13,5,21,-1, // 3 sided face
// back bot left
	12,20,4,-1, // 3 sided face
// done	
	-1 
};

// new face vert mesh called meshN, 24 verts, 14 faces
const float tf = sqrtf(2) + 1; // approx 2.414
const float s = .5f; // looks too big, scale it down a little
const float th = tf * s;
const pointf3 rhombicuboctavertsN[] = {
	// prism, long on x
	// index 0
	{-th, -s,-s},
	{ th, -s,-s},
	{-th,  s,-s},
	{ th,  s,-s},
	{-th, -s, s},
	{ th, -s, s},
	{-th,  s, s},
	{ th,  s, s},
	// prism, long on y
	// index 8
	{ -s,-th,-s},
	{  s,-th,-s},
	{ -s, th,-s},
	{  s, th,-s},
	{ -s,-th, s},
	{  s,-th, s},
	{ -s, th, s},
	{  s, th, s},
	// prism, long on z
	// index 16
	{ -s,-s,-th},
	{  s,-s,-th},
	{ -s, s,-th},
	{  s, s,-th},
	{ -s,-s, th},
	{  s,-s, th},
	{ -s, s, th},
	{  s, s, th},
	// index 24
};

// new face vert mesh called meshN, 24 verts, 26 faces
const S32 rhombicuboctafacesN[] = {
// pretty easy after drawing the 3 prisms on paper and labeling the verts
// basic 4 sided, 6 faces
// front
	18,19,17,16,-1,
// back
	23,22,20,21,-1,
// left
	6,2,0,4,-1,
// right
	3,7,5,1,-1,
// bottom
	13,12,8,9,-1,
// top
	14,15,11,10,-1,

// diagonal 4 sided, 12 faces
// front top
	10,11,19,18,-1,
// front bot
	16,17,9,8,-1,
// back top
	15,14,22,23,-1,
// back bot
	21,20,12,13,-1,
// left top
	14,10,2,6,-1,
// left bot
	4,0,8,12,-1,
// right top
	11,15,7,3,-1,
// right bot
	1,5,13,9,-1,
// front right
	17,19,3,1,-1,
// front left
	18,16,0,2,-1,
// back right
	5,7,23,21,-1,
// back left
	6,4,20,22,-1,

// 3 sided, 8 faces
// front top right
	11,3,19,-1,
// front top left
	10,18,2,-1,
// front bot right
	17,1,9,-1,
// front bot left
	16,8,0,-1,
// back top right
	15,23,7,-1,
// back top left
	14,6,22,-1,
// back bot right
	21,13,5,-1,
// back bot left
	20,4,12,-1,
// done
	-1 
};

#ifndef USE_MESH_N
// more raw/gpu mesh types, triangles only, more faces and verts
const pointf3 tetraverts[12] = {
// right front
	{-1, 1,-1},
	{ 1, 1, 1},
	{ 1,-1,-1},
// right back
	{ 1, 1, 1},
	{-1,-1, 1},
	{ 1,-1,-1},
// left back
	{ 1, 1, 1},
	{-1, 1,-1},
	{-1,-1, 1},
// left front
	{-1, 1,-1},
	{ 1,-1,-1},
	{-1,-1, 1},
};

const face tetrafaces[4] = {
// right front
	{ 0, 1, 2},
// right back
	{ 3, 4, 5},
// left back
	{ 6, 7, 8},
// left front
	{ 9,10,11},
};

// hexafaces and hexaverts, no table here, borrow from prismfaces and prismverts in u_modelutil.h

const pointf3 octaverts[24]={
// right front top
	{ 0, 1, 0},
	{ 1, 0, 0},
	{ 0, 0,-1},
// right back top
	{ 0, 1, 0},
	{ 0, 0, 1},
	{ 1, 0, 0},
// left back top
	{ 0, 1, 0},
	{-1, 0, 0},
	{ 0, 0, 1},
// left front top
	{ 0, 1, 0},
	{ 0, 0,-1},
	{-1, 0, 0},
// right front bot
	{ 0,-1, 0},
	{ 0, 0,-1},
	{ 1, 0, 0},
// right back bot
	{ 0,-1, 0},
	{ 1, 0, 0},
	{ 0, 0, 1},
// left back bot
	{ 0,-1, 0},
	{ 0, 0, 1},
	{-1, 0, 0},
// left front bot
	{ 0,-1, 0},
	{-1, 0, 0},
	{ 0, 0,-1},
};

const face octafaces[8]={
// right front top
	{ 0, 1, 2},
// right back top
	{ 3, 4, 5},
// left back top
	{ 6, 7, 8},
// left front top
	{ 9,10,11},
// right front bot
	{12,13,14},
// right back bot
	{15,16,17},
// left back bot
	{18,19,20},
// left front bot
	{21,22,23},
};
#endif

// convert facesN and vertsN to triangles and more verts 
// pass the outputs to copyverts, copyfaces, and addmat
//(example tetrahedron with 4 faces and 4 verts becomes 4 faces and 12 verts)
//(example hexahedron with 6 faces and 8 verts becomes 12 faces and 24 verts)
// this function could be moved out of this file up to say u_modelutil
void facesNtovertfaces(const S32* facesN,const pointf3* vertsN,vector<face>& facesOut,vector<pointf3>& vertsOut,vector<uv>& uvsOut)
{
	// no checks
	const S32* fp = facesN; // start face pointer
	S32 nfacesides;
	S32	numfaceverts = 0; // how many verts are on faceOut, to calc vert index
	//vector<pointf3> tVerts; // temp verts for 1 face
	//vector<uv> tUvs; // temp uvs for 1 face
	while(true) {
		// count up a face
		nfacesides = 0;
		const S32* cp = fp;
		while(*cp++ != -1) { // end of face ?
			++nfacesides;
		}
		if (nfacesides == 0) // no more faces
			break;

		// now process a face
		//  copy over verts
		//tVerts.clear();
		//tUvs.clear();
		for(S32 i=0;i<nfacesides;++i) {
			S32 vi = fp[i];
			const pointf3& vert = vertsN[vi];
			//tVerts.push_back(vert);
			vertsOut.push_back(vert);
			//uvsOut.push_back(uvx(.25f,.85f));
		}
		// calc uvs for this face
		// need a normal
		pointf3& v0 = vertsOut[numfaceverts];
		pointf3& v1 = vertsOut[numfaceverts + 1];
		pointf3& v2 = vertsOut[numfaceverts + 2];
		pointf3 d1 = pointf3x(v1.x-v0.x,v1.y-v0.y,v1.z-v0.z);
		pointf3 d2 = pointf3x(v2.x-v0.x,v2.y-v0.y,v2.z-v0.z);
		pointf3 right,nrm,up; // tangent space (like x y z basis on a face)
		pointf3 tup = pointf3x(0,1,0); // true up
		cross3d(&d1,&d2,&nrm);
		normalize3d(&nrm,&nrm);
		const float polethresh = .99f; // see if normal is straight  up or down
		if (nrm.y >= polethresh) { // norm is pointing up
			right = pointf3x(1,0,0);
			nrm = pointf3x(0,1,0);
			up = pointf3x(0,0,1);
		} else if (nrm.y <=- polethresh) { // norm is pointing down
			right = pointf3x(1,0,0); // right
			nrm = pointf3x(0,-1,0); // down
			up = pointf3x(0,0,-1); // towards
		} else { // norm is some other way
			cross3d(&nrm,&tup,&right);
			normalize3d(&right,&right);
			cross3d(&right,&nrm,&up);
		}
		// need a matrix from uvw to xyz
		mat3 m3;
		m3.e[0][0] = right.x;
		m3.e[0][1] = right.y;
		m3.e[0][2] = right.z;
		m3.e[1][0] = nrm.x;
		m3.e[1][1] = nrm.y;
		m3.e[1][2] = nrm.z;
		m3.e[2][0] = up.x;
		m3.e[2][1] = up.y;
		m3.e[2][2] = up.z;
		// ortho normal matrix can be inverted with just a transpose
		mat3 im3;
		transposemat3(&m3,&im3);
//#define TESTMAT
#ifdef TESTMAT
		pointf3 testin0 = pointf3x(1,0,0);
		pointf3 testin1 = pointf3x(0,1,0);
		pointf3 testin2 = pointf3x(0,0,1);
		pointf3 testout0,testout1,testout2;
		mulmatvec3(&m3,&testin0,&testout0);
		mulmatvec3(&m3,&testin1,&testout1);
		mulmatvec3(&m3,&testin2,&testout2);
		pointf3 testback0,testback1,testback2;
		mulmatvec3(&im3,&testout0,&testback0);
		mulmatvec3(&im3,&testout1,&testback1);
		mulmatvec3(&im3,&testout2,&testback2);
#endif
		for(S32 i=0;i<nfacesides;++i) {
			uv auv;
			pointf3 puv;
			mulmatvec3(&im3,&vertsOut[numfaceverts + i],&puv);
			auv.u = puv.x;
			auv.v = -puv.z;
			//auv.u = vertsOut[numfaceverts + i].x;
			//auv.v = vertsOut[numfaceverts + i].z;
			uvsOut.push_back(auv);
		}
		//  make a triangle fan from a many sided face
		for (S32 i=1;i<nfacesides-1;++i) {
			face f;
			f.vertidx[0] = numfaceverts; // common vert
			f.vertidx[1] = numfaceverts + i; // clockwise
			f.vertidx[2] = numfaceverts + i + 1; // order
			f.fmatidx = 0; // don't need, but be nice and set to 0 anyway
			facesOut.push_back(f);
		}
		// done process a face

		fp = cp; // move on to text face
		numfaceverts += nfacesides; // log vert index used
	}
}

// regular mesh
tree2* buildmesh(const C8* treename,const C8* texname,const C8* matname,const C8* modname,const face* faces,S32 nfaces,const pointf3* verts,S32 nverts,const uv* uvs = 0)
{
	tree2* t=new tree2(treename);
	modelb* m;
	if (modname)
		m=model_create(modname);
	else
		m=model_create(unique());
	if (model_getrc(m)==1) {
		m->copyverts(verts,nverts); // use verts calculated from facesNtovertfaces
		m->addfaces(faces,nfaces,0); // use faces calculated from facesNtovertfaces
// create a texture
		if (texname) {
			textureb* texmat0;
			texmat0=texture_create(texname);
			if (texture_getrc(texmat0)==1) {
				texmat0->load();
			}
			m->addmat(matname,SMAT_HASWBUFF|SMAT_HASTEX|SMAT_CALCLIGHTS|SMAT_HASSHADE,texmat0,0,50,nfaces,nverts);
			if (uvs)
				m->copyuvs0(uvs,nverts);
			else
				m->calcuvs(); // only if no uvs defined
		} else
			m->addmat(matname,SMAT_HASWBUFF|SMAT_CALCLIGHTS|SMAT_HASSHADE,0,0,50,nfaces,nverts);
		m->calcnorms();
		m->close();
	}
	t->setmodel(m);
	return t;
}

// meshN mesh
tree2* buildmeshN(const C8* treename,const C8* texname,const C8* matname,const C8* modname,const S32* facesN,const pointf3* vertsN)
{
	tree2* t=new tree2(treename);
	modelb* m;
	if (modname)
		m=model_create(modname);
	else
		m=model_create(unique());
	if (model_getrc(m)==1) {
		vector<pointf3> verts;
		vector<face> faces;
		vector<uv> uvs;
		facesNtovertfaces(facesN,vertsN,faces,verts,uvs); // 2 in and 2 out, this is the workhorse function
		S32 nverts = verts.size();
		S32 nfaces = faces.size();
		m->copyverts(verts); // use verts calculated from facesNtovertfaces
		m->addfaces(faces,0); // use faces calculated from facesNtovertfaces
// create a texture
		if (texname) {
			textureb* texmat0;
			texmat0=texture_create(texname);
			if (texture_getrc(texmat0)==1) {
				texmat0->load();
			}
			m->addmat(matname,SMAT_HASWBUFF|SMAT_HASTEX|SMAT_CALCLIGHTS|SMAT_HASSHADE,texmat0,0,50,nfaces,nverts);
			//if (uvs.empty())
			//	m->calcuvs();
			//else
				m->copyuvs0(uvs);
		} else
			m->addmat(matname,SMAT_HASWBUFF|SMAT_CALCLIGHTS|SMAT_HASSHADE,0,0,50,nfaces,nverts);
		m->calcnorms();
		m->close();
	}
	t->setmodel(m);
	return t;
}

tree2* buildtetrahedron(const C8* texname,const C8* matname,const C8* modname)
{
#ifdef USE_MESH_N
	return buildmeshN("tetrahedron",texname,matname,modname,tetrafacesN,tetravertsN);
#else
	return buildmesh("tetrahedron",texname,matname,modname,tetrafaces,NUMELEMENTS(tetrafaces),tetraverts,NUMELEMENTS(tetraverts));
#endif
}

tree2* buildhexahedron(const C8* texname,const C8* matname,const C8* modname)
{
#ifdef USE_MESH_N
	return buildmeshN("hexahedron",texname,matname,modname,hexafacesN,hexavertsN);
#else
	//return buildmesh("hexahedron",texname,matname,modname,prismfaces,NUMELEMENTS(prismfaces),prismverts,NUMELEMENTS(prismverts),prismuvs); // this one has uvs defined, no need to calcuvs
	return buildmesh("hexahedron",texname,matname,modname,prismfaces,NUMELEMENTS(prismfaces),prismverts,NUMELEMENTS(prismverts)); // this one has uvs defined, no need to calcuvs
#endif
}

tree2* buildoctahedron(const C8* texname,const C8* matname,const C8* modname)
{
#ifdef USE_MESH_N
	return buildmeshN("octahedron",texname,matname,modname,octafacesN,octavertsN);
#else
	return buildmesh("octahedron",texname,matname,modname,octafaces,NUMELEMENTS(octafaces),octaverts,NUMELEMENTS(octaverts));
#endif
}

// these solids can only be made with meshN system
tree2* builddodecahedron(const C8* texname,const C8* matname,const C8* modname)
{
	return buildmeshN("dodecahedron",texname,matname,modname,dodecafacesN,dodecavertsN);
}

tree2* buildicosahedron(const C8* texname,const C8* matname,const C8* modname)
{
	return buildmeshN("icosahedron",texname,matname,modname,icosafacesN,icosavertsN);
}

tree2* buildtruncatedhexahedron(const C8* texname,const C8* matname,const C8* modname)
{
	return buildmeshN("truncatedhexahedron",texname,matname,modname,truncatedhexafacesN,truncatedhexavertsN);
}

tree2* buildrhombicuboctahedron(const C8* texname,const C8* matname,const C8* modname)
{
	return buildmeshN("buildrhombicuboctahedron",texname,matname,modname,rhombicuboctafacesN,rhombicuboctavertsN);
}
