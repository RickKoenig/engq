#include <m_eng.h>

#include "u_states.h"

#include "u_modelutil.h"

//#define USENAMESPACE
#ifdef USENAMESPACE
namespace physics2d {
#endif

	// physics demo, simpler, less accurate
void* printshapes,*printnshapes,*printcount; // element handles
string ps = ""; // printnshapes
S32 cntr = 0; // frame counter
float mul = 0; // more if held down for awhile
float aback = .5f; // move everything back to see

// global time
float ptime = 0;
float timestep = 1;
float timemul = 3;
//bool laststep = false;
S32 fpssave;

// global physics parameters

//pointf2 littleg;
pointf2 littleg = pointf2x(0,-.125f);
float vdamp = .995f;//.95; // velocity damp
float rvdamp = .995f; // rot velocity damp

float elast = .65f;//1;//.7;//1;//.5; //1;//.95; //1;
float ustatic = .3f;
float udynamic = .25f;
//float elastfric = true; // conservative friction
bool elastfric = false;

//float norot = true; // make moi infinite
bool norot = false;

// total energies
float penergy = 0;
float kenergy = 0;
float renergy = 0;
float tenergy = 0;

class shape;
vector<shape*> shapes;
U32 maxshapes = 100;
U32 minshapes;
const S32 NRECTPOINTS = 4;

static tree2* roottree,*backgroundtree;

////// helper objects ///////////

tree2* helpertree;
tree2* mastertree;
S32 nhelper;
S32 nmaxhelper;

// init resources
void setupcircle(tree2* t,float x,float y,float rot,float rad);

void initpoints()
{
	helpertree = new tree2("helpertree");
	helpertree->trans.z = aback; // move this in line with background
#if 1
	mastertree = buildplane_xy(pointf2x(1,1),"ball1.png","pdiff");
	mastertree->mod->mats[0].msflags &= ~SMAT_HASWBUFF; // turn off zbuffer
#if 0
	tree2* ht = mastertree->newdup();
	//circle1->trans.x = -.5f;
	setupcircle(ht,(float)WX/2,(float)WY,PI*(1.0f/16.0f),
			6);
	helpertree->linkchild(ht);
#endif
#endif
}

// draw a helper object
void drawpoint(const pointf2& pos,float rad) {
	//float tbr = 2*rad;
#if 1
	tree2* ht = mastertree->newdup();
	//circle1->trans.x = -.5f;
	setupcircle(ht,pos.x,pos.y,PI*(1.0f/16.0f),rad);
	//		6);
	helpertree->linkchild(ht);
#endif
#if 0
	sprite_setsize(tbr,tbr);
	sprite_sethand(.5,.5);
	sprite_setangle(null);
	sprite_draw(pos.x,viewy - pos.y,"ball1.png");
#else
#endif
}

// num points to 0
void resetpoints()
{
	/*
	return;
	list<tree2*>::iterator it;
	for (it = helpertree->children.begin();it != helpertree->children.end(); ++it) {
		tree2* t = *it;
		t->flags |= TF_DONTDRAW;
	} */
	delete helpertree;
	helpertree = new tree2("helpertree");
	helpertree->trans.z = aback; // move this in line with background
	nhelper = 0;
}

// free resources
void exitpoints()
{
	delete helpertree; // current
	delete mastertree; // the template
	nhelper = 0;
	nmaxhelper = 0;
}

//float kind = {"Wall":0,"Plank":1,"Ball":2,"num":3};
enum kind {Wall,Plank,Ball,num};

// collision results
struct ci {
	pointf2 cn; // normal, direction to apply force
	pointf2 cp; // center of collision
	float penm; // amount of penetration
};
ci collinfo;

// handy math
pointf2 vmul2sv(float s,const pointf2& vin)
{
	return pointf2x(s*vin.x,s*vin.y);
}

pointf2 vadd2vv(const pointf2& a,const pointf2& b)
{
	return pointf2x(a.x + b.x,a.y + b.y);
}

pointf2 vsub2vv(const pointf2& a,const pointf2& b)
{
	return pointf2x(a.x - b.x,a.y - b.y);
}

float sdot2vv(const pointf2& a,const pointf2& b)
{
	return a.x*b.x + a.y*b.y;
}

pointf2 vcross2zv(float a,const pointf2& b)
{
	return pointf2x(-a*b.y,a*b.x);
}

float scross2vv(const pointf2& a,const pointf2& b)
{
	return a.x*b.y - a.y*b.x;
}

void fixupcp(pointf2* cp,const pointf2* nrm,float penm) {
	cp->x += nrm->x*penm;
	cp->y += nrm->y*penm;
}

// rotate an array of points
void rotpoints2d(const pointf2 p[],pointf2 pr[],float ang,S32 np)
{
	S32 i;
	float fs=sinf(ang);
	float fc=cosf(ang);
	for (i=0;i<np;++i) {
		pr[i].x = fc*p[i].x - fs*p[i].y;
		pr[i].y = fc*p[i].y + fs*p[i].x;
	}
}


// assume not same point, return 0 to almost 4
float cheapatan2delta(const pointf2& from,const pointf2& to) {
	float dx = to.x - from.x;
	float dy = to.y - from.y;
	float ax = abs(dx);
	float ay = abs(dy);
	float ang = dy/(ax+ay);
	if (dx<0)
		ang = 2 - ang;
	else if (dy<0)
		ang = 4 + ang;
	return ang;
}

// intersection of 2 lines
bool getintersection2d(const pointf2& la,const pointf2& lb,const pointf2& lc,const pointf2& ld,pointf2* i0) {
	float e = lb.x - la.x;
	float f = lc.x - ld.x;
	float g = lc.x - la.x;
	float h = lb.y - la.y;
	float j = lc.y - ld.y;
	float k = lc.y - la.y;
	float det = e*j - f*h;
	if (det == 0)
		return false;
	det = 1/det;
	float t0 = (g*j - f*k)*det;
	float t1 = -(g*h - e*k)*det;
	if (t0>=0 && t0<=1 && t1>=0 && t1<=1) {
		if (i0) {
			i0->x = la.x + (lb.x - la.x)*t0;
			i0->y = la.y + (lb.y - la.y)*t0;
		}
		return true;
	}
	return false;
}

pointf2 vs[NRECTPOINTS];
bool util_point2plank(const pointf2& p,const pointf2 pr[])
{
	//return false;
	S32 i;
	S32 sgn = 0;
	for (i=0;i<NRECTPOINTS;++i) {
		vs[i] = vsub2vv(pr[i],p);
	}
	for (i=0;i<NRECTPOINTS;++i) {
		float c = scross2vv(vs[i],vs[(i + 1)%NRECTPOINTS]);
		if (sgn == 0) {
			if (c >= 0) {
				sgn = 1;
			} else {
				sgn = -1;
			}
		} else {
			if (sgn == 1 && c < 0)
				return false;
			if (sgn == -1 && c >= 0)
				return false;
		}
	}
	return true;
}

// end handy math

float littlegm2;

class shape {
public:
	enum kind kind;
	float invmass;
	float invmoi;
	pointf2 pos;
	float rot;
	pointf2 vel;
	float rotvel;
	tree2* t;
	shape() : invmass(0),invmoi(0),pos(pointf2x()),rot(0),vel(pointf2x()),rotvel(0)
	{
		logger("default shape cons\n");
		t = 0;
		shapes.push_back(this); // keep track of this shape
	}
	virtual void draw() = 0;
	~shape()
	{
		logger("destroying shape %d\n",kind);
		delete t;
	}
};


////// Wall (immovable)
class wall : public shape {
public:
	pointf2 norm;
	float d;
	wall(float xa,float ya,float nxa,float nya)
	{
		//if (shapes.size() == maxshapes)
		//	return;
		pointf2 ps = pointf2x(xa,ya);
		norm = pointf2x(nxa,nya);
		normalize2d(&norm,&norm);
		vel = pointf2x(0,0);
		invmass = 0;
		invmoi = 0;
		norm = norm;
		d = sdot2vv(ps,norm);
		kind = Wall;
	}
	void draw()
	{
	}
};

/*Wall.prototype.show = function() {
	ps += "(W " +
	this.norm.x.toFixed(2) + " " +
	this.norm.y.toFixed(2) + " " +
	this.d.toFixed(2) + ")\n";
};

Wall.prototype.draw = function() {
};
*/
////// Ball
class ball : public shape {
public:
	float rad;
	ball(float ma,float xa,float ya,float ra,float vxa,float vya,float vra,float rada)
	{
		//if (shapes.size() == maxshapes)
		//	return;
		if (ma != 0)
			invmass = 1/ma;
		else
			invmass = ma;
		if (invmass) {
			invmoi = 2*invmass/(rada*rada);
		}
		if (norot)
			invmoi = 0;
		pos = pointf2x(xa,ya);
		rot = ra;
		vel = pointf2x(vxa,vya);
		rotvel = vra;
		rad = rada;
		kind = Ball;
		//this.show = ballshow;
		//this.draw = balldraw;

		t = buildplane_xy(pointf2x(1,1),"ball5.png","pdiff");
		t->name = "circle1";
		t->mod->mats[0].msflags &= ~SMAT_HASWBUFF; // turn off zbuffer
		//circle1->trans.x = -.5f;
		setupball();
		roottree->linkchild(t);
	}

#if 1
	void setupball()
	{
		t->rot.z = rot;
		t->trans.x = pos.x/WY - .5f*WX/WY;
		t->trans.y = pos.y/WY - .5f;
		t->scale.x = rad/WY*2;
		t->scale.y = rad/WY*2;
	}
#endif
	void draw()
	{
		setupball();
	}

};
/*
Ball.prototype.show = function() {
	ps += "(B " +
	this.invmass.toFixed(2) + " " +
	this.pos.x.toFixed(2) + " " +
	this.pos.y.toFixed(2) + " " +
	this.rot.toFixed(2) + " " +
	this.vel.x.toFixed(2) + " " +
	this.vel.y.toFixed(2) + " " +
	this.rotvel.toFixed(2) + " " +
	this.rad.toFixed(2) + ")\n";
};

Ball.prototype.draw = function() {
	var tbr = 2*this.rad;
	sprite_setsize(tbr,tbr);
	sprite_sethand(.5,.5);
	sprite_setangle(-this.rot);
	sprite_draw(this.pos.x,viewy - this.pos.y,"ball5.png");
};
*/

////// Plank
class plank : public shape {
public:
	float w,h;
	pointf2 p[NRECTPOINTS],pr[NRECTPOINTS];
	plank(float ma,
		float xa,float ya,float ra,
		float vxa,float vya,float vra,
		float wa,float ha) {
		//if (shapes.size() == maxshapes)
		//	return;
		if (ma != 0)
			invmass = 1/ma;
		else
			invmass = ma;
		if (invmass) {
			invmoi = 12*invmass/(wa*wa+ha*ha);
		}
		if (norot)
			invmoi = 0;
		pos = pointf2x(xa,ya);
		rot = ra;
		vel = pointf2x(vxa,vya);
		rotvel = vra;
		w = wa;
		h = ha;
		kind = Plank;
		//this.show = plankshow;
		//this.draw = plankdraw;
		//this.p = [new Point2(),new Point2(),new Point2(),new Point2()];
		//this.pr = [new Point2(),new Point2(),new Point2(),new Point2()];

		t = buildplane_xy(pointf2x(1,1),"plank3.png","pdiff");
		t->name = "plank1";
		t->mod->mats[0].msflags &= ~SMAT_HASWBUFF; // turn off zbuffer
		//plank2->trans.x = -.5f;
		setupplank();
		roottree->linkchild(t);
	}

