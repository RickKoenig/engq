// draw the mainmenu's shape class without doing anything else
#include <m_eng.h>
#include <l_misclibm.h>
#include "u_gametheory.h"
#include "u_states.h"

namespace {

const S32 maxdim = 8;
S32 dim = 2;
const S32 maxres = 1000;

shape* rl;
shape* focus;

pbut* pb;
vscroll* vs[maxdim];
hscroll* hs[maxdim];
text* txh[maxdim];
text* txv[maxdim];
text* pot[maxdim][maxdim];
pbut* pop[maxdim][maxdim];
pbut* pom[maxdim][maxdim];
text* po;
hscroll* hsd;
text* txd;
text* pra;
text* prb;

S32 payoffm[maxdim][maxdim];
float rproba[maxdim];
float rprobb[maxdim];
float proba[maxdim];
float probb[maxdim];
S32 dira[maxdim];
S32 dirb[maxdim];

float payoff;

S32 xspace,yspace;

void setdim()
{
	S32 i,j;
	for (i=0;i<maxdim;++i) {
		S32 vis1=i<dim;
		S32 vis2=i<dim-1;
		vs[i]->setvis(vis2);
		hs[i]->setvis(vis2);
		txv[i]->setvis(vis1);
		txh[i]->setvis(vis1);
	}
	for (j=0;j<maxdim;++j) {
		for (i=0;i<maxdim;++i) {
			S32 vis=i<dim && j<dim;
			pot[j][i]->setvis(vis);
			pop[j][i]->setvis(vis);
			pom[j][i]->setvis(vis);
		}
	}
}

void calcpayoff()
{
	S32 i,j;
	payoff=0;
	for (j=0;j<dim;++j)
		for (i=0;i<dim;++i) {
			payoff+=proba[j]*probb[i]*payoffm[j][i];
		}
}

// wants a bigger payoff
void calcproba()
{
	S32 i;
	proba[0]=rproba[0];
	float psum=proba[0];
	for (i=1;i<dim-1;++i) {
		proba[i]=rproba[i]*(1-psum);
		psum+=proba[i];
	}
	proba[i]=1-psum;
}

// wants a smaller payoff
void calcprobb()
{
	S32 i;
	probb[0]=rprobb[0];
	float psum=probb[0];
	for (i=1;i<dim-1;++i) {
		probb[i]=rprobb[i]*(1-psum);
		psum+=probb[i];
	}
	probb[i]=1-psum;
}

void calcperturba()
{
	calcpayoff();
	float basepayoff=payoff;
	S32 i;
	for (i=0;i<dim-1;++i) {
		float save=rproba[i];
		rproba[i]+=.01f;
		calcproba();
		calcpayoff();
		rproba[i]=save;
		if (payoff>basepayoff)
			dira[i]=1;
		else if (payoff<basepayoff)
			dira[i]=-1;
		else
			dira[i]=0;
	}
	calcproba();
}

void calcperturbb()
{
	calcpayoff();
	float basepayoff=payoff;
	S32 i;
	for (i=0;i<dim-1;++i) {
		float save=rprobb[i];
		rprobb[i]+=.01f;
		calcprobb();
		calcpayoff();
		rprobb[i]=save;
		if (payoff>basepayoff)
			dirb[i]=-1;
		else if (payoff<basepayoff)
			dirb[i]=1;
		else
			dirb[i]=0;
	}
	calcprobb();
}

void drawpayoff()
{
	C8 str[50];
	S32 i;
	
	float prob=0;
	for (i=0;i<dim;++i)
		prob+=proba[i];
	sprintf(str,"%3.2f",prob);
	pra->settname(str);
	
	prob=0;
	for (i=0;i<dim;++i)
		prob+=probb[i];
	sprintf(str,"%3.2f",prob);
	prb->settname(str);

	sprintf(str,"payoff %8.4f",payoff);
	po->settname(str);
}

// wants a bigger payoff
void drawproba()
{
	S32 i;
	C8 str[50];
	static C8* dirs="<.>";
	for (i=0;i<dim;++i) {
		sprintf(str,"%3.3f%c",proba[i],dirs[dira[i]+1]);
		txh[i]->settname(str);
	}
}

// wants a smaller payoff
void drawprobb()
{
	S32 i;
	C8 str[50];
	static C8* dirs="^.v";
	for (i=0;i<dim;++i) {
		sprintf(str,"%3.3f%c",probb[i],dirs[dirb[i]+1]);
		txv[i]->settname(str);
	}
}

}

