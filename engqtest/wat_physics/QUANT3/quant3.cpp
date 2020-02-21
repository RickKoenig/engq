// borrowed form 'simple poster' 'u_s_post.cpp'
#include <m_eng.h>
#include <l_misclibm.h>
#include "../../u_states.h"
#include "../../u_fplot.h"

#include "m_perf.h"

namespace {

// ui
shape* rl;
shape* focus,*oldfocus;

pbut* bquit;

listbox* lenergies;
// add energy
text* tenergy;
hscroll* henergy;
text* tamp;
hscroll* hamp;
text* tph;
hscroll* hph;
pbut* badd;
// add bell curve of energies
text* tmean;
hscroll* hmean;
text* tmamp;
hscroll* hmamp;
text* twidth;
hscroll* hwidth;
text* tpc;
hscroll* hpc;
pbut* baddbell;
// reset energies
pbut* breset;
// calc
pbut* bcalc;
// time and space sliders
hscroll* hcntvel;
text* tcntinfo;
hscroll* hcntval;
pbut* bprev,*bnext;
text* tcomp;
text *tinfo3d;
pbut *bstop;
// end ui;

// log stuff
#define LTRIGSIZE 15 // trig table
//#define LTIMESIZE 15 // hardcore! lots of memory used around 2GB
#define LTIMESIZE 14 // less severe
#define LSPACESIZE 13
#define LANIMSIZE 17 // resolution of animation

// powers of 2
#define TRIGSIZE (1<<LTRIGSIZE)
#define TIMESIZE (1<<LTIMESIZE)
#define SPACESIZE (1<<LSPACESIZE)
#define ANIMSIZE (1<<LANIMSIZE)

// non powers of 2
#define MAXANIMSPEED (ANIMSIZE/200)
#define MAXENERGY 512 // 0 to 31 // actually not energy but different eigenstates
#define ENERGYARRSIZE (MAXENERGY+1)
#define MAXAMP 100
enum {T_RI_X,T_P_X,T_RI,T_RIX,
	X_RI_T,X_P_T,X_RI,X_RIT,
	NUMGRAPHS}; // graph types
S32 comprate=10; //#define COMPRATE 10 // how many time frames of computation to performe every proc frame
#define VIEWXSTART 35
#define VIEWYSTART 14
#define VIEWXSIZE 740
#define VIEWYSIZE 740

// arrays
#define USEVECTOR
#ifdef USEVECTOR
vector<float> sintb; // float sintb[TRIGSIZE]; // trig table
vector<float> ak; // float ak[ENERGYARRSIZE]; // 0 to 31 // 0 not used
vector<S32> phk; // S32 phk[ENERGYARRSIZE]; // quantum states table (energy squareds/momentum, will change for non zero potential function)
//vector <vector<float> > probs; // float probs[TIMESIZE][SPACESIZE]; // [t][x];
vector <vector<float> > reals; // float reals[TIMESIZE][SPACESIZE]; // [t][x];
vector <vector<float> > imags; // float imags[TIMESIZE][SPACESIZE]; // [t][x];
vector <vector<float> > angstreal; // float angstreal[TIMESIZE][ENERGYARRSIZE]; // [t][n]
vector <vector<float> > angstimag; // float angstimag[TIMESIZE][ENERGYARRSIZE]; // [t][n]
vector <vector<float> > angsx; // float angsx[SPACESIZE][ENERGYARRSIZE]; // [x][n]
#else
float sintb[TRIGSIZE]; // trig table
float ak[ENERGYARRSIZE]; // 0 to 31 // 0 not used
S32 phk[ENERGYARRSIZE]; // quantum states table (energy squareds/momentum, will change for non zero potential function)
//float probs[TIMESIZE][SPACESIZE]; // [t][x];
float reals[TIMESIZE][SPACESIZE]; // [t][x];
float imags[TIMESIZE][SPACESIZE]; // [t][x];
float angstreal[TIMESIZE][ENERGYARRSIZE]; // [t][n]
float angstimag[TIMESIZE][ENERGYARRSIZE]; // [t][n]
float angsx[SPACESIZE][ENERGYARRSIZE]; // [x][n]
#endif
// end arrays
float sumk; // sum of the energies
float isumk; // graphical normalizer
float normk; // sqrt(2/(sum of squared energies)) // real normalizer
float sint(S32 t)
{
	return sintb[(TRIGSIZE-1)&t];
}

float cost(S32 t)
{
	return sintb[(TRIGSIZE-1)&(t+TRIGSIZE/4)];
}


S32 kind=T_RIX; // T_P_X; // animate time p against x // type of graph
S32 countr; // animation
float pitch,yaw;
bool docomp;  // update computation
S32 comptime=TIMESIZE; // nothing to compute

S32 lshift(S32 v,S32 s)
{
	if (s>=0)
		return v<<s;
	else
		return v>>-s;
}

void compute()
{
	comptime=0;
}

void computeproc()
{
	S32 x,t,n,rt,rx;
	static S32 exlate[ENERGYARRSIZE];
	static S32 nexlate; // build a list of non zero energies
	if (comptime==0) {
		sumk=0;
		float sumk2=0;
		for (n=1;n<ENERGYARRSIZE;n++) {
			sumk+=ak[n];
			sumk2+=ak[n]*ak[n];
		}
		if (sumk==0) {
			comptime=TIMESIZE;
			return;
		}
// not so important
		isumk=1.0f/sumk; // this scales/looks better
		normk=sqrtf(2.0f/sumk2); // this one is the correct normalizer
		nexlate=0;
		for (n=1;n<ENERGYARRSIZE;n++) {
			if (ak[n]) {
				exlate[nexlate++]=n;
			}
		}
		perf_start(TEST1);
		for (x=0;x<SPACESIZE;x++)
			for (n=1;n<ENERGYARRSIZE;n++) {
				rx=lshift(n*x,LTRIGSIZE-LSPACESIZE-1);
				angsx[x][n]=sint(rx)*ak[n];
			}
		for (t=0;t<TIMESIZE;t++) {
			for (n=1;n<ENERGYARRSIZE;n++) {
				rt=lshift(n*n*t+phk[n],LTRIGSIZE-LTIMESIZE);
				angstreal[t][n]=cost(rt);
				angstimag[t][n]=sint(rt);
			}
		}
		perf_end(TEST1);
	}
	if (comptime==TIMESIZE)
		return;
	perf_start(TEST2);
	if (wininfo.fpsavg2>wininfo.fpswanted+2.0f)
		++comprate;
	else if (wininfo.fpscurrent2<wininfo.fpswanted-2.0f) {
		if (comprate<=0)
			comprate=1;
		else if (comprate<10)
			--comprate;
		else
			comprate=comprate*9/10;
	}
	if (comprate<=0)
		comprate=1;
	S32 endtime=comptime+comprate;
	if (endtime>TIMESIZE)
		endtime=TIMESIZE;
	for (t=comptime;t<endtime;++t) {
#ifdef USEVECTOR
		vector<float> & realst=reals[t]; // try and speed this up
		vector<float> & imagst=imags[t];
//		vector<float> & probst=probs[t];
		vector<float> & angstrealt=angstreal[t];
		vector<float> & angstimagt=angstimag[t];
#else
		float* realst=reals[t]; // try and speed this up
		float* imagst=imags[t];
//		float* probst=probs[t];
		float* angstrealt=angstreal[t];
		float* angstimagt=angstimag[t];
#endif
		for (x=0;x<SPACESIZE;++x) {
			float ampr=0,ampi=0;
#ifdef USEVECTOR
			vector<float> & angsxx=angsx[x];
#else
			float* angsxx=angsx[x];
#endif
			perf_start(TEST3);
			S32 np;
			for (np=0;np<nexlate;++np) {
				n=exlate[np];
				float xp=angsxx[n];
				ampr+=xp*angstrealt[n];
				ampi+=xp*angstimagt[n];
			}
			perf_end(TEST3);
			perf_start(TEST4);
			ampr*=isumk; // go with graphical normals, better scaling
			ampi*=isumk;
			realst[x]=ampr;
			imagst[x]=ampi;
//			probst[x]=ampr*ampr+ampi*ampi;
			perf_end(TEST4);
		}
	}
	comptime=endtime;
	perf_end(TEST2);
}
void update_energy_list()
{
	S32 i;
	S32 oldidx=lenergies->getidx();
	lenergies->clear();
	for (i=1;i<ENERGYARRSIZE;++i) { // 1 to 31, 31 in all
		if (!ak[i])
			phk[i]=0;
//		if (ak[i]) {
			lenergies->printf("E %3d %5d:A %6.2f,P %4d",i,i*i,ak[i],phk[i]);
//		}
	}
	lenergies->setidxc(oldidx);
}

void update_text()
{
		C8 str[50];
		sprintf(str,"Energy %d",henergy->getidx());
		tenergy->settname(str);
		sprintf(str,"Amp %d",hamp->getidx());
		tamp->settname(str);
		S32 p=hph->getidx();
		sprintf(str,"Phase %d",p);
		tph->settname(str);
		sprintf(str,"Mean Energy %d",hmean->getidx());
		tmean->settname(str);
		sprintf(str,"Mean Amp %d",hmamp->getidx());
		tmamp->settname(str);
		sprintf(str,"Width %d",hwidth->getidx());
		twidth->settname(str);
		sprintf(str,"PhaseDelta %d",hpc->getidx());
		tpc->settname(str);
}

} // end anonymous namespace


