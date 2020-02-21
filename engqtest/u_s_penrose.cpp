/// draw lots of stuff
#define D2_3D
#include <m_eng.h>
#include "u_states.h"

#include <d3d9.h>
#include "d2_dx9.h" // for seetmodel3version // temporary
#include "u_modelutil.h"
#define DONAMESPACE
#ifdef DONAMESPACE
namespace penrose {
#endif
U32 togvidmode;

const C8* penname = "penrosedata.bin";

////////////////////////// main

//static tree2* h;
//static S32 ghrot; // 0 thru 9, the 10 possible rotations of Penrose tiles

float rx,ry; // relative mouse coords
float lrx,lry;
float levx,levy,levd;
float drx,dry;
float rotfact=-3;
//viewport2 mainvppanel;
//S32 sel[2];
S32 done;
S32 mrkcnt;
S32 mrkper = 0;
S32 mrkmul = 10;
float closedist = .2f; // for now, should be positive// should be changed to a tileinfo structure, angle, rules etc.
float bestcurdists;

struct tileinfo {
	static const S32 nverts = 4;
	float fangle; // in degrees
	C8* name;
	S32 notch[nverts];
	float cx,cy; // center
	float sf,cf; // sin and cos of fangle
	pointf2 verts[nverts];
};
const float fara = 72*PIOVER180;
const float tara = 36*PIOVER180;
tileinfo tileinfos[]={
	{fara, "fat.png",{-1,-2, 2, 1}},
	{tara,"thin.png",{ 1,-1, 2,-2}},
};

const S32 ntileinfos = sizeof(tileinfos)/sizeof(tileinfos[0]);
float golden=1.6180339887f;
S32 ntiles;


struct menuvar edv[]={
// user vars
	{"@lightred@--- PLOTTER USER VARS ---",NULL,D_VOID,0},
//	{"sel0",&sel[0],D_INT,1},
//	{"sel1",&sel[1],D_INT,1},
	{"ntiles",&ntiles,D_INT|D_RDONLY,1},
	{"orthosize",&mainvp.ortho_size,D_FLOAT,FLOATUP},
	{"panx",&mainvp.camtrans.x,D_FLOAT,FLOATUP/4},
	{"pany",&mainvp.camtrans.y,D_FLOAT,FLOATUP/4},
	{"rotfact",&rotfact,D_FLOAT,FLOATUP/4},
	{"mrkper",&mrkper,D_INT,1},
	{"mrkmul",&mrkmul,D_INT,1},
	{"closedist",&closedist,D_FLOAT,FLOATUP/4},//line2btricull
	{"bestcurdists",&bestcurdists,D_FLOAT|D_RDONLY,FLOATUP/4},
	{"rx",&rx,D_FLOAT|D_RDONLY,FLOATUP},
	{"ry",&ry,D_FLOAT|D_RDONLY,FLOATUP},
	{"done",&done,D_INT|D_RDONLY,1},
	{"drx",&drx,D_FLOAT|D_RDONLY,FLOATUP},
	{"dry",&dry,D_FLOAT|D_RDONLY,FLOATUP},
	{"levx",&levx,D_FLOAT|D_RDONLY,FLOATUP},
	{"levy",&levy,D_FLOAT|D_RDONLY,FLOATUP},
	{"levd",&levd,D_FLOAT|D_RDONLY,FLOATUP},
};
const int nedv=sizeof(edv)/sizeof(edv[0]);

struct tile {
	U32 kind; // 0 fat, 1 thin
//	S32 hrot; // in degrees
	tree2* t;
	bool mark;
	bool check;
};

vector<tile> tiles;
vector<tile> prototiles;
S32 curtile;
pointf2 hand;
float handrot;

tree2* roottree;//,*roottree2;
tree2* panel;
//tree2* spawn0,*spawn1;
//tree2* prototiles[NTILES];

void clamprot(float& r)
{
	while(r >= TWOPI)
		r -= TWOPI;
	while(r < 0)
		r += TWOPI;
}

// rotate a point ccw
pointf2 rotaxis(const pointf2& in,float ang)
{
	pointf2 out;
	float sa = sinf(ang);
	float ca = cosf(ang);
	out.x = ca*in.x - sa*in.y;
	out.y = sa*in.x + ca*in.y;
	return out;
}

void newmark()
{
	S32 j;
	for (j=0;j<(S32)tiles.size();++j) { // uncheck all unmarked tiles
		if (!tiles[j].mark)
			tiles[j].check = false;
	}
}

void clearmarks()
{
	S32 i;
	for (i=0;i<(S32)tiles.size();++i) {
		tiles[i].mark = false;
		tiles[i].check = false;
	}
	done = 0;
	mrkcnt = mrkper;
}

void cleardups()
{
// clear dups
	curtile = -1;
	S32 i,j;
	for (i=0;i<(S32)tiles.size();) {
		tile& ti = tiles[i];
		tree2* t = ti.t;
		for (j=i+1;j<(S32)tiles.size();++j) {
			tile& ti2 = tiles[j];
			tree2* t2 = ti2.t;
			float ra = fabsf(t->rot.z - t2->rot.z);
			float dx = t->trans.x - t2->trans.x;
			float dy = t->trans.y - t2->trans.y;
			if (ti.kind == ti2.kind && ra < PI*(1/20.0f) && dx*dx+dy*dy < closedist*closedist) {
				break;
			}
		}
		if (j != (S32)tiles.size()) {
			delete t;
			tiles.erase(tiles.begin()+i);
			curtile = -1;
		} else {
			++i;
		}
	}
	clearmarks();
}

S32 findclosesttile(const vector<tile>& ts,float fx,float fy)
{
	S32 i,f=-1;
	float bdist = 1e20f;
	for (i=0;i<(S32)ts.size();++i) {
		const tile& t = ts[i];
		pointf2x d(fx - t.t->trans.x,fy - t.t->trans.y);
		float dist = d.x*d.x + d.y*d.y;
		const tileinfo& ti = tileinfos[t.kind];
//		float ar = ti.fangle;
		float har = -t.t->rot.z;
//		float rdx =  dx*cohar + dy*sihar;
//		float rdy = -dx*sihar + dy*cohar;
		pointf2 rd = rotaxis(d,har);
		float miny = -ti.sf;
		float xy = ti.cf/ti.sf;
		//1.0f/tanf(ar);
		if (rd.x+xy*rd.y >= 0 && rd.x+xy*rd.y< 1 && rd.y>=miny && rd.y< 0 && dist < bdist) {
			bdist = dist;
			f = i;
		}
	}
	return f;
}

void addprototile(S32 kind,float posx,float posy)
{
	tree2* r;
	const C8* modelname = tileinfos[kind].name;
	r = new tree2(modelname);
	modelb* modb=model_create(modelname);
	if (model_getrc(modb)==1) {
	    buildpatch(modb,1,1,.9375,.5,planexy_surf0(1.875f,1.0f),modelname,"tex"); // protect against texture wrap
	}
	r->setmodel(modb);
	modb->mats[0].msflags |= SMAT_CLAMPU|SMAT_CLAMPV;
	r->trans.z = .5f;
	//if (kind != 0)
	//	r->flags |= TF_DONTDRAW;
	roottree->linkchild(r);
	tile ret;
	ret.t = r;
	//ret.hrot = 0;
	ret.kind = kind;
	ret.mark = false;
	ret.check = false;
	prototiles.push_back(ret);
}

void addtile(S32 kind,float posx,float posy,float rotz)
{
	tree2* t = prototiles[kind].t->newdup();
	t->trans.x = posx;
	t->trans.y = posy;
	t->trans.z = 0;
	t->rot.z = rotz;
	//t->flags &= ~TF_DONTDRAW;
	roottree->linkchild(t);
	tile ret;
	ret.t = t;
	ret.kind = kind;
	ret.mark = false;
	ret.check = false;
	//ret.hrot = 0;
	tiles.push_back(ret);
}

void buildpanel()
{
	tree2* r;
	r = new tree2("panel");
	modelb* modb=model_create("panel");
	if (model_getrc(modb)==1) {
	    buildpatch(modb,1,1,1,1,planexy_surf0(1,1),0,"tex");
	}
	r->setmodel(modb);
	modb->mats[0].msflags |= SMAT_CLAMPU|SMAT_CLAMPV;
	r->trans.z = 1;
	r->trans.x = 2.0f*4.0f/3.0f-.5f-.05f;
	r->trans.y = 1.95f;
	r->treecolor.w = .5f;
	roottree->linkchild(r);
	panel = r;
}

bool checkclose(S32 ta,S32 tb,S32& ta0,S32& ta1,S32& tb0,S32& tb1)
{
	//return false;
	// logger("checking %d against %d\n",ta,tb);
	pointf2 ips[4],jps[4];
	S32 i,j;
	const tileinfo& tia = tileinfos[tiles[ta].kind];
	for (i=0;i<4;++i) { // ta
		ips[i] = rotaxis(tia.verts[i],tiles[ta].t->rot.z);
		ips[i].x += tiles[ta].t->trans.x;
		ips[i].y += tiles[ta].t->trans.y;
		// logger("pr0[%d] = (%f,%f)\n",i,ips[i].x,ips[i].y);
	}
	const tileinfo& tib = tileinfos[tiles[tb].kind];
	for (i=0;i<4;++i) { // tb
		jps[i] = rotaxis(tib.verts[i],tiles[tb].t->rot.z);
		jps[i].x += tiles[tb].t->trans.x;
		jps[i].y += tiles[tb].t->trans.y;
		// logger("pr1[%d] = (%f,%f)\n",i,jps[i].x,jps[i].y);
	}
// check pairs of verts
	bestcurdists = 1e20f;
	for (j=0;j<4;++j) {
		S32 j0 = j;
		S32 notchj = tib.notch[j0];
		S32 j1 = (j+1)%4;
		for (i=0;i<4;++i) {
			S32 i0 = i; // backwards
			S32 i1 = (i+1)%4;
			S32 notchi = tia.notch[i0];
			if (notchi + notchj)
				continue;
			float dx0 = ips[i1].x - jps[j0].x;
			float dy0 = ips[i1].y - jps[j0].y;
			float dx1 = ips[i0].x - jps[j1].x;
			float dy1 = ips[i0].y - jps[j1].y;
			float d0s = dx0*dx0 + dy0*dy0;
			float d1s = dx1*dx1 + dy1*dy1;
			float dss = max(d0s,d1s);
			if (dss < bestcurdists) {
				bestcurdists = dss;
				ta0 = i0;
				ta1 = i1;
				tb0 = j0;
				tb1 = j1;
			}
		}
	}
	return bestcurdists < closedist*closedist;
}

//closedist
void moveclose(S32 a,S32 b,S32 ta0,S32 tb1)
{
	const float& zi = tiles[a].t->rot.z;
	float& zj = tiles[b].t->rot.z;
	float dr = zj - zi;
	clamprot(dr); // 0 to 2PI
	dr *= 10.0f/TWOPI; // 0 to 10, ten settings
	dr += .5f; // round to nearest
	dr = floor(dr);
	dr *= TWOPI/10.0f; // back to radians
	zj = dr + zi; // modify object 'j'
	clamprot(zj);
	// calc pos of obj 'a'
	pointf2 pa = tileinfos[tiles[a].kind].verts[ta0];
	pointf2 pat = rotaxis(pa,tiles[a].t->rot.z);
	pat.x += tiles[a].t->trans.x;
	pat.y += tiles[a].t->trans.y;
	//logger("pat = (%f,%f)\n",pat.x,pat.y);
	// calc pos of obj 'b'
	pointf2 pb = tileinfos[tiles[b].kind].verts[tb1];
	pointf2 pbt = rotaxis(pb,tiles[b].t->rot.z);
	pbt.x += tiles[b].t->trans.x;
	pbt.y += tiles[b].t->trans.y;
	// logger("pbt = (%f,%f)\n",pbt.x,pbt.y);
	tiles[b].t->trans.x += (pat.x - pbt.x);
	tiles[b].t->trans.y += (pat.y - pbt.y);
	//zi = 1;
	//zj = 2;
}

void procmarks()
{
	if (done)
		return;
	--mrkcnt;
	if (mrkcnt >= 0)
		return;
	mrkcnt = mrkper;
	if (tiles.empty())
		return;
	if (!tiles[0].check) { // start with 1 marked tile
		tiles[0].check = true;
		tiles[0].mark = true;
		return;
	}
	S32 i,j;
	for (j=0;j<(S32)tiles.size();++j) { // find an unchecked tile
		if (tiles[j].check)
			continue;
		for (i=0;i<(S32)tiles.size();++i) { // compare against marked tiles
			if (!tiles[i].check)
				continue;
			if (!tiles[i].mark)
				continue;
			S32 ta0,ta1,tb0,tb1;
			if (!checkclose(i,j,ta0,ta1,tb0,tb1))
				continue;
			else {
				moveclose(i,j,ta0,tb1); // move unmarked tile along side of marked one,
				tiles[j].mark = true; // and mark it
				newmark();
				tiles[j].check = true;
				return;
			}
		}
		tiles[j].check = true; // this unmarked tile is not close to any marked tiles, mark it
		return;
	}
	done = true;
}

void calctileinfos()
{
	S32 i;
	for (i=0;i<ntileinfos;++i) {
		tileinfo& t = tileinfos[i];
		t.sf = sinf(t.fangle);
		t.cf = cosf(t.fangle);
		t.verts[0].x = 0;
		t.verts[0].y = 0;
		t.verts[1].x = 1;
		t.verts[1].y = 0;
		t.verts[3].x = t.cf;
		t.verts[3].y = -t.sf;
		t.verts[2].x = t.verts[1].x + t.verts[3].x;
		t.verts[2].y = t.verts[1].y + t.verts[3].y;
		t.cx = t.verts[2].x*.5f;
		t.cy = t.verts[2].y*.5f;
	}
}

void movetile()
{
	if (curtile<0)
		return;
	tile& ct = tiles[curtile];
	tree2* h = ct.t;
//	float cx = tileinfos[ct.kind].cx;
//	float cy = tileinfos[ct.kind].cy;
	pointf2x c(tileinfos[ct.kind].cx,tileinfos[ct.kind].cy);
	float a = h->rot.z;
//	float a = -(ct.hrot)*PIOVER180;
//	float sa = sinf(a);
//	float ca = cosf(a);
//	float rcx =  ca*cx + sa*cy;
//	float rcy = -sa*cx + ca*cy;
	pointf2 rc = rotaxis(c,a);
	rc.x += h->trans.x;
	rc.y += h->trans.y;
	levx = rx - rc.x;
	levy = ry - rc.y;
	levd = sqrtf(levx*levx+levy*levy);
	drx = rx - lrx;
	dry = ry - lry;
	float crs = drx*levy - dry*levx;
	h->rot.z += crs * rotfact;
//	ct.hrot += (S32)(crs*rotfact);
	if (KEY==']') {
		h->rot.z += 36*PIOVER180;
	}
	if (KEY=='[') {
		h->rot.z -= 36*PIOVER180;
	}
	if (KEY=='r') {
		h->rot.z = 0;
	}
	if (KEY=='z') {
		h->trans.x = h->trans.y = 0;
		curtile = -1;
		return;
	}
	clamprot(h->rot.z);
	/* if (rx!=lrx || ry!=lry) {
		ct.hrot += 5;
		if (ct.hrot >= 360) {
			ct.hrot -= 360;
		}
	} */
	a = h->rot.z-handrot;
//	sa = sinf(a);
//	ca = cosf(a);
//	float rhandx =  ca*handx + sa*handy;
//	float rhandy = -sa*handx + ca*handy;
	pointf2 rhand = rotaxis(hand,a);
	h->trans.x = rx + rhand.x;
	h->trans.y = ry + rhand.y;
	//h->rot.z = float(ct.hrot)*PIOVER180;
	lrx = rx;
	lry = ry;
}

void deflation()
{
	vector<tile> oldtiles = tiles;
	tiles.clear();
	U32 i;
	for (i=0;i<oldtiles.size();++i) {
		const tile& ot = oldtiles[i];
		const tree2* t = ot.t;
		int k = ot.kind;
		float x = t->trans.x * golden;
		float y = t->trans.y * golden;
		float r = t->rot.z;
		//addtile(k,x,y,r);
		if (k == 0) { // fat adds 5 tiles
			pointf2x pi(1+golden,0);
			pointf2 po = rotaxis(pi,r-PI/5.0f*1);
			float nr = r + PI;
			clamprot(nr);
			addtile(0,x+po.x,y+po.y,nr);

			pi = pointf2x(golden,0);
			po = rotaxis(pi,r);
			nr = r + PI + PI/5.0f;
			clamprot(nr);
			addtile(0,x+po.x,y+po.y,nr);

			pi = pointf2x(golden,0);
			po = rotaxis(pi,r - PI/5.0f*2);
			nr = r + PI - PI/5.0f;
			clamprot(nr);
			addtile(0,x+po.x,y+po.y,nr);

			pi = pointf2x(1,0);
			po = rotaxis(pi,r - PI/5.0f*1);
			nr = r + PI/5.0f;
			clamprot(nr);
			addtile(1,x+po.x,y+po.y,nr);

			pi = pointf2x(1+golden,0);
			po = rotaxis(pi,r - PI/5.0f*2);
			nr = r + PI/5.0f*3;
			clamprot(nr);
			addtile(1,x+po.x,y+po.y,nr);
		} else {
			pointf2x pi(0,0);
			pointf2 po = rotaxis(pi,0);
			float nr = r + PI/5.0f;
			clamprot(nr);
			addtile(0,x+po.x,y+po.y,nr);

			pi = pointf2x(golden,0);
			po = rotaxis(pi,r);
			pointf2 po2 = rotaxis(pi,r - PI/5);
			po.x += po2.x;
			po.y += po2.y;
			nr = r + PI;
			clamprot(nr);
			addtile(0,x+po.x,y+po.y,nr);

			pi = pointf2x(golden,0);
			po = rotaxis(pi,r);
			nr = r - PI/5.0f*3;
			clamprot(nr);
			addtile(1,x+po.x,y+po.y,nr);

			pi = pointf2x(golden,0);
			po = rotaxis(pi,r);
			pi = pointf2x(1,0);
			po2 = rotaxis(pi,r - PI/5.0f*2);
			po.x += po2.x;
			po.y += po2.y;
			po2 = rotaxis(pi,r - PI/5.0f*3);
			po.x += po2.x;
			po.y += po2.y;
			nr = r - PI/5.0f*2 + PI;
			clamprot(nr);
			addtile(1,x+po.x,y+po.y,nr);
		}
		delete oldtiles[i].t;
	}
	cleardups();
	clearmarks();
}

void cleartiles()
{
	U32 i;
	for (i=0;i<tiles.size();++i)
		delete tiles[i].t;
	tiles.clear();
	curtile = -1;
	clearmarks();
}

#if 0
void addtile(S32 kind,float posx,float posy,float rotz)
{
	tree2* t = prototiles[kind].t->newdup();
	t->trans.x = posx;
	t->trans.y = posy;
	t->trans.z = 0;
	t->rot.z = rotz;
	roottree->linkchild(t);
	tile ret;
	ret.t = t;
	ret.kind = kind;
	ret.mark = false;
	ret.check = false;
	//ret.hrot = 0;
	tiles.push_back(ret);
}
#endif

#ifdef DONAMESPACE
}
using namespace penrose;
#endif

