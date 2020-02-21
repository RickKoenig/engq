/*
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <engine1.h>

#include "tagread.h"
#include "sagger.h"
*/
#include <m_eng.h>
#include <l_misclibm.h>

// tagfile stuff
struct shead           // 32 bytes before actual data...
{
	char headid[4];
	int filesize;
	int res1;
	int res2;
	char filename[16];           // this is the dos file name
};

struct tagheader {
	char name[16];
	wavehandle *wh;
};

tag *tagread(char *fname)
{
	tag *tagret;
	int i,j,ts;
	int *table;
	FILE *fp;
	char tempstr[64];

	fp=fopen2(fname,"rb");
//	logger("in tagread '%s'\n",fname);
	if (!fp)
		errorexit("can't open '%s'",fname);
//	tagret=(struct tag *)memalloc(sizeof(struct tag));
	tagret=new tag;

	fread(&tagret->ntag,1,4,fp); // get number of tags
	if (tagret->ntag==0) errorexit("ntag == 0");

	fread(&ts,1,4,fp); //read table size in now
//	table = memalloc(ts);
	if (tagret->ntag*4!=ts)
		errorexit("bad '%s'",fname);
	table = new S32[tagret->ntag];

	fread(table,1,ts,fp);

//	tagret->th=(struct tagheader *)memalloc(sizeof(struct tagheader)*tagret->ntag);
	tagret->th=new tagheader[tagret->ntag];


//	whs=(struct wavehandle **)memalloc(sizeof(struct wavehandle *)*ntag);
	//fread(tagret->th->off,1,tagret->ntag*sizeof(struct tagheader),fp);
/*	for (i=0;i<tagret->ntag;i++) {
		my_strupr(tagret->th[i].name);
		mgetname(tagret->th[i].name,tempstr);
		logger("#define %s %d\n",tempstr,i);
	} */
	FILE* gd=fopen2("gardentag.h","w");
	FILE* gs=fopen2("gardentag.txt","w");
	S32 opn=0;
	for (i=0;i<tagret->ntag;i++) {
		fseek(fp,table[i],SEEK_SET);  //skip over 4 byte checksum + 4 BYTE FILESIZE + 4 BYTE RESERVE + 4BYTE RESERVE
		shead sh;
		fread(&sh,sizeof(sh),1,fp);
//		fread(&tagret->th[i].name,1,sizeof(tagret->th[i].name),fp); //now read filename
		memcpy(tagret->th[i].name,sh.filename,16);
//		logger("filename:%s size %d\n",tagret->th[i].name,sh.filesize);
		fprintf(gs,"%s\n",sh.filename);

		//fseek(fp,tagret->th[i].off+sizeof(struct shead),FILE_START);
		//fseek(fp,tagret->th[i].off+sizeof(struct shead),FILE_START);
		for (j=0;j<i;++j)
			if (!strcmp(tagret->th[i].name,tagret->th[j].name))
				break;
		if (i==j) {
			mgetname(tagret->th[i].name,tempstr);
			my_strupr(tempstr);
//			logger("#define %s %d\n\n",tempstr,i);
		} else {
			sprintf(tempstr,"OPEN%d",opn++);
		}
		fprintf(gd,"#define %s %d\n",tempstr,i);
// write out a .wave file
		U32 fs=ftell(fp);
		U8* rb=new U8[sh.filesize];
		fread(rb,1,sh.filesize,fp);
		FILE* fw=fopen2(sh.filename,"wb");
		fwrite(rb,1,sh.filesize,fw);
		fclose(fw);
		delete[] rb;
		fseek(fp,fs,SEEK_SET);
// done

		tagret->th[i].wh=wave_load((char *)fp,WOPT_FILEPTR);
//		if (tagret->th[i].wh==NULL)
//			errorexit("can't read '%s'",tagret->th[i].name);
	}
//	memfree(table);
	delete[] table;
	fclose(fp);
	fclose(gd);
	fclose(gs);
	return tagret;
}

tag* newtagread(char* tagtext)
{
	int i;
//	char** sc;
//	int nsc;
	struct tag* ret;
//	sc=loadscript(tagtext,&nsc);
	script sc(tagtext);
	S32 nsc=sc.num();
//	ret=(struct tag*)memzalloc("newtagread",sizeof(struct tag));
//	ret->th=(struct tagheader*)memzalloc("newtagread2",sizeof(struct tagheader)*nsc);
	ret=new tag;
	ret->th=new tagheader[nsc];
	ret->ntag=nsc;
	for (i=0;i<nsc;++i) {
//		ret->th[i].wh=wave_load(sc[i],0);
		ret->th[i].wh=wave_load(sc.idx(i).c_str(),0);
	}
//	freescript(sc,nsc);
	return ret;
}

wavehandle *taggetwhbyidx(tag *t,int idx)
{
	if (t==NULL)
		return NULL;
	if (idx<t->ntag)
		return t->th[idx].wh;
	return NULL;
}

wavehandle *taggetwhbyname(tag *t,char *name)
{
	int i;
	if (t==NULL)
		return NULL;
	for (i=0;i<t->ntag;i++)
		if (!my_stricmp(name,t->th[i].name))
			return t->th[i].wh;
	return NULL;
}

int taggetnum(struct tag *t)
{
	if (t==NULL)
		return 0;
	return t->ntag;
}

char *taggetname(tag *t,int idx)
{
	if (t==NULL)
		return NULL;
	if (idx<t->ntag)
		return t->th[idx].name;
	return NULL;
}

void tagfree(tag *t)
{
	int i;
// free sounds
	if (t==NULL)
		return;
	for (i=0;i<t->ntag;i++)
		wave_unload(t->th[i].wh);
// free tag header
//	memfree(t->th);
	delete[] t->th;
// free tag
//	memfree(t);
	delete t;
}

tag *vagread(char **names,int num)
{
//	int sbufflen,freq,sixteenbit,stereo;
//	C8* sbuff;
	struct tag *t;
	int i;//,j;
//	t=(struct tag *)memalloc(sizeof(struct tag));
	t=new tag;
//	t->th=memalloc(sizeof(struct tagheader)*num);
	t->th=new tagheader[num];
	t->ntag=num;
	for (i=0;i<num;i++) {
		mystrncpy(t->th[i].name,names[i],16);
//		sbuff=vag_load(names[i],&sbufflen,&freq,&sixteenbit,&stereo);
//		if (sbuff) {
//			t->th[i].wh=wave_build(sbuff,sbufflen,freq,sixteenbit,stereo);
//			memfree(sbuff);
//		} else
//			t->th[i].wh=NULL;
//		logger("vag '%s' size is %d\n",names[i],filelen(names[i]));
		t->th[i].wh=vag_load(names[i]);
	}
	return t;
}
