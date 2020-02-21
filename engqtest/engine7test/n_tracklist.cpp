#include <string.h>

/*#include <engine7cpp.h>

#include "usefulcpp.h"
#include "constructorcpp.h"
 */

#define D2_3D
#define INCLUDE_WINDOWS
#include <m_eng.h>
#include <l_misclibm.h>
#include "n_deflatecpp.h"
#include "n_usefulcpp.h"
#include "n_constructorcpp.h"
#include "n_facevertscpp.h"
#include "n_polylistcpp.h"
#include "n_meshtestcpp.h"
#include "n_carenalobbycpp.h"
#include "n_trackhashcpp.h"
#include "n_tracklistcpp.h"
#include "n_newconstructorcpp.h"

#define MAXTRACKS 10000

//static char **track_sc;
//static int ntrack_sc;
static script* track_sc;
static unsigned char (*track_hashs)[NUMHASHBYTES];
//#define NAMESIZEI 32

void inittracklist()
{
	struct trk tk;
	struct newtrack *ntk;
	int i,j=0;
//	char **sc;
//	int nsc;
//	track_hashs=(unsigned char (*)[NUMHASHBYTES])memzalloc(MAXTRACKS*NUMHASHBYTES*sizeof(unsigned char));
	track_hashs=new U8[MAXTRACKS] [NUMHASHBYTES];
	pushandsetdirdown("constructed");
//	sc=doadir(&nsc,0);
	scriptdir sc(0);
//	ntrack_sc=0;
//	track_sc=0;
	track_sc=new script;
	for (i=0;i<sc.num();i++)
//		if (fileext(sc[i],".trk")) {
		if (isfileext(sc.idx(i).c_str(),"trk")) {
			if (j==MAXTRACKS)
				continue;
			char str[50];
//			if (sc.idx(i).size()>NAMESIZEI)
//				errorexit("track name too long, %s",sc.idx(i).c_str());
			mgetname(sc.idx(i).c_str(),str);
//			track_sc=addscript(track_sc,strclone(str),&ntrack_sc);
			track_sc->addscript(str);
			if (loadtrackscript(str,&tk))
				gettrackhash((unsigned char *)&tk,sizeof(tk),track_hashs[j]);
			else if (ntk=loadnewtrackscript(str)) {
				unsigned char *data;
				U32 datalen;
				data=writenewtrack(ntk,&datalen);
				gettrackhash(data,datalen,track_hashs[j]);
//				memfree(data);
				delete[] data;
				freenewtrackscript(ntk);
			} else
				errorexit("unknown track type '%s'",str);
			j++;
		}
//	freescript(sc,nsc);
	popdir();
	for (i=0;i<track_sc->num();i++)
		logger("track %d: is '%s'\n",i,track_sc->idx(i).c_str());
}

int tracklistnum()
{
//	return ntrack_sc;
	if (!track_sc)
		return 0;
	return track_sc->num();
}

const char *uniquetrackname(const char *trkname)
{
	int i;//,len;
//	static char utn[NAMESIZE];
//	mystrncpy(utn,trkname,NAMESIZE);
	static string utn;
	utn=trkname;
	for (i=0;i<track_sc->num();i++)
		if (!my_stricmp(track_sc->idx(i).c_str(),utn.c_str()))
			break;
	if (i==track_sc->num()) {
		utn=trkname;
		return utn.c_str();
	}
//	utn[NAMESIZEI-3]='\0'; // make len 29 or less (NAMESIZE-3)
//	len=strlen(utn);
//	len=utn.size();
//	if (len>2 && utn[len-2]=='_' && utn[len-1]>='a' && utn[len-1]<='z')
//		len-=2;
//	utn[len]='_';
//	utn[len+2]='\0';
//	len++;
//	S32 n=track_sc->num();
/*	for (utn[len]='a' ; utn[len]<='z' ; utn[len]++) {
		for (i=0;i<n;i++)
			if (!my_stricmp(track_sc->idx(i).c_str(),utn.c_str()))
				break;
		if (i==n)
			return utn.c_str();
	} */
	utn+='x';
	return uniquetrackname(utn.c_str());
}

const char *hastrackhash(unsigned char hash[NUMHASHBYTES])
{
	int i,j,n=track_sc->num();
	for (i=0;i<n;i++) {
		for (j=0;j<NUMHASHBYTES;j++)
			if (track_hashs[i][j]!=hash[j])
				break;
		if (j==NUMHASHBYTES)
			return track_sc->idx(i).c_str();
	}
	return 0;
}

void freetracklist()
{
//	freescript(track_sc,ntrack_sc);
	delete track_sc;
//	memfree(track_hashs);
	delete[] track_hashs;
}

