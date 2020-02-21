// simple tests and one time utils
#include <m_eng.h>
#include <l_misclibm.h>
#include "u_states.h"
#include <tinyxml.h>
#include "math_abstract.h"

namespace {
// ui
shape* rl;
shape* focus,*oldfocus;

pbut* bquit;
pbut* bp03n,*bp03d,*bp02n,*bp02d,*bp01n,*bp01d,*bp00n,*bp00d;
pbut* bp13n,*bp13d,*bp12n,*bp12d,*bp11n,*bp11d,*bp10n,*bp10d;
pbut* bp03nm,*bp03dm,*bp02nm,*bp02dm,*bp01nm,*bp01dm,*bp00nm,*bp00dm;
pbut* bp13nm,*bp13dm,*bp12nm,*bp12dm,*bp11nm,*bp11dm,*bp10nm,*bp10dm;
text* po0,*po1;
text *tp0,*tp1;
text* padd,*psub,*ptimes,*pdiv,*pmod;
text* beq,*bneq,*bg,*bge,*bl,*ble;

S32 p0n[4];
S32 p0d[4]={1,1,1,1};
S32 p1n[4];
S32 p1d[4]={1,1,1,1};
polynomial p0,p1;

#if 0
void testpolys()
{
	polynomial pd(1,0,1,-1); // mod X^3+X-1
	polynomial pp(1); // X^0
	polynomial pm(1,0); // powers of X
	S32 i;
	logger("MOD = %s\n",pd.tostring().c_str());
	for (i=0;i<20;++i) {
		logger("X^%2d = %s\n",i,pp.tostring().c_str());
		pp = pp * pm;
		pp = pp % pd;
	}
}
#endif
#if 1
void testpolys()
{
	complexf ca(4,5);
	complexf cb(6,7);
	complexf cc=ca/cb;
	logger("complexf division is '%s'\n",cc.tostring().c_str());
	const float wr=-.5f;
	const float wi=sqrtf(3.0f)*.5f;
	C8 s[]="EXRSYZ";
	mat2c g[6]={
		mat2c(  1,  0,  0,  1),
		mat2c(  1,  0,  0, -1),
		mat2c( wr,-wi, wi, wr),
		mat2c( wr, wi,-wi, wr),
		mat2c( wr, wi, wi,-wr),
		mat2c( wr,-wi,-wi,-wr)
	};
	S32 i,j;
	for (j=0;j<6;++j) {
		for (i=0;i<6;++i) {
			logger("%c * %c is %s\n",s[j],s[i],(g[j]*g[i]).tostring().c_str());
		}
	}
}
#endif

void calcpolys()
{
	vector<fraction> fv;
	fv.push_back(fraction(p0n[0],p0d[0]));
	fv.push_back(fraction(p0n[1],p0d[1]));
	fv.push_back(fraction(p0n[2],p0d[2]));
	fv.push_back(fraction(p0n[3],p0d[3]));
	p0=fv;
	fv.clear();
	fv.push_back(fraction(p1n[0],p1d[0]));
	fv.push_back(fraction(p1n[1],p1d[1]));
	fv.push_back(fraction(p1n[2],p1d[2]));
	fv.push_back(fraction(p1n[3],p1d[3]));
	p1=fv;
}

void updatetext()
{
	C8* bs[2]={"FALSE","TRUE"};
	C8 s[100];
	sprintf(s,"A = %3d/%3d X^3 + %3d/%3d X^2 + %3d/%3d X + %3d/%3d",
		p0n[3],p0d[3],p0n[2],p0d[2],p0n[1],p0d[1],p0n[0],p0d[0]);
	po0->settname(s);
	sprintf(s,"B = %3d/%3d X^3 + %3d/%3d X^2 + %3d/%3d X + %3d/%3d",
		p1n[3],p1d[3],p1n[2],p1d[2],p1n[1],p1d[1],p1n[0],p1d[0]);
	po1->settname(s);
	sprintf(s,"A = %s",p0.tostring().c_str());
	tp0->settname(s);
	sprintf(s,"B = %s",p1.tostring().c_str());
	tp1->settname(s);
	sprintf(s,"A + B  =  %s",(p0+p1).tostring().c_str());
	padd->settname(s);
	sprintf(s,"A - B  =  %s",(p0-p1).tostring().c_str());
	psub->settname(s);
	sprintf(s,"A * B  =  %s",(p0*p1).tostring().c_str());
	ptimes->settname(s);
	sprintf(s,"A / B  =  %s",(p0/p1).tostring().c_str());
	pdiv->settname(s);
	sprintf(s,"A %% B  =  %s",(p0%p1).tostring().c_str());
	pmod->settname(s);
	sprintf(s,"A == B  =  %s",bs[p0==p1]);
	beq->settname(s);
	sprintf(s,"A ! =B  =  %s",bs[p0!=p1]);
	bneq->settname(s);
	sprintf(s,"A > B  =  %s",bs[p0>p1]);
	bg->settname(s);
	sprintf(s,"A >= B  =  %s",bs[p0>=p1]);
	bge->settname(s);
	sprintf(s,"A < B  =  %s",bs[p0<p1]);
	bl->settname(s);
	sprintf(s,"A <= B  =  %s",bs[p0<=p1]);
	ble->settname(s);
}

void buttonclicked(S32 p,S32 idx,S32 nd,S32 pm)
{
	S32* pi;
	if (p==0)
		if (nd==0)
			pi=p0n;
		else
			pi=p0d;
	else
		if (nd==0)
			pi=p1n;
		else
			pi=p1d;
	if (pm==0)
		++pi[idx];
	else
		--pi[idx];
	calcpolys();
	updatetext();
}

}
void polynomial_init()
{
	video_setupwindow(1024,768);
	pushandsetdir("scratch");
	rl=res_loadfile("polyres.txt");
	popdir();
	bquit=rl->find<pbut>("BQUIT");
	bp03n=rl->find<pbut>("P03N");
	bp03d=rl->find<pbut>("P03D");
	bp02n=rl->find<pbut>("P02N");
	bp02d=rl->find<pbut>("P02D");
	bp01n=rl->find<pbut>("P01N");
	bp01d=rl->find<pbut>("P01D");
	bp00n=rl->find<pbut>("P00N");
	bp00d=rl->find<pbut>("P00D");

	bp13n=rl->find<pbut>("P13N");
	bp13d=rl->find<pbut>("P13D");
	bp12n=rl->find<pbut>("P12N");
	bp12d=rl->find<pbut>("P12D");
	bp11n=rl->find<pbut>("P11N");
	bp11d=rl->find<pbut>("P11D");
	bp10n=rl->find<pbut>("P10N");
	bp10d=rl->find<pbut>("P10D");

	bp03nm=rl->find<pbut>("P03NM");
	bp03dm=rl->find<pbut>("P03DM");
	bp02nm=rl->find<pbut>("P02NM");
	bp02dm=rl->find<pbut>("P02DM");
	bp01nm=rl->find<pbut>("P01NM");
	bp01dm=rl->find<pbut>("P01DM");
	bp00nm=rl->find<pbut>("P00NM");
	bp00dm=rl->find<pbut>("P00DM");

	bp13nm=rl->find<pbut>("P13NM");
	bp13dm=rl->find<pbut>("P13DM");
	bp12nm=rl->find<pbut>("P12NM");
	bp12dm=rl->find<pbut>("P12DM");
	bp11nm=rl->find<pbut>("P11NM");
	bp11dm=rl->find<pbut>("P11DM");
	bp10nm=rl->find<pbut>("P10NM");
	bp10dm=rl->find<pbut>("P10DM");
	po0=rl->find<text>("POA");
	po1=rl->find<text>("POB");
	tp0=rl->find<text>("PA");
	tp1=rl->find<text>("PB");
	padd=rl->find<text>("PADD");
	psub=rl->find<text>("PSUB");
	ptimes=rl->find<text>("PTIMES");
	pdiv=rl->find<text>("PDIV");
	pmod=rl->find<text>("PMOD");
	beq=rl->find<text>("PE");
	bneq=rl->find<text>("PNE");
	bg=rl->find<text>("PG");
	bge=rl->find<text>("PGE");
	bl=rl->find<text>("PL");
	ble=rl->find<text>("PLE");
	focus=oldfocus=0;
	updatetext();
	testpolys();
}