void penrose_init()
{
	S32 i;
	calctileinfos();
	pushandsetdir("penrose");
	lightinfo.dodefaultlights = true;
	lightinfo.uselights = false;
	video_setupwindow(GX,GY);
	roottree=new tree2("roottree");
// build proto tiles for panel
	for (i=0;i<ntileinfos;++i) {
		addprototile(i,0,0);
	}
// add some real tiles
	if (fileexist(penname)) {
		FILE* fr = fopen2(penname,"rb");
		fread(&mainvp.ortho_size,sizeof(mainvp.ortho_size),1,fr);
		fread(&mainvp.camtrans.x,sizeof(mainvp.camtrans.x),1,fr);
		fread(&mainvp.camtrans.y,sizeof(mainvp.camtrans.y),1,fr);
		fread(&rotfact,sizeof(rotfact),1,fr);
		fread(&mrkper,sizeof(mrkper),1,fr);
		fread(&mrkmul,sizeof(mrkmul),1,fr);
		fread(&closedist,sizeof(closedist),1,fr);
//		S32 i;
		S32 tsize;
		fread(&tsize,sizeof(tsize),1,fr);
		for (i=0;i<tsize;++i) {
			S32 knd;
			float rx,ry,rr;
			fread(&knd,sizeof(knd),1,fr);
			fread(&rx,sizeof(rx),1,fr);
			fread(&ry,sizeof(ry),1,fr);
			fread(&rr,sizeof(rr),1,fr);
			addtile(knd,rx,ry,rr);
		}
		fclose(fr);
	} else {
		mainvp.ortho_size=12;
		S32 idx = 1;
		float dx = 1;
		float dy = -1*sinf(tileinfos[idx].fangle);
		float dyx = cosf(tileinfos[idx].fangle);
		addtile(0,-2,1,0);
		addtile(idx,-1,1,0);
		addtile(idx,-1+dx,1,0);
		addtile(0,-1+dyx,1+dy,0);
		addtile(idx,-1+dx+dyx,1+dy,0);
	}
	curtile = -1;
	clearmarks();

// setup viewport
//	mainvp.lookat=0;
	mainvp.isortho=true;
	mainvp.backcolor=C32LIGHTBLUE;
	if(mainvp.isortho) {
		mainvp.zfront=-100;
		mainvp.zback=100;
	} else {
		mainvp.zfront=.25f;
		mainvp.zback=400;
	}
	mainvp.camzoom=1;//3.2f; // it'll getit from tree camattach if you have one
	mainvp.flags=VP_CLEARBG|VP_CLEARWB|VP_CHECKER;
	mainvp.xres=WX;
	mainvp.yres=WY;
	mainvp.xstart=0;
	mainvp.ystart=0;
//	mainvp.camtrans.x = (float)mainvp.xsrc / mainvp.ysrc;
//	mainvppanel = mainvp;
//	mainvppanel.flags=0;
//	mainvppanel.ortho_size=4;
	mainvp.xsrc=4; // aspect ratio
	mainvp.ysrc=3;
	mainvp.useattachcam=false;
// keep track of current video device
	togvidmode=videoinfo.video_maindriver;
	extradebvars(edv,nedv);
	buildpanel();
}