void quant3_init()
{
	video_setupwindow(1024,768);
	pushandsetdir("wat_physics/quant3");
// ui
	rl=res_loadfile("quant3res.txt");
	bquit=rl->find<pbut>("BQUIT");

	lenergies=rl->find<listbox>("LENERGIES");

	breset=rl->find<pbut>("BRESET");

	tenergy=rl->find<text>("TENERGY");
	henergy=rl->find<hscroll>("HENERGY");
	henergy->setminmaxval(1,MAXENERGY);
	tamp=rl->find<text>("TAMP");
	hamp=rl->find<hscroll>("HAMP");
	hamp->setminmaxval(0,MAXAMP);
	tph=rl->find<text>("TPH");
	hph=rl->find<hscroll>("HPH");
	hph->setminmaxval(-TIMESIZE/2,TIMESIZE/2);
	hph->setidx(0);
	badd=rl->find<pbut>("BADD");

	tmean=rl->find<text>("TMEAN");
	hmean=rl->find<hscroll>("HMEAN");
	hmean->setminmaxval(0,MAXENERGY*2);
	tmamp=rl->find<text>("TMAMP");
	hmamp=rl->find<hscroll>("HMAMP");
	hmamp->setminmaxval(0,MAXAMP);
	twidth=rl->find<text>("TWIDTH");
	hwidth=rl->find<hscroll>("HWIDTH");
	hwidth->setminmaxval(0,MAXENERGY);
	tpc=rl->find<text>("TPC");
	hpc=rl->find<hscroll>("HPC");
	hpc->setminmaxval(-TIMESIZE/2,TIMESIZE/2);
	hpc->setidx(0);
	baddbell=rl->find<pbut>("BADDBELL");
	bcalc=rl->find<pbut>("BCALC");
	hcntvel=rl->find<hscroll>("HCNTVEL");
	hcntvel->setminmaxval(-MAXANIMSPEED,MAXANIMSPEED);
	hcntvel->setidx(0);
	tcntinfo=rl->find<text>("TCNTINFO");
	hcntval=rl->find<hscroll>("HCNTVAL");
	bprev=rl->find<pbut>("BPREV");
	bnext=rl->find<pbut>("BNEXT");
	tcomp=rl->find<text>("TCOMP");
	tinfo3d=rl->find<text>("TINFO3D");
	bstop=rl->find<pbut>("BSTOP");
	focus=oldfocus=0;
// end ui
// init arrays
// init trig tables
	S32 i;
	printf("init trig\n");
#ifdef USEVECTOR
	sintb.assign(TRIGSIZE,0);
#endif
	for (i=0;i<TRIGSIZE;i++) {
		sintb[i]=sinf(i*TWOPI/TRIGSIZE);
	}
// init energies
#ifdef USEVECTOR
	ak.assign(ENERGYARRSIZE,0);
#endif
	ak[1]=50;
	ak[2]=50;
#ifdef USEVECTOR
	phk.assign(ENERGYARRSIZE,0);
#endif
// init tables
#ifdef USEVECTOR
	vector<float> ts(SPACESIZE,0.0f);
//	probs.assign(TIMESIZE,ts);
	reals.assign(TIMESIZE,ts);
	imags.assign(TIMESIZE,ts);
	vector<float> es(ENERGYARRSIZE,0.0f);
	angstreal.assign(TIMESIZE,es);
	angstimag.assign(TIMESIZE,es);
	angsx.assign(SPACESIZE,es);
#endif
// end init arrays
	lenergies->setidxc(0);
	update_energy_list();
	docomp=true;
	compute();
	docomp=false;
	update_text();
}

