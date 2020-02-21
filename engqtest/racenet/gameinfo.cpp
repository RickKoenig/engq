#define D2_3D
#include <m_eng.h>
#include <l_misclibm.h>

#include "gameinfo.h"
#include "stubhelper.h"
#include "../hw35_line2road.h"
#include "../engine7test/n_usefulcpp.h"
#include "online_uplay.h"

//static C8 gametypechar[]={'T','S','C','\0'};
#define WORSTTIME 1000000000

gameinf* globalgame;	// game info gets loaded here.

static bool iswhitespace(C8 c)
{
	return c==' ' || c=='\t' || c=='\n' || c=='\r';
}

static const C8* skipws(const C8* s)
{
	while(iswhitespace(*s))
		++s;
	return s;
}

static const C8* gettildastring(const C8 *s,string& ts)
{
	ts.clear();
	if (*s=='~') {
		++s;
		while(1) {
			if (*s=='~') {
				++s;
				break;
			}
			if (*s=='\0')
				break;
			ts+=*s++;
		}
	}
	return s;
}

static const C8* gettok(const C8* s,string& str)
{
	str.clear();
	while (*s==',') // skip over any commas
		++s;
	while(1) {
		if (iswhitespace(*s) || *s=='\0')
			break;
		if (*s=='~' || *s==',') {
			++s;
			break;
		}
		str+=*s++;
	}
	return s;
}

static const C8* getint(const C8* s,U32& theint)
{
	string temp;
	s=gettok(s,temp);
	theint=atoi(temp.c_str());
	return s;
}

static const C8* getfloat(const C8* s,float& thefloat)
{
	string temp;
	s=gettok(s,temp);
	thefloat=(float)atof(temp.c_str());
	return s;
}

static const C8* getip(const C8* s,U32& theip)
{
	string temp;
	s=gettok(s,temp);
	theip=str2ip(temp.c_str());
	return s;
}

static const C8* getgamekind(const C8* s,C8& ch)
{
	if (*s==',')
		++s;
	if (*s>='0' && *s<='9') { // no gametype, set to 'F' for no gametype
		ch='F';
		return s;
	}
	ch=*s;
	++s;
	return s;
}