	void setupplank()
	{
		t->rot.z = rot;
		t->trans.x = pos.x/WY - .5f*WX/WY;
		t->trans.y = pos.y/WY - .5f;
		t->scale.x = w/WY;
		t->scale.y = h/WY;
	}
	void calcpr()
	{
#if 1
		p[0].x = -.5f*w;
		p[0].y =  .5f*h;
		p[1].x =  .5f*w;
		p[1].y =  .5f*h;
		p[2].x =  .5f*w;
		p[2].y = -.5f*h;
		p[3].x = -.5f*w;
		p[3].y = -.5f*h;
		rotpoints2d(p,pr,rot,NRECTPOINTS);
		S32 i;
		for (i=0;i<NRECTPOINTS;++i) {
			pr[i] = vadd2vv(pr[i],pos);
		}
#endif
	}
	void draw()
	{
		setupplank();
	}
};

/*
Plank.prototype.show = function() {
	ps += "(P " +
	this.invmass.toFixed(2) + " " +
	this.pos.x.toFixed(2) + " " +
	this.pos.y.toFixed(2) + " " +
	this.rot.toFixed(2) + " " +
	this.vel.x.toFixed(2) + " " +
	this.vel.y.toFixed(2) + " " +
	this.rotvel.toFixed(2) + " " +
	this.w.toFixed(2) + " " +
	this.h.toFixed(2) + ")\n";
};

Plank.prototype.draw = function () {
	var tbw = this.w;
	var tbh = this.h;
	if (tbw < tbh) {
		sprite_setangle(-this.rot + Math.PI*.5);
		tbw = this.h; // try again
		tbh = this.w;
	} else {
		sprite_setangle(-this.rot);
	}
	var rat = Math.round(tbw/tbh);
	if (rat < 1)
		rat = 1;
	else if (rat > 6)
		rat = 6;
	sprite_setsize(tbw,tbh);
	sprite_sethand(.5,.5);
	sprite_draw(this.pos.x,viewy - this.pos.y,"plank" + rat + ".png");
};
*/

void freeshapes()
{
	vector<shape*>::iterator it;
	for (it=shapes.begin();it!=shapes.end();++it)
		delete(*it);
	shapes.clear();
}

S32 togvidmode;
#if 0
	"bark.tga",
	"clinton.pcx",
	"rengst.jpg",
	"stones.jpg",
	"gamesel.pcx"
#endif

#if 1
void setupcircle(tree2* t,float x,float y,float rot,float rad)
{
	t->rot.z = rot;
	t->trans.x = x/WY - .5f*WX/WY;
	t->trans.y = y/WY - .5f;
	t->scale.x = rad/WY*2;
	t->scale.y = rad/WY*2;
}
#endif


bool wall2wall(shape* a,shape* b)
{
	//logger("W2W\n");
	return false;
}

bool plank2wall(shape* sp,shape* sw)
{
	//logger("P2W\n");
	plank* p = dynamic_cast<plank*>(sp);
	wall* w = dynamic_cast<wall*>(sw);
	//return false;
	//logger_str += "(plank2wall )";
//	bool newcp = true;
//	if (newcp) { // new way, better with deeper penetration
		S32 i;
		pointf2 sum = pointf2x();
		S32 npnt = 0;
		for (i=0;i<NRECTPOINTS;++i) { // take all the points that are penetrating and find the deepest one
			const pointf2& vert = p->pr[i];
			//var vert = vadd2vv(sp.pr[i],sp.pos);
			float penm = w->d - sdot2vv(vert,w->norm);
			if (penm > 0) {
				// vert COLLIDING
				sum = vadd2vv(sum,vert);
				++npnt;
			}
		}
		if (npnt) {
			// run through all the intersections
/*			var is = new Point2();
			for (i=0;i<NRECTPOINTS;++i) { // take all the points that are penetrating and find the deepest one
				var la0 = sp.pr[i];
				var la1 = sp.pr[(i + 1)%NRECTPOINTS];
				if (getintersection2dplane(la0,la1,sw.norm,sw.d,is)) {
					sum = vadd2vv(sum,is);
					++npnt;
				}
			}	*/ // seems better if commented out
			collinfo.cn = w->norm;
			float finpnt = 1.0f / npnt;
			sum.x *= finpnt;
			sum.y *= finpnt;
			float penm = w->d - sdot2vv(sum,w->norm);
			collinfo.penm = penm;
			collinfo.cp = pointf2x(sum.x,sum.y);
			return true;
		}
		return false;
//	} else { // old way
/*		var i;
		var sum = new Point2(0,0);
		var bestvert;
		var bestpenm = 0;
		for (i=0;i<NRECTPOINTS;++i) { // take all the points that are penetrating and find the deepest one
			var vert = sp.pr[i];
			//var vert = vadd2vv(sp.pr[i],sp.pos);
			var penm = sw.d - sdot2vv(vert,sw.norm);
			if (penm > 0) {
				// vert COLLIDING
				if (penm > bestpenm) {
					bestpenm = penm;
					bestvert = vert;
				}
			}
		}
		if (bestpenm > 0) {
			collinfo.cn = sw.norm;
			collinfo.penm = bestpenm;
			collinfo.cp = new Point2(bestvert.x,bestvert.y);
			fixupcp(collinfo.cp,sw.norm,bestpenm);
			return true;
		}
		return false; */
//	}
}

pointi2 movei[] = {{0,1},{0,-1},{1,0},{-1,0}};
//[{"x":0,"y":1},{"x":0,"y":-1},{"x":1,"y":0},{"x":-1,"y":0}];

pointf2 arr[NRECTPOINTS][NRECTPOINTS];
pointi2 warr[NRECTPOINTS+NRECTPOINTS];


bool plank2plank(shape* sa,shape* sb)
{
	//logger("P2P\n");
	plank* a = dynamic_cast<plank*>(sa);
	plank* b = dynamic_cast<plank*>(sb);
	//return false;
	//logger_str += "(plank2plank )";
	S32 i,j;
	// build 2d array of differences
	// TODO need an early out (AABB)
	for (j=0;j<NRECTPOINTS;++j) {
		for (i=0;i<NRECTPOINTS;++i) {
//			var new Point2()
//			pointf2x diff(a->pr[i].x-b->pr[j].x,a->pr[i].y-b->pr[j].y);
			arr[j][i] = vsub2vv(a->pr[i],b->pr[j]);
		}
	}
	S32 wi = 0; // walk
	S32 wj = 0;
	const pointf2* wp = &arr[0][0];
	// find lowest y value, then lowest x value (incase of 2 or more lowest y values)
	for (j=0;j<NRECTPOINTS;++j) {
		for (i=0;i<NRECTPOINTS;++i) {
			const pointf2& lop = arr[j][i];
			if (lop.y < wp->y || (lop.y == wp->y && lop.x < wp->x)) { // there should be no points at the same place
				wi = i;
				wj = j;
				wp = &lop;
			}
		}
	}

	pointi2 wloc = pointi2x(wi,wj); //pointi2x wloc(wi,wj);
	S32 widx = 0;
	// bool hilits[NRECTPOINTS+NRECTPOINTS]; // used just for drawing
	// ::fill(hilits,hilits+NRECTPOINTS+NRECTPOINTS,false);
	warr[widx++] = wloc;
	float wang = 0;
	// walk thru the points, doing gift wrapping
	while(widx < NRECTPOINTS+NRECTPOINTS) {
		// try the 4 'nearest' points (by connection, not distance)
		S32 k;
		S32 bestk=0;
		S32 nwi,nwj;
		float bestang = 5.0f; // bigger than any angle 0-4 from cheapatan2delta
		for (k=0;k<NRECTPOINTS;++k) { // use the one with the lowest angle
			nwi = (wi + movei[k].x + NRECTPOINTS)%NRECTPOINTS;
			nwj = (wj + movei[k].y + NRECTPOINTS)%NRECTPOINTS;
			const pointf2& pdest = arr[nwj][nwi];
			//logger("wp = %f,%f pdest = %f,%f\n",wp->x,wp->y,pdest.x,pdest.y);
			float ang = cheapatan2delta(*wp,pdest);
			//logger("k = %d, ang = %f, wang = %f, bestang = %f\n",k,ang,wang,bestang);
			if (ang <= bestang && ang >= wang) {
				bestk = k;
				bestang = ang;
				//logger("new best k = %d, best ang = %f\n",bestk,bestang);
			}
		}
		nwi = (wi + movei[bestk].x + NRECTPOINTS)%NRECTPOINTS;
		nwj = (wj + movei[bestk].y + NRECTPOINTS)%NRECTPOINTS;
		//logger("warr[%d] = %d,%d\n",widx,nwi,nwj);
		warr[widx++] = pointi2x(nwi,nwj);
		wi = nwi;
		wj = nwj;
		wp = &arr[wj][wi];
		wang = bestang;
	}
	float bestpen = 1e20f;
	S32 bestidx = 0;
	bool coll = false;
	pointf2 bestnrm;

	// got 8 points, find if inside and if so find closest line with 2 points
	for (i=0;i<NRECTPOINTS+NRECTPOINTS;++i) {
		j = (i+1)%(NRECTPOINTS+NRECTPOINTS);
		const pointf2& p0 = arr[warr[i].y][warr[i].x];
		const pointf2& p1 = arr[warr[j].y][warr[j].x];
		pointf2 pd = vsub2vv(p1,p0);
		pointf2 nrm = pointf2x(pd.y,-pd.x);
		normalize2d(&nrm,&nrm);
		float d = sdot2vv(nrm,p0);
		if (d <= 0) { // no collision
			coll = false;
			break;
		}
		float d1 = sdot2vv(p0,pd);
		float d2 = sdot2vv(p1,pd);
		if (d < bestpen && ((d1 >= 0 && d2<= 0) || (d1 <= 0 && d2 >= 0))) {
			// left of line segment and a line from point intersects line segment at 90 degrees
			bestpen = d;
			bestidx = i;
			bestnrm = nrm;
			coll = true;
		}
	}
	// find line segment and point
	if (coll) {
		i = bestidx;
		j = (i+1)%(NRECTPOINTS+NRECTPOINTS);
		bestnrm.x = -bestnrm.x;
		bestnrm.y = -bestnrm.y;
		collinfo.cn = bestnrm;
		float pen = bestpen;
		pointf2 cp = pointf2x();
		bool newcp = true;
		if (newcp) {
 			// better for deeper penetrations
			// pick a more central collision point
			pointf2 paccum = pointf2x();
			S32 pcnt = 0;
			//cp = point
			// use all points inside and intersections
			for (i=0;i<NRECTPOINTS;++i) {
				if (util_point2plank(b->pr[i],a->pr)) {
					paccum.x += b->pr[i].x;
					paccum.y += b->pr[i].y;
					++pcnt;
				}
			}
			for (i=0;i<NRECTPOINTS;++i) {
				if (util_point2plank(a->pr[i],b->pr)) {
					paccum.x += a->pr[i].x;
					paccum.y += a->pr[i].y;
					++pcnt;
				}
			}
			pointf2 is = pointf2x();
			for (i=0;i<NRECTPOINTS;++i) {
				const pointf2& la0 = a->pr[i];
				const pointf2& la1 = a->pr[(i + 1)%NRECTPOINTS];
				for (j=0;j<NRECTPOINTS;++j) {
					const pointf2& lb0 = b->pr[j];
					const pointf2& lb1 = b->pr[(j + 1)%NRECTPOINTS];
					if (getintersection2d(la0,la1,lb0,lb1,&is)) {
						paccum.x += is.x;
						paccum.y += is.y;
						++pcnt;
					}
				}
			}
			if (!pcnt)
				errorexit("pcnt == 0");
			float fpcnt = 1.0f / pcnt;
			cp.x = paccum.x * fpcnt;
			cp.y = paccum.y * fpcnt;
			collinfo.cp = cp;
		} else {
			if (warr[i].x == warr[j].x) { // same point in a
				pointf2 cp2 = a->pr[warr[i].x];
				cp = cp2;
			} else if (warr[i].y == warr[j].y) { // same point in b
				pointf2 cp2 = b->pr[warr[i].y];
				cp = cp2;
				cp.x -= pen * bestnrm.x;
				cp.y -= pen * bestnrm.y;
			} else { // what ??
				//cp = pointf2x(3,3);
				errorexit("what");
			}
			collinfo.cp = cp;
			fixupcp(&collinfo.cp,&bestnrm,pen);
		}
		collinfo.penm = pen;
	}
	return coll;
}

bool ball2wall(shape* sb,shape* sw)
{
	ball* b = dynamic_cast<ball*>(sb);
	wall* w = dynamic_cast<wall*>(sw);
	//logger("B2W\n");
	//return false;
	//logger_str += "(ball2wall )";
	float penm = w->d + b->rad - sdot2vv(b->pos,w->norm);
	if (penm <= 0)
		return false;
	// COLLIDING
	collinfo.cn = w->norm; // normal
	collinfo.penm = penm;
	collinfo.cp = vmul2sv(-b->rad,w->norm);
	collinfo.cp = vadd2vv(collinfo.cp,sb->pos);
	fixupcp(&collinfo.cp,&w->norm,penm);
	return true;
}

bool ball2plank(shape* sb,shape* sp)
{
	//logger("B2P\n");
	ball* b = dynamic_cast<ball*>(sb);
	plank* p = dynamic_cast<plank*>(sp);
	//return false;
	//logger_str += "(ball2plank )";
// TODO AABB early out
	S32 bestidx = 0;
	bool coll = false;
	pointf2 bestnrm = pointf2x();
	float bestpen = 1e20f;
	int i,j;
//	see if ball is close to edge
	for (i=0;i<NRECTPOINTS;++i) {
		j = (i+1)%(NRECTPOINTS);
		const pointf2& p0 = p->pr[i];
		const pointf2& p1 = p->pr[j];
		pointf2 pd = vsub2vv(p1,p0);
		pointf2 nrm = pointf2x(pd.y,-pd.x);
		normalize2d(&nrm,&nrm);
		float d = sdot2vv(nrm,p0); // line in d,nrm  format
		float pen = sdot2vv(nrm,b->pos) - d + b->rad;
		if (pen <= 0) {
			coll = false;
			break; // too far away, no collision
		}
		// now work 90 degrees from nrm
		float d1 = sdot2vv(p0,pd);
		float d2 = sdot2vv(p1,pd);
		float dp = sdot2vv(b->pos,pd);
//		if (pen < bestpen) {
		if (pen < bestpen && ((d1 >= dp && d2<= dp) || (d1 <= dp && d2 >= dp))) {
			// left of line segment and a line from point intersects line segment at 90 degrees
			bestpen = pen;
			bestidx = i;
			bestnrm = nrm;
			coll = true;
		}
	}
	//if (coll)
	//	if (bestpen > 20)
	//		var q = 3.1;
	if (!coll && i == NRECTPOINTS) { // check corners
		float bestdist2 = 1e20f;
		for (i=0;i<NRECTPOINTS;++i) {
			pointf2 del = vsub2vv(p->pr[i],b->pos);
			float dist2 = del.x*del.x + del.y*del.y;
			if (dist2 >= b->rad*b->rad)
				continue;
			if (dist2 < bestdist2) {
				bestdist2 = dist2;
				bestidx = i;
				coll = true;
			}
		}
		if (coll) {
			bestnrm = vsub2vv(p->pr[bestidx],b->pos);
			normalize2d(&bestnrm,&bestnrm); // this might be wrong, could be 0
			bestpen = b->rad - sqrtf(bestdist2);
			//if (bestpen > 20)
			//	var q = 3.1;
		}
	}
	if (coll) {
		collinfo.penm = bestpen;
		collinfo.cp = vmul2sv(b->rad,bestnrm);
		collinfo.cp = vadd2vv(collinfo.cp,b->pos);
		bestnrm.x = -bestnrm.x;
		bestnrm.y = -bestnrm.y;
		collinfo.cn = bestnrm;
		fixupcp(&collinfo.cp,&bestnrm,bestpen);
	}
	return coll;
//	return false;
}

bool ball2ball(shape* a,shape* b)
{
	//logger("B2B\n");
	ball* ba = dynamic_cast<ball*>(a);
	ball* bb = dynamic_cast<ball*>(b);
	//return false;
	//logger_str += "(ball2ball )";
	pointf2 del = vsub2vv(a->pos,b->pos);
	float dist2 = del.x*del.x + del.y*del.y;
	float rsum = ba->rad + bb->rad;
	if (dist2 >= rsum*rsum)
		return false;
	float d = sqrtf(dist2);
	//var nrm = del;
	//normalize2d(nrm);
	pointf2 nrm = vmul2sv(1.0f/d,del);
	pointf2 cp = vmul2sv(-ba->rad,nrm);
	cp = vadd2vv(cp,a->pos);
	float penm = rsum - sqrtf(dist2);
	collinfo.penm = penm;
	collinfo.cn = nrm;
	collinfo.cp = cp;
	fixupcp(&collinfo.cp,&nrm,penm);
	return true;
}

// this
#if 0
var collmatrix = [
		[wall2wall],
		[plank2wall,plank2plank],
		[ball2wall,ball2plank,ball2ball]
	];
#endif

// typedef or no typedef

//#define USETYPEDEF
#ifdef USETYPEDEF
// typedef
typedef bool (*collfunc)(shape* a,shape* b);
collfunc collmatrix[3][3] = {
	{wall2wall},
	{plank2wall,plank2plank},
	{ball2wall,ball2plank,ball2ball}
};
#else
// or this, no typedef
bool (*collmatrix[3][3])(shape* a,shape* b) = {
		{wall2wall},
		{plank2wall,plank2plank},
		{ball2wall,ball2plank,ball2ball}
};
#endif

void collide(shape* sa,shape* sb)
{
	//return;
	float tim = sa->invmass + sb->invmass;
	if (tim <= 0)
		return;
	// switch objects if necessary
	kind satype = sa->kind;
	kind sbtype = sb->kind;
	if (satype < sbtype) {
		shape* t = sa;
		sa = sb;
		sb = t;
		kind tk = satype;
		satype = sbtype;
		sbtype = tk;
	}

	// do the collision
#if 1
	bool res = collmatrix[satype][sbtype](sa,sb);
	if (!res) // no collision
		return;
#endif

#if 1
	pointf2 cn = collinfo.cn; // normal of impulse from b to a
	pointf2 cp = collinfo.cp; // where the collision took place
	float penm = collinfo.penm; // how deep the collision was
	// display collision info
	pointf2 cp2 = vmul2sv(.5f*penm,cn);
	cp2 = vadd2vv(cp2,cp);
	//if (laststep) {
		drawpoint(cp,6);
		drawpoint(cp2,3);
	//}
#if 1
	// velocity update very long
	// calc rel vel
	pointf2 rveltrans = vsub2vv(sa->vel,sb->vel); // rel vel, trans part, a rel to b
	pointf2 rvelk = rveltrans;
	pointf2 ra,rva,rb,rvb;
	float racn,rbcn;
	if (sa->invmoi) {
		ra = vsub2vv(cp,sa->pos);
		rva = vcross2zv(sa->rotvel,ra);
		rvelk = vadd2vv(rvelk,rva);
	}
	if (sb->invmoi) {
		rb = vsub2vv(cp,sb->pos);
		rvb = vcross2zv(sb->rotvel,rb);
		rvelk = vsub2vv(rvelk,rvb);
	}

	// calc k, the impulse
	float rvelm = -sdot2vv(rvelk,cn); // vel rel to -normal, should be positive
	if (rvelm <= 0)  // pen velocity
		return; // already moving away
	//  impulse formula
	float timm = tim;
	if (sa->invmoi) {
		racn = scross2vv(ra,cn);
		timm += racn*racn*sa->invmoi;
	}
	if (sb->invmoi) {
		rbcn = scross2vv(rb,cn);
		timm += rbcn*rbcn*sb->invmoi;
	}
	float k = (1+elast)*rvelm/timm;

	// apply impulse maybe do later
	if (sa->invmass) {
		float dva = k*sa->invmass;
		pointf2 tva = vmul2sv(dva,cn);
		sa->vel = vadd2vv(sa->vel,tva);
	}
	if (sa->invmoi) {
		sa->rotvel += k*racn*sa->invmoi;
	}
	if (sb->invmass) {
		float dvb = k*sb->invmass;
		pointf2 tvb = vmul2sv(dvb,cn);
		sb->vel = vsub2vv(sb->vel,tvb);
	}
	if (sb->invmoi) {
		sb->rotvel -= k*rbcn*sb->invmoi;
	}
	// new friction
	float f = 0;
	pointf2 tang = {0,0};
	float racnt = 0,rbcnt = 0;
	if (ustatic > 0) {
		// calc a new rvel
		pointf2 rvelf = rveltrans;
		if (sa->invmoi) {
			pointf2 ra = vsub2vv(cp,sa->pos);
			pointf2 rva = vcross2zv(sa->rotvel,ra);
			rvelf = vadd2vv(rvelf,rva);
		}
		if (sb->invmoi) {
			pointf2 rb = vsub2vv(cp,sb->pos);
			pointf2 rvb = vcross2zv(sb->rotvel,rb);
			rvelf = vsub2vv(rvelf,rvb);
		}
		// try a new direction of force here
		tang = pointf2x(cn.y,-cn.x); // 90 degrees to normal
		float rvelt = -sdot2vv(rvelf,tang);
		if (rvelt < 0) { // make sure force is opposite the rvelf
			rvelt = -rvelt;
			tang.x = -tang.x;
			tang.y = -tang.y;
		}
		if (rvelt > 0) {
			float timt = tim;
			if (sa->invmoi) {
				racnt = scross2vv(ra,tang);
				timt += racnt*racnt*sa->invmoi;
			}
			if (sb->invmoi) {
				rbcnt = scross2vv(rb,tang);
				timt += rbcnt*rbcnt*sb->invmoi;
			}
			if (elastfric) {
				f = 2*rvelt/timt; // this f will bounce it back
			} else {
				f = rvelt/timt; // this f will stop objects
				float fs = k * ustatic;
				if (f > fs) { // then slip
					float fd = k * udynamic;
					f = fd;
				}
			}
		}
	}
	// apply new friction impulse
	if (f != 0) {
		if (sa->invmass) {
			float dvat = f*sa->invmass;
			pointf2 tvat = vmul2sv(dvat,tang);
			sa->vel = vadd2vv(sa->vel,tvat);
		}
		if (sa->invmoi) {
			sa->rotvel += f*racnt*sa->invmoi;
		}
		if (sb->invmass) {
			float dvbt = f*sb->invmass;
			pointf2 tvbt = vmul2sv(dvbt,tang);
			sb->vel = vsub2vv(sb->vel,tvbt);
		}
		if (sb->invmoi) {
			sb->rotvel -= f*rbcnt*sb->invmoi;
		}
	}
	// position update due to penetration, maybe do sooner
	float resolvepen = .1f; // 0 to 1, 0 never, 1 instant
	if (sa->invmass) {
		float pena = resolvepen*penm*sa->invmass/tim;
		pointf2 tda = vmul2sv(pena,cn);
		sa->pos = vadd2vv(sa->pos,tda);
	}
	if (sb->invmass) {
		float penb = resolvepen*penm*sb->invmass/tim;
		pointf2 tdb = vmul2sv(penb,cn);
		sb->pos = vsub2vv(sb->pos,tdb);
	}
#endif
}

#endif
// move an object given it's position,rotation,velocity,and rotation velocity using littleg and timestep
void moveobj(pointf2& p,float& r,pointf2& pv,float& rv,float ts)
{
#if 1
		// air friction
		pv = vmul2sv(vdamp,pv); // beware, should be damp^ts, we'll see
		rv *= rvdamp;
		// integrator
		// p1 = p0 + v0t + 1/2at^2
        const pointf2 at2 = vmul2sv(.5f*ts*ts,littleg);
		const pointf2 vt = vmul2sv(ts,pv);
		//logger("vt = %f %f, at2 = %f %f\n",vt.x,vt.y,at2.x,at2.y);
		// this code doesn't compile correctly in code blocks unless you use buggy variable, objects will move real slow in release version of code blocks
//#define BUGGY
#ifdef BUGGY
		float buggy; // why does this make it work ???
		buggy = vt.x;
		buggy = at2.x;
#endif
		p = vadd2vv(p,vt);
		p = vadd2vv(p,at2); // this order matters for gnu compilers for some reason, buggy, O o1 and o3 worked, none, o2 didn't
		//p.x += .1;
		// v1 = v0 + at
		pointf2 at = vmul2sv(ts,littleg);
		//logger("at = %f\n",at); // bad
		//logger("at = %f %f\n",at.x,at.y); // good
		pv = vadd2vv(pv,at);
		// rotate
		r += ts*rv;
		r = normalangrad(r);
		//logger("svel = %f %f\n",pv.x,pv.y);
#endif
}
	// run the physics etc
void procshapes(float ts)
{
    //logger("procshapes %f\n",ts);
	//return;
	penergy = 0;
	kenergy = 0;
	renergy = 0;
	U32 i,j;
	// move all objects
	for (i=0;i<shapes.size();++i) {
		shape* s = shapes[i];
		//logger("shape %3d, pos %f, %f vel %f, %f\n",i,s->pos.x,s->pos.y,s->vel.x,s->vel.y);
		// preprocess planks
        if (s->kind == Plank) {
			(dynamic_cast<plank*>(s))->calcpr();
		}
		// move
		if (s->invmass <= 0)
			continue;	// can't move walls etc.
		moveobj(s->pos,s->rot,s->vel,s->rotvel,ts);
	}
#if 1
	// do shape to shape collisions
	for (i=0;i<shapes.size();++i) {
		shape* sa = shapes[i];
		for (j=i+1;j<shapes.size();++j) {
			shape* sb = shapes[j];
			collide(sa,sb);
		}
	}
#endif
	// update stats
	for (i=0;i<shapes.size();++i) {
		shape* s = shapes[i];
		if (s->invmass) {
			float m = 1/s->invmass;
			penergy -= m*sdot2vv(s->pos,littleg); // littleg is <0
			kenergy += m*.5f*sdot2vv(s->vel,s->vel);
		}
		if (s->invmoi) {
			renergy += .5f*s->rotvel*s->rotvel/s->invmoi;
		}
	}
	tenergy = penergy + kenergy + renergy;
}


// move physics data to tree graphic hierarchy
void drawshapes()
{
	U32 i;
	ps = "shapes: ";
	for (i=0;i<shapes.size();++i) {
		shape* s = shapes[i];
//		s->show(); // text
		s->draw(); // update graphic data with physics data
	}
	//drawpoint(new Point2(100,200),8);
}

// remove some shapes from the end
void removeshapes(S32 rem) {
	S32 b = shapes.size() - rem;
	S32 e = shapes.size();
	for (S32 c=e-1;c>=b;--c) {
		delete(shapes[c]);
	}
	shapes.resize(shapes.size() - rem);
	//for (i=shapes.size() -1;
}

// input events
// add or remove shapes depending on the (mouse) button value
void morelessshapes(S32 v) // v is increase or decrease in number of shapes
{
	logger("(morelessshapes %d\n",v);
	float ds = v*floorf(expf(mul/16));
	if (ds > 0) { // add new shapes
		S32 i;
		for (i=0;i<ds;++i) {
			if (shapes.size() >= maxshapes)
				return;
			if ((shapes.size() % 2) == 1) {
				new ball(1,
					200,200,0,
					5,20*mt_frand(),0,
					50*mt_frand()+25
				);
			} else {
				float w = 125*mt_frand()+20;
				float h = 125*mt_frand()+20;
				new plank(1,
					200,200,0,
//					5,15,0,
					5,20*mt_frand(),0,
					w,h
				);
			}
		}
	} else if (ds < 0) { // remove shapes
		if (minshapes > shapes.size() + ds) {
			ds = (float)(minshapes - shapes.size());
		}
		removeshapes((S32)-ds); // remove from end
	}
	++mul;
}


// rep reset events
void resetmul() {
	//logger("(resetmul\n");
	mul = 0;
}



#ifdef USENAMESPACE
}
using namespace physics2d;
#endif

