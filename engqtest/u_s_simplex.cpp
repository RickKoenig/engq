// draw the mainmenu's shape class without doing anything else
#include <m_eng.h>
#include <l_misclibm.h>
#include "u_states.h"

namespace simplex {
shape* rl;
shape* focus;

pbut* pbutquit,*pbutreset,*pbutgame,*pbutbiasp,*pbutbiasz;
pbut* pbutbiasm,*pbutload,*pbutsave,*pbutreduce,*pbutsolve;
const S32 maxdim = 18;
S32 dimx = 18;
S32 dimy = 9;
S32 payx;
S32 payy;
S32 dimxsave,dimysave;
S32 startx=0;
S32 starty=10;
S32 startxp;
S32 startyp;
S32 startxm;
S32 startym;
S32 spacex = 70;
S32 spacey = 50;
pbut* pot[maxdim][maxdim];
pbut* pop[maxdim][maxdim];
pbut* pom[maxdim][maxdim];
pbut* poz[maxdim][maxdim];

hscroll* hsdx;
text* txdx;
hscroll* hsdy;
text* txdy;

enum circol {BLK,RED,YEL,BLU};

circol bestpivots[maxdim][maxdim];

// maximize p
float matorig[maxdim][maxdim] = {
/*	//x  y  z  s  t  u  p  ans
	{ 1, 2, 0, 1, 0, 0, 0, 1},
	{ 0, 1, 2, 0, 1, 0, 0, 1},
	{ 2, 0, 1, 0 ,0, 1, 0, 1},
	{-1,-1,-1, 0, 0, 0, 1, 0}, */

	{ 0, 2,-1,-1,-1,-1,-1,-1},
	{-2, 0, 2,-1,-1,-1,-1,-1},
	{ 1,-2, 0, 2,-1,-1,-1,-1},
	{ 1, 1,-2, 0, 2,-1,-1,-1},
	{ 1, 1, 1,-2, 0, 2,-1,-1},
	{ 1, 1, 1, 1,-2, 0, 2,-1},
	{ 1, 1, 1, 1, 1,-2, 0, 2},
	{ 1, 1, 1, 1, 1, 1,-2, 0},

};

bool saddle[maxdim][maxdim];
// maximize p
float matsave[maxdim][maxdim];
	//x  y  z  s  t  u  p  ans

float mat[maxdim][maxdim];

C8* floatform = "%5.3f";

con32 *acon; // console output

void findbestpivots()
{
	S32 i,j;
	// clear them out
	bool bestcola[maxdim];
	fill(bestcola,bestcola+maxdim,false);
	for (j=0;j<maxdim;++j) {
		for (i=0;i<maxdim;++i) {
			bestpivots[j][i] = BLK;
		}
	}
// phase 1, get onto simplex
	// look for 'starred' rows with negative solutions
	bool starredrowa[maxdim];
	fill(starredrowa,starredrowa+maxdim,false);
	for (i=0;i<dimx-2;++i) { // look for 'basic' solutions
		S32 nonzerocount = 0;
		bool neg = false;
		S32 rw = maxdim;
		for (j=0;j<dimy-1;++j) { // looking for column with all 0's except for 1 negative number
			if (mat[j][i] !=0) {
				++nonzerocount;
				if (mat[j][i]<0) {
					rw = j;
					neg = true;
				}
			}
		}
		if (nonzerocount==1 && neg) {
			starredrowa[rw] = true; // 'starred' row
		}
	}
	for (j=0;j<dimy-1;++j) {
		if (starredrowa[j]) { // first 'starred' row
			break;
		}
	}
	float bestcol = 0;
	if (j!=dimy-1) { // j is first 'starred' row
		for (i=0;i<dimx-2;++i) {
			float v = mat[j][i];
			if (v > bestcol)
				bestcol = v;
		}
		for (i=0;i<dimx-2;++i) { // find largest #
			float v = mat[j][i];
			if (v == bestcol && v>0) {
				bestcola[i] = true;
			}
		}
		// for each bestcol, find best row
		for (i=0;i<dimx-2;++i) {
			if (bestcola[i]) {
				float bestrow = 1e20f;
				for (j=0;j<dimy-1;++j) { // skip 'p' row
					float v = mat[j][i];
					if (v>0) {
						v = mat[j][dimx-1]/v;
						if (v < bestrow) {
							bestrow = v;
						}
					}
				}
				S32 bs = 0; // best in starred row
				S32 bns = 0; // best in non-starred row
				for (j=0;j<dimy-1;++j) { // skip 'p' row
					float v = mat[j][i];
					if (v>0) {
						v = mat[j][dimx-1]/v;
						if (v == bestrow) {
							bestpivots[j][i] = RED;
							if (starredrowa[j])
								++bs;
							else
								++bns;
						}
					}
				}
				if (bs>0 && bns>0) { // choose starred instead of nonstarred if best is in both
					for (j=0;j<dimy-1;++j) { // skip 'p' row
						if (!starredrowa[j]) {
							bestpivots[j][i] = BLU;
						}
					}
				}
			}
		}
		return;
	}
// phase 2, move around on simplex
	// find best column, skip 'p' or 'ans'
//	bestcol = 0;
//	for (i=0;i<dimx-2;++i) {
//		float v = mat[dimy-1][i];
//		if (v < bestcol) // look for neg in bottom row
//			bestcol = v;
//	}
	// find best column, skip 'p' or 'ans'
/*	for (i=0;i<dimx-2;++i) { // skip 'p' or 'ans'
		float v = mat[dimy-1][i];
		if (v < 0) { // look for neg in bottom row
//		if (v == bestcol) {
			bestcola[i] = true;
		}
	} */
	// for each bestcol, find best row
	for (i=0;i<dimx-2;++i) {
		if (mat[dimy-1][i]<0) { // look for neg in bottom row
//		if (bestcola[i]) {
			float bestrow = 1e20f;
			for (j=0;j<dimy-1;++j) { // skip 'p' row
				float v = mat[j][i];
				if (v>0) {
					v = mat[j][dimx-1]/v; // find lowest ans/v
					if (v < bestrow) {
						bestrow = v;
					}
				}
			}
			for (j=0;j<dimy-1;++j) { // skip 'p' row
				float v = mat[j][i];
				if (v>0) {
					v = mat[j][dimx-1]/v;
					if (v == bestrow) {
						bestpivots[j][i] = YEL;
					}
				}
			}
		}
	}
}

void updatenumbers()
{
	S32 i,j;
	for (j=0;j<maxdim;++j) {
		for (i=0;i<maxdim;++i) {
			float v = mat[j][i];
			C8 str[50];
			sprintf(str,floatform,v);
			pot[j][i]->settname(str);
			if (v>EPSILON) {
				pot[j][i]->setcolor(C32GREEN);
			} else if (v<-EPSILON) {
				pot[j][i]->setcolor(C32RED);
			} else {
				pot[j][i]->setcolor(C32BLACK);
			}
		}
	}
	findbestpivots();
}

void dopivot(S32 c,S32 r)
{
	S32 i,j;
	float p = mat[r][c];
	// check for bad pivot, largest ones are better
	if (abs(p)<=EPSILON) {
		con32_printf(acon,"Can't pivot at column %d, row %d\n",c,r);
		return;
	}
	con32_printf(acon,"pivot at column %d, row %d\n",c,r);
	// first scale the pivot to 1
	float inv = 1.0f/p;
	for (i=0;i<dimx;++i)
		mat[r][i] *= inv;
	mat[r][c] = 1.0f;
	// now for each non pivot row, add the right amount to to make mat[j][c] == 0
	for (j=0;j<dimy;++j) {
		if (j==r)
			continue;
		float add = -mat[j][c];
		for (i=0;i<dimx;++i)
			mat[j][i] += add*mat[r][i];
		mat[j][c] = 0;
	}
	updatenumbers();
}

void setdim()
{
	S32 i,j;
	C8 str[50];
	sprintf(str,"dimx %d",dimx);
	txdx->settname(str);
	sprintf(str,"dimy %d",dimy);
	txdy->settname(str);
	hsdx->setidx(dimx-1);
	hsdy->setidx(dimy-1);
	for (j=0;j<maxdim;++j) {
		for (i=0;i<maxdim;++i) {
			S32 vis=i<dimx && j<dimy;
			pot[j][i]->setvis(vis);
			pop[j][i]->setvis(vis);
			pom[j][i]->setvis(vis);
			poz[j][i]->setvis(vis);
		}
	}
	con32_printf(acon,"%d columns, %d rows\n",dimx,dimy);
	payx = dimx - dimy - 1;
	payy = dimy - 1;
	updatenumbers();
}

void getorig()
{
	S32 i,j;
	for (j=0;j<maxdim;++j) {
		for (i=0;i<maxdim;++i) {
			mat[j][i]=matorig[j][i];
		}
	}
	dimx = 18;
	dimy = 9;
	setdim();
}

void doload()
{
	dimx = dimxsave;
	dimy = dimysave;
	S32 i,j;
	for (j=0;j<maxdim;++j) {
		for (i=0;i<maxdim;++i) {
			mat[j][i]=matsave[j][i];
		}
	}
	setdim();
	updatenumbers();
}

void dosave()
{
	dimxsave = dimx;
	dimysave = dimy;
	S32 i,j;
	for (j=0;j<maxdim;++j) {
		for (i=0;i<maxdim;++i) {
			matsave[j][i]=mat[j][i];
		}
	}
}

// for now a square payoff matrix
bool isgamedim()
{
	return payx>=1 && payy>=1;
}

void dobiasp()
{
	if (!isgamedim()) {
		con32_printf(acon,"Can't payoff plus, bad dimensions\n");
		return;
	}
	S32 i,j;
	for (j=0;j<payy;++j) {
		for (i=0;i<payx;++i) {
			mat[j][i] += 1.0f;
		}
	}
	updatenumbers();
}

void dobiasm()
{
	if (!isgamedim()) {
		con32_printf(acon,"Can't payoff minus, bad dimensions\n");
		return;
	}
	S32 i,j;
	for (j=0;j<payy;++j) {
		for (i=0;i<payx;++i) {
			mat[j][i] -= 1.0f;
		}
	}
	updatenumbers();
}

void dobiasz()
{
	if (!isgamedim()) {
		con32_printf(acon,"Can't payoff 0, bad dimensions\n");
		return;
	}
	S32 i,j;
	for (j=0;j<payy;++j) {
		for (i=0;i<payx;++i) {
			mat[j][i] = 0;
		}
	}
	updatenumbers();
}

void removerow(S32 r)
{
	if (r >= dimy)
		return;
	con32_printf(acon,"Removing row %d\n",r);
	S32 i,j;
	for (j=r;j<dimy-1;++j) {
		for (i=0;i<dimx;++i) {
			mat[j][i] = mat[j+1][i];
		}
	}
	--dimy;
	setdim();
}

void removecolumn(S32 c)
{
	if (c >= dimx)
		return;
	con32_printf(acon,"Removing column %d\n",c);
	S32 i,j;
	for (j=0;j<dimy;++j) {
		for (i=c;i<dimx-1;++i) {
			mat[j][i] = mat[j][i+1];
		}
	}
	--dimx;
	setdim();
}

void getgame()
{
	if (!isgamedim()) {
		con32_printf(acon,"Can't reduce, bad dimensions\n");
		return;
	} 
	S32 i,j;
	// everything except payoff
	for (j=0;j<dimy;++j) {
		for (i=0;i<dimx;++i) {
			if (i>=payx || j>=payy)
				mat[j][i] = 0;
		}
	}
	// ans
	for (j=0;j<payy;++j)
		mat[j][dimx-1]=1;
	// p
	mat[dimy-1][dimx-2]=1;
	// slack
	for (j=0;j<payy;++j)
		mat[j][payx+j]=1;
	// -1's
	for (i=0;i<payx;++i)
		mat[payy][i]=-1;
	updatenumbers();
}

void doreduce()
{
	if (!isgamedim()) {
		con32_printf(acon,"Can't reduce, bad dimensions\n");
		return;
	} 
	// clear saddle points
	S32 i,j;
	bool maxcol[maxdim][maxdim];
	bool minrow[maxdim][maxdim];
	for (j=0;j<dimy;++j) {
		for (i=0;i<dimx;++i) {
			maxcol[j][i] = false;
			minrow[j][i] = false;
			saddle[j][i] = false;
		}
	}
	// check for row dowminance
	for (j=0;j<payy;++j) {
		S32 j2;
		for (j2=0;j2<payy;++j2) {
			if (j == j2)
				continue;
			for (i=0;i<payx;++i) {
				if (mat[j][i] < mat[j2][i]) {
					break; // nope
				}
			}
			if (i == payx) { // yes
				con32_printf(acon,"row %d dominates over row %d\n",j,j2);
				removerow(j2);
				removecolumn(payx); // remove a slacker column too.
				getgame();
				return;
			}
		}
	}
	con32_printf(acon,"no row dominance\n");
	// check for column dominance
	for (i=0;j<payx;++i) {
		S32 i2;
		for (i2=0;i2<payx;++i2) {
			if (i == i2)
				continue;
			for (j=0;j<payy;++j) {
				if (mat[j][i] > mat[j][i2]) {
					break; // nope
				}
			}
			if (j == payy) { // yes
				con32_printf(acon,"col %d dominates over col %d\n",i,i2);
				removecolumn(i2);
				getgame();
				return;
			}
		}
	}
	con32_printf(acon,"no column dominance\n");
	// check for saddle points, lowest row, highest column
	float barr;
	// min row
	for (j=0;j<payy;++j) {
		barr = 1e20f;
		for (i=0;i<payx;++i) {
			if (mat[j][i] < barr) {
				barr = mat[j][i];
			}
		}
		for (i=0;i<payx;++i) {
			if (mat[j][i] == barr) {
				minrow[j][i] = true;
			}
		}
	}
	// max col
	for (i=0;i<payx;++i) {
		barr = -1e20f;
		for (j=0;j<payy;++j) {
			if (mat[j][i] > barr) {
				barr = mat[j][i];
			}
		}
		for (j=0;j<payy;++j) {
			if (mat[j][i] == barr) {
				maxcol[j][i] = true;
			}
		}
	}
	// both
	bool foundsaddle = false;
	for (j=0;j<payy;++j) {
		for (i=0;i<payx;++i) {
			if (maxcol[j][i] && minrow[j][i]) {
				con32_printf(acon,"saddle at row %d, col %d\n",j,i);
				foundsaddle = true;
				saddle[j][i] = true;
			}
		}
	}
	if (foundsaddle)
		return;
	else
		con32_printf(acon,"no saddle points\n");
}

S32 checkbasiccol(S32 c)
{
	S32 nzcount = 0;
	S32 onecount = 0;
	S32 ol = 0;
	S32 j;
	for (j=0;j<dimy;++j) {
		if (mat[j][c] != 0.0f)
			++nzcount;
		if (mat[j][c] == 1.0f) {
			++onecount;
			ol = j;
		}
	}
	if (onecount == 1 && nzcount == 1)
		return ol;
	return -1;
}

void dosolve()
{
	if (!isgamedim()) {
		con32_printf(acon,"Can't solve, bad dimensions\n");
		return;
	} 
	// show current answer
	// look for pivoted columns
	S32 i;
	// main vars
//	S32 mvc = 0;
	S32 br = 0;
	for (i=0;i<payx;++i) {
		br = checkbasiccol(i);
		if (br != -1) {
			// ++mvc;
			con32_printf(acon,"V%d = %7.4f/%7.4f = %7.4f\n",i,mat[br][dimx-1],mat[br][i],mat[br][dimx-1]/mat[br][i]);
		}
	}
	// slack vars
	for (i=0;i<payx;++i) {
		br = checkbasiccol(i+payx);
		if (br != -1) {
			con32_printf(acon,"S%d = %7.4f/%7.4f = %7.4f\n",i,mat[br][dimx-1],mat[br][i+payx],mat[br][dimx-1]/mat[br][i+payx]);
		}
	}
	if (/*mvc == payx && */ mat[dimy-1][dimx-1] > 0.0f) {
		con32_printf(acon,"Normalized solution: e = %7.4f\n",1.0f/mat[dimy-1][dimx-1]);
		for (i=0;i<payx;++i) {
			br = checkbasiccol(i);
			if (br != -1) {
				con32_printf(acon,"P%d = %7.4f\n",i,mat[br][dimx-1]/mat[br][i]/mat[dimy-1][dimx-1]);
			}
		}
	}
}

} // end namespace simplex
using namespace simplex;

