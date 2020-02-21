// QM with varying potentials, at x<0 infinite 'left' wall and at x>1 same thing...
#include <m_eng.h>
#include <l_misclibm.h>
#include "../../u_states.h"
#include "../../u_fplot.h"
#include "../../rungekutta.h"

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
text *tunits;
pbut *bstop;
// end ui;

// log stuff
//#define LTRIGSIZE 15 // trig table
#define LTIMESIZE 15 // hardcore! lots of memory used around 2GB, not anymore, using more dynamic (quicker) on the fly animation
//#define LTIMESIZE 14 // less severe
#define LSPACESIZE 13
//#define LSPACESIZE 11 // less severe
#define LANIMSIZE 19 // resolution of animation

// powers of 2
//#define TRIGSIZE (1<<LTRIGSIZE)
#define TIMESIZE (1<<LTIMESIZE)
#define SPACESIZE (1<<LSPACESIZE)
#define ANIMSIZE (1<<LANIMSIZE)

// non powers of 2
#define MAXANIMSPEED (ANIMSIZE/200)
#define MAXENERGY 2500 // 0 to 31 // actually not energy but different eigenstates
#define ENERGYARRSIZE (MAXENERGY+1)
#define MAXAMP 100
enum {T_RI_X,T_P_X,T_RI,T_RIX,
//	X_RI_T,X_P_T,X_RI,X_RIT,
	NUMGRAPHS}; // graph types
//S32 comprate=10; //#define COMPRATE 10 // how many time frames of computation to performe every proc frame
#define VIEWXSTART 33
#define VIEWYSTART 22
#define VIEWXSIZE 724
#define VIEWYSIZE 724

// arrays
#define USEVECTOR
#ifdef USEVECTOR
//vector<float> sintb; // float sintb[TRIGSIZE]; // trig table
vector<float> ak; // float ak[ENERGYARRSIZE]; // 0 to 31 // 0 not used
vector<S32> phk; // S32 phk[ENERGYARRSIZE]; // quantum states table (energy squareds/momentum, will change for non zero potential function)
//vector <vector<float> > probs; // float probs[TIMESIZE][SPACESIZE]; // [t][x];
//vector <vector<float> > reals; // float reals[TIMESIZE][SPACESIZE]; // [t][x];
//vector <vector<float> > imags; // float imags[TIMESIZE][SPACESIZE]; // [t][x];
vector<float> realst; // float realsx[SPACESIZE]; // [x]; // animate t
vector<float> imagst; // float imagsx[SPACESIZE]; // [x]; // animate t
vector <vector<float> > angstreal; // float angstreal[TIMESIZE][ENERGYARRSIZE]; // [t][n]
vector <vector<float> > angstimag; // float angstimag[TIMESIZE][ENERGYARRSIZE]; // [t][n]
vector <vector<float> > angsx; // float angsx[SPACESIZE][ENERGYARRSIZE]; // [x][n]
vector<float> energies; // float energies[ENERGYARRAYSIZE]; // [n]
#else
//float sintb[TRIGSIZE]; // trig table
float ak[ENERGYARRSIZE]; // 0 to 31 // 0 not used
S32 phk[ENERGYARRSIZE]; // quantum states table (energy squareds/momentum, will change for non zero potential function)
//float probs[TIMESIZE][SPACESIZE]; // [t][x];
float reals[TIMESIZE][SPACESIZE]; // [t][x];
float imags[TIMESIZE][SPACESIZE]; // [t][x];
float angstreal[TIMESIZE][ENERGYARRSIZE]; // [t][n]
float angstimag[TIMESIZE][ENERGYARRSIZE]; // [t][n]
float angsx[SPACESIZE][ENERGYARRSIZE]; // [x][n]
#endif
S32 exlate[ENERGYARRSIZE];
S32 nexlate; // build a list of non zero energies
//float eigen_energies[ENERGYARRSIZE]; // 1 to maxenergy
// end arrays
float sumk; // sum of the energies
float isumk; // graphical normalizer
float normk; // sqrt(2/(sum of squared energies)) // real normalizer
/*
float sint(S32 t)
{
	return sintb[(TRIGSIZE-1)&t];
}

float cost(S32 t)
{
	return sintb[(TRIGSIZE-1)&(t+TRIGSIZE/4)];
}
*/

