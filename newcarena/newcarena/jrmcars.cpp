// this is where jrm's are loaded into a tree2*  struct
/*#include <windows.h>

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include <engine7cpp.h>
#include <misclib7cpp.h>
//#include "vidd3d.h"

#include "deflatecpp.h"
#include "jrmcarscpp.h"
*/

#define D2_3D
#define INCLUDE_WINDOWS
#include <m_eng.h>
// #include "n_deflatecpp.h"
/*#include <l_misclibm.h>

#include "carenalobbycpp.h"
#include "carenalobbyres.h"
#include "jrmcarscpp.h"
#include "loadweapcpp.h"
#include "usefulcpp.h"
#include "constructorcpp.h"
#include "newconstructorcpp.h"
#include "trackhashcpp.h"
#include "system/u_states.h"*/

float jrmcarscale;
pointf3 jrmcarrot;
pointf3 jrmcartrans;

//char excludelist[MAXLIST][80];//={
//int usenewprs;
#pragma pack(1)
struct olctimestamp {
	unsigned short month,day,year;
};
#pragma pack()
#define MAXG 60
#define makeform(a,b,c,d) (((d)<<24)+((c)<<16)+((b)<<8)+(a))
static int facemode,altcars;
static char *printform(unsigned int f)
{
	static char frm[5];
	frm[4]='\0';
	memcpy(frm,&f,4);
	return frm;
}

struct group {
	S32 vertidx,nvert,nface,faceidx;
};

static void setjrmcar()
{
	jrmcarscale= .1f; // .014f; was
	jrmcarrot.x= -90;
	jrmcarrot.y= 0;
	jrmcarrot.z= 0;
	jrmcartrans.x= 0;
	jrmcartrans.y= 0;//-.07f;
	jrmcartrans.z= 0;
}

static tree2* buildandfreejrmmodel(char *modelnamearg,char *texnamearg,const vector<pointf3>& verts,
							const vector<uv>& uvs,const vector<face>& faces,
							struct group *groups,int ngroup)
{
	int i,j;
	static int mn;
	char modelname[300],texname[300];
	tree2* scene,*part=0;
//	logger("jrm model name '%s'\n",modelnamearg);
	if (ngroup<1 || faces.size()<1 || verts.size()<1)
		return 0;
//	scene=alloctree(MAXG,NULL);
//	mystrncpy(scene->name,modelnamearg,NAMESIZE);
	scene=new tree2(modelnamearg);
//	usenocolorkey=1;
	for (j=0;j<ngroup;j++) {
		if (texnamearg)
			if (altcars)
				sprintf(texname,"p_%s_texture.tga",texnamearg);
			else
				sprintf(texname,"%s_texture.tga",texnamearg);
		sprintf(modelname,"%s_%d",modelnamearg,mn);
//		buildmodelstart(modelname);
		modelb* mod = model_create(modelname);
		mod->copyverts(&verts[groups[j].vertidx],groups[j].nvert);
		mod->copyuvs0(uvs);
//		mod->copynorms(norms,norms.);
		if (texnamearg) {
			textureb* at=texture_create(unique());
			if (lightinfo.uselights) {
//				buildmodelsetmaterial(
//					SMAT_HASTEX|SMAT_HASWBUFF|/*SMAT_WRAPU|SMAT_WRAPV|*/SMAT_CALCLIGHTS|SMAT_HASSHADE,
//					texname,NULL,modelname,NULL);
				mod->addmat("tex",SMAT_HASTEX|SMAT_HASWBUFF|SMAT_CALCLIGHTS|SMAT_HASSHADE,at,0,30,groups[j].nface,groups[j].nvert);
			} else {
//				buildmodelsetmaterial(
//					SMAT_HASTEX|SMAT_HASWBUFF/*|SMAT_WRAPU|SMAT_WRAPV*/,
//					texname,NULL,modelname,NULL);
				mod->addmat("tex",SMAT_HASTEX|SMAT_HASWBUFF,at,0,30,groups[j].nface,groups[j].nvert);
			}
		} else {
			if (lightinfo.uselights) {
//				buildmodelsetmaterial(
//					SMAT_HASWBUFF|SMAT_CALCLIGHTS|SMAT_HASSHADE,
//					NULL,NULL,modelname,NULL);
				mod->addmat("tex",SMAT_HASWBUFF|SMAT_CALCLIGHTS|SMAT_HASSHADE,0,0,30,groups[j].nface,groups[j].nvert);
			} else {
//				buildmodelsetmaterial(
//					SMAT_HASWBUFF,NULL,NULL,modelname,NULL);
				mod->addmat("tex",SMAT_HASWBUFF,0,0,30,groups[j].nface,groups[j].nvert);
			}
		}
//		if (uvs)
//			buildmodeladduvs(verts+groups[j].vertidx,uvs+groups[j].vertidx,groups[j].nvert);
//		else
//			buildmodeladdflat(verts+groups[j].vertidx,groups[j].nvert);
		if (groups[j].nface==2)
			facemode=3;
		else
			facemode=1;
		for (i=0;i<groups[j].nface;i++) {
			if (facemode!=2)
//				buildmodelface(faces[i+groups[j].faceidx].vertidx[0],
//							   faces[i+groups[j].faceidx].vertidx[1],
//							   faces[i+groups[j].faceidx].vertidx[2]);
				mod->addface(faces[i+groups[j].faceidx].vertidx[0],
							   faces[i+groups[j].faceidx].vertidx[1],
							   faces[i+groups[j].faceidx].vertidx[2]);
			if (facemode!=1)
//				buildmodelface(faces[i+groups[j].faceidx].vertidx[1],
//							   faces[i+groups[j].faceidx].vertidx[0],
//							   faces[i+groups[j].faceidx].vertidx[2]);
				mod->addface(faces[i+groups[j].faceidx].vertidx[1],
							   faces[i+groups[j].faceidx].vertidx[0],
							   faces[i+groups[j].faceidx].vertidx[2]);
		}
//		part=alloctreebuildmodelend(MAXG);
		mod->close();
		part=new tree2(modelnamearg);
		part->setmodel(mod);
//		mystrncpy(part->name,modelnamearg,NAMESIZE);
//		linkchildtoparent(part,scene);
		scene->linkchild(part);
		mn++;
	}
//	usenocolorkey=0;
//	memfree(verts);
//	memfree(faces);
//	if (uvs)
//		memfree(uvs);
	if (ngroup==1) {
//		unhooktree(part);
		part->unlink();
//		freetree(scene);
		delete scene;
		return part;
	}
	return scene;
}