void penrose_proc()
{
	//logger("penrose proc\n");
	rx = float(MX-WX/2)/WX;
	ry = float(WY/2-MY)/WY;
	rx *= float(mainvp.xsrc)/mainvp.ysrc;
	rx *= mainvp.ortho_size;
	ry *= mainvp.ortho_size;
	rx += mainvp.camtrans.x;
	ry += mainvp.camtrans.y;
	if (KEY==K_ESCAPE)
		poporchangestate(STATE_MAINMENU);
	if (KEY=='a')
		showcursor(1);
	if (KEY=='h')
		showcursor(0);
	if (KEY==' ')
		video3dinfo.favorshading^=1;
	if (wininfo.mmiddleclicks) {
		logger("setting vidmode from %d to %d\n",togvidmode,togvidmode^1);
		togvidmode^=1;
		video_init(togvidmode,0);
		changestate(STATE_PENROSE);
	}
	if (KEY=='s') {
		logger("setting vidmode from %d to %d\n",togvidmode,togvidmode^2);
		togvidmode^=2;
		video_init(togvidmode,0);
		changestate(STATE_PENROSE);
	}
	if (KEY=='=') {
		changeglobalxyres(1);
		changestate(STATE_PENROSE);
	}
	if (KEY=='-') {
		changeglobalxyres(-1);
		changestate(STATE_PENROSE);
	}
	if (KEY=='g') {
		deflation();
		mainvp.ortho_size *= golden;
		//mainvp.backcolor.c32 ^= C32BLUE.c32 ^ C32LIGHTBLUE.c32;
	}
	if (KEY=='f') {
		deflation();
	}
	if (KEY=='d') {
		cleardups();
		//mainvp.backcolor.c32 ^= C32BLUE.c32 ^ C32LIGHTBLUE.c32;
	}
	if (KEY=='c') {
		cleartiles();
		//mainvp.backcolor.c32 ^= C32BLUE.c32 ^ C32LIGHTBLUE.c32;
	}
/*	if (KEY=='\'') {
		++curtile;
	if (curtile >= (S32)tiles.size())
			curtile -= tiles.size();
	}
	if (KEY==';') {
		--curtile;
		if (curtile < 0)
			curtile += tiles.size();
	} */
	// update 1 tile with trans mousepos and rot using '[' and ']' keys
/// pickup tile ///
	if (MBUT && !LMBUT) {
		if (rx >= panel->trans.x) {
			S32 cpt = findclosesttile(prototiles,rx,ry);
			if (cpt>=0) {
				//++sel[cpt];
				// ++pancnt;
				tree2* pt = prototiles[cpt].t;
				addtile(cpt,pt->trans.x,pt->trans.y,pt->rot.z);
			}
		}
		curtile = findclosesttile(tiles,rx,ry);
		if (curtile>=0) {
			const tile& t = tiles[curtile];
			hand.x = t.t->trans.x - rx;
			hand.y = t.t->trans.y - ry;
			handrot = t.t->rot.z;
		}
		lrx = rx;
		lry = ry;
	}
/// drop tile ///
	if (!MBUT) {
		if (curtile >= 0) {
			if (rx >= panel->trans.x) { // delete tile
				delete tiles[curtile].t;
				tiles.erase(tiles.begin()+curtile);
				//++pancnt;
			}
		}
		curtile = -1;
	}
/// move tile ///
	S32 i;
	if (curtile>=0) {
		clearmarks();
		movetile();
	} else {
		for (i=0;i<mrkmul;++i)
			procmarks();
	}
	for (i=0;i<(S32)tiles.size();++i) {
		if (tiles[i].mark)
			tiles[i].t->treecolor = pointf3x(1,1,1);
		else
			tiles[i].t->treecolor = pointf3x(1,1,1,.5);
	}
	//doflycam(&mainvp);
	// trans and scale panel and protos
	const float bord = .9875f;
//	const float bord = 1;
	panel->trans.x = mainvp.camtrans.x + .5f*((float)mainvp.xsrc/mainvp.ysrc-.25f)*mainvp.ortho_size*bord;
	panel->trans.y = mainvp.camtrans.y + .5f*mainvp.ortho_size*bord;
	panel->scale.x = mainvp.ortho_size*bord*.125f;
	panel->scale.y = mainvp.ortho_size*bord;
	prototiles[0].t->trans.x = mainvp.camtrans.x + .60f * mainvp.ortho_size -tileinfos[0].cx;
	prototiles[0].t->trans.y = mainvp.camtrans.y +  .4f * mainvp.ortho_size -tileinfos[0].cy;
	prototiles[1].t->trans.x = mainvp.camtrans.x + .60f * mainvp.ortho_size -tileinfos[1].cx;
	prototiles[1].t->trans.y = mainvp.camtrans.y + .25f * mainvp.ortho_size -tileinfos[1].cy;

	roottree->proc();
	ntiles = tiles.size();
}