S32 kind=T_RIX; // T_P_X; // animate time p against x // type of graph
S32 countr; // animation
float pitch,yaw;
//bool docomp;  // update computation
S32 comptime=TIMESIZE; // nothing to compute

S32 lshift(S32 v,S32 s)
{
	if (s>=0)
		return v<<s;
	else
		return v>>-s;
}

float energy=100*PI*PI+.1f;
float qn; // estimated quantum number
float initslope=1.0f;
float v1start=.25f;
float v1end=.75f;
float v1=0.0f; // 200.0f;
float vscale=1.0f/100.0f;
enum {POSX,POSY,POSU,NOFFSETS};
float objstate[NOFFSETS]; // rungekutta x,y
int offsets[NOFFSETS]={POSX,POSY,POSU};
float pscale=1.0f;
float tscale=1.0f;
// real units
float mass=1; // mass in electron mass units
float length=.5f; // in nano meters , 1e-9 meters
// constants
float me=9.109e-31f; // mass of electron in kilograms
float hbar=1.055e-34f; // reduced plank's constant in   Joules * seconds
float j_per_ev=1.602e-19f; // Joules per electron volt
// conversions
float conv_energy;	// factor to real units
float conv_time;	// factor to real units

struct menuvar edv[]={
// user vars
	{"@lightred@---- QUANT4 USER VARS -----------------",NULL,D_VOID,0},
	{"v1start",&v1start,D_FLOAT,FLOATUP/64},
	{"v1end",&v1end,D_FLOAT,FLOATUP/64},
	{"v1",&v1,D_FLOAT,FLOATUP/4},
	{"vscale",&vscale,D_FLOAT,FLOATUP/1024},
	{"pscale",&pscale,D_FLOAT,FLOATUP/64},
	{"tscale",&tscale,D_FLOAT,FLOATUP/64},
	{"initslope",&initslope,D_FLOAT,FLOATUP/64},
	{"blength nanometers",&length,D_FLOAT,FLOATUP/64},
	{"cmass mass electron",&mass,D_FLOAT,FLOATUP/64},
};
const int nedv=sizeof(edv)/sizeof(edv[0]);

// uses 
// energy,v1,v1start,v1end,
// initslope // not as important
// here 't' == 'x', (run thru x)
// all 'reals'
void initfs(float *os)
{
	os[POSX]=0; // t or x
	os[POSY]=0; // psi(x)
	qn=sqrt(fabsf(energy))/PI;
	if (qn<1.0f)
		qn=1.0f;
	os[POSU]=PI*qn*initslope; // d/dx psi(x) // initial slope
}

void changefs(float *os,float *delos,float curt)
{
	float vx;
	if (os[POSX]>=v1start && os[POSX]<=v1end)
		vx=v1*PI*PI;
	else
		vx=0;
	delos[POSX]=1;						// dx/dt =  1 // x == t
	delos[POSY]= os[POSU];				// dy/dt =  u // u is subst for dy/dx
	delos[POSU]=(vx-energy)*os[POSY];	// du/dt = d2y/dx2 = (vx-energy) * y
}

// runge kutta
// 1 2d x,y point influenced by x,y,t
// can find qn by how many nodes it has 'qn-1' (change signs)
S32 drawfunctionrk2d(float* objs, S32* offs,S32 noffs,
					void (*initfunc)(float *os),
					void (*changefunc)(float *os,float *delos,float time),
					float* finaly,S32 qn)
{
	S32 sc=0; // sign change
	bool pos=true; // start out positive
	initfunc(objs);
	pointf2 p=pointf2x(objs[POSX],objs[POSY]); // x,psi(x)
//	logger("t=%7.3f, rk %7.3f, %7.3f\n",0.0f,p.x,p.y);
	S32 i;
	float tinc=1.0f/SPACESIZE;
	for (i=1;i<=SPACESIZE;++i) {
		float t=float(i)*tinc;
		dorungekutta(objs,offs,noffs,t,tinc,changefunc);
		pointf2 c=pointf2x(objs[POSX],objs[POSY]);
//		drawfline(p,c,funccolor);
		if (qn)
			angsx[i-1][qn]=p.y;
		p=c;
		if (pos && p.y<0.0f || !pos && p.y>0.0f) { // change signs
			pos=!pos;
			++sc;
		}
//		logger("t=%7.3f, rk %7.3f, %7.3f\n",t,p.x,p.y);
	}
	if (finaly)
		*finaly=p.y;
	if (qn) {
		float psum=0;
		for (i=0;i<SPACESIZE;++i) {
			psum+=angsx[i][qn]*angsx[i][qn];
		}
		psum*=2.0f/float(SPACESIZE);
		logger("psum for %d is %7.3f\n",qn,psum);
		if (psum<.999f || psum>1.001f) { // normalize
			float psr=1.0f/sqrtf(psum);
			for (i=0;i<SPACESIZE;++i) {
				angsx[i][qn]*=psr;
			}
		}
	}
	return sc;
}

