#include <m_eng.h>
#include "u_s_ar.h"
#include "u_ar_scan.h"
#include "u_ar_parse.h"

// blend a = (1-f)*a + f*b
// assume same size
void blendbitmaps(bitmap32* a,const bitmap32* b,float f)
{
	S32 i,prod = a->size.x*a->size.y;
	C32* ap = a->data;
	C32* bp = b->data;
	S32 fi = (S32)(f*256);
	S32 fim = 256 - fi;
	for (i=0;i<prod;++i) {
		C32 av = ap[i];
		C32 bv = bp[i];
		av.r = (fim*av.r + fi*bv.r) >> 8;
		av.g = (fim*av.g + fi*bv.g) >> 8;
		av.b = (fim*av.b + fi*bv.b) >> 8;
		ap[i] = av;
	}
}

//#define USENS // use namespace, turn off for debugging VS2005
#ifdef USENS
namespace u_ar_parse {
#endif

#ifdef FIVEDOTS
const float bitmapblendval = .76f; // mostly original
#endif

#ifdef LSHAPE
//const float bitmapblendval = .01f; // all scan
const float bitmapblendval = .1625f; // mostly scan
//const float bitmapblendval = .5f; // half and half
#endif

// convert array of circles into a message, later also return orientation of 3D object
string parsearcode(const vector<circleScan>& data)
{
	stringstream ss;
	ss << "ar parsed: numcircles " << data.size();
	return ss.str();
}


#ifdef USENS
}
//using namespace u_ar_parse;
#endif

//const S32 doar::npc = 5;
const float doar::zoomfactor = 3.583f; // from the camera
const float doar::matchstepmin = .00001f;//.0001f;
const float doar::saddle = .6f; // tighten search
const float doar::grad = 1.2f; // loosen search
//const S32 doar::degrees = 6;
const float doar::pitchthreshold = 1.0001f;
const S32 doar::maxmatchstable = 1000;
const S32 doar::maxbigerror = 5000;
const float doar::znear = .125f;
const float doar::zfar = 25.0f;
const float doar::errorthresh = .04f;//.02f;
const pointf3 doar::refpoints[npc] = {
	{ 0, 1},
	{ 1, 1},
	{ 1,-1},
	{-1,-1},
	{-1, 1},
};

// convert scanned points to rot trans
doar::doar(const pointf2& inresa,const pointf2& outresa) : inres(inresa),outres(outresa),asp(outresa.y/outresa.x),havescanpoints(false)
{
	di[0].name = "transx";
	di[1].name = "transy";
	di[2].name = "transz";
	di[3].name = "rotx";
	di[4].name = "roty";
	di[5].name = "rotz";
	di[0].ptr = &itrans.x;
	di[1].ptr = &itrans.y;
	di[2].ptr = &itrans.z;
	di[3].ptr = &irot.x;
	di[4].ptr = &irot.y;
	di[5].ptr = &irot.z;
	doresetrottrans();
	matchstable = 0;
	bigerror = 0;
	matchstep = matchstepmin;
	curdegree = 0;
//	pospitch = false;
//	negpitch = false;
	//currentstr = "start";
}

void doar::setscanpoints(const pointf2* inpnts)
{
//	pospitch = false;
//	negpitch = false;
// convert scanned points to plotter space
	float xf = outres.x/inres.x;
	float yf = outres.y/inres.y;
	S32 i;
	for (i=0;i<npc;++i) {
		sp[i].x = inpnts[i].x*xf;
		sp[i].y = (inres.y - inpnts[i].y)*yf;
	}
// sort scanned points like this, 0 is the extra point
// 4 0 1
// 3 . 2
// find center point
	scan_cent = pointf2x();
	for (i=0;i<npc;++i) {
		scan_cent.x += sp[i].x;
		scan_cent.y += sp[i].y;
	}
	scan_cent.x /= npc;
	scan_cent.y /= npc;
// sort point clockwise with atan2
	sort(sp,sp+npc,*this);
// now find '0' the index colinear with surrounding 2 points using cross products
	float bestcp = 1e20f;
	S32 bestcpidx = -1;
	for (i=0;i<npc;++i) {
		S32 im = (i+npc-1)%npc;
		S32 ip = (i+1)%npc;
		// 2 dimensional cross product
		float cp = (sp[ip].x - sp[i].x) * (sp[im].y - sp[i].y) - (sp[ip].y - sp[i].y) * (sp[im].x - sp[i].x);
		// looking for closest one to 0, colinear
		cp = fabsf(cp);
		if (cp < bestcp) {
			bestcpidx = i;
			bestcp = cp;
		}
	}
// shift scanned points so 0 is the middle point
	pointf2 sav[npc];
	for (i=0;i<npc;++i) {
		sav[i] = sp[(i + bestcpidx)%npc];
	}
	copy(sav,sav+npc,sp);
#if 0
// shortcut 1, check pitch against relative distance between points 4,1 and points 3,2
	float dist41 = dist2d(&sp[4],&sp[1]);
	float dist32 = dist2d(&sp[3],&sp[2]);
	if (dist41 * pitchthreshold < dist32)
		pospitch = true;
	else if (dist32 * pitchthreshold < dist41)
		negpitch = true;
	S32 sbool = sizeof(bool);
#endif
	//matchstable = 0;
	//bigerror = 0;
	pipelinecheckmatch();
	havescanpoints = true;
}

