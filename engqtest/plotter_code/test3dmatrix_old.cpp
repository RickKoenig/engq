#include "../u_ar_scan.h"
#include "../u_ar_parse.h"

#define TEST3DMATRIX
#ifdef TEST3DMATRIX

//pointf3 arrot = {0.0f,0.0f,0.0f};
//pointf3 artrans = {0.0f,0.0f,3.0f};

doar* anar;

arparseinfo arpi;

S32 resetrottrans = 0;
S32 calcmatch; // how many to calc
S32 fastcalcmatch; // how many to calc in 1 frame
//S32 matchstable;
//S32 bigerror;
const S32 maxbigerror = 500; // mix it up
S32 curdegrees;
//string currentstr = "doing transx";
const float matchstepmin = .0001f;
//float matchstep = matchstepmin;
const float saddle = .6f; // tighten search
const float grad = 1.2f; // loosen search
const S32 degrees = 6;
S32 curdegree = 2;
const float errorthresh = .05f;

deginfo di[degrees] = {
	{"transx",&arpi.artrans.x},
	{"transy",&arpi.artrans.y},
	{"transz",&arpi.artrans.z},
	{"rotx",&arpi.arrot.x},
	{"roty",&arpi.arrot.y},
	{"rotz",&arpi.arrot.z},
};

float xres = 4.0f;
float yres = 3.0f;
float scanxres = 640.0f;
float scanyres = 480.0f;
float matcherror;
float asp = yres/xres;
float zoomfactor = 3.583f;//.2775f;
float znear = .125f;
float zfar = 25.0f;

// which scan to analyze
S32 scanidx = 0;

const S32 npc = 5;
// scanned points, assume 640 by 480

const pointf2 qrcode00017[npc] = {
   {186, 77},
   {477, 267},
   {149, 238},
   {316, 91},
   {441, 105},
};

const pointf2 qrcode00018[npc] = {
   {173, 89},
   {280, 91},
   {176, 306},
   {388, 299},
   {383, 93},
};

const pointf2 qrcode00019[npc] = {
   {99, 50},
   {300, 261},
   {95, 257},
   {303, 68},
   {205, 59},
};

const pointf2 qrcode00020[npc] = {
   {291, 126},
   {100, 418},
   {139, 125},
   {477, 412},
   {440, 132},
};

const pointf2 qrcode00021[npc] = {
   {455, 50},
   {402, 419},
   {503, 336},
   {592, 258},
   {250, 211},
};

const pointf2 qrcode00022[npc] = {
   {53, 70},
   {20, 314},
   {386, 310},
   {340, 83},
   {201, 78},
};

const pointf2 qrcode00023[npc] = {
   {330, 28},
   {194, 68},
   {142, 362},
   {501, 220},
   {37, 110},
};

const pointf2 qrcode00024[npc] = {
   {13, 226},
   {359, 450},
   {79, 413},
   {53, 331},
   {383, 273},
};

const pointf2 qrcode00026[npc] = {
   {448, 15},
   {17, 160},
   {91, 32},
   {592, 130},
   {501, 62},
};

// list of scanned points
const pointf2* scannedpointsets[] =
{
	qrcode00017,
	qrcode00018,
	qrcode00019,
	qrcode00020,
	qrcode00021,
	qrcode00022,
	qrcode00023,
	qrcode00024,
	qrcode00026,
};

const S32 numscans = NUMELEMENTS(scannedpointsets);

const pointf3 refpoints[npc] = {
	{ 0, 1},
	{ 1, 1},
	{ 1,-1},
	{-1,-1},
	{-1, 1},
};


// convert to 2d using 'division', also clip
bool convertto2d(const pointf3& p3,pointf2& p2)
{
// clip out znear and zfar
	if (p3.w < znear || p3.w > zfar)
		return false;
// do the perspective division
	p2 = pointf2x(p3.x/p3.w,p3.y/p3.w);
	return true;
}