void physics2dinit()
{
    littlegm2 = sdot2vv(littleg,littleg);
	pushandsetdir("physics2d");
	lightinfo.dodefaultlights=true;
	video_setupwindow(600,500);
//	video_setupwindow(800,600);
//	video_setupwindow(GX,GY);
	roottree = new tree2("roottree");
	roottree->trans.z = aback;
	initpoints();
// draw background
    backgroundtree = new tree2("backgroundtree");
    backgroundtree->trans.z = aback;
	//tree2* m=buildprism(pointf3x((float)WX/WY,1,1),"take0005.jpg","pdiff");
	tree2* bt = buildplane_xy(pointf2x((float)WX/WY,1),"take0005.jpg","pdiff");
	bt->name = "background";
//	tree2* m=buildprism(pointf3x(1,1,1),"take0005.jpg","pdiff");
	bt->mod->mats[0].msflags &= ~SMAT_HASWBUFF; // turn off zbuffer
	backgroundtree->linkchild(bt);
	// build some planks
// load up physics2d objects
	//shapes = loadshapes();
	// TODO: should share models (refcount) not unique
	if (shapes.size() <= 4) {
		shapes.clear();
#define SETIM
#ifdef SETIM
		// immovable shapes
		new wall(0,0,
			1,0
		);
		new wall((float)WX,0,
			-1,0
		);
		new wall(0,0,
			0,1
		);
		new wall(0,(float)WY,
			0,-1
		);
		new plank(0,
			400,200,PI*(3.0f/2.0f),
			0,0,0,
			150,50
		);
#endif

		minshapes = shapes.size();

		// movable shapes
#define SET1
#ifdef SET1
		new plank(1,
			260,480,PI*(1/16.0f),
			0,-.5f,0,
			150,50
		);
		new plank(1,
			200,380,PI*(1.0f/16.0f),
			5,-.5,0,
			150,50
		);

 		new ball(1,
			200,80,PI*(1.0f/16.0f),
			2,-.5,-.8f,
			50
		);
 		new ball(1,
			200,180,PI*(5.0f/16.0f),
			2,.5,0,
			20
		);
#endif
//#define SET2
#ifdef SET2
            new plank(0,
                    200, 180, 0,
                    0, 0, 0,
                    150, 50
            );
            new ball(1,
                    200, 240, 0,
                    0, -3, 0,
                    50
            );
#endif
//#define SET3
#ifdef SET3
		new plank(1,
			260,360,PI*(3.0f/16.0f),
			0,-2,0,
			150,50
		);
		new plank(1,
			200,280,PI*(0.0f/16.0f),
			0,0,0,
			150,50
		);

#endif
	}
// setup viewport
	mainvp.backcolor=C32LIGHTBLUE;
	mainvp.zfront=.025f;
	mainvp.zback=400;
	mainvp.camzoom=1;//3.2f; // it'll getit from tree camattach if you have one
	mainvp.flags=VP_CLEARBG|VP_CLEARWB|VP_CHECKER;
//	mainvp.xsrc=4;
//	mainvp.ysrc=3;
	mainvp.xsrc=WX;
	mainvp.ysrc=WY;
//	mainvp.xsrc=1;
//	mainvp.ysrc=1;
	mainvp.camtrans = pointf3x();
	mainvp.camrot = pointf3x();
	mainvp.useattachcam=false;
//	mainvp.lookat=0;
	mainvp.isortho=false;
	mainvp.ortho_size=30;
	mainvp.xstart = 0;//WX/5;
	mainvp.ystart = 0;//WY/3;
	mainvp.xres = WX;
	mainvp.yres = WY;
	//mainvp.camtrans.z = -.5f;
// keep track of current video device
	togvidmode=videoinfo.video_maindriver;
	fpssave = wininfo.fpswanted;
	wininfo.fpswanted = 60;//20;
}