void penrose_draw3d()
{
//	video_beginscene(); // clear zbuf etc.
	video_setviewport(&mainvp); // clear zbuf etc.
	video_buildworldmats(roottree);
	dolights();
	video_drawscene(roottree);

/*	video_setviewport(&mainvppanel); // clear zbuf etc.
	video_buildworldmats(roottree2);
	dolights();
	video_drawscene(roottree2); */
//	video_endscene(); // nothing right now
}

void penrose_exit()
{
	FILE* fw = fopen(penname,"wb");
	fwrite(&mainvp.ortho_size,sizeof(mainvp.ortho_size),1,fw);
	fwrite(&mainvp.camtrans.x,sizeof(mainvp.camtrans.x),1,fw);
	fwrite(&mainvp.camtrans.y,sizeof(mainvp.camtrans.y),1,fw);
	fwrite(&rotfact,sizeof(rotfact),1,fw);
	fwrite(&mrkper,sizeof(mrkper),1,fw);
	fwrite(&mrkmul,sizeof(mrkmul),1,fw);
	fwrite(&closedist,sizeof(closedist),1,fw);
	S32 i;
	S32 tsize = tiles.size();
	fwrite(&tsize,sizeof(tsize),1,fw);
	for (i=0;i<tsize;++i) {
		tree2* tt = tiles[i].t;
		fwrite(&tiles[i].kind,sizeof(tiles[i].kind),1,fw);
		fwrite(&tt->trans.x,sizeof(tt->trans.x),1,fw);
		fwrite(&tt->trans.y,sizeof(tt->trans.y),1,fw);
		fwrite(&tt->rot.z,sizeof(tt->rot.z),1,fw);
	}
	fclose(fw);
	logger("logging roottree\n");
	roottree->log2();
//	logger("logging roottree2\n");
//	roottree2->log2();
	logger("logging reference lists\n");
	logrc();
//	S32 i;
//	for (i=0;i<NTILES;++i) {
//		delete prototiles[i];//=buildprism(pointf3x(1,1,1),rndtex[i],"tex");
//	}
	delete roottree;
//	delete roottree2;
	tiles.clear();
	prototiles.clear();
	extradebvars(0,0);
	popdir();
}