void quant3_proc()
{
// bail
	if (KEY==K_ESCAPE)
		poporchangestate(STATE_MAINMENU);
// ui
	if (wininfo.mleftclicks)
		focus=rl->getfocus();
	S32 ret=-1;
	if (focus) {
		ret=focus->proc();
	}
	if (oldfocus && oldfocus!=focus)
		oldfocus->deactivate();
	oldfocus=focus;
	bool slidanimval=false;
// quit
	if (focus == bquit) {
		if (ret==1)
		poporchangestate(STATE_MAINMENU);
// calc
	} else if (focus == bcalc) {
		if (ret==1) {
			if (docomp) {
				compute();
				docomp=false;
			}
		}
// reset energies
	} else if (focus == breset) {
		if (ret==1) {
#ifdef USEVECTOR
			ak.assign(ak.size(),0);
#else
			memset(ak,0,sizeof(ak));
#endif
			update_energy_list();
			docomp=true;
			focus=lenergies;
		}
// click in energies list
	} else if (focus == lenergies) { // 0 to 30
		if (ret>=0 && ret<MAXENERGY) {
			++ret;
			henergy->setidx(ret); // 1 to 31
			hmean->setidx(ret);
			hamp->setidx((S32)(ak[ret]));
			S32 p;
			p=phk[ret];
			p&=TIMESIZE-1;
			if (p>=TIMESIZE/2)
				p-=TIMESIZE;
			hph->setidx(p);
		}
		update_text();
// add energy
	} else if (focus == henergy) {
		update_text();
	} else if (focus == hamp) {
		update_text();
	} else if (focus == hph) {
		update_text();
	} else if (focus == badd) {
		if (ret==1) {
			S32 n=henergy->getidx();
			S32 a=hamp->getidx();
			S32 p=hph->getidx();
			ak[n]=(float)a;
			phk[n]=p;
			update_energy_list();
			docomp=true;
			focus=lenergies;
			lenergies->setidxc(n-1);
		}
// add bell energies
	} else if (focus == hmean) {
		update_text();
	} else if (focus == hmamp) {
		update_text();
	} else if (focus == hwidth) {
		update_text();
	} else if (focus == hpc) {
		update_text();
	} else if (focus == baddbell) {
		if (ret==1) {
			float mean=(float)(hmean->getidx());
			float wid=float(hwidth->getidx());
			float mamp=float(hmamp->getidx());
			int phase=hpc->getidx();
			S32 x;
			for (x=1;x<ENERGYARRSIZE;x++) {
				float a;
				if (wid==0)	{
					if (x==mean)
						a=mamp;
					else
						a=0;
				} else {
					float temp=(x-mean)/wid;
					a=mamp*expf(-temp*temp);
					if (a>=.125f)
						phk[x]=(x-S32(mean))*phase;
					else
						a=0;
				}
				ak[x]+=a;
			}
			update_energy_list();
			docomp=true;
			focus=lenergies;
			if (mean>0)
				lenergies->setidxc(S32(mean)-1);
		}
	} else if (focus==hcntval && MBUT&1) {
		countr=hcntval->getidx();
//		hcntvel->setidx(0);
		slidanimval=true; // prevent velocity update when sliding X/T
	} else if (focus==bprev) {
		if (ret==1) {
			if (kind==0)
				kind=NUMGRAPHS-1;
			else
				--kind;
		}
	} else if (focus==bnext) {
		if (ret==1) {
			if (kind==NUMGRAPHS-1)
				kind=0;
			else
				++kind;
		}
	} else if (focus==bstop) {
		if (ret==1) {
			hcntvel->setidx(0);
		}
	}
// end ui
// animation update frame countr
	if (!slidanimval)
		countr+=(hcntvel->getidx());
	countr&= (ANIMSIZE-1); 
	C8 str[250];
	if (kind<NUMGRAPHS/2) // animate time
		sprintf(str,"K%d : T%5d : VT%2d",kind,lshift(countr,LTIMESIZE-LANIMSIZE),hcntvel->getidx());
	else // animate space
		sprintf(str,"K%d : X%5d : VX%2d",kind,lshift(countr,LSPACESIZE-LANIMSIZE),hcntvel->getidx());
	tcntinfo->settname(str);
	hcntval->setminmaxval(0,ANIMSIZE-1);
	hcntval->setidx(countr);
	computeproc();
	sprintf(str,"rt %3d %6d/%6d %3d%%",comprate,comptime,TIMESIZE,comptime*100/TIMESIZE);
	tcomp->settname(str);
	if (kind==T_RIX || kind==X_RIT) {
		if (MBUT && MX>=VIEWXSTART && MY>=VIEWYSTART && MX<VIEWXSTART+VIEWXSIZE && MY<VIEWYSTART+VIEWYSIZE) {
			pitch=pitch+float(wininfo.dmy);
			yaw=yaw+float(wininfo.dmx);
			if (pitch>=360)
				pitch-=360;
			else if (pitch<0)
				pitch+=360;
			if (yaw>=360)
				yaw-=360;
			else if (yaw<0)
				yaw+=360;

		}
		sprintf(str,"Drag mouse in graph, pitch %7.2f, yaw %7.2f",pitch,yaw);
		tinfo3d->settname(str);
		tinfo3d->setvis(true);
	} else {
		tinfo3d->setvis(false);
	}
}