static int checkprs(char *name,pointf3 *pos,pointf3 *rot,pointf3 *scale)
{
	char *str=name;
//	logger("the name is '%s'\n",name);
// scan for a ','
	while(*str) {
		if (*str==',')
			break;
		str++;
	}
	if (!*str) {
		*pos=zerov;
		*rot=zerov;
		scale->x=1;
		scale->y=1;
		scale->z=1;
		return 0;
	}
	*str='\0';
	str++;
	pos->x=(float)atof(strtok(str,","));
	pos->y=(float)atof(strtok(NULL,","));
	pos->z=(float)atof(strtok(NULL,","));
	rot->x=(float)atof(strtok(NULL,","));
	rot->y=(float)atof(strtok(NULL,","));
	rot->z=(float)atof(strtok(NULL,","));
	scale->x=(float)atof(strtok(NULL,","));
	scale->y=(float)atof(strtok(NULL,","));
	scale->z=(float)atof(strtok(NULL,","));
//	logger("new prs for '%s' is (%f,%f,%f) (%f,%f,%f) (%f,%f,%f)\n",
//		name,pos->x,pos->y,pos->z,rot->x,rot->y,rot->z,scale->x,scale->y,scale->z);
	return 1;
}

static tree2* loadnewjrms(const char *carnamearg,const char *textnamearg)
{
	int hasnewprs=0;
//	extern char dswmediadir[];
//	char jrmfolder[300];
	unsigned int form;
	int rd;
	struct olctimestamp ts;
	char carname[300];
#define NAMESIZEI 32
	char childnames[MAXG][NAMESIZEI];
	struct pointf3 childoffsets[MAXG];
	struct pointf3 childscales[MAXG];
	struct pointf3 childrots[MAXG];
	int childidx[MAXG];
	int nchild;
	tree2* treelist[MAXG];
	int ntreelist;
	char modelname[300],texname[NAMESIZEI];
	int dobuild=0;
	tree2* s,*t;

	struct group groups[MAXG];
	int nvgroup=-1,nfgroup=-1,nuvgroup=-1;
//	int nvert;
//	struct pointf3 *verts;
//	int nuv;
//	struct uv *uvs;
//	struct face *faces;
//	int nface;
	vector<pointf3> verts;
	vector<uv> uvs;
	vector<face> faces;
	int i,j,k;
	pointf3 npos,nrot,nscale;
// load up some cars !!!
//	logger("load binary online car data '%s' '%s' ------------------------------------------\n",carnamearg,textnamearg);
////////////////////////////// vertices ///////////////////////////////
//	if (!stubinfo.useslextra)
//		return NULL;
//	setlastcam(NULL);
	logger("loading carena car '%s' '%s'\n",carnamearg,textnamearg);
//	strcpy(jrmfolder,dswmediadir);
//	strcat(jrmfolder,"cajrm");
//	pushandsetdir(jrmfolder);
	sprintf(carname,"%s.jrm",carnamearg);
	altcars=0;
	if (!fileexist(carname)) {
		logger("error: bin %s doesn't exist, trying alternate\n",carname);
		sprintf(carname,"p_%s.jrm",carnamearg);
		if (!fileexist(carname)) {
			logger("error: bin %s doesn't exist\n",carname);
//			popdir();
			return NULL;
		}
		altcars=1;
	}
//	fileopen(carname,READ);
	FILE* fp=fopen2(carname,"rb");
//	rd=fileread(&ts,sizeof(ts));
	rd=fread(&ts,1,sizeof(ts),fp);
	if (rd!=6) {
		logger("file too small!\n");
//		fileclose();
		fclose(fp);
//		popdir();
		return 0;
	}
//	logger("online car '%s' has date %2d/%2d/%4d\n",carname,ts.month,ts.day,ts.year);
	ntreelist=nchild=0;
	texname[0]='\0';
	while(1) {
//		rd=fileread(&form,sizeof(form)); // read form
		rd=fread(&form,1,sizeof(form),fp);
		if (!rd)
			break;
		if (rd!=sizeof(form)) {
			logger("no form!\n");
//			fileclose();
			fclose(fp);
//			popdir();
			return 0;
		}
		switch(form) {
		case makeform('F','R','A','M'):
//			fileread(&i,sizeof(i));
			fread(&i,1,sizeof(i),fp);
//			logger("FRAM: framenumber %d\n",i);
			break;
		case makeform('M','D','L','N'):
			if (dobuild) {
				t=NULL;
				if (texname[0])
					t=buildandfreejrmmodel(modelname,texname,verts,uvs,faces,groups,nvgroup);
				else
					t=buildandfreejrmmodel(modelname,      0,verts,uvs,faces,groups,nvgroup);
				if (t) {
					treelist[ntreelist++]=t;
//					t->dissolve=.5f;
					if (1) {//usenewprs) {
						t->trans=npos;
						t->rot.x=normalangrad(nrot.x*PIOVER180);
						t->rot.y=normalangrad(nrot.y*PIOVER180);
						t->rot.z=normalangrad(nrot.z*PIOVER180);
						t->scale=nscale;
					}
				}
			}
			dobuild=1;
			nvgroup=nuvgroup=nfgroup=0;
//			nvert=nface=nuv=0;
//			verts=0;
//			faces=0;
//			uvs=0;
			verts.clear();
			uvs.clear();
			faces.clear();
			texname[0]='\0';
//			fileread(&i,sizeof(i));
			fread(&i,1,sizeof(i),fp);
			if (i>=300) {
				logger("name too big\n");
//				popdir();
//				fileclose();
				fclose(fp);
				return 0;
			}
//			fileread(modelname,i);
			fread(modelname,1,i,fp);
			modelname[i]='\0';
			hasnewprs=checkprs(modelname,&npos,&nrot,&nscale);
//			logger("MDLN: model name is '%s'\n",modelname);
			break;
		case makeform('M','T','E','X'):
//			fileread(&i,sizeof(i)); // get num letters
			fread(&i,1,sizeof(i),fp);
//			fileread(&j,sizeof(j)); // get tex number
			fread(&j,1,sizeof(j),fp);
//			fileread(texname,i);
			fread(texname,1,i,fp);
			texname[i]='\0';
//			logger("MTEX: texidx %d, name '%s'\n",j,texname);
			if (!strcmp(texname,"NONEATALL"))
				texname[0]='\0';
			else if (textnamearg)
				strcpy(texname,textnamearg);
			break;
		case makeform('V','E','R','T'):
//			fileread(&i,sizeof(i)); // get num verts
			fread(&i,1,sizeof(i),fp);
//			fileread(&j,sizeof(j)); // get mesh #
			fread(&j,1,sizeof(j),fp);
//			logger("VERT:   nvert %d, meshidx %d\n",i,j);
			if (j-1!=nvgroup)
				errorexit("meshv doesn't mesh '%s'\n",carnamearg);
			groups[nvgroup].vertidx=verts.size();
			groups[nvgroup].nvert=i;
//			logger("meshgroupv %d: bin vertidx %d, nvert %d\n",nvgroup,nvert,i);
			nvgroup++;
			if (i) {
				S32 os=verts.size();
//				verts=(struct pointf3 *)memrealloc(verts,sizeof(struct pointf3)*(i+nvert));
				verts.resize(os+i);
				for (k=0;k<i;k++) {
//					fileread(&verts[nvert+k],12);
					fread(&verts[os+k],1,12,fp);
					verts[os+k].w=0;
				}
			}
//			nvert+=i;
			break;
		case makeform('T','X','U','V'):
//			fileread(&i,sizeof(i)); // get num verts
			fread(&i,1,sizeof(i),fp);
//			fileread(&j,sizeof(j)); // get mesh #
			fread(&j,1,sizeof(j),fp);
//			logger("TXUV:   nvert %d, meshidx %d\n",i,j);
			if (j-1!=nuvgroup)
				errorexit("meshuv doesn't mesh '%s'\n",carnamearg);
//			logger("meshgroupuv %d: bin uvidx %d, nuv %d\n",nuvgroup,nuv,i);
			nuvgroup++;
			if (i) {
				S32 os=uvs.size();
//				verts=(struct pointf3 *)memrealloc(verts,sizeof(struct pointf3)*(i+nvert));
				uvs.resize(os+i);
				for (k=0;k<i;k++) {
//					fileread(&verts[nvert+k],12);
					struct uv8 {
						float u,v;
					} us;
					fread(&us,1,sizeof(us),fp);
					uvs[os+k].u=us.u;
					uvs[os+k].v=1-us.v;
					//uvs[os+k].w=0;
				}
			}
			break;
		case makeform('F','A','C','E'):
//			fileread(&i,sizeof(i)); // get num faces
			fread(&i,1,sizeof(i),fp);
//			fileread(&j,sizeof(j)); // get mesh #
			fread(&j,1,sizeof(j),fp);
//			logger("FACE:   nface %d, meshidx %d\n",i,j);
			if (j-1!=nfgroup)
				errorexit("meshf doesn't mesh '%s'\n",carnamearg);
			groups[nfgroup].faceidx=faces.size();
			groups[nfgroup].nface=i;
//			logger("meshgroupf %d: bin faceidx %d, nface %d\n",nfgroup,nface,i);
			nfgroup++;
/*			if (i)
				faces=(struct face *)memrealloc(faces,sizeof(struct face)*(i+nface));
			for (k=0;k<i;k++) {
//				fileread(&j,4);
				fread(&j,1,sizeof(j),fp);
				faces[nface+k].vertidx[0]=j-1;
//				fileread(&j,4);
				fread(&j,1,sizeof(j),fp);
				faces[nface+k].vertidx[1]=j-1;
//				fileread(&j,4);
				fread(&j,1,sizeof(j),fp);
				faces[nface+k].vertidx[2]=j-1;
				faces[nface+k].fmatidx=0;
			}
			nface+=i; */
			if (i) {
				S32 os=faces.size();
//				verts=(struct pointf3 *)memrealloc(verts,sizeof(struct pointf3)*(i+nvert));
				faces.resize(os+i);
				for (k=0;k<i;k++) {
					fread(&j,1,sizeof(j),fp);
					faces[os+k].vertidx[0]=j-1;
	//				fileread(&j,4);
					fread(&j,1,sizeof(j),fp);
					faces[os+k].vertidx[1]=j-1;
	//				fileread(&j,4);
					fread(&j,1,sizeof(j),fp);
					faces[os+k].vertidx[2]=j-1;
					faces[os+k].fmatidx=0;
				}
			}
			break;
		case makeform('C','H','L','D'):
//			fileread(&i,sizeof(i)); // get num children
			fread(&i,1,sizeof(i),fp);
			for (j=0;j<i;j++) {
				k=0;
				while(1) {
//					fileread(&childnames[nchild+j][k],1);
					fread(&childnames[nchild+j][k],1,1,fp);
					if (childnames[nchild+j][k]=='\0')
						break;
					k++;
					if (k>=NAMESIZEI) {
						logger("child names too big in binonlinecars\n");
//						popdir();
//						fileclose();
						fclose(fp);
						return 0;
					}
				}
			}
			for (j=0;j<i;j++) {
				childidx[nchild+j]=ntreelist;
//				fileread(&childoffsets[nchild+j].x,sizeof(float));
				fread(&childoffsets[nchild+j].x,1,sizeof(float),fp);
//				fileread(&childoffsets[nchild+j].y,sizeof(float));
				fread(&childoffsets[nchild+j].y,1,sizeof(float),fp);
//				fileread(&childoffsets[nchild+j].z,sizeof(float));
				fread(&childoffsets[nchild+j].z,1,sizeof(float),fp);
				childoffsets[nchild+j].w=0;
//				fileread(&childscales[nchild+j].x,sizeof(float));
				fread(&childscales[nchild+j].x,1,sizeof(float),fp);
//				fileread(&childscales[nchild+j].y,sizeof(float));
				fread(&childscales[nchild+j].y,1,sizeof(float),fp);
//				fileread(&childscales[nchild+j].z,sizeof(float));
				fread(&childscales[nchild+j].z,1,sizeof(float),fp);
				childscales[nchild+j].w=0;
//				fileread(&childrots[nchild+j].x,sizeof(float));
				fread(&childrots[nchild+j].x,1,sizeof(float),fp);
//				fileread(&childrots[nchild+j].y,sizeof(float));
				fread(&childrots[nchild+j].y,1,sizeof(float),fp);
//				fileread(&childrots[nchild+j].z,sizeof(float));
				fread(&childrots[nchild+j].z,1,sizeof(float),fp);
				childrots[nchild+j].w=0;
			}
/*			logger("CHLD: nchild %d\n",i);
			for (j=0;j<i;j++)
				logger("       offset %f,%f,%f, rot %f,%9f,%f, scale %f,%f,%f, name '%s'\n",
					childoffsets[nchild+j].x,childoffsets[nchild+j].y,childoffsets[nchild+j].z,
					childrots[nchild+j].x,childrots[nchild+j].y,childrots[nchild+j].z,
					childscales[nchild+j].x,childscales[nchild+j].y,childscales[nchild+j].z,
					childnames[nchild+j]); */
			nchild+=i;
			break;
		default:
			logger("unknown form %s\n",printform(form));
//			fileclose();
			fclose(fp);
//			popdir();
			return 0;
		}
	}
//	fileclose();
	fclose(fp);
	if (dobuild) {
		t=NULL;
		if (texname[0])
			t=buildandfreejrmmodel(modelname,texname,verts,uvs,faces,groups,nvgroup);
		else
			t=buildandfreejrmmodel(modelname,      0,verts,uvs,faces,groups,nvgroup);
		if (t) {
			treelist[ntreelist++]=t;
//			t->dissolve=.5f;
			if (1) {//usenewprs) {
				t->trans=npos;
				t->rot.x=normalangrad(nrot.x*PIOVER180);
				t->rot.y=normalangrad(nrot.y*PIOVER180);
				t->rot.z=normalangrad(nrot.z*PIOVER180);
				t->scale=nscale;
			}
		}
	}
//	popdir();
//	s=alloctree(MAXG,NULL);
	s=new tree2("jrm2");
	for (i=0;i<ntreelist;i++) {
		for (j=0;j<nchild;j++) {
			if (!strcmp(childnames[j],treelist[i]->name.c_str()))
				break;
		}
		if (j==nchild)
//			linkchildtoparent(treelist[i],s);
			s->linkchild(treelist[i]);
		else {
			k=childidx[j];
//			logger("child k %d, j %d, (%f %f %f)\n",k,j,childrots[j].x,childrots[j].y,childrots[j].z);
//			linkchildtoparent(treelist[i],treelist[k]);
			treelist[k]->linkchild(treelist[i]);
			if (0) {//!usenewprs) {
				treelist[i]->scale=childscales[j];

				treelist[i]->rot.x=normalangrad(childrots[j].x*PIOVER180);
				treelist[i]->rot.y=normalangrad(childrots[j].y*PIOVER180);
				treelist[i]->rot.z=normalangrad(childrots[j].z*PIOVER180);
				treelist[i]->trans=childoffsets[j];
			}
		}
	}
//	logger("DONE loadnewjrms!\n");
//	mystrncpy(s->name,carnamearg,NAMESIZE);
	s->name=carnamearg;
//	s->rot.x=-PI/2;
	pointf3 jrmcarrotr;
	jrmcarrotr.x=jrmcarrot.x*PIOVER180;
	jrmcarrotr.y=jrmcarrot.y*PIOVER180;
	jrmcarrotr.z=jrmcarrot.z*PIOVER180;
	jrmcarrotr.w=1;
	s->rot=jrmcarrotr;
	s->trans=jrmcartrans;//od.newcaroffy;
//	setVEC(&s->scale,jrmcarscale,jrmcarscale,jrmcarscale);
	s->scale=pointf3x(jrmcarscale,jrmcarscale,jrmcarscale);
	return s;
}
#if 0
//float shinycarscale;
//VEC shinycarrot;
//VEC shinycarrotr;
//VEC shinycartrans;

