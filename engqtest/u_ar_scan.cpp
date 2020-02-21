#include <m_eng.h>
#include "u_s_ar.h"
#include "u_ar_scan.h"
#include "u_ar_parse.h"

// global for debprint
S32 blackwhitethresh = 135; // for method2 lshape

#define USENS // use namespace, turn off for debugging VS2005
#ifdef USENS
namespace ar_scan{
#endif

#ifdef FIVEDOTS
#define METHOD1 // tested method, not great
#endif

#ifdef LSHAPE
#define METHOD2 // new method, untested
#endif


// scanning parameters
const U32 maxbright = 255*3;
U32 thresh = maxbright*40/100; // 0 to 255*3, set by darkratio and doscanar
const U32 minrad = 4;
const S32 maxrad = 30;
const S32 extrarad = 5;
//const float darkratio = .002325f; // good with live scan
//float darkratio = .00525f; // good with what's in the gallery
float darkratio = .003906f; // stability test

U32 getbright(const C32& v)
{
	U32 bright = (U32)v.r + (U32)v.g + (U32)v.b;
	return bright;
}

// already normalized
bool isbright(const C32& v) {
	return v.r >= 0x80;
}

// returns largest circle within dark threshold
S32 scancircle(const bitmap32* b,S32 x,S32 y)
{
	static S32 radstablearr[2*maxrad+1][2*maxrad+1]; // 0 is at maxrad, range is -maxrad to +maxrad
	static vector<vector<pointi2> >radstable(maxrad+1); // set points at a all radii
	// build tables if first time
	static bool inited = false;
	S32 i,j;
	if (!inited) {
#if 0
		// test isqrt to 200
		for (i=0;i<=200;++i)
			logger("sqrt(%d) = %d\n",i,isqrt(i));
		// test ilog2 to 200
		for (i=0;i<=200;++i)
			logger("ilog2(%d) = %d\n",i,ilog2(i));
#endif
		// calculate fast tables for circle radius, radstable and radstablearr
		for (j=-maxrad;j<=maxrad;++j) {
			for (i=-maxrad;i<=maxrad;++i) {
				U32 r = isqrt(i*i+j*j);
				if (r <= maxrad) {
					pointi2 p = pointi2x(i,j);
					radstable[r].push_back(p);
				}
				radstablearr[j+maxrad][i+maxrad] = r;
			}
		}
		// show radius bitmap
#if 0
		logger("radstable 2d array\n");
		for (j=0;j<=2*maxrad;++j) {
			for (i=0;i<=2*maxrad;++i) {
				logger("%3d",radstablearr[j][i]);
			}
			logger("\n");
		}
#endif
#if 0
		// list all point lists of radstable
		logger("list of point lists\n");
		for (j=0;j<(signed)radstable.size();++j) {
			logger("list of rad %d size %d\n",j,radstable[j].size());
			for (i=0;i<(signed)radstable[j].size();++i) {
				logger("\t(%d,%d)\n",radstable[j][i].x,radstable[j][i].y);
			}
		}
#endif
		inited = true;
	}
	// find max radius circle all dark
	S32 r;
	for (r=0;r<maxrad;++r) {
		S32 rls = radstable[r].size();
		for (i=0;i<rls;++i) {
			C32 v = clipgetpixel32(b,x+radstable[r][i].x,y+radstable[r][i].y);
			if (v.a == 0) // out of boundary condition
				continue;
			bool ib = isbright(v);
			// if any bright here... return with this radius
			if (ib)
				return r >= minrad ? r : -1;
		}
	}
	return r;
}

bool circleOverlap(const circleScan& a,const circleScan& b,S32 extra = 1)
{
	if (a.r <= 0 || b.r <=0)
		return false;
	S32 dr = (a.r + b.r) * extra;
	S32 dx = a.x - b.x;
	S32 dy = a.y - b.y;
	return dx*dx + dy*dy <= dr*dr;
}

#ifdef USENS
}

using namespace ar_scan;
#endif