// end runge kutta
void findenergyn(S32 qn)
{
	if (v1==0.0f) {
// infinite well, analytical solution
		energy=PI*PI*qn*qn; // e = qn^2;
		energies.push_back(energy);
		S32 i;
		for (i=0;i<SPACESIZE;++i) {
			angsx[i][qn]=sinf(qn*PI*i/SPACESIZE); // standing sin waves x=sin(n*pi*x/L); // where L=1;
		}
		return;
	}
// general method using runge kutta
	else {
//		logger("finding energy for quant number (1..n) %d\n",qn);
		S32 di=35; // doubling iterations
		float estart=min(v1*PI*PI,0.0f);
		float estep=50000.0f;
		energy=estart;
		bool firstcross=false;
		S32 i;
		for (i=0;i<di;) {
			float finaly;
			S32 sc=drawfunctionrk2d(objstate,offsets,3,initfs,changefs,&finaly,0);
//			logger("energy %9.5f, energy/(PI^2) %9.5f, sign changes %d, finaly %9.5f\n",energy,energy/(PI*PI),sc,finaly);
			if (sc<qn)
				energy+=estep;
			else {
				firstcross=true;
				energy-=estep;
			}
			if (firstcross) {
				estep*=.5f;
				++i;
			}
			
		}
		energies.push_back(energy);
		drawfunctionrk2d(objstate,offsets,3,initfs,changefs,0,qn);
	}
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
//			lenergies->printf("E %3d %5d:A %6.2f,P %4d",i,i*i,ak[i],phk[i]);
			lenergies->printf("E %3d %7.3f:A %6.2f,P %5.2f",i,energies[i]/PI/PI,ak[i],TWOPI*phk[i]/TIMESIZE);
//		}
	}
	lenergies->setidxc(oldidx);
}

void compute_energies()
{
	energies.clear();
	energies.push_back(0.0f);
	S32 qinit=1;
	S32 qfinal=MAXENERGY;
	S32 q;
	for (q=qinit;q<=qfinal;++q) {
		if (ak[q])
			findenergyn(q);
		else
			energies.push_back(0.0f);
	}
/*	S32 x,n;
	for (x=0;x<SPACESIZE;x++)
		for (n=1;n<ENERGYARRSIZE;n++) {
			float rx=x*n*PI/SPACESIZE;
			//rx=lshift(n*x,LTRIGSIZE-LSPACESIZE-1);
			angsx[x][n]=sinf(rx);//sint(rx)*ak[n];
		} */
	update_energy_list();
}

void compute()
{
// should be done with doubles
//	conv_energy=hbar*hbar*PI*PI/(2.0f*mass*length*length);	// factor to real units
// tame the hbars' exponent
	conv_energy=(hbar*1e18f*PI*PI)*(hbar/me)/(2*mass*length*length*j_per_ev);
	conv_time=2.0f*PI*hbar/(conv_energy*j_per_ev);	// factor to real units

	C8 str[200];
	sprintf(str,"units: length %f nanometers, mass %f emus, energies %g ev, time %g sec",length,mass,conv_energy,conv_time);
	tunits->settname(str);
	nexlate=0;
	S32 n;
	for (n=1;n<ENERGYARRSIZE;n++) {
		if (ak[n])
			exlate[nexlate++]=n;
	}
	compute_energies();
	comptime=0;
}