// unoptimized
void buildrottrans3d_cref(const pointf3* rpy,const pointf3* transp,mat4* rotm)
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
	viewm.e[0][0] = xres*.5f;
	viewm.e[1][1] = yres*.5f;
	viewm.e[3][0] = xres*.5f;
	viewm.e[3][1] = yres*.5f;
// multiply together
	mulmat3d(&rollm,&pitchm,rotm);
	mulmat3d(rotm,&yawm,rotm);
	mulmat3d(rotm,&transm,rotm);
	mulmat4(rotm,&persm,rotm);
	mulmat4(rotm,&viewm,rotm);
}

pointf2 scan_cent;
//S32 lastscanidx = -1;
bool pointcomp(const pointf2& a,const pointf2& b)
{
	float a_ang = atan2f(a.y-scan_cent.y,a.x-scan_cent.x);
	float b_ang = atan2f(b.y-scan_cent.y,b.x-scan_cent.x);
	return a_ang > b_ang; // go clockwise, normally <
}

// run 3d points all the way to plotter space
// returns true when all points within clip volume
bool pipeline(const pointf3* inpoints,pointf2* outpoints)
{
	S32 i;
	pointf3x rpy = arpi.arrot;
	pointf3x trans = arpi.artrans;
	mat4 rm;
	pointf3 qc;
	pointf2 pr;
// unopimized rottrans
	buildrottrans3d_cref(&rpy,&trans,&rm); // does it all, calc the whole matrix
	for (i=0;i<npc;++i) {
		const pointf3& q = inpoints[i];
		xformvec(&rm,&q,&qc); // transform 
		if (convertto2d(qc,pr)) { // perspective division
			outpoints[i] = pr;
		} else {
			return false;
		}
	}
	return true;
}

float checkmatch(const pointf2* rp, const pointf2* sp)
{
	S32 i;
	float sum = 0;
	for (i=0;i<npc;++i) {
		float d = dist2dsq(&rp[i],&sp[i]);
		sum += d;
	}
	return sqrtf(sum);
}

// transform reference points to plotter space, then check for a match
float pipelinecheckmatch(const pointf3* refpoints,pointf2* rp,const pointf2* sp)
{
	float m;
	bool passed = pipeline(refpoints,rp);
	if (passed)
		m = checkmatch(sp,rp);
	else
		m = 1e20f; // can't match
	return m;
}

// move closer to the solution
void calcmatchstep(const pointf2* sp,pointf2* rp)
{
	// look for trends in the error
	float cerror = pipelinecheckmatch(refpoints,rp,sp);
	float fsave = *di[curdegree].ptr;
	*di[curdegree].ptr = fsave + arpi.matchstep;
	float perror = pipelinecheckmatch(refpoints,rp,sp);
	*di[curdegree].ptr = fsave - arpi.matchstep;
	float merror = pipelinecheckmatch(refpoints,rp,sp);
	
	// 9 cases
	if (cerror >= 1e20f)
		arpi.artrans.z = (zfar + znear) * .5f;
	if (cerror == perror || cerror == merror) { // same in at least one direction, halve, 5 cases
		*di[curdegree].ptr = fsave;
		arpi.matchstep *= saddle;
	} else if (cerror > perror) { // 2 cases
		if (cerror > merror) { // either direction is okay..
			;//error("calc rottrans: rare!, at a maximum, just checking");
		}
		*di[curdegree].ptr = fsave + arpi.matchstep;
		arpi.matchstep *= grad;
	} else { // cerror < perror
		if (cerror > merror) { // 2 cases
			*di[curdegree].ptr = fsave - arpi.matchstep;
			arpi.matchstep *= grad;
		} else {
			*di[curdegree].ptr = fsave;
			arpi.matchstep *= saddle;
		}
	}
	if (arpi.matchstep < matchstepmin) {

		arpi.matchstep = matchstepmin;
		if (cerror > errorthresh) { // converging on wrong solution, start recording this
			++arpi.bigerror;
			arpi.matchstable = 0;
			if (arpi.bigerror > maxbigerror) {
				//error("kicking rotx during '%s'\n",di[curdegree].name);
				// error("kicking it with '%s'\n",di[curdegree].name);
				// start all over
				arpi.arrot.x = mt_frand() * TWOPI;
				arpi.arrot.y = mt_frand() * TWOPI;
				arpi.arrot.z = mt_frand() * TWOPI;
				arpi.bigerror = 0;
			}
			//*di[curdegree].ptr += matchstep;
		} else {
			++arpi.matchstable;
		}
		arpi.currentstr = string("now doing ") + di[curdegree].name;
		// switch to another degree of freedom
		++curdegree;
	
		if (curdegree >= degrees)
			curdegree = 0;

	} else {
		arpi.matchstable = 0;
	}
}