void doar::doresetrottrans()
{
	itrans = pointf3x(0.0f,0.0f,5.0f);
	irot = pointf3x();
	matchstable = 0;
	bigerror = 0;
	matchstep = matchstepmin;
	curdegree = 0;
	havescanpoints = false;
	pipelinecheckmatch();
}

// move closer to the solution
void doar::calcmatchstep()
{
	// look for trends in the error
	// 0
	float cerror = pipelinecheckmatch();
	float fsave = *di[curdegree].ptr;
	// +
	*di[curdegree].ptr = fsave + matchstep;
	float perror = pipelinecheckmatch();
	// -
	*di[curdegree].ptr = fsave - matchstep;
	float merror = pipelinecheckmatch();
	
	// 9 cases: 5 ==, 1 min, 1 max, 2 slope
	if (cerror >= 1e20f)
		itrans.z = (zfar + znear) * .5f;
	if (cerror == perror || cerror == merror) { // same in at least one direction, halve, 5 cases
		*di[curdegree].ptr = fsave;
		matchstep *= saddle;
	} else if (cerror > perror) { // 2 cases
		if (cerror > merror) { // either direction is okay..
			;//error("calc rottrans: rare!, at a maximum, just checking");
		}
		*di[curdegree].ptr = fsave + matchstep;
		matchstep *= grad;
	} else { // cerror < perror
		if (cerror > merror) { // 2 cases
			*di[curdegree].ptr = fsave - matchstep;
			matchstep *= grad;
		} else {
			*di[curdegree].ptr = fsave;
			matchstep *= saddle;
		}
	}
	if (curdegree>=3) // angles
		*di[curdegree].ptr = snormalangrad(*di[curdegree].ptr);
	if (matchstep < matchstepmin) {
		// moving very slowly now
		matchstep = matchstepmin;
		if (cerror > errorthresh) { // converging on wrong solution, start recording this
			++bigerror; // but still to far away from solution
			matchstable = 0; // not stable
			if (bigerror > maxbigerror) { // give up, try random rotation
				// start all over
#if 1
				irot.x = mt_frand() * TWOPI;
				irot.y = mt_frand() * TWOPI;
				irot.z = mt_frand() * TWOPI;
#else // riskier but faster method
				irot.x = -irot.x;
				irot.y = irot.y + PI*.5f;
				itrans.z = (zfar + znear) * .5f;
#endif
				bigerror = 0;
			}
			//*di[curdegree].ptr += matchstep;
		} else {
			++matchstable; // good solution, but keep changing to another degree of freedom
		}
		// switch to another degree of freedom, when converging to wrong solution
		++curdegree;
		if (curdegree >= degrees)
			curdegree = 0;
	} else {
		matchstable = 0; // very sensitive, all matchsteps must be very small
	}
}

bool doar::isdone() 
{
	return matchstable >= maxmatchstable;
}

void doar::getarparseinfo(arparseinfo& arpi)
{
	arpi.arrot = irot;
	arpi.artrans = itrans;
	arpi.matcherror = matcherror;
	arpi.matchstable = matchstable;
	arpi.bigerror = bigerror;
	arpi.matchstep = matchstep;
	arpi.currentstr = string("now doing ") + di[curdegree].name;
//	arpi.pospitch = pospitch;
//	arpi.negpitch = negpitch;
}

const pointf2* doar::getrp()
{
	return rp;
}

const pointf2* doar::getsp()
{
	return sp;
}

// private methods

// convert to 2d using 'division', also clip
bool doar::convertto2d(const pointf3& p3,pointf2& p2)
{
// clip out znear and zfar
	if (p3.w < znear || p3.w > zfar)
		return false;
// do the perspective division
	p2 = pointf2x(p3.x/p3.w,p3.y/p3.w);
	return true;
}