void computeproc()
{
	S32 t,n;
	if (comptime==0) {
		sumk=0;
		float sumk2=0;
		for (n=1;n<ENERGYARRSIZE;n++) {
			sumk+=ak[n];
			sumk2+=ak[n]*ak[n];
		}
		if (sumk==0) {
//			comptime=TIMESIZE;
			return;
		}
//  figure out normalize
		isumk=1.0f/sumk; // this scales/looks better
		normk=sqrtf(2.0f/sumk2); // this one is the correct normalizer, not so important
		perf_start(TEST1);
		for (t=0;t<TIMESIZE;t++) {
			S32 np;
			for (np=0;np<nexlate;++np) {
				n=exlate[np];
//				float rt=energies[n]*t/PI/PI*tscale *TWOPI/TIMESIZE + phk[n]*TWOPI/TIMESIZE;
				float rt=(-energies[n]*t/PI/PI*tscale+phk[n])*TWOPI/TIMESIZE;
				//rt=lshift(n*n*t+phk[n],LTRIGSIZE-LTIMESIZE);
				angstreal[t][n]=cosf(rt)*ak[n];//cost(rt);
				angstimag[t][n]=sinf(rt)*ak[n];//sint(rt);
			}
		}
		perf_end(TEST1);
		comptime=TIMESIZE;
	}
//	if (comptime==TIMESIZE)
		return;
	perf_start(TEST2);
/*	if (wininfo.fpsavg2>wininfo.fpswanted+2.0f)
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
		endtime=TIMESIZE; */
	perf_end(TEST2);
}

void computet(S32 t)
{
//	for (t=comptime;t<endtime;++t) {
#ifdef USEVECTOR
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
		S32 x;
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
				S32 n=exlate[np];
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
//	}
}

void update_text()
{
		C8 str[50];
		sprintf(str,"Energy %d",henergy->getidx());
		tenergy->settname(str);
		sprintf(str,"Amp %d",hamp->getidx());
		tamp->settname(str);
		S32 p=hph->getidx();
		sprintf(str,"Phase %5.2f",p*TWOPI/TIMESIZE);
		tph->settname(str);
		sprintf(str,"Mean Energy %d",hmean->getidx());
		tmean->settname(str);
		sprintf(str,"Mean Amp %d",hmamp->getidx());
		tmamp->settname(str);
		sprintf(str,"Width %d",hwidth->getidx());
		twidth->settname(str);
		sprintf(str,"PhaseDelta %5.2f",hpc->getidx()*TWOPI/TIMESIZE);
		tpc->settname(str);
}

} // end anonymous namespace

void quant4_init()
{
	video_setupwindow(1024,768);
	extradebvars(edv,nedv);
	pushandsetdir("wat_physics/quant4");
// ui
	rl=res_loadfile("quant4res.txt");
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
	tunits=rl->find<text>("TUNITS");
	bstop=rl->find<pbut>("BSTOP");
	focus=oldfocus=0;
// end ui
// init arrays
// init trig tables
//	S32 i;
//	printf("init trig\n");
#if 0
#ifdef USEVECTOR
	sintb.assign(TRIGSIZE,0);
#endif
	for (i=0;i<TRIGSIZE;i++) {
		sintb[i]=sinf(i*TWOPI/TRIGSIZE);
	}
#endif
// init energies
#ifdef USEVECTOR
	ak.assign(ENERGYARRSIZE,0);
#endif
	ak[1]=50;
//	ak[2]=50;
#ifdef USEVECTOR
	phk.assign(ENERGYARRSIZE,0);
#endif
// init tables
#ifdef USEVECTOR
	vector<float> ts(SPACESIZE,0.0f);
//	probs.assign(TIMESIZE,ts);
	realst.assign(SPACESIZE,0);
	imagst.assign(SPACESIZE,0);
	vector<float> es(ENERGYARRSIZE,0.0f);
	angstreal.assign(TIMESIZE,es);
	angstimag.assign(TIMESIZE,es);
	angsx.assign(SPACESIZE,es);
#endif
// end init arrays
	lenergies->setidxc(0);
//	docomp=true;
	compute();
//	docomp=false;
	update_text();
	update_energy_list();
}

void quant4_proc()
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
//			if (docomp) {
				compute();
//				docomp=false;
//			}
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
//			docomp=true;
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
//			docomp=true;
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
//			docomp=true;
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
//	if (kind<NUMGRAPHS/2) // animate time
		sprintf(str,"K%d : T%6.4f : VT%6.4f",kind,float(lshift(countr,LTIMESIZE-LANIMSIZE))/TIMESIZE,float(hcntvel->getidx())/TIMESIZE);