void simplexinit()
{
	video_setupwindow(1280-40,1024-200);
	acon=con32_alloc(480,200,C32BLACK,C32WHITE);
	con32_printf(acon,"Simplex method.\n");
	S32 i,j;
	C8 str[50];

	pushandsetdir("simplex");
	rl=res_loadfile("simplexres.txt");
	popdir();
	for (j=0;j<maxdim;++j) {
		for (i=0;i<maxdim;++i) {
			pbut* b;
			sprintf(str,"PBUT%d%dp",i,j);
			b = new pbut(str,i*spacex+5,j*spacey+10,10,10,"-");
			pom[j][i]=b;
			rl->addchild(b);
			sprintf(str,"PBUT%d%d",i,j);
			b = new pbut(str,i*spacex,j*spacey+25,50,10,".");
			pot[j][i]=b;
			rl->addchild(b);
			sprintf(str,"PBUT%d%dm",i,j);
			b = new pbut(str,i*spacex+35,j*spacey+10,10,10,"+");
			pop[j][i]=b;
			rl->addchild(b);
			sprintf(str,"PBUT%d%dz",i,j);
			b = new pbut(str,i*spacex+20,j*spacey+10,10,10,"0");
			poz[j][i]=b;
			rl->addchild(b);
		}
	}

	pbutquit=rl->find<pbut>("PBUTQUIT");
	pbutreset=rl->find<pbut>("PBUTRESET");
	pbutgame=rl->find<pbut>("PBUTGAME");
	pbutbiasp=rl->find<pbut>("PBUTBIASP");
	pbutbiasm=rl->find<pbut>("PBUTBIASM");
	pbutbiasz=rl->find<pbut>("PBUTBIASZ");
	pbutload=rl->find<pbut>("PBUTLOAD");
	pbutsave=rl->find<pbut>("PBUTSAVE");
	pbutreduce=rl->find<pbut>("PBUTREDUCE");
	pbutsolve=rl->find<pbut>("PBUTSOLVE");
	hsdx=rl->find<hscroll>("SCLHDX");
	hsdx->setnumidx(maxdim);
	hsdx->setidx(dimx-1);
	hsdy=rl->find<hscroll>("SCLHDY");
	hsdy->setnumidx(9);
	hsdy->setidx(dimy-1);
	txdx=rl->find<text>("TEXTDX");
	sprintf(str,"dimx %d",hsdx->getidx()+1);
	txdx->settname(str);
	txdy=rl->find<text>("TEXTDY");
	sprintf(str,"dimy %d",hsdy->getidx()+1);
	txdy->settname(str);
// init the rest
	getorig();
	getgame();
	dosave();
//	setdim();
	focus=0;
}