// test in 1d for maximal candidates
void ar_test_wider_neighbor1D()
{
	logger("testing 1d wider neighbor\n");
	static const S32 data[] = {
		1,30,30,4,5,6,7,6,4,5,6,7,6,7,8,7,8,9,8,9,10,9,8,7,6,5,6,5,6,8,9,8,7,8,7,6,7,6,5,4,3,2
	};
	const S32 wid = 3;
	S32 i,j;
	logger("numelements = %d\n",NUMELEMENTS(data));
	for (i=wid;i<NUMELEMENTS(data)-wid;++i) {
		S32 mid = data[i];
		for (j=-wid;j<=wid;++j) {
			//logger("checking at %d + %d\n",i,j);
			S32 chk = data[i+j];
			//if (j != 0 && mid <= chk)
			if (mid < chk)
				break; // not a maximum
		}
		if (j > wid) {
			logger("max at %3d: is %3d\n",i,data[i]);
		}
	}
}

#ifdef METHOD1
// convert captured bitmap into an array of circles, part of 5 point method
vector<circleScan> doscanar(bitmap32* b,string& result)
{
	S32 i,j,k;
	stringstream res;
	logger("ar scanning bitmap of size %d by %d\n",b->size.x,b->size.y);
	S32 prod = b->size.x*b->size.y;
	C32* p = b->data;
#if 1
	// do a histogram
	vector<U32> hist(maxbright+1);
	for (i=0;i<prod;++i) {
		U32 brt = getbright(p[i]);
		++hist[brt];
	}
	S32 darks = 0;
	for (i=0;i<maxbright;++i) {
		if ((float)darks/prod >= darkratio) {
			break;
		}
		darks += hist[i];
	}
	thresh = i;
	// show histogram
#if 0
	logger("histogram\n");
	S32 cnt = 0;
	for (i=0;i<=maxbright;++i) {
		if (hist[i]) {
			logger("hist[brt %3d] = %6d\n",i,hist[i]);
			cnt += hist[i];
		}
	}
	logger("total hist = %d\n",cnt);
#endif
#endif
	// list of large circles
	vector<circleScan> ret;
	// photo norm, either C32BLACK or C32WHITE (1 bit)
	C32* dp = b->data;
	prod = b->size.x*b->size.y;
	S32 blackcnt = 0;
	for (i=0;i<prod;++i) {
		C32 v = dp[i];
		U32 bright = getbright(v);
		if (bright < thresh) {
			v = C32BLACK;
			++blackcnt;
		} else {
			v = C32WHITE;
		}
		dp[i] = v;
	}
	res << blackcnt << "/" << prod << " percent " << (float)blackcnt/prod;
	// record all circles
	// real scan, look for overlaps, pick largest ones
	for (j=0;j<b->size.y;++j) {
		for (i=0;i<b->size.x;++i) {
			S32 r = scancircle(b,i,j);
			if (r>=0) {
				circleScan cs = {i,j,r};
				for (k=0;k<(signed)ret.size();++k) {
					// is close to other circle in the list
					if (circleOverlap(cs,ret[k])) {
						// is new circle bigger than one in the list
						if (cs.r > ret[k].r) {
							ret[k] = cs; // replace
						} else { // new circle is smaller than one in the list that is close, throw out
							break;
						}
					}
				}
				// if nothing overlaps, add new circle to the list
				if (k == ret.size())
					ret.push_back(cs);
			}
		}
	}
	// run though list once again and check for overlaps
	while(true) {
		S32 overlapcount = 0;
		for (j=0;j<(signed)ret.size();++j) {
			for (i=0;i<(signed)ret.size();++i) {
				// mark overlaps with radius 0
				// choose one with large radius, 0 radius of the other
				if (i != j && circleOverlap(ret[i],ret[j],extrarad)) {
					if (ret[i].r > ret[j].r)
						ret[j].r = 0;
					else
						ret[i].r = 0;
				}
			}
		}
		if (overlapcount == 0) // no more overlaps
			break;
	}
	// clean up ret, remove entries with rad == 0
	for (i=0;i<(signed)ret.size();++i) {
		if (ret[i].r == 0) {
			ret[i] = ret[ret.size()-1];
			ret.pop_back();
			--i; // re check copied entry
		}
	}
	// draw some circles
	for (i=0;i<(signed)ret.size();++i) {
		clipcircleo32(b,ret[i].x,ret[i].y,ret[i].r,C32GREEN);
	}
#if 1
	// show the list of circles
		logger("list of scaned circles\n");
		for (i=0;i<(signed)ret.size();++i)
			logger("scan %3d : x = %4d, y = %4d, r = %d\n",i,ret[i].x,ret[i].y,ret[i].r);
#endif
	result = string(res.str());
	return ret;
}
#endif

