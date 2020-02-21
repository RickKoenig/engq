#include <m_eng.h>
#include <ctype.h> // for tolower
#include "u_states.h"

static S32 counter;

#if 0
static C8* dirs[]={
	"/srcw32/xmen/xmen_data", // data directory first
	"/srcw32/xmen/soft",
	"/srcw32/xmen/hard",
	"/srcw32/xmen/states",
};
#define NDIRS (sizeof(dirs)/sizeof(dirs[0]))

static C8* exts[]={"c","h","cad","lst","txt"};
#define NEXTS (sizeof(exts)/sizeof(exts[0]))

static U32 isdelim(U8 c)
{
	if (c==' ' || c=='\t')
		return 1;
	if (c=='\n' || c=='\r')
		return 1;
	if (c=='\"')
		return 1;
	return 0;
}

static void fcopy(const C8* src,const C8* dest)
{
	FILE* fh=fopen2(src,"rb");
	FILE* fw=fopen2(dest,"wb");
	C8 c;
	while(1) {
		U32 nread=fread(&c,1,1,fh);
		if (!nread)
			break;
		fwrite(&c,1,1,fw);
	}
	fclose(fh);
	fclose(fw);
}

static void replacer(const C8* infile,const C8* what,const C8* rep)
{
	FILE* fh=fopen2(infile,"rb");
	FILE* fw=fopen2("temp","wb");
	S32 whatlen,replen,curpos;
	C8 c;
	whatlen=strlen(what);
	replen=strlen(rep);
	curpos=0;
//	bool usedU8=true;
	while(1) {
		U32 nread=fread(&c,1,1,fh);
		if (!nread)
			break;
		fwrite(&c,1,1,fw);
		c=tolower(c);
		if (c==tolower(what[curpos])) {
			++curpos;
			if (curpos==whatlen) {
				fseek(fh,-(curpos+1),SEEK_CUR);
				fread(&c,1,1,fh);
				fseek(fh,curpos,SEEK_CUR);
				if (isdelim(c)) {
					logger("'%s' found in '%s'\n",what,infile);
					fseek(fw,-curpos,SEEK_CUR);
					fwrite(rep,1,replen,fw);
				}
//				fseek(fh,1,SEEK_CUR);
				curpos=0;
			}
		} else {
//			fseek(fh,-curpos,SEEK_CUR);
			curpos=0;
		}
	}
	fclose(fh);
	fclose(fw);
//	Sleep(100);
	fcopy("temp",infile);
	remove("temp");
}

static void finder(const C8* what,const C8* newwhat)
{
	S32 i,j,k;//,m;
//	C8** sc;
//	S32 nsc;
//	U8** scr=0;
//	U32 nscr=0;
	for (i=0;i<(S32)NDIRS;++i) {
		pushandsetdir(dirs[i]);
		rename(what,newwhat);
//		sc=doadir(0,&nsc);
		script sc=scriptdir(0);
		for (j=0;j<sc.num();++j) {
//		for (j=0;j<nsc;++j) {
			C8 ext[100];
//			if (!my_stricmp("spgscript.c",sc[j]))
//				continue;
			if (!my_stricmp("logfile.txt",sc[j]))
				continue;
			if (!my_stricmp("files.txt",sc[j]))
				continue;
			if (mgetext(sc[j],ext)) {
				for (k=0;k<(S32)NEXTS;++k) {
					if (!my_stricmp(ext,exts[k])) {
						C8 str[100];
						strcpy(str,"back/");
						strcat(str,sc[j]);
						if (!fileexist(str)) {
							logger("'%s' doesn't exist, copying\n",str);
							fcopy(sc[j],str);
						}
//						strcat(str,"b");
//						logger("reading '%s'\n",sc[j]);
/*						scr=loadscript(sc[j],&nscr);
						for (m=0;m<nscr;++m)
							if (!my_stricmp(what,scr[m])) {
								logger("'%s' found in '%s'\n",what,sc[j]);
								break;
							}
						freescript(scr,nscr); */
						replacer(sc[j],what,newwhat);
					}
				}
			}
		}
//		freescript(sc,nsc);
		popdir();
	}
}