void polynomial_proc()
{
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
// quit
	if (ret==1) {
		if (focus == bquit) {
			poporchangestate(STATE_MAINMENU);
		} else if (focus == bp03n) {
			buttonclicked(0,3,0,0);
		} else if (focus == bp03d) {
			buttonclicked(0,3,1,0);
		} else if (focus == bp02n) {
			buttonclicked(0,2,0,0);
		} else if (focus == bp02d) {
			buttonclicked(0,2,1,0);
		} else if (focus == bp01n) {
			buttonclicked(0,1,0,0);
		} else if (focus == bp01d) {
			buttonclicked(0,1,1,0);
		} else if (focus == bp00n) {
			buttonclicked(0,0,0,0);
		} else if (focus == bp00d) {
			buttonclicked(0,0,1,0);
		} else if (focus == bp13n) {
			buttonclicked(1,3,0,0);
		} else if (focus == bp13d) {
			buttonclicked(1,3,1,0);
		} else if (focus == bp12n) {
			buttonclicked(1,2,0,0);
		} else if (focus == bp12d) {
			buttonclicked(1,2,1,0);
		} else if (focus == bp11n) {
			buttonclicked(1,1,0,0);
		} else if (focus == bp11d) {
			buttonclicked(1,1,1,0);
		} else if (focus == bp10n) {
			buttonclicked(1,0,0,0);
		} else if (focus == bp10d) {
			buttonclicked(1,0,1,0);
		} else if (focus == bp02nm) {
			buttonclicked(0,3,0,1);
		} else if (focus == bp03dm) {
			buttonclicked(0,3,1,1);
		} else if (focus == bp03nm) {
			buttonclicked(0,2,0,1);
		} else if (focus == bp02dm) {
			buttonclicked(0,2,1,1);
		} else if (focus == bp01nm) {
			buttonclicked(0,1,0,1);
		} else if (focus == bp01dm) {
			buttonclicked(0,1,1,1);
		} else if (focus == bp00nm) {
			buttonclicked(0,0,0,1);
		} else if (focus == bp00dm) {
			buttonclicked(0,0,1,1);
		} else if (focus == bp13nm) {
			buttonclicked(1,3,0,1);
		} else if (focus == bp13dm) {
			buttonclicked(1,3,1,1);
		} else if (focus == bp12nm) {
			buttonclicked(1,2,0,1);
		} else if (focus == bp12dm) {
			buttonclicked(1,2,1,1);
		} else if (focus == bp11nm) {
			buttonclicked(1,1,0,1);
		} else if (focus == bp11dm) {
			buttonclicked(1,1,1,1);
		} else if (focus == bp10nm) {
			buttonclicked(1,0,0,1);
		} else if (focus == bp10dm) {
			buttonclicked(1,0,1,1);
		}
	}
}

void polynomial_draw2d()
{
	rl->draw();
}

void polynomial_exit()
{
	delete rl;
}
