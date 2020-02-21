//#define BOX2BOX
//#define CIRCLE2BOX
//#define CIRCLE2CIRCLE
#define COLLUTILS
//#define BOXPOINT
#ifdef COLLUTILS
//#define OLDPLANK2PLANK
//#define NEWCP

void calcbox2box()
{
	calcpr(box0);
	calcpr(box1);
	//acollide = util_plank2plank(box0,box1,abestcp,abestpendir,penm);
#ifdef BOXPOINT
	pcollide = util_point2plank(point0,box1);
	ldist = util_point2line(point0,la,lb);
}

// dot product of 2 vectors
float sdot2vv(const pointf2& a,const pointf2& b)
{
	return a.x*b.x + a.y*b.y;
}

// product of scalar with vector
pointf2 vmul2sv(float s,const pointf2& v)
{
	return pointf2x(s * v.x,s * v.y);

}

pointf2 vadd2vv(const pointf2& a,const pointf2& b)
{
	return pointf2x(a.x+b.x,a.y+b.y);
}

pointf2 vsub2vv(const pointf2& a,const pointf2& b)
{
	return  pointf2x(a.x-b.x,a.y-b.y);
}

float scross2vv(const pointf2& a,const pointf2& b)
{
	return a.x*b.y - a.y*b.x;
}

float dist2(const pointf2& a,const pointf2& b)
{
	float dx = a.x - b.x;
	float dy = a.y - b.y;
	return dx*dx + dy*dy;
}

float dist(const pointf2& a,const pointf2& b)
{
	return sqrtf(dist2(a,b));
}