void inittest3dmatrix()
{
	anar = new doar(pointf2x(scanxres,scanyres),pointf2x(xres,yres));
	arpi.arrot = pointf3x();
	arpi.artrans = pointf3x(0.0f,0.0f,3.0f);
	arpi.matchstable = 0;
	arpi.bigerror = 0;
	arpi.matchstep = matchstepmin;
	arpi.currentstr = "start";
}

void drawtest3dmatrix()
{
	S32 i;
// UI adjustments
// limit range of index of scanned points
	if (scanidx >= numscans)
		scanidx = 0;
	else if (scanidx < 0)
		scanidx = numscans - 1;
	if (resetrottrans) {
		arpi.arrot.x = 0.0f;
		arpi.arrot.y = 0.0f;
		arpi.arrot.z = 0.0f;
		arpi.artrans.x = 0.0f;
		arpi.artrans.y = 0.0f;
		arpi.artrans.z = 3.0f;
		resetrottrans = 0;
		curdegree = 0;
		arpi.matchstep = matchstepmin;
	}
	if (calcmatch < 0)
		calcmatch = 0;
	fastcalcmatch = range(1,fastcalcmatch,10000);
	pointf2 rp[npc]; // reference points after transform
	pointf2 sp[npc]; // scanned points

// convert scanned points to plotter space
	float xf = xres/scanxres;
	float yf = yres/scanyres;
	for (i=0;i<npc;++i) {
		sp[i].x = scannedpointsets[scanidx][i].x*xf;
		sp[i].y = (scanyres - scannedpointsets[scanidx][i].y)*yf;
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
	sort(sp,sp+npc,pointcomp);
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

// calc rot and trans by stepping and checking for errors
	for (i=0;i<fastcalcmatch;++i) {
		if (calcmatch > 0) {
			calcmatchstep(sp,rp);
			--calcmatch;
		}
	}

// transform reference points to plotter space
	bool passed = pipeline(refpoints,rp);
	if (passed)
		matcherror = checkmatch(rp,sp);
	else
		matcherror = 1e20f; // can't match

// draw screen borders
	drawbox2(pointf2x(0,0),pointf2x(xres,yres),C32(180,180,255));

// draw scanned plotter points
	for (i=0;i<npc;++i) {
		pointf2 ps = sp[i];
		drawfpoint(ps,C32RED);
		pointi2 pi=math2screen(ps);
		outtextxybf32(B32,pi.x-8,pi.y-8,C32BLACK,C32WHITE,"%d",i);
	}

 // draw reference plotter points
	if (passed) {
		for (i=0;i<npc;++i) {
			pointf2 pr = rp[i];
			drawfpoint(pr,C32GREEN);
			pointi2 pi=math2screen(pr);
			outtextxybf32(B32,pi.x-8,pi.y-8,C32BLACK,C32WHITE,"%d",i);
		}
	}
}

void exittest3dmatrix()
{
	delete anar;
	anar = 0;
}

#endif // TEST3DMATRIX