static void sectioner(C8* sectname,C8* prefix)
{
	S32 j,k,m;
//	C8** sc;
//	S32 nsc;
//	C8** sc2;
//	S32 nsc2;
	S32 plen=strlen(prefix);
	pushandsetdir(dirs[0]);
//	sc=doadir(0,&nsc);
	script sc=scriptdir(0);
	for (j=0;j<sc.num();++j) {
//	for (j=0;j<nsc;++j) {
		C8 ext[100];
		if (mgetext(sc[j],ext)) {
			if (!my_stricmp(ext,"cad")) {
				logger("cad found '%s'\n",sc[j]);
				video_lock();
				clipclear32(B32,C32GREEN);
				outtextxybf32(B32,10,10,C32BLACK,C32WHITE,"%s",sc[j]);
				video_unlock();
				winproc();
//				sc2=loadscript(sc[j],&nsc2);
				script sc2(sc[j]);
				for (k=0;k<sc2.num();++k) {
//				for (k=0;k<nsc2;++k) {
					if (!my_stricmp("section",sc2[k])) {
						if (!my_stricmp(sectname,sc2[k+1])) {
							for (m=0;m<plen;++m)
								if (tolower(prefix[m])!=tolower(sc2[k+2][m]))
									break;
							if (m!=plen || plen==0) {
								C8 str[100];
								strcpy(str,prefix);
								strcat(str,sc2[k+2]);
								finder(sc2[k+2],str);
								logger("sectname '%s' found '%s' in '%s'\n",sectname,sc2[k+2],sc[j]);
								logger("finder(%s,%s)\n",sc2[k+2],str);
							} else {
								logger("sectname '%s' found '%s' in '%s', omitting, has same prefix\n",sectname,sc2[k+2],sc[j]);
							}
						}
					}
				}
//				freescript(sc2,nsc2);
			}
		}
	}
//	freescript(sc,nsc);
	popdir();
//	finder("w1m.spx","abc_w1m.spx");
}
static void sectionerf(C8* sectname,C8* prefix)
{
	S32 j,k,m;
//	C8** sc;
//	S32 nsc;
//	C8** sc2;
//	S32 nsc2;
	S32 plen=strlen(prefix);
	pushandsetdir(dirs[0]);
//	sc=doadir(0,&nsc);
	script sc=scriptdir(0);
	for (j=0;j<sc.num();++j) {
//	for (j=0;j<nsc;++j) {
		C8 ext[100];
		if (mgetext(sc[j],ext)) {
			if (!my_stricmp(ext,"cad")) {
				C8 str[50];
				strcpy(str,sc[j]);
				if (strlen(str)<12)
					continue;
				str[11]='\0';
				if (my_stricmp(str,"dangerroomb"))
					continue;
				logger("fight cad found '%s'\n",sc[j]);
				video_lock();
				clipclear32(B32,C32GREEN);
				outtextxybf32(B32,10,10,C32BLACK,C32WHITE,"%s",sc[j]);
				video_unlock();
				winproc();
//				sc2=loadscript(sc[j],&nsc2);
				script sc2(sc[j]);
				for (k=0;k<sc2.num();++k) {
					if (!my_stricmp("section",sc2[k])) {
						if (!my_stricmp(sectname,sc2[k+1])) {
							for (m=0;m<plen;++m)
								if (tolower(prefix[m])!=tolower(sc2[k+2][m]))
									break;
							if (m!=plen || plen==0) {
								C8 str[100];
								strcpy(str,prefix);
								strcat(str,sc2[k+2]);
								finder(sc2[k+2],str);
								logger("sectname '%s' found '%s' in '%s'\n",sectname,sc2[k+2],sc[j]);
								logger("finder(%s,%s)\n",sc2[k+2],str);
							} else {
								logger("sectname '%s' found '%s' in '%s', omitting, has same prefix\n",sectname,sc2[k+2],sc[j]);
							}
						}
					}
				}
//				freescript(sc2,nsc2);
			}
		}
	}
//	freescript(sc,nsc);
	popdir();
//	finder("w1m.spx","abc_w1m.spx");
}
/*section sounds2
entry 1 abm_TOAD_ENTRY01.WAV
wince 3 TOAD_WINCE01.WAV TOAD_WINCE02.WAV TOAD_WINCE03.WAV
ca_triplestrike 1 abm_mod_TRIPLE_STRIKE.WAV
*/
static void sectioners2(C8* sectname,C8* prefix)
{
	S32 j,k,m,n;
//	C8** sc;
//	S32 nsc;
//	C8** sc2;
//	S32 nsc2;
	S32 plen=strlen(prefix);
	pushandsetdir(dirs[0]);
//	sc=doadir(0,&nsc);
	script sc=scriptdir(0);
	for (j=0;j<sc.num();++j) {
//	for (j=0;j<nsc;++j) {
		if (strlen(sc[j])>25)
			logger("long file name '%s'\n",sc[j]);
	}
	for (j=0;j<sc.num();++j) {
//	for (j=0;j<nsc;++j) {
		C8 ext[100];
		if (mgetext(sc[j],ext)) {
			if (!my_stricmp(ext,"cad")) {
				logger("cad found '%s'\n",sc[j]);
				video_lock();
				clipclear32(B32,C32GREEN);
				outtextxybf32(B32,10,10,C32WHITE,C32BLACK,"%s",sc[j]);
				video_unlock();
				winproc();
//				sc2=loadscript(sc[j],&nsc2);
				script sc2(sc[j]);
				for (k=0;k<sc2.num();++k) {
//				for (k=0;k<nsc2;++k) {
					if (!my_stricmp("section",sc2[k])) {
						if (!my_stricmp(sectname,sc2[k+1])) {
// in the section
							n=2;
							while(k+n<sc2.num()) {
//							while(k+n<nsc2) {
								C8 ext[50];
								if (!mgetext(sc2[k+n],ext)) {
									logger("'%s' has no extension\n",sc2[k+n]);
									++n;
									continue;
								}
								if (my_stricmp(ext,"wav")) {
									logger("'%s' isn't a wave\n",sc2[k+n]);
									++n;
									continue;
								}
								for (m=0;m<plen;++m)
									if (tolower(prefix[m])!=tolower(sc2[k+n][m]))
										break;
								if (m!=plen || plen==0) {
									C8 str[100];
									strcpy(str,prefix);
									strcat(str,sc2[k+n]);
									finder(sc2[k+n],str);
									logger("sectname '%s' found '%s' in '%s'\n",sectname,sc2[k+n],sc[j]);
									logger("finder(%s,%s)\n",sc2[k+n],str);
								} else {
									logger("sectname '%s' found '%s' in '%s', omitting, has same prefix\n",sectname,sc2[k+n],sc[j]);
								}
								++n;
							}
// end of in section
						}
					}
				}
//				freescript(sc2,nsc2);
			}
		}
	}
//	freescript(sc,nsc);
	popdir();
//	finder("w1m.spx","abc_w1m.spx");
}

