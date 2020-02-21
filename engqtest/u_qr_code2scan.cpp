#include <m_eng.h>

struct squarei {
	S32 x,y,s;
	bool overlap(const squarei& rhs) const;
};

bool squarei::overlap(const squarei& rhs) const
{
	return x+s>=rhs.x-rhs.s && rhs.x+rhs.s>=x-s  &&  y+s>=rhs.y-rhs.s && rhs.y+rhs.s>=y-s;
}

class equiv {
//	vector<squarei> clusters;
	vector<vector<squarei> > split;
public:
// assume well presented , clusters overlap every other member in same cluster, otherwise, might get more clusters
	void addsi(const squarei& si);
	vector<pointi2> gettargets();
};

void equiv::addsi(const squarei& si)
{
	U32 i,j;
	for (j=0;j<split.size();++j) {
		vector<squarei>& cluster = split[j];
		for (i=0;i<cluster.size();++i) {
			if (si.overlap(cluster[i])) {
				cluster.push_back(si);
				break;
			}
		}
		if (i!=cluster.size())
			break;
	}
	if (j == split.size()) {
		vector<squarei> nc; // new cluster
		nc.push_back(si);
		split.push_back(nc);
	}
}

bool pred(const vector<squarei>& a,const vector<squarei>& b)
{
	return a.size() > b.size(); // highest cluster numbers first, reverse order
}

bool predp(const pointi2& a,const pointi2& b)
{
	return a.x + 2*a.y < b.x + 2*b.y;
}

vector<pointi2> equiv::gettargets()
{
	::sort(split.begin(),split.end(),pred);
	vector<pointi2> ret;
	U32 i,j;
	U32 targs = min(split.size(),3U);
	for (j=0;j<targs;++j) {
		const vector<squarei>& cluster = split[j];
		U32 sumx=0,sumy=0;
		for (i=0;i<cluster.size();++i) {
			sumx += cluster[i].x;
			sumy += cluster[i].y;
		}
		pointi2 p;
		p.x = (sumx)/cluster.size();
		p.y = (sumy)/cluster.size();
		ret.push_back(p);
	}
	::sort(ret.begin(),ret.end(),predp);
	return ret;
}

S32 distsqi(const pointi2& a,const pointi2&b)
{
	S32 dx = a.x - b.x;
	S32 dy = a.y - b.y;
	return dx*dx + dy*dy;
}

S32 docross(pointi2 v0,pointi2 v1)
{
	return v0.x*v1.y - v1.x*v0.y;
}

class lintrans {
	mat3 m,invm;
public:
	lintrans(const pointf2& a,const pointf2& b,const pointf2& c,const pointf2& ta,const pointf2& tb,const pointf2& tc);
	pointf2 trans(const pointf2& in) const;
	pointf2 invtrans(const pointf2& in) const;
};

lintrans::lintrans(const pointf2& a,const pointf2& b,const pointf2& c,const pointf2& ta,const pointf2& tb,const pointf2& tc)
{
	mat3 ma,invma,mb;

	identmat3(&ma);
	ma.e[0][0] = b.x-a.x;
	ma.e[0][1] = b.y-a.y;
	ma.e[1][0] = c.x-a.x;
	ma.e[1][1] = c.y-a.y;
	ma.e[2][0] = a.x;
	ma.e[2][1] = a.y;
	inversemat3(&ma,&invma);

	identmat3(&mb);
	mb.e[0][0] = tb.x-ta.x;
	mb.e[0][1] = tb.y-ta.y;
	mb.e[1][0] = tc.x-ta.x;
	mb.e[1][1] = tc.y-ta.y;
	mb.e[2][0] = ta.x;
	mb.e[2][1] = ta.y;

	mulmat3(&invma,&mb,&m);
	inversemat3(&m,&invm);
}

pointf2 lintrans::trans(const pointf2& in) const
{
	pointf3x in3(in.x,in.y,1);
	pointf3 out3;
	mulmatvec3(&m,&in3,&out3);
	pointf2x out(out3.x,out3.y);
	return out;
}

pointf2 lintrans::invtrans(const pointf2& in) const
{
	pointf3x in3(in.x,in.y,1);
	pointf3 out3;
	mulmatvec3(&invm,&in3,&out3);
	pointf2x out(out3.x,out3.y);
	return out;
}