#ifdef METHOD2 // use L shape
// average rgb into all u32 channel
void makegray(bitmap32* b)
{
	S32 i,prod = b->size.x*b->size.y;
	C32* data = b->data;
	for (i=0;i<prod;++i) {
		U32 v = getbright(data[i])/3;
		C32 cv(v);
		data[i] = cv;
	}
}

// filter noise, work in U32 mode
void guass5(bitmap32* b)
{
	static S32 gf[5][5] = {
		{ 2, 4, 5, 4, 2},
		{ 4, 9,12, 9, 4},
		{ 5,12,15,12, 5},
		{ 2, 4, 5, 4, 2},
		{ 4, 9,12, 9, 4}
	};
	S32 i,j,m,n,prod = b->size.x*b->size.y;
	bitmap32* bc = bitmap32copy(b);
	for (j=0;j<b->size.y;++j) {
		for (i=0;i<b->size.x;++i) {
			U32 v = 0;
			for (n=-2;n<=2;++n) {
				for (m=-2;m<=2;++m) {
					v += gf[n+2][m+2]*clipgetpixel32(bc,i+m,j+n).c32;
				}
			}
			v /= 159;
			clipputpixel32(b,i,j,C32(v));
		}
	}
	bitmap32free(bc);
}

// edge detection, work in u32 channel
void sobel(bitmap32* b)
{
	static S32 sbx[3][3] = {
		{-1, 0, 1},
		{-2, 0, 2},
		{-1, 0, 1}
	};
	static S32 sby[3][3] = {
		{-1,-2,-1},
		{ 0, 0, 0},
		{ 1, 2, 1}
	};
	S32 i,j,m,n,prod = b->size.x*b->size.y;
	bitmap32* bc = bitmap32copy(b);
	for (j=0;j<b->size.y;++j) {
		for (i=0;i<b->size.x;++i) {
			U32 v = 0;
			if (i != 0 && i != b->size.x -1 && j != 0 && j != b->size.y -1) {
				U32 sumx = 0;
				U32 sumy = 0;
				for (n=-1;n<=1;++n) {
					for (m=-1;m<=1;++m) {
						sumx += sbx[n+1][m+1]*clipgetpixel32(bc,i+m,j+n).c32;
						sumy += sby[n+1][m+1]*clipgetpixel32(bc,i+m,j+n).c32;
					}
				}
				v = (U32)sqrtf((float)(sumx*sumx + sumy*sumy));
				v = range(0U,v,255U);
			}
			clipputpixel32(b,i,j,C32(v));
		}
	}
	bitmap32free(bc);
}

struct angr {
	float ang;
	float r;
	U32 bright;
	S32 x,y;
};