void gametheoryinit()
{
	S32 i,j;
	C8 str[50];
//	rungametheory();
	video_setupwindow(800,600);
	factory2<shape> fact;
	pushandsetdir("gametheory");
	script sc("gametheoryres.txt");
	popdir();
	rl=fact.newclass_from_handle(sc);
	po=rl->find<text>("TEXTPO");
	pb=rl->find<pbut>("PBUTQUIT");
	hsd=rl->find<hscroll>("SCLHD");
	pra=rl->find<text>("TEXTPA");
	prb=rl->find<text>("TEXTPB");
	hsd->setnumidx(8);
	hsd->setidx(2-1);
	txd=rl->find<text>("TEXTD");
	sprintf(str,"dim %d",hsd->getidx()+1);
	txd->settname(str);
	memset(rproba,0,sizeof(rproba));
	memset(rprobb,0,sizeof(rprobb));
// vscroll
	for (i=0;i<maxdim;++i) {
		sprintf(str,"SCLV%d",i);
		vs[i]=rl->find<vscroll>(str);
		vs[i]->setnumidx(maxres+1);
		vs[i]->setidx(0);
	}
// hscroll
	for (i=0;i<maxdim;++i) {
		sprintf(str,"SCLH%d",i);
		hs[i]=rl->find<hscroll>(str);
		hs[i]->setnumidx(maxres+1);
		hs[i]->setidx(0);
	}
// vtext
	for (i=0;i<maxdim;++i) {
		sprintf(str,"TEXTV%d",i);
		txv[i]=rl->find<text>(str);
//		sprintf(str,"%3.3f",probb[i]);
//		txv[i]->settname(str);
	}
// htext
	for (i=0;i<maxdim;++i) {
		sprintf(str,"TEXTH%d",i);
		txh[i]=rl->find<text>(str);
//		sprintf(str,"%3.3f",proba[i]);
//		txh[i]->settname(str);
	}
	calcproba();
	calcprobb();
// payoff text
	xspace=vs[1]->getx()-vs[0]->getx();
	yspace=hs[1]->gety()-hs[0]->gety();
	for (j=0;j<maxdim;++j) {
		for (i=0;i<maxdim;++i) {
			sprintf(str,"TEXT%d%d",j,i);
			pot[j][i]=rl->find<text>(str);
			sprintf(str,"%d",payoffm[j][i]);
			pot[j][i]->settname(str);
		}
	}
	S32 stx=pot[0][0]->getx();
	S32 sty=pot[0][0]->gety();
	for (j=0;j<maxdim;++j) {
		for (i=0;i<maxdim;++i) {
			pot[j][i]->setx(stx+i*xspace);
			pot[j][i]->sety(sty+j*yspace);
		}
	}
// payoff plus
	for (j=0;j<maxdim;++j) {
		for (i=0;i<maxdim;++i) {
			sprintf(str,"PBUT%d%dp",j,i);
			pop[j][i]=rl->find<pbut>(str);
		}
	}
	stx=pop[0][0]->getx();
	sty=pop[0][0]->gety();
	for (j=0;j<maxdim;++j) {
		for (i=0;i<maxdim;++i) {
			pop[j][i]->setx(stx+i*xspace);
			pop[j][i]->sety(sty+j*yspace);
		}
	}
// payoff minus
	for (j=0;j<maxdim;++j) {
		for (i=0;i<maxdim;++i) {
			sprintf(str,"PBUT%d%dm",j,i);
			pom[j][i]=rl->find<pbut>(str);
		}
	}
	stx=pom[0][0]->getx();
	sty=pom[0][0]->gety();
	for (j=0;j<maxdim;++j) {
		for (i=0;i<maxdim;++i) {
			pom[j][i]->setx(stx+i*xspace);
			pom[j][i]->sety(sty+j*yspace);
		}
	}
// init the rest
	setdim();
	calcpayoff();
	focus=0;
}

void gametheoryproc()
{
	if (KEY==K_ESCAPE)
		poporchangestate(STATE_MAINMENU);
	if (wininfo.mleftclicks)
		focus=rl->getfocus();
	C8 str[50];
	S32 i,j;
	if (focus) {
		S32 ret=focus->proc();
		if (focus==hsd) {
// change dimension
			if (ret>=0) {
				dim=ret+1;
				sprintf(str,"dim %d",dim);
				txd->settname(str);
				setdim();
			}
		} else if (focus==pb) {
// quit
			if (ret==1)
				poporchangestate(STATE_MAINMENU);
		} else {
// vscls
			for (i=0;i<maxdim;++i)
				if (focus==vs[i])
					break;
			if (i!=maxdim) {
				if (ret>=0) {
					rprobb[i]=(float)ret/maxres;
				}
			} else {
// hscls
				for (i=0;i<maxdim;++i)
					if (focus==hs[i])
						break;
				if (i!=maxdim) {
					if (ret>=0) {
						rproba[i]=(float)ret/maxres;
					}
				} else {
// payoff plus
					for (j=0;j<maxdim;++j) {
						for (i=0;i<maxdim;++i) {
							if (focus==pop[j][i])
								break;
						}
						if (i!=maxdim)
							break;
					}
					if (j!=maxdim) {
						if (ret==1) {
							++payoffm[j][i];
							sprintf(str,"%d",payoffm[j][i]);
							pot[j][i]->settname(str);
						}
					} else {
// payoff minus
						for (j=0;j<maxdim;++j) {
							for (i=0;i<maxdim;++i) {
								if (focus==pom[j][i])
									break;
							}
							if (i!=maxdim)
								break;
						}
						if (j!=maxdim) {
							if (ret==1) {
								--payoffm[j][i];
								sprintf(str,"%d",payoffm[j][i]);
								pot[j][i]->settname(str);
							}
						}
					}
				}
			}
		}
	}
	calcperturba();
	calcperturbb();
	calcproba();
	calcprobb();
	drawproba();
	drawprobb();
	calcpayoff();
	drawpayoff();
}

void gametheorydraw2d()
{
//	video_lock();
	clipclear32(B32,C32BLUE);
	rl->draw();
//	video_unlock();
}

void gametheoryexit()
{
	delete rl;
}