void physics2dproc()
{
	if (KEY==K_ESCAPE)
		poporchangestate(STATE_MAINMENU);
	if (KEY=='a')
		showcursor(1);
	if (KEY=='h')
		showcursor(0);
	if (KEY==' ')
		video3dinfo.favorshading^=1;
	if (KEY == 'r')
		changestate(STATE_PHYSICS2D);
#if 0
	if (wininfo.mmiddleclicks) {
		logger("setting vidmode from %d to %d\n",togvidmode,togvidmode^1);
		togvidmode^=1;
		video_init(togvidmode,0);
		changestate(STATE_PHYSICS2D);
	}
	if (KEY=='s') {
		logger("setting vidmode from %d to %d\n",togvidmode,togvidmode^2);
		togvidmode^=2;
		video_init(togvidmode,0);
		changestate(STATE_PHYSICS2D);
	}
	if (KEY=='=') {
		changeglobalxyres(1);
		changestate(STATE_PHYSICS2D);
	}
	if (KEY=='-') {
		changeglobalxyres(-1);
		changestate(STATE_PHYSICS2D);
	}
	mainvp.xres=WX;
	mainvp.yres=WY;
	mainvp.xstart=0;
	mainvp.ystart=0;
#endif

	resetpoints(); // new set of points from collisions
	//if (MBUT)
	//	morelessshapes(1);
	if (wininfo.mleftclicks)
		morelessshapes(1);
	if (wininfo.mrightclicks)
		morelessshapes(-1);
	resetmul();
	float ts = timestep / timemul;
	//S32 i;
	//for (i=0;i<timemul;++i) {
		//laststep = i == timemul - 1;
		procshapes(ts);
	//}
	// draw
	drawshapes();
#if 0
	printareadraw(printnshapes,"nshapes = " + shapes.length + ", mul " + mul + ", time = " + time);
	printareadraw(printcount,"count " + cntr + 	", penergy " + penergy.toFixed(3) +
												", kenergy " + kenergy.toFixed(3) +
												", renergy " + renergy.toFixed(3) +
												", tenergy " + tenergy.toFixed(3));
	printareadraw(printshapes,ps);
#endif
	// next frame
	++cntr;
	ptime += timestep;



	doflycam(&mainvp);
	//roottree->proc();
}

void physics2ddraw3d()
{
	video_setviewport(&mainvp); // clear zbuf etc.
	video_buildworldmats(backgroundtree);
	video_buildworldmats(roottree);
	video_buildworldmats(helpertree);
	dolights();
	video_drawscene(backgroundtree);
	video_drawscene(roottree);
	video_drawscene(helpertree);
}

void physics2dexit()
{
//#define FREEEARLY
#ifdef FREEEARLY
	freeshapes(); // maybe free tree's, if not, let delete roottree do that
	exitpoints(); // free helper system
#endif
	logger("logging roottree\n");
	roottree->log2();
	logger("logging backgroundtree\n");
	backgroundtree->log2();
	logger("logging mastertree\n");
	mastertree->log2();
#ifndef FREEEARLY
	logger("logging helpertree\n");
	helpertree->log2();
#endif
	logger("logging reference lists\n");
	logrc();
	//freeshapes(); // maybe free tree's, if not, let delete roottree do that
#ifndef FREEEARLY
	freeshapes(); // maybe free tree's, if not, let delete roottree do that
	exitpoints();
#endif
	delete roottree;
	delete backgroundtree;
	logger("logging reference lists after free\n");
	logrc();
	wininfo.fpswanted = fpssave;
	popdir();
}