void simplexproc()
{
	if (KEY==K_ESCAPE)
		poporchangestate(STATE_MAINMENU);
	if (wininfo.mleftclicks)
		focus=rl->getfocus();
	S32 i,j;
	if (focus) {
		S32 ret=focus->proc();
		if (focus==hsdx) {
// change dimension
			if (ret>=0) {
				if (dimx!=ret+1) {
					dimx=ret+1;
					setdim();
				}
			}
		} else if (focus==hsdy) {
// change dimension
			if (ret>=0) {
				if (dimy!=ret+1) {
					dimy=ret+1;
					setdim();
				}
			}
// quit
		} else if (focus==pbutquit) {
			if (ret==1)
				poporchangestate(STATE_MAINMENU);
// reset
		} else if (focus==pbutreset) {
			if (ret==1) {
				con32_printf(acon,"==Reset\n");
				getorig();
			}
// load
		} else if (focus==pbutload) {
			if (ret==1) {
				con32_printf(acon,"== Load\n");
				doload();
			}
// save
		} else if (focus==pbutsave) {
			if (ret==1) {
				con32_printf(acon,"== Save\n");
				dosave();
			}
// bias plus
		} else if (focus==pbutbiasp) {
			if (ret==1) {
				con32_printf(acon,"== Payoff plus\n");
				dobiasp();
			}
// bias minus
		} else if (focus==pbutbiasm) {
			if (ret==1) {
				con32_printf(acon,"== Payoff minus\n");
				dobiasm();
			}
// bias 0
		} else if (focus==pbutbiasz) {
			if (ret==1) {
				con32_printf(acon,"== Payoff zero\n");
				dobiasz();
			}
// reduce
		} else if (focus==pbutreduce) {
			if (ret==1) {
				con32_printf(acon,"== Reduce\n");
				doreduce();
			}
// solve
		} else if (focus==pbutsolve) {
			if (ret==1) {
				con32_printf(acon,"== Solve\n");
				dosolve();
			}
// game theory
		} else if (focus==pbutgame) {
			if (ret==1) {
				con32_printf(acon,"== Game Theory\n");
				getgame();
			}
// mat plus
		} else {
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
					++mat[j][i];
					updatenumbers();
				}
			} else {
// mat minus
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
						--mat[j][i];
						updatenumbers();
					}
				} else {
// pivot number
					for (j=0;j<maxdim;++j) {
						for (i=0;i<maxdim;++i) {
							if (focus==pot[j][i])
								break;
						}
						if (i!=maxdim)
							break;
					}
					if (j!=maxdim) {
						if (ret==1) {
							dopivot(i,j);
						}
					} else {
// mat zero
						for (j=0;j<maxdim;++j) {
							for (i=0;i<maxdim;++i) {
								if (focus==poz[j][i])
									break;
							}
							if (i!=maxdim)
								break;
						}
						if (j!=maxdim) {
							if (ret==1) {
								mat[j][i] = 0;
								updatenumbers();
							}
						}
					}
				}
			}
		}
	}
}