void quant3_draw2d()
{
	clipclear32(B32,C32(0,0,128));
	rl->draw();
	S32 i;
	switch(kind) {
// r,i against x, animate t
	case T_RI_X: 
		{
			static C32 rgbcolsy[3]={C32RED,C32WHITE,C32GREEN};
			fplot afplot(VIEWXSTART,VIEWYSTART,VIEWXSIZE,VIEWYSIZE,0.0f,-1.0f,1.0f,1.0f,B32,"X","R,I",rgbcolsy);
			afplot.drawaxis();
			afplot.drawlabels();
			afplot.startlinev();
			for (i=0;i<SPACESIZE;++i) {
				float x=i*(1.0f/SPACESIZE);
				float y=reals[lshift(countr,LTIMESIZE-LANIMSIZE)][i];
				afplot.flinev(x,y,C32RED);
			}
			afplot.flinev(1.0f,0.0f,C32RED);
			afplot.startlinev();
			for (i=0;i<SPACESIZE;++i) {
				float x=i*(1.0f/SPACESIZE);
				float y=imags[lshift(countr,LTIMESIZE-LANIMSIZE)][i];
				afplot.flinev(x,y,C32GREEN);
			}
			afplot.flinev(1.0f,0.0f,C32GREEN);
			break;
		}
// p against x, animate t
	case T_P_X: 
		{
			fplot afplot(VIEWXSTART,VIEWYSTART,VIEWXSIZE,VIEWYSIZE,0.0f,0.0f,1.0f,1.0f,B32,"X","P",0);
			afplot.drawaxis();
			afplot.drawlabels();
			afplot.startlinev();
			for (i=0;i<SPACESIZE;++i) {
				float x=i*(1.0f/SPACESIZE);
				float re=reals[lshift(countr,LTIMESIZE-LANIMSIZE)][i];
				float im=imags[lshift(countr,LTIMESIZE-LANIMSIZE)][i];
				float y=re*re+im*im;//probs[lshift(countr,LTIMESIZE-LANIMSIZE)][i];
				afplot.flinev(x,y,C32CYAN);
			}
			afplot.flinev(1.0f,0.0f,C32CYAN);
			break;
		}
// r,i, animate t
	case T_RI: 
		{
			fplot afplot(VIEWXSTART,VIEWYSTART,VIEWXSIZE,VIEWYSIZE,-1.0f,-1.0f,1.0f,1.0f,B32,"R","I",0);
			afplot.drawaxis();
			afplot.drawlabels();
			afplot.startlinev();
			for (i=0;i<SPACESIZE;++i) {
				float x=reals[lshift(countr,LTIMESIZE-LANIMSIZE)][i];
				float y=imags[lshift(countr,LTIMESIZE-LANIMSIZE)][i];
				afplot.flinev(x,y,C32CYAN);
			}
			afplot.flinev(0.0f,0.0f,C32CYAN);
			break;
		}
// 3d plot, animate t
	case T_RIX:
		{
			fplot3d afplot3d(VIEWXSTART,VIEWYSTART,VIEWXSIZE,VIEWYSIZE,-1.0f,-1.0f,0.0f,1.0f,1.0f,1.0f,B32,"R","I","X",pitch,yaw);
			afplot3d.drawaxis();
			afplot3d.drawlabels();
			afplot3d.startlinev();
			for (i=0;i<SPACESIZE;++i) {
				float x=reals[lshift(countr,LTIMESIZE-LANIMSIZE)][i];
				float y=imags[lshift(countr,LTIMESIZE-LANIMSIZE)][i];
				float z=i*(1.0f/SPACESIZE);
				afplot3d.flinev(x,y,z,C32LIGHTGRAY,1.5f);
			}
			afplot3d.flinev(0.0f,0.0f,1.0f,C32LIGHTGRAY,1.5f);
			break;
		}
// r,i against t, animate x
	case X_RI_T:
		{
			static C32 rgbcolsy[3]={C32RED,C32WHITE,C32GREEN};
			fplot afplot(VIEWXSTART,VIEWYSTART,VIEWXSIZE,VIEWYSIZE,0.0f,-1.0f,1.0f,1.0f,B32,"T","R,I",rgbcolsy);
			afplot.drawaxis();
			afplot.drawlabels();
			afplot.startlinev();
			for (i=0;i<TIMESIZE;i++) {
				float x=i*(1.0f/TIMESIZE);
				float y=reals[i][lshift(countr,LSPACESIZE-LANIMSIZE)];
				afplot.flinev(x,y,C32RED);
			}
			float x=1.0f;
			float y=reals[0][lshift(countr,LSPACESIZE-LANIMSIZE)];
			afplot.flinev(x,y,C32RED);
			afplot.startlinev();
			for (i=0;i<TIMESIZE;i++) {
				float x=i*(1.0f/TIMESIZE);
				float y=imags[i][lshift(countr,LSPACESIZE-LANIMSIZE)];
				afplot.flinev(x,y,C32GREEN);
			}
			y=imags[0][lshift(countr,LSPACESIZE-LANIMSIZE)];
			afplot.flinev(x,y,C32GREEN);
			break;
		}
// p against t, animate x
	case X_P_T:
		{
			fplot afplot(VIEWXSTART,VIEWYSTART,VIEWXSIZE,VIEWYSIZE,0.0f,-1.0f,1.0f,1.0f,B32,"T","P",0);
			afplot.drawaxis();
			afplot.drawlabels();
			afplot.startlinev();
			for (i=0;i<TIMESIZE;i++) {
				float x=i*(1.0f/TIMESIZE);
				float re=reals[i][lshift(countr,LSPACESIZE-LANIMSIZE)];
				float im=imags[i][lshift(countr,LSPACESIZE-LANIMSIZE)];
				float y=re*re+im*im;//probs[i][lshift(countr,LSPACESIZE-LANIMSIZE)];
				afplot.flinev(x,y,C32CYAN);
			}
			float x=1.0f;
				float re=reals[0][lshift(countr,LSPACESIZE-LANIMSIZE)];
				float im=imags[0][lshift(countr,LSPACESIZE-LANIMSIZE)];
				float y=re*re+im*im;//probs[i][lshift(countr,LSPACESIZE-LANIMSIZE)];
			afplot.flinev(x,y,C32CYAN);
			break;
		}
// r,i animate x
	case X_RI:
		{
			fplot afplot(VIEWXSTART,VIEWYSTART,VIEWXSIZE,VIEWYSIZE,-1.0f,-1.0f,1.0f,1.0f,B32,"R","I",0);
			afplot.drawaxis();
			afplot.drawlabels();
			afplot.startlinev();
			for (i=0;i<TIMESIZE;i++) {
				float x=reals[i][lshift(countr,LSPACESIZE-LANIMSIZE)];
				float y=imags[i][lshift(countr,LSPACESIZE-LANIMSIZE)];
				afplot.flinev(x,y,C32CYAN);
			}
			float x=reals[0][lshift(countr,LSPACESIZE-LANIMSIZE)];
			float y=imags[0][lshift(countr,LSPACESIZE-LANIMSIZE)];
			afplot.flinev(x,y,C32CYAN);
			break;
		}
// 3d plot, animate x
	case X_RIT:
		{
			fplot3d afplot3d(VIEWXSTART,VIEWYSTART,VIEWXSIZE,VIEWYSIZE,-1.0f,-1.0f,0.0f,1.0f,1.0f,1.0f,B32,"R","I","T",pitch,yaw);
			afplot3d.drawaxis();
			afplot3d.drawlabels();
			afplot3d.startlinev();
			for (i=0;i<TIMESIZE;i++) {
				float x=reals[i][lshift(countr,LSPACESIZE-LANIMSIZE)];
				float y=imags[i][lshift(countr,LSPACESIZE-LANIMSIZE)];
				float z=i*(1.0f/TIMESIZE);
				afplot3d.flinev(x,y,z,C32LIGHTGRAY,1.5f);
			}
			float x=reals[0][lshift(countr,LSPACESIZE-LANIMSIZE)];
			float y=imags[0][lshift(countr,LSPACESIZE-LANIMSIZE)];
			afplot3d.flinev(x,y,1.0f,C32LIGHTGRAY,1.5f);
			break;
		}
	default:
		break;
	}
}

void quant3_exit()
{
#ifdef USEVECTOR
	sintb.clear();
	ak.clear();
	phk.clear();
//	probs.clear();
	reals.clear();
	imags.clear();
	angstreal.clear();
	angstimag.clear();
	angsx.clear();
#endif
	delete rl;
	popdir();
}