/*static void dupmodelrec(TREE *t)
{
	char str[50];
	int i;
//	dupmodel(t); // doesn't work for reflecttex
	if (t->mod) {
		strcpy(str,"D_");
		strcat(str,t->mod->name);
		str[NAMESIZE-1]='\0';
		strcpy(t->mod->name,str);
	}
	for (i=0;i<t->nchildren;i++)
		dupmodelrec(t->children[i]);
}
*/
/*TREE *loadshinycar(char *name)
{
	extern int timalpha;
	char str[50];
	TREE *ret=NULL;
	timalpha=1;
	pushandsetdir(name);
	sprintf(str,"%s.mxs",name);
	if (fileexist(str)) {
		ret=loadscene(str);
		shinycarrotr.x=shinycarrot.x*PIOVER180;
		shinycarrotr.y=shinycarrot.y*PIOVER180;
		shinycarrotr.z=shinycarrot.z*PIOVER180;
		ret->rot=shinycarrotr;
		ret->trans=shinycartrans;
		setVEC(&ret->scale,shinycarscale,shinycarscale,shinycarscale);
		dupmodelrec(ret);
	}
	popdir();
	timalpha=0;
	return ret;
}
*/
////////////////////// start of some new jrm and w3d file extraction stuff //////////////////////////////////
////////////////////// start of some new jrm and w3d file extraction stuff //////////////////////////////////
////////////////////// start of some new jrm and w3d file extraction stuff //////////////////////////////////

