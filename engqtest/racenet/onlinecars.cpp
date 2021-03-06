// this is where jrm's are loaded into a TREE * struct
#include <windows.h>

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include <engine1.h>
#include <misclib.h>
#include "vidd3d.h"

#include "deflate.h"
#include "onlinecars.h"

char excludelist[MAXLIST][80];//={
//int usenewprs;
#pragma pack(1)
struct olctimestamp {
	unsigned short month,day,year;
};
#pragma pack()
#define MAXG 60
#define makeform(a,b,c,d) (((d)<<24)+((c)<<16)+((b)<<8)+(a))
extern int usenocolorkey; // force textures to be opaque
static int facemode,altcars;
static char *printform(unsigned int f)
{
	static char frm[5];
	frm[4]='\0';
	memcpy(frm,&f,4);
	return frm;
}

static TREE *buildandfreejrmmodel(char *modelnamearg,char *texnamearg,struct pointf3 *verts,int nvert,
							struct uv *uvs,struct face *faces,int nfaces,
							struct group *groups,int ngroup)
{
	int i,j;
	static int mn;
	char modelname[300],texname[300];
	TREE *scene,*part;
	logger("jrm model name '%s'\n",modelnamearg);
	if (ngroup<1 || nfaces<1 || nvert<1)
		return NULL;
	scene=alloctree(MAXG,NULL);
	mystrncpy(scene->name,modelnamearg,NAMESIZE);
	usenocolorkey=1;
	for (j=0;j<ngroup;j++) {
		if (texnamearg)
			if (altcars)
				sprintf(texname,"p_%s_texture.tga",texnamearg);
			else
				sprintf(texname,"%s_texture.tga",texnamearg);
		sprintf(modelname,"%s_%d",modelnamearg,mn);
		buildmodelstart(modelname);
		if (texnamearg) {
			if (uselights) {
				buildmodelsetmaterial(
					SMAT_HASTEX|SMAT_HASWBUFF|SMAT_WRAPU|SMAT_WRAPV|SMAT_CALCLIGHTS|SMAT_HASSHADE,
					texname,NULL,modelname,NULL);
			} else {
				buildmodelsetmaterial(
					SMAT_HASTEX|SMAT_HASWBUFF|SMAT_WRAPU|SMAT_WRAPV,
					texname,NULL,modelname,NULL);
			}
		} else {
			if (uselights) {
				buildmodelsetmaterial(
					SMAT_HASWBUFF|SMAT_CALCLIGHTS|SMAT_HASSHADE,
					NULL,NULL,modelname,NULL);
			} else {
				buildmodelsetmaterial(
					SMAT_HASWBUFF,NULL,NULL,modelname,NULL);
			}
		}
		if (uvs)
			buildmodeladduvs(verts+groups[j].vertidx,uvs+groups[j].vertidx,groups[j].nvert);
		else
			buildmodeladdflat(verts+groups[j].vertidx,groups[j].nvert);
		if (groups[j].nface==2)
			facemode=3;
		else
			facemode=1;
		for (i=0;i<groups[j].nface;i++) {
			if (facemode!=2)
				buildmodelface(faces[i+groups[j].faceidx].vertidx[0],
							   faces[i+groups[j].faceidx].vertidx[1],
							   faces[i+groups[j].faceidx].vertidx[2]);
			if (facemode!=1)
				buildmodelface(faces[i+groups[j].faceidx].vertidx[1],
							   faces[i+groups[j].faceidx].vertidx[0],
							   faces[i+groups[j].faceidx].vertidx[2]);
		}
		part=alloctreebuildmodelend(MAXG);
		mystrncpy(part->name,modelnamearg,NAMESIZE);
		linkchildtoparent(part,scene);
		mn++;
	}
	usenocolorkey=0;
	memfree(verts);
	memfree(faces);
	if (uvs)
		memfree(uvs);
	if (ngroup==1) {
		unhooktree(part);
		freetree(scene);
		return part;
	}
	return scene;
}

static int checkprs(char *name,VEC *pos,VEC *rot,VEC *scale)
{
	char *str=name;
	logger("the name is '%s'\n",name);
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
	logger("new prs for '%s' is (%f,%f,%f) (%f,%f,%f) (%f,%f,%f)\n",
		name,pos->x,pos->y,pos->z,rot->x,rot->y,rot->z,scale->x,scale->y,scale->z);
	return 1;
}