/*
*/
/*
	pushandsetdir("/srcw32/xmen/xmen_data");
	pushandsetdir("/srcw32/xmen/soft");
	pushandsetdir("/srcw32/xmen/hard");
	pushandsetdir("/srcw32/xmen/states");
	sc=doadir(0,&nsc);
	logger("dir of files has %d tokens\n",nsc);
	for (i=0;i<nsc;++i) {
		logger("files '%s'\n",sc[i]);
	}
	freescript(sc,nsc);
	sc=doadir(1,&nsc);
	logger("dir of dirs has %d tokens\n",nsc);
	for (i=0;i<nsc;++i) {
		logger("dirs '%s'\n",sc[i]);
	}
	freescript(sc,nsc);
	popdir();
*/
//}

static void dorenamer()
{
//	S32 changestateval=0;
//	U8** sc;
//	U32 nsc;
//	U8** scr=0;
//	U32 nscr=0;
//	S32 i;
	logger("---------------- file renamer ---------------------------------------\n");
//	pushandsetdir("renamer");
//	sc=loadscript("replacer.txt",&nsc);
//	sc=loadscript("renamer.txt",&nsc);
//	popdir();
/*	logger("renamer.txt script has %d tokens\n",nsc);
	for (i=0;i<nsc;++i) {
		if (!strcmp("reading",sc[i])) {
			U8 str[80];
			U32 len;
			if (changestateval!=10)
				continue;
			strcpy(str,sc[i+1]);
			len=strlen(str);
			if (len>32+2)
				errorexit("string too long <%s>\n",str);
			str[len-1]='\0';
			scr=addscript(scr,&str[1],&nscr);
		} else if (!strcmp("changestate",sc[i])) {
			changestateval=atoi(sc[i+1]);
			logger("changestateval = %d\n",changestateval);
		}
	}
	logger("reading 'files' has %d tokens\n",nscr);
	for (i=0;i<nscr;++i) {
		logger("reading '%s'\n",scr[i]);
	} */
//	freescript(sc,nsc);
//	freescript(scr,nscr);
	sectioner("sobject_fight","abc_");
	sectioner("health_icon","abg_");
	sectioners2("sounds2","abm_");
//	sectioner("sobject_fight","",0);

//	finder("w1m.spx","abc_w1m.spx");
//	finder("mod_TRIPLE_STRIKE.WAV","abm_mod_TRIPLE_STRIKE.WAV");
	finder("fountain.spx","abd_fountain.spx");
	finder("hud.spx","abe_hud.spx");
	finder("rounds.spx","abf_rounds.spx");
	finder("spark.spx","abh_spark.spx");
	finder("smlicons.spx","abi_smlicons.spx");
	finder("font16.spx","abj_font16.spx");
	finder("dangerroomb0.cad","abk_dangerroomb0.cad");
	finder("dangerroomb1.cad","abk_dangerroomb1.cad");
	finder("dangerroomb2.cad","abk_dangerroomb2.cad");
	finder("dangerroomb3.cad","abk_dangerroomb3.cad");
	finder("dangerroomb4.cad","abk_dangerroomb4.cad");
	finder("dangerroomb5.cad","abk_dangerroomb5.cad");
	finder("dangerroomb6.cad","abk_dangerroomb6.cad");
	finder("dangerroomb7.cad","abk_dangerroomb7.cad");
	finder("dangerroomb8.cad","abk_dangerroomb8.cad");
	finder("dangerroomb9.cad","abk_dangerroomb9.cad");
	sectionerf("background","abl_");
	sectionerf("music","abn_");
	logger("---------------- end file renamer -----------------------------------\n");
}
#endif

void renamerinit()
{
	video_setupwindow(640,480);
//	dorenamer();
	counter=0;
}

void renamerproc()
{
	if (KEY==K_ESCAPE)
		poporchangestate(STATE_MAINMENU);
}

void renamerdraw2d()
{
//	video_lock();
	clipclear32(B32,C32BLUE);	
	outtextxyf32(B32,WX/2,16,C32WHITE,"Renamer %d",counter++);
//	video_unlock();
}

void renamerexit()
{
}