void testtrans()
{
	pointf2x a0(3,4),a1(5,7),a2(9,11);
	pointf2x ta0(15,3),ta1(19,22),ta2(3,17);
	lintrans lt(a0,a1,a2,ta0,ta1,ta2);
	pointf2 res0 = lt.trans(a0);
	pointf2 res1 = lt.trans(a1);
	pointf2 res2 = lt.trans(a2);
	pointf2 invres0 = lt.invtrans(ta0);
	pointf2 invres1 = lt.invtrans(ta1);
	pointf2 invres2 = lt.invtrans(ta2);
}

#if 0 // test
void permtargets(vector<pointi2>& ps,S32 d0,S32 d1,S32 d2)
{
	vector<pointi2> psb = ps;
	ps[d0] = psb[0];
	ps[d1] = psb[1];
	ps[d2] = psb[2];
}
#endif

vector<vector<U8> > doqrscan(bitmap32* pic)
{
	logger("in doscan\n");
	vector<vector<U8> > data;
	// find theshold and separate into dark and light
	S32 i,j,k;
#if 0
	// average
	U32 vt = 0;
	for (j=0;j<pic->size.y;++j) {
		for (i=0;i<pic->size.x;++i) {
			C32 p = clipgetpixel32(pic,i,j);
			U32 v = p.r + p.g + p.b;
			vt += v;
		}
	}
	S32 np = pic->size.x * pic->size.y;
	U32 va = vt/np; // average
//	va -= 150;
#else
	// median
	U32 minv=3*255,maxv = 0;
	for (j=0;j<pic->size.y;++j) {
		for (i=0;i<pic->size.x;++i) {
			C32 p = clipgetpixel32(pic,i,j);
			U32 v = p.r + p.g + p.b;
			if (v < minv)
				minv = v;
			if (v > maxv)
				maxv = v;
		}
	}
	U32 va = (minv+maxv)/2; // median
#endif
	// build rectangular bool array of light0,dark1
	vector<vector<bool> > pix(pic->size.y);
	for (j=0;j<pic->size.y;++j) {
		pix[j].assign(pic->size.x,false);
		for (i=0;i<pic->size.x;++i) {
			C32 p = clipgetpixel32(pic,i,j);
			U32 v = p.r + p.g + p.b;
			bool bv = v < va;
			C32 oc = bv ? C32BLACK : C32WHITE;
			clipputpixel32(pic,i,j,oc);
			pix[j][i] = bv;
		}
	}
	int dc = 0;
	int lc = 0;
	for (j=0;j<pic->size.y;++j) {
//		pix[j].assign(pic.size.x,false);
		for (i=0;i<pic->size.x;++i) {
			if (pix[j][i])
				++dc;
			else
				++lc;
		}
	}
	logger("lc = %d, dc = %d\n",lc,dc);
	bitmap32* picb = bitmap32copy(pic);
// find light and dark strips in each row and column
//#define FIXP
#ifdef FIXP
	// generate test pattern (0 is white, 1 is black)
	// example nbit = 3
/*	000
	100
	010
	110
	001
	101
	011
	111 */
	const S32 nbit = 3;
	vector<vector<S32> > wbh(1<<nbit); // white black white black counts per scanline, even is white, sum is width
	vector<vector<S32> > wbv(nbit); // white black white black counts per scanline, even is white, sum is width

	pix.assign(1<<nbit,vector<bool>()); // rebuild pix with test pattern
	for (j=0;j<1<<nbit;++j) {
		pix[j].assign(nbit,false);
		for (i=0;i<nbit;++i) {
			pix[j][i] = (j&(1<<i)) != 0;
		}
	}
#else
	vector<vector<S32> > wbh(pix.size()); // white black white black counts per scanline, even is black, sum is width
	vector<vector<S32> > wbv(pix[0].size()); // white black white black counts per scanline, even is black, sum is width
#endif

// find finder pattern
	// assume perfect alignment for now
	for (j=0;j<(S32)pix.size();++j) {
		bool dark = false;
		S32 cnt = 0;
		for (i=0;i<(S32)pix[0].size();++i) {
			bool bv = pix[j][i];
			if (bv == dark) {
				++cnt;
			} else {
				wbh[j].push_back(cnt);
				dark = !dark;
				cnt = 1;
			}
		}
		wbh[j].push_back(cnt);
	}
	for (i=0;i<(S32)pix[0].size();++i) {
		bool dark = false;
		S32 cnt = 0;
		for (j=0;j<(S32)pix.size();++j) {
			bool bv = pix[j][i];
			if (bv == dark) {
				++cnt;
			} else {
				wbv[i].push_back(cnt);
				dark = !dark;
				cnt = 1;
			}
		}
		wbv[i].push_back(cnt);
	}
// have finder pattern lengths
// look for 1:1:3:1:1 ratios horizontal
	for (j=0;j<(S32)wbh.size();++j) {
		S32 x = wbh[j][0];
		for (i=1;i<(S32)wbh[j].size()-5;i+=2) {
			float minhival =  1e20f;
			float maxloval = -1e20f;
			S32 xs = x;
			for (k=0;k<5;++k) {
				float loval,hival;
				if (k==2) { // center is 3 by 3
					loval = (2.5f/3.0f) * float(wbh[j][i+k]);
					hival = (3.5f/3.0f) * float(wbh[j][i+k]);
				} else {
					loval = .5f * 3 * float(wbh[j][i+k]);
					hival = 1.5f * 3 * float(wbh[j][i+k]);
				}
				if (hival < minhival)
					minhival = hival;
				if (loval > maxloval)
					maxloval = loval;
				xs += wbh[j][i+k];
			}
			if (maxloval < minhival) { // found a finder pattern candidate
#ifdef FIXP
				logger("");
#else
				cliphline32(pic,x,j,xs,C32GREEN); // mark green
#endif
			}
			x += wbh[j][i];
			x += wbh[j][i+1];
		}
	}
// look for 1:1:3:1:1 ratios vertical
	for (i=0;i<(S32)wbv.size();++i) {
		S32 y = wbv[i][0];
		for (j=1;j<(S32)wbv[i].size()-5;j+=2) {
			float minhival =  1e20f;
			float maxloval = -1e20f;
			S32 ys = y;
			for (k=0;k<5;++k) {
				float loval,hival;
				if (k==2) { // center is 3 by 3
					loval = (5.0f/6.0f) * float(wbv[i][j+k]);
					hival = (7.0f/6.0f) * float(wbv[i][j+k]);
				} else {
					loval = 1.5f * float(wbv[i][j+k]);
					hival = 4.5f * float(wbv[i][j+k]);
				}
				if (hival < minhival)
					minhival = hival;
				if (loval > maxloval)
					maxloval = loval;
				ys += wbv[i][j+k];
			}
			if (maxloval < minhival) { // found a finder pattern candidate
#ifdef FIXP
				logger("");
#else
				for (k=y;k<=ys;++k) { // mark magenta, but if already green make yellow (intersection)
					C32 c = clipgetpixel32(pic,i,k) == C32GREEN ? C32YELLOW : C32MAGENTA;
					clipputpixel32(pic,i,k,c);
				}
#endif
			}
			y += wbv[i][j];
			y += wbv[i][j+1];
		}
	}
// find large candidates
	vector<squarei> squares;
	for (j=0;j<pic->size.y;++j) {
		for (i=0;i<pic->size.x;++i) {
			C32 v = clipgetpixel32(pic,i,j);
			if (v == C32YELLOW) {
				S32 m,n;
				for (k=1;k<300;++k) { // find largest centered rect
					bool ok = true;
					for (n=-k;n<=k;++n) {
						for (m=-k;m<=k;++m) {
							v = clipgetpixel32(pic,i+m,j+n);
							if (v != C32YELLOW) {
								ok = false;
								break;
							}
						}
					}
					if (!ok) {
						break;
					}
				}
				if (k>=3) {
					// logger("bigsquare found at %d,%d, size %d\n",i,j,k);
					squarei s;
					s.x = i; s.y = j; s.s = k;
					squares.push_back(s);
				}
			}
		}
	}
// find largest square radius
	S32 maxsq = 0; // (say ends up a 1) // -1 to 1 inclusive size 3
	for (j=0;j<(S32)squares.size();++j) {
		if (squares[j].s > maxsq)
			maxsq = squares[j].s; // radius of largest axis aligned square that can fit inside the 3 by 3 square target
	}
// fixed now, make more forgiving (right now their are only 3 squares of s == 7 and they are the right ones
	vector<squarei> bigsquares2;
	equiv eq; // find overlapping clusters
	for (j=0;j<(S32)squares.size();++j) {
//		if (squares[j].s >= maxsq) {
		if (squares[j].s + 12 >= maxsq) {
			//logger("big square found at %d,%d, size %d\n",squares[j].x,squares[j].y,squares[j].s);
			//clipputpixel32(pic,squares[j].x,squares[j].y,C32LIGHTRED);
			bigsquares2.push_back(squares[j]);
			eq.addsi(squares[j]);
		}
	}
	vector<pointi2> targets = eq.gettargets();
	for (j=0;j<(S32)targets.size();++j) {
		clipcircle32(pic,targets[j].x,targets[j].y,2,C32LIGHTMAGENTA);
	}
	if (targets.size()!=3) {
		logger("wrong number of square targets %d != 3\n",targets.size());
		bitmap32free(picb);
		return data;
	}
	for (j=0;j<3;++j) {
		logger("target %d = (%4d,%4d)\n",j,targets[j].x,targets[j].y);
	}
#if 0 // test
//	permtargets(targets,0,1,2);
//	permtargets(targets,0,2,1);
//	permtargets(targets,1,0,2);
//	permtargets(targets,1,2,0);
//	permtargets(targets,2,0,1);
	permtargets(targets,2,1,0);
#endif
	// find upper right corner
#if 1
	S32 d01 = distsqi(targets[0],targets[1]);
	S32 d12 = distsqi(targets[1],targets[2]);
	S32 d20 = distsqi(targets[2],targets[0]);
	S32 right = 2;
	S32 bigdist = d01;
	if (d12 > bigdist) {
		right = 0;
		bigdist = d12;
	}
	if (d20 > bigdist) {
		right = 1;
		bigdist = d20;
	}
	if (right == 1) { // get upper left corner into slot 0, right angle
		pointi2 t = targets[0];
		targets[0] = targets[1];
		targets[1] = t;
	} else if (right == 2) {
		pointi2 t = targets[0];
		targets[0] = targets[2];
		targets[2] = t;
	}
	// swap slot 1 and 2 if necessary
	pointi2x v1(targets[1].x-targets[0].x,targets[1].y-targets[0].y);
	pointi2x v2(targets[2].x-targets[0].x,targets[2].y-targets[0].y);
	S32 cross = docross(v1,v2);
	if (cross<0) {
		pointi2 t = targets[1];
		targets[1] = targets[2];
		targets[2] = t;
	}
#endif
	testtrans();
	pointf2x t0((float)targets[0].x,(float)targets[0].y);
	pointf2x t1((float)targets[1].x,(float)targets[1].y);
	pointf2x t2((float)targets[2].x,(float)targets[2].y);
// now calc dist between targets and estimate version number
	outtextxyb32(pic,targets[0].x-4,targets[0].y-4,C32BLACK,C32WHITE,"0");
	outtextxyb32(pic,targets[1].x-4,targets[1].y-4,C32BLACK,C32WHITE,"1");
	outtextxyb32(pic,targets[2].x-4,targets[2].y-4,C32BLACK,C32WHITE,"2");
	clipcircle32(pic,targets[0].x+maxsq,targets[0].y+maxsq,3,C32LIGHTBLUE);
	clipcircle32(pic,targets[1].x-maxsq,targets[1].y+maxsq,3,C32LIGHTBLUE);
	clipcircle32(pic,targets[2].x+maxsq,targets[2].y-maxsq,3,C32LIGHTBLUE);
	// k = maxsq*4+maxsq/2; // times 4.5
	S32 dx1 = targets[1].x-targets[0].x; // (say 18)
	S32 dy1 = targets[1].y-targets[0].y;
//	S32 dx2 = targets[2].x-targets[0].x;
//	S32 dy2 = targets[2].y-targets[0].y;
	float d1 = sqrtf(float(dx1*dx1+dy1*dy1)); // distance from target0 to target1 in pixels
//	float d2 = sqrtf(float(dx2*dx2+dy2*dy2)); // distance from target0 to target2
	// walk right in q space
	float qx,qy=3.0f;
	float rx=0,lx=0;
	pointf2x q0(3,3),q1(21,3),q2(3,21);
	lintrans lt(q0,q1,q2,t0,t1,t2);
	for (qx=3.0f;qx<40.0f;qx+=.01f) { // this should cover versions 1 to 6
		pointf2 p = lt.trans(pointf2x(qx,qy));
		C32 c = clipgetpixel32(picb,(S32)p.x,(S32)p.y);
		// logger("rpx = %f,rpy = %f, RGB = %02x%02x%02x\n",p.x,p.y,c.r,c.g,c.b);
		if (c.r) {
			rx = dist2d(&t0,&p);
			break;
		}
	}
	for (qx=3.0f;qx>0.0f;qx-=.01f) {
		pointf2 p = lt.trans(pointf2x(qx,qy));
		C32 c = clipgetpixel32(picb,(S32)p.x,(S32)p.y);
		// logger("lpx = %f,rpy = %f, RGB = %02x%02x%02x\n",p.x,p.y,c.r,c.g,c.b);
		if (c.r) {
			lx = dist2d(&t0,&p);
			break;
		}
	}
	logger("lx = %f, rx = %f\n",lx,rx);
//	float x = (2.0f*maxsq+1.0f)/3.0f; // size of pixel (say 1)
	float newx = (rx + lx)/3.0f;
	logger("newx = %f\n",newx);
//	logger("x = %f, newx = %f\n",x,newx);
	logger("distx %f\n",d1); // x only dist from 1 to 0
//	logger("distx %d\n",dx1); // x only dist from 1 to 0
//	float pver = (dx1/x-10.0f)/4.0f; // (say (18/1-10)/4 = 2
//	logger("pver = %f\n",pver);
	float newpver = (d1/newx-10.0f)/4.0f; // (say (18/1-10)/4 = 2
//	float newpver = (dx1/newx-10.0f)/4.0f; // (say (18/1-10)/4 = 2
	logger("newpver = %f\n",newpver);
//	S32 iver = S32(pver+.5f);
	S32 iver = S32(newpver+.5f);
	logger("iver = %d\n",iver);
	// extra: check timing patterns NYI
	if (iver < 1 || iver > 6) {
		logger("not version 1 to 6 !\n");
		bitmap32free(picb);
		return data;
	}
	q1 = pointf2x(float(4*iver+13),3);
	q2 = pointf2x(3,float(4*iver+13));
	lt = lintrans(q0,q1,q2,t0,t1,t2);

// now have a mapping from pic to discreet
// VERSION 2 ONLY FOR NOW, 25 by 25
	// calculate scan info
#if 1
	const S32 modu = 4*iver + 17;
	data.assign(modu,vector<U8>());
	for (j=0;j<modu;++j) {
		data[j].assign(modu,0);
		for (i=0;i<modu;++i) {
			pointf2x q((float)i,(float)j);
			pointf2 p = lt.trans(q);
			S32 px = (S32)(p.x+.5f);
			S32 py = (S32)(p.y+.5f);
			clipputpixel32(pic,px,py,C32BROWN);
			if (py>=0 && py<(S32)pix.size() && px>=0 && px<(S32)pix[0].size())
				data[j][i] = pix[py][px];
			else
				data[j][i] = 1;
		}
	}
#else
	const S32 modu = 25;
	float m = (float)d1/(21-3);
//	m *= 1.0315f;
	float bx = targets[0].x-3*m;
	float by = targets[0].y-3*m;
//	b += 1.5f;
//	for (j=0;j<25;++j) {
//		float f = m*j + b;
//		//logger("%d %f\n",j,f);
//	}
// restore old pic
//	bitmap32free(pic);
//	pic = bitmap32copy(picb);
	// convert to U8 and mark samples as BROWN
//	vector<vector<U8> > data(modu);
	data.assign(modu,vector<U8>());
	for (j=0;j<modu;++j) {
		data[j].assign(modu,0);
		S32 py = S32(m*j+by);
		for (i=0;i<modu;++i) {
			S32 px = S32(m*i+bx);
			clipputpixel32(pic,px,py,C32BROWN);
			if (py>=0 && py<(S32)pix.size() && px>=0 && px<(S32)pix[0].size())
				data[j][i] = pix[py][px];
			else
				data[j][i] = 1;
		}
	}
#endif
	bitmap32free(picb);
	picb = 0;
	// done scan
	logger("scan successful\n\n");
	return data;
}