// check the .w3d and .jrm for file stability, if 30 seconds since any read or writes, before or after
static int filestable(char *name)
{
	return 1; // doesn't work well in 98 when multiple windows are running ERROR_SHARING_VIOLATION
	float range;
	DWORD diff;
	HANDLE fh;
	FILETIME ftt,fts,ftf={0,0}; // "100 nano second" timers
	GetSystemTimeAsFileTime(&fts);
	fh=CreateFile(name,0,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if (fh!=INVALID_HANDLE_VALUE) {
		GetFileTime(fh,NULL,NULL,&ftf);
	} else {
		ftf.dwHighDateTime=0xffffffff;
		ftf.dwLowDateTime=0xffffffff;
		logger("filestable GetLastError %08x\n",GetLastError());
		exit(1);
		return 0;
	}
	CloseHandle(fh);

	logger("file stable check %30s:    systime %08X:%08X, filetime %08X:%08X\n",
		name,fts.dwHighDateTime,fts.dwLowDateTime,ftf.dwHighDateTime,ftf.dwLowDateTime);
	if (ftf.dwHighDateTime>fts.dwHighDateTime ||
		(ftf.dwHighDateTime==fts.dwHighDateTime && ftf.dwLowDateTime>fts.dwLowDateTime)) {
		ftt=fts;
		fts=ftf;
		ftf=ftt;
	}
	ftt.dwHighDateTime=fts.dwHighDateTime-ftf.dwHighDateTime;
	ftt.dwLowDateTime=fts.dwLowDateTime-ftf.dwLowDateTime;
	if (fts.dwLowDateTime<ftf.dwLowDateTime)
		ftt.dwHighDateTime--; // borrow
	logger("file stable check diff %08x,%08x\n",ftt.dwHighDateTime,ftt.dwLowDateTime);
	if (ftt.dwHighDateTime>=0x100) {
		logger("hi range ");
		diff=ftt.dwHighDateTime;
		range=429.50f; // this number is 2^32 / 10 million
	} else {
		logger ("lo range ");
		diff=(ftt.dwHighDateTime<<16)|((ftt.dwLowDateTime>>16)&0xffff);
		range=429.50f/65536.0f;
	}
	range*=diff;
	logger("diff time float is %f\n",range); // in seconds
	if (range>30)
		return 1;
	return 0;
}
#endif
// apply a decal 'd' with mask 'dm' to a bitmap 'b'
static void applymask(struct bitmap32 *b,struct bitmap32 *d,struct bitmap8 *dm)
{
//	C32 cbv,cdv,cdmv;
	C32 bv,dv,nbv;
	U32 dmv;
	int i,j;
	U8* pdmv=dm->data;
	for (j=0;j<b->size.y;j++)
		for (i=0;i<b->size.x;i++) {
			bv=clipgetpixel32(b,i,j);
			dv=clipgetpixel32(d,i,j);
/*			if (texformat==TEX565NCK) {
				cbv=color565to24(bv);
				cdv=color565to24(dv);
			} else if (texformat==TEX555NCK) {
				cbv=color555to24(bv);
				cdv=color555to24(dv);
			} else
				errorexit("wrong tex format %d\n",texformat); */
			dmv=*pdmv++;
//			dmv=255;
			nbv.r=bv.r+(dv.r-bv.r)*dmv/255;
			nbv.g=bv.g+(dv.g-bv.g)*dmv/255;
			nbv.b=bv.b+(dv.b-bv.b)*dmv/255;
//			nbv.r=nbv.g=nbv.b=dmv;
/*			if (texformat==TEX565NCK)
				nbv=color24to32(cdmv,565);
			else
				nbv=color24to32(cdmv,555); */
			clipputpixel32(b,i,j,nbv);
		}
}

static int utcount;
static textureb* newtex;
//static int jrmtexformat,jrmbppformat;
static void changetexrec(tree2* t,struct bitmap32 *tex32)
{
	char str[80];
	textureb *oldtex;
	if (t->mod) {
		oldtex=t->mod->mats[0].thetexarr[0];
		if (oldtex) {
//			freetexture(oldtex);
			textureb::rc.deleterc(oldtex);
			if (!newtex) {
				sprintf(str,"ut_%d",utcount++);
//				newtex=buildtexture(str,tex32->x,tex32->y,jrmtexformat);
				newtex=texture_create(str);
//				locktexture(newtex);
//				clipblit32(tex32,newtex->texdata,0,0,0,0,tex32->x,tex32->y);
//				unlocktexture(newtex);
				newtex->addbitmap(tex32,false);
			}
			t->mod->mats[0].thetexarr[0]=textureb::rc.newrc(newtex);
//			newtex->refcount++;
		}
	}
	list<tree2*>::iterator i;
	for (i=t->children.begin();i!=t->children.end();++i)
		changetexrec(*i,tex32);
}
static void switchtex(tree2 *jrm,bitmap32 *tex32)
{
	changetexrec(jrm,tex32);
	if (newtex)
//		freetexture(newtex);
	textureb::rc.deleterc(newtex);
	newtex=0;
}
static struct bitmap32 *scanforpaint(char *filename,int paint)
{
//	struct bitmap24 *b24;
	struct bitmap32 *tex32;
	int flen,curpos=0;
	unsigned int id,len,skp,i,j;
	int lowestpaint=100000,highestpaint=0,curpaint;
	int lowestpaintp=0,curpaintp=0,foundpaintp=0;	// file index
//	fileopen(filename,READ);
	FILE* fp=fopen2(filename,"rb");
	flen=getfilesize(fp);
	while(curpos<flen) {
//		offsets[noffsets++]=curpos;
		id=filereadU32LE(fp);
		len=filereadU32LE(fp);
//		logger("curpos %08x: id %08x, len %08x\n",curpos,id,len);
		skp=(len+3)&0xfffffffc;
		if (id==0xffffff21) { // found a bitmap
			char *p;
			unsigned char texname[80];
			i=filereadU16LE(fp);
			if (i>=80)
				errorexit("w3d tex name too long");
//			fileread(texname,i);
			fread(texname,1,i,fp);
			texname[i]='\0';
			my_strlwr((char *)texname);
//			j=filereadbyte();
			j=filereadU8(fp);
			if (j==1) { // found a jpeg type
//				if (strlen(texname)<6)
//					errorexit("texname too short '%s'",texname);
				if (p=strstr((char *)texname,"paint")) { // found one with 'paint' in it
					curpaint=atoi(p+5);
//					curpaintp=fileskip(0,FILE_CURRENT);
					curpaintp=ftell(fp);
					if (curpaint==paint)
						foundpaintp=curpaintp;
					if (curpaint<0 || curpaint>=100)
						errorexit("bad number");
					if (curpaint<lowestpaint) {
						lowestpaint=curpaint;
						lowestpaintp=curpaintp;
					}
					if (curpaint>highestpaint)
						highestpaint=curpaint;
				}
			}
		}
		curpos+=skp+sizeof(id)+sizeof(len);
//		fileskip(curpos,FILE_START);
		fseek(fp,curpos,SEEK_SET);
	}
	if (!foundpaintp)
		foundpaintp=lowestpaintp;
	if (!foundpaintp) {
//		fileclose();
		fclose(fp);
		return 0;
	}
//	logger("lowest paint %d, highest %d\n",lowestpaint,highestpaint);
//	fileskip(foundpaintp,FILE_START);
	fseek(fp,foundpaintp,SEEK_SET);
	tex32=jpegread(fp);
//	tex32=conv24to32ck(b24,jrmbppformat);
//	bitmap24free(b24);
//	fileclose();
	fclose(fp);
	return tex32;
}
static struct bitmap32 *scanfordecal(char *filename,int decal)
{
//	struct bitmap24 *b24;
	struct bitmap32 *tex32;
	int flen,curpos=0;
	unsigned int id,len,skp,i,j;
	int lowestdecal=100000,highestdecal=0,curdecal;
	int lowestdecalp=0,curdecalp=0,founddecalp=0;	// file index
//	fileopen(filename,READ);
	FILE* fp=fopen2(filename,"rb");
	flen=getfilesize(fp);
	while(curpos<flen) {
//		offsets[noffsets++]=curpos;
//		id=filereadlong2();
		id=filereadU32LE(fp);
//		len=filereadlong2();
		len=filereadU32LE(fp);
//		logger("curpos %08x: id %08x, len %08x\n",curpos,id,len);
		skp=(len+3)&0xfffffffc;
		if (id==0xffffff21) { // found a bitmap
			char *p;
			unsigned char texname[80];
//			i=filereadword2();
			i=filereadU16LE(fp);
			if (i>=80)
				errorexit("w3d tex name too long");
//			fileread(texname,i);
			fread(texname,1,i,fp);
			texname[i]='\0';
			my_strlwr((char *)texname);
//			j=filereadbyte();
			j=filereadU8(fp);
			if (j==1) { // found a jpeg type
//				if (strlen(texname)<6)
//					errorexit("texname too short '%s'",texname);
				if (p=strstr((char *)texname,"decal")) { // found one with 'decal' in it
					curdecal=atoi(p+5);
//					curdecalp=fileskip(0,FILE_CURRENT);
					curdecalp=ftell(fp);
					if (curdecal==decal)
						founddecalp=curdecalp;
					if (curdecal<0 || curdecal>=100)
						errorexit("bad number");
					if (curdecal<lowestdecal) {
						lowestdecal=curdecal;
						lowestdecalp=curdecalp;
					}
					if (curdecal>highestdecal)
						highestdecal=curdecal;
				}
			}
		}
		curpos+=skp+sizeof(id)+sizeof(len);
//		fileskip(curpos,FILE_START);
		fseek(fp,curpos,SEEK_SET);
	}
	if (!founddecalp) {
//		fileclose();
		fclose(fp);
//		return NULL;
		return 0;
	}
//	fileskip(founddecalp,FILE_START);
	fseek(fp,founddecalp,SEEK_SET);
	tex32=jpegread(fp);
//	tex32=conv24to32ck(b24,jrmbppformat);
//	bitmap24free(b24);
//	fileclose();
	fclose(fp);
//	logger("lowest decal %d, highest %d\n",lowestdecal,highestdecal);
	return tex32;
}

static struct bitmap8 *scanfordecalmask(char *filename,int decal)
{
	unsigned char *compdata;
	int x,y,s;//,n;
	struct bitmap8 *tex;
	int flen,curpos=0;
	unsigned int id,len,skp,i,j;
	int lowestdecal=100000,highestdecal=0,curdecal;
	int lowestdecalp=0,curdecalp=0,founddecalp=0;	// file index
//	fileopen(filename,READ);
	FILE* fp=fopen2(filename,"rb");
	flen=getfilesize(fp);
	while(curpos<flen) {
//		offsets[noffsets++]=curpos;
//		id=filereadlong2();
		id=filereadU32LE(fp);
//		len=filereadlong2();
		len=filereadU32LE(fp);
//		logger("curpos %08x: id %08x, len %08x\n",curpos,id,len);
		skp=(len+3)&0xfffffffc;
		if (id==0xffffff21) { // found a bitmap
			char *p;
			unsigned char texname[80];
//			i=filereadword2();
			i=filereadU16LE(fp);
			if (i>=80)
				errorexit("w3d tex name too long");
//			fileread(texname,i);
			fread(texname,1,i,fp);
			texname[i]='\0';
			my_strlwr((char *)texname);
//			j=filereadbyte();
			j=filereadU8(fp);
			if (j==3) { // found a IDAT png like type
//				if (strlen(texname)<6)
//					errorexit("texname too short '%s'",texname);
				if (p=strstr((char *)texname,"decal")) { // found one with 'decal' in it
					curdecal=atoi(p+5);
//					curdecalp=fileskip(0,FILE_CURRENT);
					curdecalp=ftell(fp);
					if (curdecal==decal)
						founddecalp=curdecalp;
					if (curdecal<0 || curdecal>=100)
						errorexit("bad number");
					if (curdecal<lowestdecal) {
						lowestdecal=curdecal;
						lowestdecalp=curdecalp;
					}
					if (curdecal>highestdecal)
						highestdecal=curdecal;
				}
			}
		}
		curpos+=skp+sizeof(id)+sizeof(len);
//		fileskip(curpos,FILE_START);
		fseek(fp,curpos,SEEK_SET);
	}
	if (!founddecalp) {
//		fileclose();
		fclose(fp);
//		return NULL;
		return 0;
	}
//	fileskip(founddecalp,FILE_START);
	fseek(fp,founddecalp,SEEK_SET);

//	x=filereadlong2();
//	y=filereadlong2();
//	s=filereadlong2();
	x=filereadU32LE(fp);
	y=filereadU32LE(fp);
	s=filereadU32LE(fp);
//	compdata=(unsigned char *)memalloc(s);
	compdata=new U8[s];
//	fileread(compdata,s);
	fread(compdata,1,s,fp);
	tex=deflate_decompress_mask(compdata,s,x,y);
//	memfree(compdata);
	delete[] compdata;
//	fileclose();
	fclose(fp);
	return tex;
}

static char eofsig[584]={
	0x54,0x58,0x55,0x56,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x54,0x58,0x55,0x56,
	0x00,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x54,0x58,0x55,0x56,0x00,0x00,0x00,0x00,
	0x03,0x00,0x00,0x00,0x54,0x58,0x55,0x56,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,
	0x54,0x58,0x55,0x56,0x00,0x00,0x00,0x00,0x05,0x00,0x00,0x00,0x54,0x58,0x55,0x56,
	0x00,0x00,0x00,0x00,0x06,0x00,0x00,0x00,0x54,0x58,0x55,0x56,0x00,0x00,0x00,0x00,
	0x07,0x00,0x00,0x00,0x54,0x58,0x55,0x56,0x00,0x00,0x00,0x00,0x08,0x00,0x00,0x00,
	0x54,0x58,0x55,0x56,0x00,0x00,0x00,0x00,0x09,0x00,0x00,0x00,0x54,0x58,0x55,0x56,
	0x00,0x00,0x00,0x00,0x0A,0x00,0x00,0x00,0x54,0x58,0x55,0x56,0x00,0x00,0x00,0x00,
	0x0B,0x00,0x00,0x00,0x54,0x58,0x55,0x56,0x00,0x00,0x00,0x00,0x0C,0x00,0x00,0x00,
	0x46,0x41,0x43,0x45,0x02,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x01,0x00,0x00,0x00,
	0x02,0x00,0x00,0x00,0x03,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x03,0x00,0x00,0x00,
	0x02,0x00,0x00,0x00,0x46,0x41,0x43,0x45,0x02,0x00,0x00,0x00,0x02,0x00,0x00,0x00,
	0x01,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x03,0x00,0x00,0x00,0x04,0x00,0x00,0x00,
	0x03,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x46,0x41,0x43,0x45,0x02,0x00,0x00,0x00,
	0x03,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x03,0x00,0x00,0x00,
	0x04,0x00,0x00,0x00,0x03,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x46,0x41,0x43,0x45,
	0x02,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x02,0x00,0x00,0x00,
	0x03,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x03,0x00,0x00,0x00,0x02,0x00,0x00,0x00,
	0x46,0x41,0x43,0x45,0x02,0x00,0x00,0x00,0x05,0x00,0x00,0x00,0x01,0x00,0x00,0x00,
	0x02,0x00,0x00,0x00,0x03,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x03,0x00,0x00,0x00,
	0x02,0x00,0x00,0x00,0x46,0x41,0x43,0x45,0x02,0x00,0x00,0x00,0x06,0x00,0x00,0x00,
	0x01,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x03,0x00,0x00,0x00,0x04,0x00,0x00,0x00,
	0x03,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x46,0x41,0x43,0x45,0x02,0x00,0x00,0x00,
	0x07,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x03,0x00,0x00,0x00,
	0x04,0x00,0x00,0x00,0x03,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x46,0x41,0x43,0x45,
	0x02,0x00,0x00,0x00,0x08,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x02,0x00,0x00,0x00,
	0x03,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x03,0x00,0x00,0x00,0x02,0x00,0x00,0x00,
	0x46,0x41,0x43,0x45,0x02,0x00,0x00,0x00,0x09,0x00,0x00,0x00,0x01,0x00,0x00,0x00,
	0x02,0x00,0x00,0x00,0x03,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x03,0x00,0x00,0x00,
	0x02,0x00,0x00,0x00,0x46,0x41,0x43,0x45,0x02,0x00,0x00,0x00,0x0A,0x00,0x00,0x00,
	0x01,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x03,0x00,0x00,0x00,0x04,0x00,0x00,0x00,
	0x03,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x46,0x41,0x43,0x45,0x02,0x00,0x00,0x00,
	0x0B,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x03,0x00,0x00,0x00,
	0x04,0x00,0x00,0x00,0x03,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x46,0x41,0x43,0x45,
	0x02,0x00,0x00,0x00,0x0C,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x02,0x00,0x00,0x00,
	0x03,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x03,0x00,0x00,0x00,0x02,0x00,0x00,0x00,
	0x43,0x48,0x4C,0x44,0x00,0x00,0x00,0x00,
};
tree2* loadnewjrms2(const char *basecarname,int paint,int decal)
{
	setjrmcar();
	tree2* jrm;
	int i,bad=0;
	char eofarea[584];
	struct bitmap32 *bm32,*decalb32;
	struct bitmap8* decalmaskb8;
	char /*jrmfolder[300],*/carnamejrm[300],w3dnametext[300];
// setup the 2 filenames, check for stability
//	for (i=0;i<MAXLIST;i++)
//		if (!my_stricmp(excludelist[i],basecarname))
//			return NULL;
//	strcpy(jrmfolder,dswmediadir);
//	strcat(jrmfolder,"cajrm");
pushandsetdir("newcarenadata/cardata");
	sprintf(carnamejrm,"%s.jrm",basecarname);
//	if (!filestable(carnamejrm)) {
//popdir();
//		return NULL;
//	}
//	fileopen(carnamejrm,READ);
	FILE* fp=fopen2(carnamejrm,"rb");
//	i=fileskip(0,FILE_END);
	fseek(fp,0,SEEK_END);
	i=ftell(fp);
	if (i<=584)
		bad=1;
	else {
//		fileskip(-584,FILE_END);
		fseek(fp,-584,SEEK_END);
//		fileread(eofarea,584);
		fread(eofarea,1,584,fp);
		bad=memcmp(eofarea,eofsig,584);
	}
//	fileclose();
	fclose(fp);
	if (bad) {
//		DeleteFile(carnamejrm);
popdir();
		errorexit("can't bad");
		return 0;
	}
//	popdir();
//	pushandsetdir(dswmediadir);
	sprintf(w3dnametext,"tcr%stexturebank.w3d",basecarname);
//	if (!filestable(w3dnametext)) {
//popdir();
//		return NULL;
//	}
//	popdir();
// get dest tex format and color conversions
/*	if (texavail[TEX565NCK]) {
		jrmtexformat=TEX565NCK;
		jrmbppformat=565;
	} else if (texavail[TEX555NCK]) {
		jrmtexformat=TEX555NCK;
		jrmbppformat=555;
	} */
// get and combine new texture
//	pushandsetdir(dswmediadir);
	bm32=scanforpaint(w3dnametext,paint);
	if (!bm32) {
popdir();
		errorexit("can't bm32");
		return 0;
	}
	decalb32=scanfordecal(w3dnametext,decal);
	decalmaskb8=scanfordecalmask(w3dnametext,decal);
	if (decalb32 && decalmaskb8)
		applymask(bm32,decalb32,decalmaskb8);//jrmtexformat);
//	popdir();
// get jrm
	jrm=loadnewjrms(basecarname,"dummy.tex");
	if (!jrm)
		errorexit("can't load new jrms");
// apply new texture
	switchtex(jrm,bm32);
	bitmap32free(bm32);
	if (decalb32)
		bitmap32free(decalb32);
	if (decalmaskb8)
		bitmap8free(decalmaskb8);
popdir();
	if (!jrm)
		errorexit("can't jrm");
	return jrm;
}