gameinf::gameinf(const C8* parsestr)
{
// first init structures to something
	U32 i;
	isvalid=false;
	ni.ip=str2ip("192.168.1.100");
	ni.port=5412;
	ni.nt=NET_BOT;
	ni.gt='?';
	ni.gameid=1;
	ni.nhumans=4;
	ni.nbots=0;
	ni.nplayers=ni.nhumans+ni.nbots;

	ti.trackname="prehistoric";
	ti.rules=7;

	pi.pid=0;
	pi.username="hot shot";
	pi.besttime=WORSTTIME;
	memset(pi.medalsearned,0,sizeof(pi.medalsearned));
	pi.carmake="synkro";
	pi.rims=1;
	//weapons already inited
	for (i=0;i<MAXENERGIES;++i)
		pi.energies[i]=0;
	pi.exp=0;
	pi.avgpos=100;
	hsv ahsv;
	ahsv.h=0;
	ahsv.s=128;
	ahsv.v=255;
	for (i=0;i<NUMHSVS;++i)
		pi.hsvs[i]=ahsv;
	//ci.username;
	ci.uid=0;
	ci.besttime=WORSTTIME;
// pre parse the string for validity
	const C8* ps=parsestr;
	if (!ps) // no string
		return;
	if (*ps!='p') // no 'p' at beginning
		return;
	++ps;
	ps=skipws(ps);
	if (*ps<'0' || *ps>'9') // no number after 'p'
		return;
	if (parsestr[strlen(parsestr)-1]!='g') // last char is a 'g'
		return;
	isvalid=true;
// now parse the string
	logger("......... parse gameinfo\n");
	ps=parsestr;
	bool done=false;
	string ts;
	U32 tint;
	float tfloat;
	C8 ch;
	while(ps && *ps && !done) {
		ps=skipws(ps);
		logger("parsing '%c'\n",*ps);
		C8 v;
		v=*ps++;
		ps=skipws(ps);
		switch(v) {
		case 'p':
			if (*ps<'0' || *ps>'9')
				break; // no pid
			ps=getint(ps,tint); // player id
			pi.pid=tint;
			logger("\tplayer id %d\n",tint);
			break;
		case 'u':
			ps=gettildastring(ps,ts); // username
			pi.username=ts;
			logger("\tusername '%s'\n",ts.c_str());
			if (iswhitespace(*ps))
				break;
			ps=getint(ps,tint); // besttime
			pi.besttime=tint;
			logger("\tbesttime %d\n",tint);
			break;
		case 'a':
			if (*ps<'0' || *ps>'9')
				break; // no medalsearned
			for (i=0;i<MAXMEDALS;++i) {
				ps=getint(ps,tint); // medal
				pi.medalsearned[i]=tint;
				logger("\tmedalearned %d:%d\n",i,tint);
				if (iswhitespace(*ps))
					break;
			}
			break;
		case 'r':
			ps=gettildastring(ps,ts); // trackname
			ti.trackname=ts;
			logger("\ttrackname '%s'\n",ts.c_str());
			if (iswhitespace(*ps))
				break;
			ps=getint(ps,tint); // rules
			ti.rules=tint;
			logger("\trules %d\n",tint);
			if (iswhitespace(*ps))
				break;
			ps=getint(ps,tint); // net type
			ni.nt=(nettype)tint;
			logger("\tnettype %d\n",tint);
			if (iswhitespace(*ps))
				break;
			ps=getint(ps,tint); // humans
			ni.nhumans=tint;
			logger("\thumans %d\n",tint);
			if (iswhitespace(*ps))
				break;
			ps=getint(ps,tint); // bots
			ni.nbots=tint;
			ni.nplayers=ni.nhumans+ni.nbots;
			logger("\tbots %d\n",tint);
			if (iswhitespace(*ps))
				break;
			ps=getgamekind(ps,ch); // tourn,skillmatch etc.
			ni.gt=ch;
			logger("\tgamekind '%c'\n",ch);
			if (iswhitespace(*ps))
				break;
			ps=getint(ps,tint); // game id
			ni.gameid=tint;
			logger("\tgameid %d\n",tint);
			break;
		case 'v':
			ps=gettildastring(ps,ts); // carmake
			pi.carmake=ts;
			logger("\tcarmake '%s'\n",ts.c_str());
			if (iswhitespace(*ps))
				break; // no hsvs
			for (i=0;i<NUMHSVS;++i) {
				ps=getint(ps,tint); // hue
				pi.hsvs[i].h=tint;
				logger("\thue %d\n",tint);
				if (iswhitespace(*ps))
					break;
				ps=getint(ps,tint); // sat
				pi.hsvs[i].s=tint;
				logger("\tsat %d\n",tint);
				if (iswhitespace(*ps))
					break;
				ps=getint(ps,tint); // val
				pi.hsvs[i].v=tint;
				logger("\tval %d\n",tint);
				if (iswhitespace(*ps))
					break;
			}
			if (iswhitespace(*ps))
				break;
			ps=getint(ps,tint); // val
			pi.rims=tint;
			logger("\trims %d\n",tint);
			break;
/*		case 't':
			if (*ps<'0' || *ps>'9')
				break; // no rims
			ps=getint(ps,tint); // rims 1-12
			pi.rims=tint;
			logger("\trims %d\n",tint);
			break; */
		case 'w':
			if (*ps<'0' || *ps>'9')
				break; // no weapons
			for (i=0;i<MAXWEAPONSCARRY;++i) {
				ps=getint(ps,tint); // weapon
				if (pi.weapons.size()<MAXWEAPONSCARRY && tint<60 && tint>=0) {
					pi.weapons.push_back(tint);
					logger("\tweapons %d\n",tint);
				}
				if (iswhitespace(*ps))
					break;
			}
			break;
		case 'e':
			if (*ps<'0' || *ps>'9')
				break; // no energies
			for (i=0;i<MAXENERGIES;++i) {
				ps=getint(ps,tint); // energies
				pi.energies[i]=tint;
				logger("\tenergy %d\n",tint);
				if (iswhitespace(*ps))
					break;
			}
			break;
		case 'x':
			if (*ps<'0' || *ps>'9')
				break; // no exp
			ps=getint(ps,tint); // exp
			pi.exp=tint;
			logger("\texperience %d\n",tint);
			if (iswhitespace(*ps))
				break;
			ps=getfloat(ps,tfloat); // exp
			pi.avgpos=tfloat;
			logger("\taverage position %f\n",tfloat);
			break;
		case 'i':
			if (*ps<'0' || *ps>'9')
				break; // no ip
			ps=getip(ps,tint); // ip
			ni.ip=tint;
			logger("\tip '%s'\n",ip2str(tint));
			if (iswhitespace(*ps))
				break;
			ps=getint(ps,tint); // port
			ni.port=tint;
			logger("\tport %d\n",tint);
			break;
		case 'c':
			ps=gettildastring(ps,ts); // challenger name
			ci.username=ts;
			logger("\tchallenger name '%s'\n",ts.c_str());
			if (iswhitespace(*ps))
				break;
			ps=getint(ps,tint); // userid
			ci.uid=tint;
			logger("\tchallenger id %d\n",tint);
			if (iswhitespace(*ps))
				break;
			ps=getint(ps,tint); // besttime
			ci.besttime=tint;
			logger("\tchallenger besttime %d\n",tint);
			break;
		case 'g':
			logger("\tgo!\n");
			done=true;
			break;
		}
	}
	logger("......... done parse gameinfo\n");
}

/*
void gameinf::save(const C8* fname)
{
	pushandsetdir("racenetdata/onlineopt");
	FILE* fw=fopen2(fname,"wb");
	fprintf(fw,"p %d",pi.pid);
	fprintf(fw," u ~%s~",pi.username.c_str());
	fprintf(fw," r ~%s~,%d,%d,%d,%d,%c%d",
		ti.trackname.c_str(),ti.rules,ni.nt,ni.nhumans,ni.nbots,gametypechar[ni.gt],ni.gameid); 
	fprintf(fw," v ~%s~,",pi.carmake.c_str());
	U32 i;
	for (i=0;i<NUMHSVS;++i) {
		fprintf(fw,"%d,%d,%d",pi.hsvs[i].h,pi.hsvs[i].s,pi.hsvs[i].v);
		if (i!=NUMHSVS-1)
			fprintf(fw,",");
	}
	fprintf(fw," w ");
	for (i=0;i<pi.weapons.size();++i) {
		fprintf(fw,"%d",pi.weapons[i]);
		if (i!=pi.weapons.size()-1)
			fprintf(fw,",");
	}
	fprintf(fw," e ");
	for (i=0;i<MAXENERGIES;++i) {
		fprintf(fw,"%d",pi.energies[i]);
		if (i!=MAXENERGIES-1)
			fprintf(fw,",");
	}
	fprintf(fw," x %d,%f",pi.exp,pi.avgpos);
	fprintf(fw," i %s,%d",ip2str(ni.ip),ni.port);
	fprintf(fw," g");
	fclose(fw);
	popdir();
}
*/