void simplexdraw2d()
{
	clipclear32(B32,C32BLUE);
	clipline32(B32,startxm-4+dimx,starty+(dimy-1)*spacey-16,startxm+spacex*dimx+4,starty+(dimy-1)*spacey-16,C32WHITE);
	clipline32(B32,startxm-4+spacex*(dimx-1)-2,starty-16,startxm-4+spacex*(dimx-1)-2,starty-16+dimy*spacey,C32WHITE);
	clipline32(B32,startxm-4+spacex*(dimx-2)-2,starty-16,startxm-4+spacex*(dimx-2)-2,starty-16+dimy*spacey,C32WHITE);
	if (isgamedim()) {
		clipline32(B32,startxm-4+spacex*payx-4,starty-16,startxm-4+spacex*payx-4,starty-16+dimy*spacey,C32GREEN);
	}
	rl->draw();
	bitmap32* cbm=con32_getbitmap32(acon);
	clipblit32(cbm,B32,0,0,20,WY*12/20,cbm->size.x,cbm->size.y);
	S32 i,j;
	for (j=0;j<dimy;++j) {
		for (i=0;i<dimx;++i) {
			if (bestpivots[j][i]) {
				C32 col;
				if (bestpivots[j][i]==BLK)
					col = C32BLACK;
				else if (bestpivots[j][i]==RED)
					col = C32RED;
				else if (bestpivots[j][i]==BLU)
					col = C32LIGHTBLUE;
				else
					col = C32YELLOW;
				if (col!=C32BLACK)
					clipcircle32(B32,startx+i*spacex+32,starty+j*spacey-6,5,col);
			}
			if (saddle[j][i]) {
				clipcircle32(B32,startx+i*spacex+12,starty+j*spacey-6,5,C32WHITE);
			}
		}
	}
}

void simplexexit()
{
	delete rl;
	con32_free(acon);
	acon = 0;
}