// find lines, work in u32 channel, slow
vector<angr> hough(bitmap32* b)
{
	vector<angr> hc;
	//return hc;
	S32 i,j,k,prod = b->size.x*b->size.y;
	float r2 = sqrtf((float)b->size.x*b->size.x+b->size.y*b->size.y);
// b will be result bc is the source
	bitmap32* b_orig = bitmap32copy(b);
	clipclear32(b,C32(0));
	U32 maxhough = 0;
#if 1
// accumulate values
	for (j=0;j<b->size.y;++j) {
		for (i=0;i<b->size.x;++i) {
			U32 bv = clipgetpixel32(b_orig,i,j).c32;
			if (bv < 128)
				continue;
			for (k=0;k<b->size.x;++k) {
				float ang = (float)k/b->size.x*PI;
				float r = i*cosf(ang) + j*sinf(ang);
				float y = (r + r2)*b->size.y / (2.0f * r2);
				S32 yi = range(0,(int)y,b->size.y-1);
				C32 v = clipgetpixel32(b,k,yi).c32;
				++v.c32;
				if (v.c32 > maxhough) {
					maxhough = v.c32;
				}
				clipputpixel32(b,k,yi,v);
			}
		}
	}
	logger("maxhough = %d\n",maxhough);
#endif

//#define FINDMAX0 // the 1 maximum
//#define FINDMAX1 // nearest neighbor
#define FINDMAX2 // larger neighborhood
//#define FINDMAX3 // cluster method

#ifdef FINDMAX0 // best max
// find the highest maximum
	C32 ismax = 0;
	S32 besti = -1,bestj = -1;
	clipblit32(b,b_orig,0,0,0,0,b->size.x,b->size.y);
	clipclear32(b,C32(0));
	for (j=0;j<b->size.y;++j) {
		for (i=0;i<b->size.x;++i) {
			C32 cm = clipgetpixel32(b_orig,i,j);
			if (cm.c32 > ismax.c32) {
				besti = i;
				bestj = j;
				ismax = cm;
			}
		}
	}
	if (ismax.c32 * 256 > 0x40 * maxhough) {
		float ang = besti * PI / b->size.x;
		float r = 2.0f*r2*bestj/b->size.y - r2;
		clipputpixel32(b,besti,bestj,clipgetpixel32(b_orig,besti,bestj));
		logger("xy coords are (%4d,%4d), ",besti,bestj);
		logger("bright %d, ",ismax.c32);
		logger("theta and radius, (%6.3f,%6.3f)\n",ang,r);
		angr ar = {ang,r,ismax.c32};
		hc.push_back(ar);
	}
#endif

#ifdef FINDMAX1 // nearest neighbor
	pointi2 dirs[8] = {
		{-1,-1},
		{ 0,-1},
		{ 1,-1},
		{-1, 0},
		{ 1, 0},
		{-1, 1},
		{ 0, 1},
		{ 1, 1}
	};
// find local maximums and clip out the rest
	clipblit32(b,b_orig,0,0,0,0,b->size.x,b->size.y);
	//return hc;
	clipclear32(b,C32(0));
	for (j=0;j<b->size.y;++j) {
		for (i=0;i<b->size.x;++i) {
			C32 ismax = clipgetpixel32(b_orig,i,j);
			for (k=0;k<8;++k) {
				C32 cm = clipgetpixel32(b_orig,i+dirs[k].x,j+dirs[k].y);
				if (cm.c32 > ismax.c32)
					break;
			}
			if (k == 8) {
			//if (true) {
				//if (true) {
				if (ismax.c32 * 256 > 0x40 * maxhough) {
					float ang = i * PI / b->size.x;
					float r = 2.0f*r2*j/b->size.y - r2;
					if (true) {
					//if (r > 10.0f) {
						clipputpixel32(b,i,j,clipgetpixel32(b_orig,i,j));
						logger("xy coords are (%4d,%4d), ",i,j);
						logger("bright %d, ",ismax.c32);
						logger("theta and radius, (%6.3f,%6.3f)\n",ang,r);
						angr ar = {ang,r,ismax.c32};
						hc.push_back(ar);
					}
				}
			}
		}
	}
#endif

#ifdef FINDMAX2 // wider max neighborhood search
	S32 m,n;
	S32 wid = 10;
// find local maximums and clip out the rest
	//clipblit32(b,bc,0,0,0,0,b->size.x,b->size.y);
	bitmap32* b_max = bitmap32copy(b);
	//return hc;
	clipclear32(b,C32(0));
	for (j=wid;j<b->size.y-wid;++j) {
		for (i=wid;i<b->size.x-wid;++i) {
			C32 ismax = clipgetpixel32(b_max,i,j);
			for (n=-wid;n<=wid;++n) {
				for (m=-wid;m<=wid;++m) {
					C32 chk = clipgetpixel32(b_max,i+m,j+n);
					if (ismax.c32 < chk.c32)
						break;
				}
				if (m <= wid)
					break;
			}
			if (n > wid) {
				if (ismax.c32 * 256 > 0x60 * maxhough) {
					float ang = i * PI / b->size.x;
					float r = 2.0f*r2*j/b->size.y - r2;
					if (true) {
					//if (r > 10.0f) {
						clipputpixel32(b,i,j,clipgetpixel32(b_max,i,j));
						logger("xy coords are (%4d,%4d), ",i,j);
						logger("bright %d, ",ismax.c32);
						logger("theta and radius, (%6.3f,%6.3f)\n",ang,r);
						angr ar = {ang,r,ismax.c32};
						hc.push_back(ar);
					}
				}
			}
		}
	}
	blendbitmaps(b,b_orig,.5f);
	bitmap32free(b_max);
#endif

#ifdef FINDMAX3 // clustering methods NYI
// just record intense values in hough space
	const int thresh = 64; // 0 - 256
// find local maximums and clip out the rest
	clipblit32(b,b_orig,0,0,0,0,b->size.x,b->size.y);
	//return hc;
	clipclear32(b,C32(0));
	for (j=0;j<b->size.y;++j) {
		for (i=0;i<b->size.x;++i) {
			C32 ismax = clipgetpixel32(b_orig,i,j);
			if (ismax.c32 * 256 > thresh * maxhough) {
				float ang = i * PI / b->size.x;
				float r = 2.0f*r2*j/b->size.y - r2;
				clipputpixel32(b,i,j,clipgetpixel32(b_orig,i,j));
				angr ar = {ang,r,ismax.c32,i,j};
				hc.push_back(ar);
				logger("xy coords are (%4d,%4d), ",ar.x,ar.y);
				logger("bright %d, ",ar.bright);
				logger("theta and radius, (%8.3f,%8.3f)\n",ar.ang,ar.r);
			}
		}
	}

#if 1
// find clusters and then find the max in each cluster
	if (hc.size() > 0) {
		angr ar = hc[0];
		hc.clear();
		hc.push_back(ar);
	}
#endif

#endif
// scale down gray intensity
#if 1
	if (maxhough > 0) {
		for (j=0;j<b->size.y;++j) {
			for (i=0;i<b->size.x;++i) {
				C32 v = clipgetpixel32(b,i,j);
				v.c32 = v.c32*255/maxhough;
				clipputpixel32(b,i,j,v);
			}
		}
	}
#endif

	bitmap32free(b_orig);
	return hc;
}