float jrmcarscale;
VEC jrmcarrot;
VEC jrmcarrotr;
VEC jrmcartrans;
TREE *loadnewjrms(char *carnamearg,char *textnamearg)
{
	int hasnewprs=0;
//	extern char dswmediadir[];
	char jrmfolder[300];
	unsigned int form;
	int rd;
	struct olctimestamp ts;
	char carname[300];
	char childnames[MAXG][NAMESIZE];
	struct pointf3 childoffsets[MAXG];
	struct pointf3 childscales[MAXG];
	struct pointf3 childrots[MAXG];
	int childidx[MAXG];
	int nchild;
	TREE *treelist[MAXG];
	int ntreelist;
	char modelname[300],texname[NAMESIZE];
	int dobuild=0;
	TREE *s,*t;

	struct group groups[MAXG];
	int nvgroup,nfgroup,nuvgroup;
	int nvert;
	struct pointf3 *verts;
	int nuv;
	struct uv *uvs;
	struct face *faces;
	int nface;
	int i,j,k;
	VEC npos,nrot,nscale;
// load up some cars !!!
//	logger("load binary online car data '%s' '%s' ------------------------------------------\n",carnamearg,textnamearg);
////////////////////////////// vertices ///////////////////////////////
//	if (!stubinfo.useslextra)
//		return NULL;
	logger("loading carena car '%s' '%s'\n",carnamearg,textnamearg);
	strcpy(jrmfolder,dswmediadir);
	strcat(jrmfolder,"cajrm");
	pushandsetdir(jrmfolder);
	sprintf(carname,"%s.jrm",carnamearg);
	altcars=0;
	if (!fileexist(carname)) {
		logger("error: bin %s doesn't exist, trying alternate\n",carname);
		sprintf(carname,"p_%s.jrm",carnamearg);
		if (!fileexist(carname)) {
			logger("error: bin %s doesn't exist\n",carname);
			popdir();
			return NULL;
		}
		altcars=1;
	}
	fileopen(carname,READ);
	rd=fileread(&ts,sizeof(ts));
	if (rd!=6) {
		logger("file too small!\n");
		fileclose();
		popdir();
		return NULL;
	}
//	logger("online car '%s' has date %2d/%2d/%4d\n",carname,ts.month,ts.day,ts.year);
	ntreelist=nchild=0;
	texname[0]='\0';
	while(1) {
		rd=fileread(&form,sizeof(form)); // read form
		if (!rd)
			break;
		if (rd!=sizeof(form)) {
			logger("no form!\n");
			fileclose();
			popdir();
			return NULL;
		}
		switch(form) {
		case makeform('F','R','A','M'):
			fileread(&i,sizeof(i));
//			logger("FRAM: framenumber %d\n",i);
			break;
		case makeform('M','D','L','N'):
			if (dobuild) {
				t=NULL;
				if (texname[0])
					t=buildandfreejrmmodel(modelname,texname,verts,nvert,uvs,faces,nface,groups,nvgroup);
				else
					t=buildandfreejrmmodel(modelname,NULL,verts,nvert,NULL,faces,nface,groups,nvgroup);
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
			nvgroup=nuvgroup=nfgroup=nvert=nface=nuv=0;
			verts=(void *)faces=(void *)uvs=NULL;
			texname[0]='\0';
			fileread(&i,sizeof(i));
			if (i>=300) {
				logger("name too big\n");
				popdir();
				fileclose();
				return NULL;
			}
			fileread(modelname,i);
			modelname[i]='\0';
			hasnewprs=checkprs(modelname,&npos,&nrot,&nscale);
//			logger("MDLN: model name is '%s'\n",modelname);
			break;
		case makeform('M','T','E','X'):
			fileread(&i,sizeof(i)); // get num letters
			fileread(&j,sizeof(j)); // get tex number
			fileread(texname,i);
			texname[i]='\0';
//			logger("MTEX: texidx %d, name '%s'\n",j,texname);
			if (!strcmp(texname,"NONEATALL"))
				texname[0]='\0';
			else if (textnamearg)
				strcpy(texname,textnamearg);
			break;
		case makeform('V','E','R','T'):
			fileread(&i,sizeof(i)); // get num verts 
			fileread(&j,sizeof(j)); // get mesh #
//			logger("VERT:   nvert %d, meshidx %d\n",i,j);
			if (j-1!=nvgroup)
				errorexit("meshv doesn't mesh '%s'\n",carnamearg);
			groups[nvgroup].vertidx=nvert;
			groups[nvgroup].nvert=i;
//			logger("meshgroupv %d: bin vertidx %d, nvert %d\n",nvgroup,nvert,i);
			nvgroup++;
			if (i)
				verts=memrealloc(verts,sizeof(struct pointf3)*(i+nvert));
			for (k=0;k<i;k++) {
				fileread(&verts[nvert+k],12);
				verts[nvert+k].w=0;
			}
			nvert+=i;
			break;
		case makeform('T','X','U','V'):
			fileread(&i,sizeof(i)); // get num verts 
			fileread(&j,sizeof(j)); // get mesh #
//			logger("TXUV:   nvert %d, meshidx %d\n",i,j);
			if (j-1!=nuvgroup)
				errorexit("meshuv doesn't mesh '%s'\n",carnamearg);
//			logger("meshgroupuv %d: bin uvidx %d, nuv %d\n",nuvgroup,nuv,i);
			nuvgroup++;
			if (i)
				uvs=memrealloc(uvs,sizeof(struct uv)*(i+nuv));
			for (k=0;k<i;k++) {
				fileread(&uvs[nuv+k],8);
				uvs[nuv+k].v=1-uvs[nuv+k].v;
				uvs[nuv+k].w=0;
			}
			nuv+=i;
			break;
		case makeform('F','A','C','E'):
			fileread(&i,sizeof(i)); // get num faces 
			fileread(&j,sizeof(j)); // get mesh #
//			logger("FACE:   nface %d, meshidx %d\n",i,j);
			if (j-1!=nfgroup)
				errorexit("meshf doesn't mesh '%s'\n",carnamearg);
			groups[nfgroup].faceidx=nface;
			groups[nfgroup].nface=i;
//			logger("meshgroupf %d: bin faceidx %d, nface %d\n",nfgroup,nface,i);
			nfgroup++;
			if (i)
				faces=memrealloc(faces,sizeof(struct face)*(i+nface));
			for (k=0;k<i;k++) {
				fileread(&j,4);
				faces[nface+k].vertidx[0]=j-1;
				fileread(&j,4);
				faces[nface+k].vertidx[1]=j-1;
				fileread(&j,4);
				faces[nface+k].vertidx[2]=j-1;
				faces[nface+k].matidx=0;
			}
			nface+=i;
			break;
		case makeform('C','H','L','D'):
			fileread(&i,sizeof(i)); // get num children 
			for (j=0;j<i;j++) {
				k=0;
				while(1) {
					fileread(&childnames[nchild+j][k],1);
					if (childnames[nchild+j][k]=='\0')
						break;
					k++;
					if (k>=NAMESIZE) {
						logger("child names too big in binonlinecars\n");
						popdir();
						fileclose();
						return NULL;
					}
				}
			}
			for (j=0;j<i;j++) {
				childidx[nchild+j]=ntreelist;
				fileread(&childoffsets[nchild+j].x,sizeof(float));
				fileread(&childoffsets[nchild+j].y,sizeof(float));
				fileread(&childoffsets[nchild+j].z,sizeof(float));
				childoffsets[nchild+j].w=0;
				fileread(&childscales[nchild+j].x,sizeof(float));
				fileread(&childscales[nchild+j].y,sizeof(float));
				fileread(&childscales[nchild+j].z,sizeof(float));
				childscales[nchild+j].w=0;
				fileread(&childrots[nchild+j].x,sizeof(float));
				fileread(&childrots[nchild+j].y,sizeof(float));
				fileread(&childrots[nchild+j].z,sizeof(float));
				childrots[nchild+j].w=0;
			}
			logger("CHLD: nchild %d\n",i);
			for (j=0;j<i;j++)
				logger("       offset %f,%f,%f, rot %f,%9f,%f, scale %f,%f,%f, name '%s'\n",
					childoffsets[nchild+j].x,childoffsets[nchild+j].y,childoffsets[nchild+j].z,
					childrots[nchild+j].x,childrots[nchild+j].y,childrots[nchild+j].z,
					childscales[nchild+j].x,childscales[nchild+j].y,childscales[nchild+j].z,
					childnames[nchild+j]);
			nchild+=i;
			break;
		default:
			logger("unknown form %s\n",printform(form));
			fileclose();
			popdir();
			return NULL;
		}
	}
	fileclose();
	if (dobuild) {
		t=NULL;
		if (texname[0])
			t=buildandfreejrmmodel(modelname,texname,verts,nvert,uvs,faces,nface,groups,nvgroup);
		else
			t=buildandfreejrmmodel(modelname,NULL,verts,nvert,NULL,faces,nface,groups,nvgroup);
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
	popdir();
	s=alloctree(MAXG,NULL);
	for (i=0;i<ntreelist;i++) {
		for (j=0;j<nchild;j++) {
			if (!strcmp(childnames[j],treelist[i]->name))
				break;
		}
		if (j==nchild)
			linkchildtoparent(treelist[i],s);
		else {
			k=childidx[j];
			logger("child k %d, j %d, (%f %f %f)\n",k,j,childrots[j].x,childrots[j].y,childrots[j].z);
			linkchildtoparent(treelist[i],treelist[k]);
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
	mystrncpy(s->name,carnamearg,NAMESIZE);
//	s->rot.x=-PI/2;
	jrmcarrotr.x=jrmcarrot.x*PIOVER180;
	jrmcarrotr.y=jrmcarrot.y*PIOVER180;
	jrmcarrotr.z=jrmcarrot.z*PIOVER180;
	s->rot=jrmcarrotr;
	s->trans=jrmcartrans;//od.newcaroffy;
	setVEC(&s->scale,jrmcarscale,jrmcarscale,jrmcarscale);
	return s;
}

float shinycarscale;
VEC shinycarrot;
VEC shinycarrotr;
VEC shinycartrans;

void dupmodelrec(TREE *t)
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

TREE *loadshinycar(char *name)
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

////////////////////// start of some new jrm and w3d file extraction stuff //////////////////////////////////
////////////////////// start of some new jrm and w3d file extraction stuff //////////////////////////////////
////////////////////// start of some new jrm and w3d file extraction stuff //////////////////////////////////

// check the .w3d and .jrm for file stability, if 30 seconds since any read or writes, before or after
static int filestable(char *name)
{
	float range;
	DWORD diff;
	HANDLE fh;
	FILETIME ftt,fts,ftf={0,0};
	GetSystemTimeAsFileTime(&fts);
	fh=CreateFile(name,0,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if (fh!=INVALID_HANDLE_VALUE) {
		GetFileTime(fh,NULL,NULL,&ftf);
	} else {
		ftf.dwHighDateTime=0xffffffff;
		ftf.dwLowDateTime=0xffffffff;
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
		range=429.50f;
	} else {
		logger ("lo range ");
		diff=(ftt.dwHighDateTime<<16)|((ftt.dwLowDateTime>>16)&0xffff);
		range=429.50f/65536.0f;
	}
	range*=diff;
	logger("diff time float is %f\n",range);
	if (range>30)
		return 1;
	return 0;
}

// apply a decal 'd' with mask 'dm' to a bitmap 'b'
static void applymask(struct bitmap16 *b,struct bitmap16 *d,struct bitmap16 *dm,int texformat)
{
	struct rgb24 cbv,cdv,cdmv;
	unsigned int bv,dv,dmv,nbv;
	int i,j;
	for (j=0;j<b->y;j++)
		for (i=0;i<b->x;i++) {
			bv=clipgetpixel16(b,i,j);
			dv=clipgetpixel16(d,i,j);
			if (texformat==TEX565NCK) {
				cbv=color565to24(bv);
				cdv=color565to24(dv);
			} else if (texformat==TEX555NCK) {
				cbv=color555to24(bv);
				cdv=color555to24(dv);
			} else
				errorexit("wrong tex format %d\n",texformat);
			dmv=clipgetpixel16(dm,i,j);
			dmv&=0x1f;
			cdmv.r=cbv.r+(cdv.r-cbv.r)*dmv/32;
			cdmv.g=cbv.g+(cdv.g-cbv.g)*dmv/32;
			cdmv.b=cbv.b+(cdv.b-cbv.b)*dmv/32;
			if (texformat==TEX565NCK)
				nbv=color24to16(cdmv,565);
			else
				nbv=color24to16(cdmv,555);
			clipputpixel16(b,i,j,nbv);
		}
}

static int utcount;
static struct texture *newtex;
static int jrmtexformat,jrmbppformat;
static void changetexrec(TREE *t,struct bitmap16 *tex16)
{
	int i;
	char str[80];
	struct texture *oldtex;
	if (t->mod) {
		oldtex=t->mod->mats[0].thetex;
		if (oldtex) {
			if (!newtex) {
				sprintf(str,"ut_%d",utcount++);
				newtex=buildtexture(str,tex16->x,tex16->y,jrmtexformat);
				locktexture(newtex);
				clipblit16(tex16,newtex->texdata,0,0,0,0,tex16->x,tex16->y);
				unlocktexture(newtex);
			}
			t->mod->mats[0].thetex=newtex;
			freetexture(oldtex);
			newtex->refcount++;
		}
	}
	for (i=0;i<t->nchildren;i++)
		changetexrec(t->children[i],tex16);
}

static void switchtex(struct tree *jrm,struct bitmap16 *tex16)
{
	changetexrec(jrm,tex16);
	if (newtex)
		freetexture(newtex);
	newtex=NULL;
}

struct bitmap16 *scanforpaint(char *filename,int paint)
{
	struct bitmap24 *b24;
	struct bitmap16 *tex16;
	int flen,curpos=0;
	unsigned int id,len,skp,i,j;
	int lowestpaint=100000,highestpaint=0,curpaint;
	int lowestpaintp=0,curpaintp=0,foundpaintp=0;	// file index
	fileopen(filename,READ);
	flen=getfilesize();
	while(curpos<flen) {
//		offsets[noffsets++]=curpos;
		id=filereadlong2();
		len=filereadlong2();
//		logger("curpos %08x: id %08x, len %08x\n",curpos,id,len);
		skp=(len+3)&0xfffffffc;
		if (id==0xffffff21) { // found a bitmap
			char *p;
			unsigned char texname[80];
			i=filereadword2();
			if (i>=80)
				errorexit("w3d tex name too long");
			fileread(texname,i);
			my_strlwr((char *)texname);
			texname[i]='\0';
			j=filereadbyte();
			if (j==1) { // found a jpeg type
//				if (strlen(texname)<6)
//					errorexit("texname too short '%s'",texname);
				if (p=strstr((char *)texname,"paint")) { // found one with 'paint' in it
					curpaint=atoi(p+5);
					curpaintp=fileskip(0,FILE_CURRENT);
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
		fileskip(curpos,FILE_START);
	}
	if (!foundpaintp)
		foundpaintp=lowestpaintp;
	if (!foundpaintp) {
		fileclose();
		return NULL;
	}
	fileskip(foundpaintp,FILE_START);
	b24=jpegread();
	tex16=conv24to16ck(b24,jrmbppformat);
	bitmap24free(b24);
	fileclose();
	return tex16;
}

struct bitmap16 *scanfordecal(char *filename,int decal)
{
	struct bitmap24 *b24;
	struct bitmap16 *tex16;
	int flen,curpos=0;
	unsigned int id,len,skp,i,j;
	int lowestdecal=100000,highestdecal=0,curdecal;
	int lowestdecalp=0,curdecalp=0,founddecalp=0;	// file index
	fileopen(filename,READ);
	flen=getfilesize();
	while(curpos<flen) {
//		offsets[noffsets++]=curpos;
		id=filereadlong2();
		len=filereadlong2();
//		logger("curpos %08x: id %08x, len %08x\n",curpos,id,len);
		skp=(len+3)&0xfffffffc;
		if (id==0xffffff21) { // found a bitmap
			char *p;
			unsigned char texname[80];
			i=filereadword2();
			if (i>=80)
				errorexit("w3d tex name too long");
			fileread(texname,i);
			my_strlwr((char *)texname);
			texname[i]='\0';
			j=filereadbyte();
			if (j==1) { // found a jpeg type
//				if (strlen(texname)<6)
//					errorexit("texname too short '%s'",texname);
				if (p=strstr((char *)texname,"decal")) { // found one with 'decal' in it
					curdecal=atoi(p+5);
					curdecalp=fileskip(0,FILE_CURRENT);
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
		fileskip(curpos,FILE_START);
	}
	if (!founddecalp) {
		fileclose();
		return NULL;
	}
	fileskip(founddecalp,FILE_START);
	b24=jpegread();
	tex16=conv24to16ck(b24,jrmbppformat);
	bitmap24free(b24);
	fileclose();
	return tex16;
}

struct bitmap16 *scanfordecalmask(char *filename,int decal)
{
	unsigned char *compdata;
	int x,y,s;//,n;
	struct bitmap16 *tex16;
	int flen,curpos=0;
	unsigned int id,len,skp,i,j;
	int lowestdecal=100000,highestdecal=0,curdecal;
	int lowestdecalp=0,curdecalp=0,founddecalp=0;	// file index
	fileopen(filename,READ);
	flen=getfilesize();
	while(curpos<flen) {
//		offsets[noffsets++]=curpos;
		id=filereadlong2();
		len=filereadlong2();
//		logger("curpos %08x: id %08x, len %08x\n",curpos,id,len);
		skp=(len+3)&0xfffffffc;
		if (id==0xffffff21) { // found a bitmap
			char *p;
			unsigned char texname[80];
			i=filereadword2();
			if (i>=80)
				errorexit("w3d tex name too long");
			fileread(texname,i);
			my_strlwr((char *)texname);
			texname[i]='\0';
			j=filereadbyte();
			if (j==3) { // found a IDAT png like type
//				if (strlen(texname)<6)
//					errorexit("texname too short '%s'",texname);
				if (p=strstr((char *)texname,"decal")) { // found one with 'decal' in it
					curdecal=atoi(p+5);
					curdecalp=fileskip(0,FILE_CURRENT);
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
		fileskip(curpos,FILE_START);
	}
	if (!founddecalp) {
		fileclose();
		return NULL;
	}
	fileskip(founddecalp,FILE_START);

	x=filereadlong2();
	y=filereadlong2();
	s=filereadlong2();
	compdata=(unsigned char *)memalloc(s);
	fileread(compdata,s);
	tex16=deflate_decompress(compdata,s,x,y);
	memfree(compdata);
	fileclose();
	return tex16;
}


char eofsig[584]={
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

TREE *loadnewjrms2(char *basecarname,int paint,int decal)
{
	TREE *jrm;
	int i,bad=0;
	char eofarea[584];
	struct bitmap16 *bm16,*decalb16,*decalmaskb16;
	char jrmfolder[300],carnamejrm[300],w3dnametext[300];
// setup the 2 filenames, check for stability
	for (i=0;i<MAXLIST;i++)
		if (!my_stricmp(excludelist[i],basecarname))
			return NULL;
	strcpy(jrmfolder,dswmediadir);
	strcat(jrmfolder,"cajrm");
	pushandsetdir(jrmfolder);
	sprintf(carnamejrm,"%s.jrm",basecarname);
	if (!filestable(carnamejrm)) {
		popdir();
		return NULL;
	}
	fileopen(carnamejrm,READ);
	i=fileskip(0,FILE_END);
	if (i<=584)
		bad=1;
	else {
		fileskip(-584,FILE_END);
		fileread(eofarea,584);
		bad=memcmp(eofarea,eofsig,584);
	}
	fileclose();
	if (bad) {
		DeleteFile(carnamejrm);
		popdir();
		return NULL;
	}
	popdir();
	pushandsetdir(dswmediadir);
	sprintf(w3dnametext,"tcr%stexturebank.w3d",basecarname);
	if (!filestable(w3dnametext)) {
		popdir();
		return NULL;
	}
	popdir();
// get dest tex format and color conversions
	if (texavail[TEX565NCK]) {
		jrmtexformat=TEX565NCK;
		jrmbppformat=565;
	} else if (texavail[TEX555NCK]) {
		jrmtexformat=TEX555NCK;
		jrmbppformat=555;
	}
// get and combine new texture
	pushandsetdir(dswmediadir);
	bm16=scanforpaint(w3dnametext,paint);
	if (!bm16) {
		popdir();
		return NULL;
	}
	decalb16=scanfordecal(w3dnametext,decal);
	decalmaskb16=scanfordecalmask(w3dnametext,decal);
	if (decalb16 && decalmaskb16)
		applymask(bm16,decalb16,decalmaskb16,jrmtexformat);
	popdir();
// get jrm
	jrm=loadnewjrms(basecarname,"dummy.tex");
// apply new texture
	switchtex(jrm,bm16);
	bitmap16free(bm16);
	if (decalb16)
		bitmap16free(decalb16);
	if (decalmaskb16)
		bitmap16free(decalmaskb16);
	return jrm;
}