// unoptimized
void doar::buildrottrans3d_cref(const pointf3* rpy,const pointf3* transp,mat4* rotm)
{
	float sn,cs;
// build roll
	mat4 rollm;
	identmat4(&rollm);
	sn = sinf(rpy->z);
	cs = cosf(rpy->z);
	rollm.e[0][0] = cs;
	rollm.e[0][1] = sn;
	rollm.e[1][0] = -sn;
	rollm.e[1][1] = cs;
// build pitch
	mat4 pitchm;
	identmat4(&pitchm);
	sn = sinf(rpy->x);
	cs = cosf(rpy->x);
	pitchm.e[1][1] = cs;
	pitchm.e[1][2] = sn;
	pitchm.e[2][1] = -sn;
	pitchm.e[2][2] = cs;
// build yaw
	mat4 yawm;
	identmat4(&yawm);
	sn = sinf(rpy->y);
	cs = cosf(rpy->y);
	yawm.e[0][0] = cs;
	yawm.e[0][2] = -sn;
	yawm.e[2][0] = sn;
	yawm.e[2][2] = cs;
// build transm
	mat4 transm;
	identmat4(&transm);
	transm.e[3][0] = transp->x;
	transm.e[3][1] = transp->y;
	transm.e[3][2] = transp->z;
// persm
	mat4 persm;
	identmat4(&persm);
	persm.e[0][0] = zoomfactor*asp;
	persm.e[1][1] = zoomfactor;
	float pp = persm.e[2][2] = zfar/(zfar - znear);
	persm.e[2][3] = 1.0f;
	persm.e[3][2] = -znear*pp;
	persm.e[3][3] = 0.0f;
// viewport
	mat4 viewm;
	identmat4(&viewm);
	viewm.e[0][0] = outres.x*.5f;
	viewm.e[1][1] = outres.y*.5f;
	viewm.e[3][0] = outres.x*.5f;
	viewm.e[3][1] = outres.y*.5f;
// multiply together
	mulmat3d(&rollm,&pitchm,rotm);
	mulmat3d(rotm,&yawm,rotm);
	mulmat3d(rotm,&transm,rotm);
	mulmat4(rotm,&persm,rotm);
	mulmat4(rotm,&viewm,rotm);
}

float doar::checkmatch()
{
	S32 i;
	float sum = 0;
	for (i=0;i<npc;++i) {
		float d = dist2dsq(&rp[i],&sp[i]);
		sum += d;
	}
	return sqrtf(sum);
}

// run 3d points all the way to plotter space
// returns true when all points within clip volume
bool doar::pipeline()
{
	S32 i;
	pointf3x rpy = irot;
	pointf3x trans = itrans;
	mat4 rm;
	pointf3 qc;
	pointf2 pr;
// unopimized rottrans
	buildrottrans3d_cref(&rpy,&trans,&rm); // does it all, calc the whole matrix
	for (i=0;i<npc;++i) {
		const pointf3& q = refpoints[i];
		xformvec(&rm,&q,&qc); // transform 
		if (convertto2d(qc,pr)) { // perspective division
			rp[i] = pr;
		} else {
			return false;
		}
	}
	return true;
}

// transform reference points to plotter space, then check for a match
float doar::pipelinecheckmatch()
{
	if (!havescanpoints)
		return 1e20f;
	float m;
	bool passed = pipeline();
	if (passed)
		m = checkmatch();
	else
		m = 1e20f; // can't match
	matcherror = m;
	return m;
}

bool doar::operator () ( const pointf2& a, const pointf2& b ) // use some member variables }
{
	float a_ang = atan2f(a.y-scan_cent.y,a.x-scan_cent.x);
	float b_ang = atan2f(b.y-scan_cent.y,b.x-scan_cent.x);
	return a_ang > b_ang; // go clockwise, normally <
}

/// end doar class

// returns byte data and work done on scan
string readarcode(const string& name,const bitmap32* pic,vector<circleScan>& cirresult,bitmap32** scanpic)
{
	if (*scanpic) {
		bitmap32free(*scanpic);
		//*scanpic = 0;
	}
	*scanpic = bitmap32copy(pic);
	logger("\n\n ------------------------ in readarcode '%s' ----------------------------------\n",name.c_str());
	stringstream ret;
	string scanret;
	cirresult = doscanar(*scanpic,scanret); // analyze raw bitmap return modules (big pixels)
	blendbitmaps(*scanpic,pic,bitmapblendval);
	// done scan
	if (cirresult.empty()) {
		logger("bad scan!\n");
	} else {
		ret << parsearcode(cirresult) << " with " << name << " scan info " << scanret;
	}
	return ret.str();
}