void applyhoughcoll(bitmap32* b, const vector<angr>& h)
{
	U32 i,n = h.size();
	for (i=0;i<n;++i) {
		const angr& ar = h[i];
		S32 xr = (S32)(ar.r*cosf(ar.ang));
		S32 yr = (S32)(ar.r*sinf(ar.ang));
		S32 xv = -yr;
		S32 yv = xr;
		xv *= 100;
		yv *= 100;
		S32 x0 = xr + xv;
		S32 y0 = yr + yv;
		S32 x1 = xr - xv;
		S32 y1 = yr - yv;
		clipcircle32(b,xr,yr,3,C32LIGHTMAGENTA);
		clipline32(b,x0,y0,x1,y1,C32YELLOW);
	}
}

void invert(bitmap32* b)
{
	S32 i,j,prod = b->size.x*b->size.y;
	for (j=0;j<b->size.y;++j) {
		for (i=0;i<b->size.x;++i) {
			U32 bv = clipgetpixel32(b,i,j).c32;
			bv = range(0U,255U - bv,255U);
			clipputpixel32(b,i,j,bv);
		}
	}
}

void blackwhite(bitmap32* b,U32 thresh)
{
	S32 i,j,prod = b->size.x*b->size.y;
	for (j=0;j<b->size.y;++j) {
		for (i=0;i<b->size.x;++i) {
			C32 bv = clipgetpixel32(b,i,j);
			if (bv.c32 < thresh)
				bv.c32 = 0;
			else
				bv.c32 = 255;
			clipputpixel32(b,i,j,bv);
		}
	}
}

// move u32 channel to rgb gray
void u32togray(bitmap32* b)
{
	S32 i,prod = b->size.x*b->size.y;
	C32* data = b->data;
	for (i=0;i<prod;++i) {
		U32 v = data[i].c32;
		v = range(0U,v,255U);
		C32 cv(v,v,v);
		data[i] = cv;
	}
}

// convert u32 channel to rgb visual of gray scale gradient with colors
void u32togradcolors(bitmap32* b)
{
	static C32 colors[7] = {
		C32(1,0,0),
		C32(0,1,0),
		C32(1,1,0),
		C32(0,0,1),
		C32(1,0,1),
		C32(0,1,1),
		C32(1,1,1),
	};
	S32 i,prod = b->size.x*b->size.y;
	C32* data = b->data;
	for (i=0;i<prod;++i) {
		U32 v = data[i].c32;
		v = range(0U,v,255U);
		v = 255 - v;
		v *= 7;
		U32 idx = v>>8;
		U32 mod = v&0xff;
		C32 cv = colors[idx];
		cv.r *= mod;
		cv.g *= mod;
		cv.b *= mod;
		data[i] = cv;
	}
}

// convert captured bitmap into an array of circles
vector<circleScan> doscanar(bitmap32* b,string& result)
{
	vector<circleScan> ret(5);

//	clipclear32(b,C32RED);
	makegray(b);

	invert(b);
	guass5(b);
	//sobel(b);
	blackwhitethresh = range(0,blackwhitethresh,255); // UI
	blackwhite(b,blackwhitethresh);
	vector<angr> hc = hough(b);

	u32togray(b);
	applyhoughcoll(b,hc);
//	u32togradcolors(b);


	stringstream res;
	res << "numlines = ";
	res << hc.size();
	//result = "dummy";
	result = res.str().c_str();
	return ret;
}
#endif