//	else // animate space
//		sprintf(str,"K%d : X%5d : VX%2d",kind,lshift(countr,LSPACESIZE-LANIMSIZE),hcntvel->getidx());
	tcntinfo->settname(str);
	hcntval->setminmaxval(0,ANIMSIZE-1);
	hcntval->setidx(countr);
	computeproc();
//	sprintf(str,"rt %3d %6d/%6d %3d%%",comprate,comptime,TIMESIZE,comptime*100/TIMESIZE);
//	tcomp->settname(str);
	if (kind==T_RIX /* || kind==X_RIT */) {
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

void quant4_draw2d()
{
	clipclear32(B32,C32(0,0,128));
	rl->draw();
	S32 i;
	computet(lshift(countr,LTIMESIZE-LANIMSIZE));
	switch(kind) {
// r,i against x, animate t
	case T_RI_X: 
		{
			static C32 rgbcolsy[3]={C32RED,C32WHITE,C32GREEN};
			fplot afplot(VIEWXSTART,VIEWYSTART,VIEWXSIZE,VIEWYSIZE,0.0f,-1.0f,1.0f,1.0f,B32,"X","R,I",rgbcolsy);
			afplot.drawaxis();
			afplot.drawlabels();
			afplot.startlinev();
			afplot.flinev(0.0f,0.0f,C32YELLOW);
			afplot.flinev(v1start,0.0f,C32YELLOW);
			afplot.flinev(v1start,v1*vscale,C32YELLOW);
			afplot.flinev(v1end,v1*vscale,C32YELLOW);
			afplot.flinev(v1end,0.0f,C32YELLOW);
			afplot.flinev(1.0f,0.0f,C32YELLOW);
			afplot.startlinev();
			for (i=0;i<SPACESIZE;++i) {
				float x=i*(1.0f/SPACESIZE);
				float y=realst[i];
				afplot.flinev(x,y*pscale,C32RED);
			}
			afplot.flinev(1.0f,0.0f,C32RED);
			afplot.startlinev();
			for (i=0;i<SPACESIZE;++i) {
				float x=i*(1.0f/SPACESIZE);
				float y=imagst[i];
				afplot.flinev(x,y*pscale,C32GREEN);
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
			afplot.flinev(0.0f,0.0f,C32YELLOW);
			afplot.flinev(v1start,0.0f,C32YELLOW);
			afplot.flinev(v1start,v1*vscale,C32YELLOW);
			afplot.flinev(v1end,v1*vscale,C32YELLOW);
			afplot.flinev(v1end,0.0f,C32YELLOW);
			afplot.flinev(1.0f,0.0f,C32YELLOW);
			afplot.startlinev();
			for (i=0;i<SPACESIZE;++i) {
				float x=i*(1.0f/SPACESIZE);
				float re=realst[i];
				float im=imagst[i];
				float y=re*re+im*im;//probs[lshift(countr,LTIMESIZE-LANIMSIZE)][i];
				afplot.flinev(x,y*pscale*pscale,C32CYAN);
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
				float x=realst[i]*pscale;
				float y=imagst[i]*pscale;
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
			afplot3d.flinev(0.0f,0.0f,0.0f,C32YELLOW);
			afplot3d.flinev(0.0f,0.0f,v1start,C32YELLOW);
			afplot3d.flinev(0.0f,v1*vscale,v1start,C32YELLOW);
			afplot3d.flinev(0.0f,v1*vscale,v1end,C32YELLOW);
			afplot3d.flinev(0.0f,0.0f,v1end,C32YELLOW);
			afplot3d.flinev(0.0f,0.0f,1.0f,C32YELLOW);
			afplot3d.startlinev();
			for (i=0;i<SPACESIZE;++i) {
				float x=realst[i]*pscale;
				float y=imagst[i]*pscale;
				float z=i*(1.0f/SPACESIZE);
				afplot3d.flinev(x,y,z,C32LIGHTGRAY,1.5f);
			}
			afplot3d.flinev(0.0f,0.0f,1.0f,C32LIGHTGRAY,1.5f);
			break;
		}
#if 0
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
#endif
	default:
		break;
	}
}

void quant4_exit()
{
	extradebvars(0,0);
#ifdef USEVECTOR
//	sintb.clear();
	ak.clear();
	phk.clear();
//	probs.clear();
	realst.clear();
	imagst.clear();
	angstreal.clear();
	angstimag.clear();
	angsx.clear();
	energies.clear();
#endif
	delete rl;
	popdir();
}