// intersection of 2 lines
bool getintersection2d(const pointf2& la,const pointf2& lb,const pointf2& lc,const pointf2& ld,pointf2* i0)
{
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
bool normalize2d(pointf2& v)
{
	float d2 = v.x*v.x + v.y*v.y;
	if (d2 == 0) {
		v.x = 1; // point in some direction if a zero vector
		v.y = 0;
		return false;
	}
	float id = 1.0f/sqrtf(d2);
	v.x *= id;
	v.y *= id;
	return true;
}

#define NRECTPOINTS 4
struct box {
	pointf2 pos;
	float rot;
	pointf2 size; // w,h;
	pointf2 p[NRECTPOINTS];
	pointf2 pr[NRECTPOINTS];
};

struct circle {
	pointf2 pos;
	float rad;
};

/*#define MAXPOINTS 16
struct poly {
	S32 npoints;
	pointf2 p[MAXPOINTS];
};
*/
#ifdef BOX2BOX
struct box box0 = {{3.5f,3},PI/8,{5,3}};
struct box box1 = {{7,3},0,{3,5}};
//struct box box0 = {{5.7f,2.3f},PI/8,{5,3}};
//struct box box1 = {{7,3},0,{3,5}};
#endif
#ifdef CIRCLE2BOX
struct circle cir0 = {{5.5f,1.5f},1};
struct box box0 = {{3.5f,3},PI/8,{5,3}};
#endif
#ifdef CIRCLE2CIRCLE
struct circle cir0 = {{4,1.5f},1};
struct circle cir1 = {{5.5f,1.5f},1};
#endif
#ifdef BOXPOINT
struct pointf2 point0 = {1,-1};
struct pointf2 la = {3,4};
struct pointf2 lb = {5,7};
#endif

pointf2 rotpoint(const pointf2& p,float rot)
{
	pointf2 rp;
	float cr = cosf(rot);
	float sr = sinf(rot);
	rp.x = cr*p.x - sr*p.y;
	rp.y = sr*p.x + cr*p.y;
	return rp;
}

void calcpr(box& b)
{
	b.p[0].x = -.5f*b.size.x;
	b.p[0].y =  .5f*b.size.y;
	b.p[1].x =  .5f*b.size.x;
	b.p[1].y =  .5f*b.size.y;
	b.p[2].x =  .5f*b.size.x;
	b.p[2].y = -.5f*b.size.y;
	b.p[3].x = -.5f*b.size.x;
	b.p[3].y = -.5f*b.size.y;
	S32 i;
	for (i=0;i<NRECTPOINTS;++i) {
		b.pr[i] = rotpoint(b.p[i],b.rot);
		b.pr[i] = vadd2vv(b.pr[i],b.pos);
	}
}

pointf2 is[3]; // handle overflow on intersection
pointf2 la0;
pointf2 la1;
pointf2 lb0;
pointf2 lb1;

pointf2 abestcp;
pointf2 abestpendir;
S32 acollide;
S32 ninside;
pointf2 insides[2];
S32 nainside,nbinside;
S32 pcollide;
float penm;
#ifdef BOXPOINT
float ldist;
#endif

#ifdef OLDPLANK2PLANK
// returns distance
float util_point2line(const pointf2& p,const pointf2& la,const pointf2& lb,pointf2* nrmr)
{
	pointf2 nrm = vsub2vv(la,lb);
	nrm = pointf2x(nrm.y,-nrm.x);
	normalize2d(nrm);
	float d = sdot2vv(nrm,la) - sdot2vv(nrm,p);
	if (nrmr)
		*nrmr = nrm;
	return fabsf(d);
//	return d;
}
#endif
bool util_point2plank(const pointf2& p,const box& b)
{
	S32 i;
	pointf2 vs[NRECTPOINTS];
	S32 sgn = 0;
	for (i=0;i<NRECTPOINTS;++i) {
		vs[i] = vsub2vv(b.pr[i],p);
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
#if 0
#if 1
// only handle 1 point inside box and 2 intersections case
bool util_plank2plank(const box& a,const box& b,pointf2& bestcp,pointf2& bestpendir,float& penm)
{
// find intersections
	pointi2 isidx[2];
	S32 i,j;
	S32 k = 0;
	for (i=0;i<NRECTPOINTS;++i) {
		la0 = a.pr[i];
		la1 = a.pr[(i + 1)%NRECTPOINTS];
		for (j=0;j<NRECTPOINTS;++j) {
			lb0 = b.pr[j];
			lb1 = b.pr[(j + 1)%NRECTPOINTS];
			if (getintersection2d(la0,la1,lb0,lb1,0)) {
				if (k >= 2) {
					return false;
				}
				isidx[k].x = i;
				isidx[k].y = j;
				++k;
			}
		}
	}
	if (k != 2)
		return false;
// find out which verts are inside other box
	S32 aidx,bidx;
	ninside = nainside = nbinside = 0;
	for (i=0;i<NRECTPOINTS;++i) {
		if (util_point2plank(a.pr[i],b)) {
			if (ninside >= 1)
				return false;
			insides[ninside++] = a.pr[i];
			aidx = i;
			++nainside;
		}
	}
	for (i=0;i<NRECTPOINTS;++i) {
		if (util_point2plank(b.pr[i],a)) {
			if (ninside >= 1)
				return false;
			insides[ninside++] = b.pr[i];
			bidx = i;
			++nbinside;
		}
	}
	if (ninside != 1)
		return false;
// 1 vert inside box, find out closest line to it
	const pointf2* lns;
	const pointf2& pt = insides[0]; // the point inside a box
	if (nainside)
		lns = b.pr; // a point from 'a' inside 'b'
	else
		lns = a.pr; // a point from 'b' inside 'a'
	// do some checks
	if (nainside) {
		if (isidx[0].y != isidx[1].y) // is intersection on same line?
			return false;
		i = isidx[0].y; // this is the line
		// check the point for usage in intersections
		S32 di = isidx[0].x - isidx[1].x;
		S32 pi;
		if (di < 0)
			di += NRECTPOINTS;
		if (di == 1) {
			pi = isidx[0].x;
		} else if (di == 3) {
			pi = isidx[1].x;
		} else
			return false; // not right line
		if (pi != aidx)
			return false; // not right point
	} else { // nbinside
		if (isidx[0].x != isidx[1].x) // is intersection on same line?
			return false;
		i = isidx[0].x; // this is the line
		// check the point for usage in intersections
		S32 di = isidx[0].y - isidx[1].y;
		S32 pi;
		if (di < 0)
			di += NRECTPOINTS;
		if (di == 1) {
			pi = isidx[0].y;
		} else if (di == 3) {
			pi = isidx[1].y;
		} else
			return false; // not right line
		if (pi != bidx)
			return false; // not right point
	}
	j = (i + 1)%NRECTPOINTS;
	penm = util_point2line(pt,lns[i],lns[j],&bestpendir);
	bestcp = pt;
	if (nbinside) {
		bestcp.x = penm*bestpendir.x + bestcp.x;
		bestcp.y = penm*bestpendir.y + bestcp.y;
		bestpendir.x = -bestpendir.x;
		bestpendir.y = -bestpendir.y;
	}
	return true;
}

#else
bool util_plank2plank(const box& a,const box& b,pointf2& bestcp,pointf2& bestpendir,float& penm)
{
	S32 i,j;
	S32 k = 0;
	for (i=0;i<NRECTPOINTS;++i) {
		la0 = a.pr[i];
		la1 = a.pr[(i + 1)%NRECTPOINTS];
		for (j=0;j<NRECTPOINTS;++j) {
			lb0 = b.pr[j];
			lb1 = b.pr[(j + 1)%NRECTPOINTS];
			if (getintersection2d(la0,la1,lb0,lb1,&is[k])) {
				++k;
				if (k > 2) {
					return false;
				}
			}
		}
	}
	if (k != 2)
		return false;
	bestcp.x = (is[0].x + is[1].x)/2;
	bestcp.y = (is[0].y + is[1].y)/2;
	bestpendir.x = is[1].y - is[0].y;
	bestpendir.y = is[0].x - is[1].x;
	if (!normalize2d(bestpendir))
		return false;
	// reuse la0 
/*	la0.x = bestcp.x - a.pos.x;
	la0.y = bestcp.y - a.pos.y;
	if (sdot2vv(la0,bestpendir) > 0) {
		bestpendir.x = -bestpendir.x;
		bestpendir.y = -bestpendir.y;
	} */
	// find verts inside plank (1 or 2)
	ninside = nainside = nbinside = 0;
	for (i=0;i<NRECTPOINTS;++i) {
		if (util_point2plank(a.pr[i],b)) {
			if (ninside >= 2)
				return false;
			insides[ninside++] = a.pr[i];
			++nainside;
		}
	}
	for (i=0;i<NRECTPOINTS;++i) {
		if (util_point2plank(b.pr[i],a)) {
			if (ninside >= 2)
				return false;
			insides[ninside++] = b.pr[i];
			++nbinside;
		}
	}
	if (!ninside)
		return false;
	if (ninside == 1) {
		float d = util_point2line(insides[0],is[0],is[1]);
		penm = d;
		//bestcp = insides[0];
/*		if (nbinside) {
			bestpendir.x = -bestpendir.x;
			bestpendir.y = -bestpendir.y;
		} */
//		bestcp.x = insides[0].x + penm*bestpendir.x;
//		bestcp.y = insides[0].y + penm*bestpendir.y;
		bestcp.x = insides[0].x;
		bestcp.y = insides[0].y;
		if (nbinside) {
			bestcp.x = insides[0].x + penm*bestpendir.x;
			bestcp.y = insides[0].y + penm*bestpendir.y;
			bestpendir.x = -bestpendir.x;
			bestpendir.y = -bestpendir.y;
		} 
		return true;
	} else {
		return false;
	}
}
#endif
#endif

void drawbox(const box& b)
{
	S32 i;
	for (i=0;i<NRECTPOINTS;++i) {
		drawfline(b.pr[i],b.pr[(i+1)%NRECTPOINTS],C32BLACK);
		pointi2 pi = math2screen(b.pr[i]);
		outtextxybf32(B32,pi.x-8,pi.y+10,C32BLACK,C32LIGHTCYAN,"%d",i);
	}
}

void drawcircle(const circle& c)
{
	drawfcircle(c.pos,C32BLACK,math2screen(c.rad));
}

#ifndef OLDPLANK2PLANK
// Minkowski difference
void drawboxsm(const box& a,const box& b)
{
	S32 i,j;
	for (j=0;j<NRECTPOINTS;++j) {
		for (i=0;i<NRECTPOINTS;++i) {
			pointf2x sa(a.pr[j].x-b.pr[i].x,a.pr[j].y-b.pr[i].y);
			pointf2x sb(a.pr[j].x-b.pr[(i+1)%NRECTPOINTS].x,a.pr[j].y-b.pr[(i+1)%NRECTPOINTS].y);
			drawfline(sa,sb,C32BLACK);
			pointi2 pi = math2screen(sa);
			outtextxybf32(B32,pi.x-8,pi.y+10,C32BLACK,C32YELLOW,"%d,%d",j,i);
		}
	}
	for (j=0;j<NRECTPOINTS;++j) {
		for (i=0;i<NRECTPOINTS;++i) {
			pointf2x sa(a.pr[j].x-b.pr[i].x,a.pr[j].y-b.pr[i].y);
			pointf2x sb(a.pr[(j+1)%NRECTPOINTS].x-b.pr[i].x,a.pr[(j+1)%NRECTPOINTS].y-b.pr[i].y);
			drawfline(sa,sb,C32BLACK);
		}
	}
}

// assume not same point, return 0 to almost 4
float cheapatan2delta(const pointf2& from,const pointf2& to)
{
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

float penline(const pointf2& p,const pointf2& la,const pointf2& lb)
{
	pointf2x n(lb.y - la.y,la.x - lb.x);
	normalize2d(n);
	float da = sdot2vv(la,n);
	float d = sdot2vv(p,n);
	return d - da;
}

bool box2boxscan(const box& a,const box& b,pointf2& cp,pointf2& abestpendir,float& penm)
// TODO do AABB early out
//void box2boxscan(const box& a, const box& b)
{
	pointi2 move[4] = {{0,1},{0,-1},{1,0},{-1,0}};
	pointf2 arr[NRECTPOINTS][NRECTPOINTS];
	S32 i,j;
	// build 2d array of differences
	pointf2 wp;
	for (j=0;j<NRECTPOINTS;++j) {
		for (i=0;i<NRECTPOINTS;++i) {
			pointf2x diff(a.pr[i].x-b.pr[j].x,a.pr[i].y-b.pr[j].y);
			arr[j][i] = diff;
		}
	}
	S32 wi,wj; // walk
	wi = wj = 0;
	wp = arr[0][0];
	// find lowest y value, then lowest x value (incase of 2 or more lowest y values)
	for (j=0;j<NRECTPOINTS;++j) {
		for (i=0;i<NRECTPOINTS;++i) {
			const pointf2& cp2 = arr[j][i];
			if (cp2.y < wp.y || (cp2.y == wp.y && cp2.x < wp.x)) { // there should be no points at the same place
				wi = i;
				wj = j;
				wp = cp2;
			}
		}
	}
	pointi2x wloc(wi,wj);
	S32 widx = 0;
	pointi2 warr[NRECTPOINTS+NRECTPOINTS];
	bool hilits[NRECTPOINTS+NRECTPOINTS]; // used just for drawing
	::fill(hilits,hilits+NRECTPOINTS+NRECTPOINTS,false);
	warr[widx++] = wloc;
	float wang = 0;
	// walk thru the points, doing gift wrapping
	while(widx < 8) {
		// try the 4 'nearest' points (by connection, not distance)
		S32 k,bestk=0;
		S32 nwi,nwj;
		float bestang = 5; // bigger than any angle 0-4
		for (k=0;k<4;++k) { // use the one with the lowest angle
			nwi = (wi + move[k].x + NRECTPOINTS)%NRECTPOINTS;
			nwj = (wj + move[k].y + NRECTPOINTS)%NRECTPOINTS;
			const pointf2& pdest = arr[nwj][nwi];
			float ang = cheapatan2delta(wp,pdest);
			if (ang <= bestang && ang >= wang) {
				bestk = k;
				bestang = ang;
			}
		}
		nwi = (wi + move[bestk].x + NRECTPOINTS)%NRECTPOINTS;
		nwj = (wj + move[bestk].y + NRECTPOINTS)%NRECTPOINTS;
		warr[widx++] = pointi2x(nwi,nwj);
		wi = nwi;
		wj = nwj;
		wp = arr[wj][wi];
		wang = bestang;
	}
	float bestpen = 1e20f;
	int bestidx = 0;
	bool coll = false;
	pointf2 bestnrm;
	// got 8 points, find if inside and if so find closest line with 2 points
	for (i=0;i<NRECTPOINTS+NRECTPOINTS;++i) {
		j = (i+1)%(NRECTPOINTS+NRECTPOINTS);
		pointf2 p0 = arr[warr[i].y][warr[i].x];
		pointf2 p1 = arr[warr[j].y][warr[j].x];
		pointf2 pd = vsub2vv(p1,p0);
		pointf2x nrm(pd.y,-pd.x);
		normalize2d(nrm);
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
	if (coll) {
		i = bestidx;
		j = (i+1)%(NRECTPOINTS+NRECTPOINTS);
		hilits[i] = true;
		hilits[j] = true;
		pointf2 collpoint = vmul2sv(bestpen,bestnrm);
		drawfpoint(collpoint,C32LIGHTMAGENTA);
		bestnrm.x = -bestnrm.x;
		bestnrm.y = -bestnrm.y;
		abestpendir = bestnrm;
		penm = bestpen;
#ifdef NEWCP // better for deeper penetrations
		// pick a more central collision point
		pointf2x paccum(0,0);
		S32 pcnt = 0;
		// use all points inside and intersections
		for (i=0;i<NRECTPOINTS;++i) {
			if (util_point2plank(b.pr[i],a)) {
				paccum.x += b.pr[i].x;
				paccum.y += b.pr[i].y;
				++pcnt;
			}
		}
		for (i=0;i<NRECTPOINTS;++i) {
			if (util_point2plank(a.pr[i],b)) {
				paccum.x += a.pr[i].x;
				paccum.y += a.pr[i].y;
				++pcnt;
			}
		}
		for (i=0;i<NRECTPOINTS;++i) {
			const pointf2& la0 = a.pr[i];
			const pointf2& la1 = a.pr[(i + 1)%NRECTPOINTS];
			for (j=0;j<NRECTPOINTS;++j) {
				const pointf2& lb0 = b.pr[j];
				const pointf2& lb1 = b.pr[(j + 1)%NRECTPOINTS];
				pointf2 is;
				if (getintersection2d(la0,la1,lb0,lb1,&is)) {
					paccum.x += is.x;
					paccum.y += is.y;
					++pcnt;
				}
			}
		}
		if (!pcnt)
			error("pcnt == 0");
		cp.x = paccum.x / pcnt;
		cp.y = paccum.y / pcnt;
#else
		if (warr[i].x == warr[j].x) { // same point in a
			cp = a.pr[warr[i].x];
		} else if (warr[i].y == warr[j].y) { // same point in b
			cp = b.pr[warr[i].y];
			cp.x -= penm * bestnrm.x;
			cp.y -= penm * bestnrm.y;
		} else { // what ??
			//cp = pointf2x(3,3);
			error("can't find penpoint");
		}
#endif
	}
	// show point walk and hilits closest line if inside
	for (i=0;i<NRECTPOINTS+NRECTPOINTS;++i) {
		pointi2 pi=math2screen(arr[warr[i].y][warr[i].x]);
		bool hilit = hilits[i];
		C32 col = hilit ? C32LIGHTGREEN : C32YELLOW;
		outtextxybf32(B32,pi.x-8,pi.y+18,C32BLACK,col,"Wi %d",i);
	}
	return coll;
}
#endif
bool circle2boxscan(const circle& b,const box& a,pointf2& cp,pointf2& abestpendir,float& penm)
{
// TODO do AABB early out
	cp = pointf2x(1,1);
//	abestpendir = pointf2x(1,0);
	penm = 2;
	int bestidx = 0;
	bool coll = false;
	pointf2 bestnrm;
	float bestpen = 1e20f;
	int i,j;
//	for (i=0;i<1;++i) {
	for (i=0;i<NRECTPOINTS;++i) {
		j = (i+1)%(NRECTPOINTS);
		pointf2 p0 = a.pr[i];
		pointf2 p1 = a.pr[j];
		pointf2 pd = vsub2vv(p1,p0);
		pointf2x nrm(pd.y,-pd.x);
		normalize2d(nrm);
		float d = sdot2vv(nrm,p0); // line in d,nrm  format
		float pen = sdot2vv(nrm,b.pos) - d + b.rad;
		if (pen <= 0) {
			coll = false;
			break; // no collision
		}
		float d1 = sdot2vv(p0,pd);
		float d2 = sdot2vv(p1,pd);
		float dp = sdot2vv(b.pos,pd);
//		if (pen < bestpen) { 
		if (pen < bestpen && ((d1 >= dp && d2<= dp) || (d1 <= dp && d2 >= dp))) { 
			// left of line segment and a line from point intersects line segment at 90 degrees
			bestpen = pen;
			bestidx = i;
			bestnrm = nrm;
			coll = true;
		}
	}
	if (!coll && i == NRECTPOINTS) { // check corners
		float bestdist2 = 1e20f;
		pointf2 bestpnt;
		for (i=0;i<NRECTPOINTS;++i) {
			pointf2 del = vsub2vv(a.pr[i],b.pos);
			float dist2 = del.x*del.x + del.y*del.y;
			if (dist2 >= b.rad*b.rad)
				continue;
			if (dist2 < bestdist2) {
				bestdist2 = dist2;
				bestpnt = a.pr[i];
				bestidx = i;
				coll = true;
			}
		}
		if (coll) {
			bestnrm = vsub2vv(a.pr[bestidx],b.pos);
			normalize2d(bestnrm); // this might be wrong, could be 0
			bestpen = b.rad - sqrtf(bestdist2);
		}
	}
	if (coll) {
		penm = bestpen;
		cp = vmul2sv(b.rad,bestnrm);
		cp = vadd2vv(cp,b.pos);
		bestnrm.x = -bestnrm.x;
		bestnrm.y = -bestnrm.y;
		abestpendir = bestnrm;
	}
	return coll;
}

bool circle2circlescan(const circle& a,const circle& b,pointf2& cp,pointf2& nrm,float& penm)
{
	pointf2 del = vsub2vv(a.pos,b.pos);
	float dist2 = del.x*del.x + del.y*del.y;
	float rsum = a.rad + b.rad;
	if (dist2 >= rsum*rsum)
		return false;
	nrm = del;
	normalize2d(nrm);
	cp = vmul2sv(-a.rad,nrm);
	cp = vadd2vv(cp,a.pos);
	penm = rsum - sqrtf(dist2);
	return true;
}

#endif

void drawcollutils()
#ifdef BOX2BOX
{
	drawbox(box0);
	drawbox(box1);
#ifdef OLDPLANK2PLANK
	acollide = util_plank2plank(box0,box1,abestcp,abestpendir,penm);
#else
	drawboxsm(box0,box1);
	//box2boxscan(box0,box1);
	acollide = box2boxscan(box0,box1,abestcp,abestpendir,penm);
#endif	
	if (acollide) {
		drawfpoint(abestcp,C32RED);
		pointf2x penp(abestcp.x + penm*abestpendir.x,abestcp.y + penm*abestpendir.y);
		drawfline(abestcp,penp,C32GREEN);
		drawfpoint(penp,C32LIGHTRED);
/*		if (ninside > 0) {
			//pointf2 op = vmul2sv(penm,abestpendir);
			//op = vadd2vv(op,insides[0]);
			//drawfpoint(op,C32LIGHTRED);
			drawfpoint(insides[0],C32LIGHTBLUE);
		}
		if (ninside > 1) {
			drawfpoint(insides[1],C32LIGHTBLUE);
		} */
	}

#ifdef BOXPOINT
	C32 col;
	if (pcollide) {
		col = C32RED;
	} else {
		col = C32GREEN;
	}
	drawfpoint(point0,col);
	drawflinec(la,lb,C32BROWN);
#endif
#endif
#ifdef CIRCLE2BOX
	drawbox(box0);
	drawcircle(cir0);
	acollide = circle2boxscan(cir0,box0,abestcp,abestpendir,penm);
	if (acollide) {
		drawfpoint(abestcp,C32RED);
		pointf2x penp(abestcp.x + penm*abestpendir.x,abestcp.y + penm*abestpendir.y);
		drawfline(abestcp,penp,C32GREEN);
		drawfpoint(penp,C32LIGHTRED);
	}
#endif
#ifdef CIRCLE2CIRCLE
	drawcircle(cir0);
	drawcircle(cir1);
	acollide = circle2circlescan(cir0,cir1,abestcp,abestpendir,penm);
	if (acollide) {
		drawfpoint(abestcp,C32RED);
		pointf2x penp(abestcp.x + penm*abestpendir.x,abestcp.y + penm*abestpendir.y);
		drawfline(abestcp,penp,C32GREEN);
		drawfpoint(penp,C32LIGHTRED);
	}
#endif
#endif
