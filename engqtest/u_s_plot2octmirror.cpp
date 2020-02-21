// first test of plotter2
#include <float.h>

#include <m_eng.h>

#include "u_plotter2.h"

using namespace u_plotter2;

#define DONAMESPACE
#ifdef DONAMESPACE
namespace octMirror {
#endif

const S32 OCT{ 8 };
S32 maxRays{ 1 };
S32 refRays;
float angMirDeg = 11.25f;
struct aline {
	pointf2 p0, p1;
};
float halfLen;
float beyond = 20; // when ray escapes, how far out to go before it dies

aline lines[OCT];
aline& turnLine = lines[2];
vector<aline> rays;
pointf2 startRay{ 0,.25f };


// for debvars
struct menuvar octmirrordv[] = {
	{"@lightgreen@--- octmirror ---",NULL,D_VOID,0},
	{"ang mir deg",&angMirDeg,D_FLOAT,FLOATUP / 4},
	{"startRayX",&startRay.x,D_FLOAT,FLOATUP / 16},
	{"startRayY",&startRay.y,D_FLOAT,FLOATUP / 16},
	{"maxRays",&maxRays,D_INT},
	{"refRays",&refRays,D_INT|D_RDONLY},
	{"beyond",&beyond,D_FLOAT,FLOATUP / 16},
};
const int noctmirrordv = NUMELEMENTS(octmirrordv);

// A*t + B*u = C and D*t + E*u = F
bool solveTwoEquations(float A, float B, float C, float D, float E, float F, float& t, float& u)
{
	float d = A * E - B * D;
	if (abs(d) < EPSILON)
		return false;
	float id = 1.0f / d;
	t = id * (C*E - B * F);
	u = id * (A*F - C * D);
	return true;
}

pointf2 reflect(const pointf2& inRay, const pointf2&perp)
{
	float k = dot2d(&perp, &inRay) / dot2d(&perp, &perp);
	pointf2 outRay;
	outRay.x = 2 * k*perp.x - inRay.x;
	outRay.y = 2 * k*perp.y - inRay.y;
	return outRay;
}

// check ray against the lines, extends inRay
bool bounceRay(aline& inRay,aline& outRay)
{
	// first find intsect
	float a = inRay.p1.x - inRay.p0.x;
	float d = inRay.p1.y - inRay.p0.y;
	float bt = -1;
	//float bu = -1;
	aline* bl = nullptr;
	for (auto& line : lines) {
		float b = line.p0.x - line.p1.x;
		float c = line.p0.x - inRay.p0.x;
		float e = line.p0.y - line.p1.y;
		float f = line.p0.y - inRay.p0.y;
		float t, u;
		bool did = solveTwoEquations(a, b, c, d, e, f, t, u);
		if (did && t > .0001f && (t < bt || bt < 0) && u > 0 && u < 1) {
			bt = t;
			//bu = u;
			bl = &line;
		}
	}
	if (bt < 0) {
		//inRay.p1.x *= 10;
		//inRay.p1.y *= 10;
		return false;
	}
	pointf2 isect;
	pointf2 delRay{ inRay.p1.x - inRay.p0.x ,inRay.p1.y - inRay.p0.y };
	isect.x = inRay.p0.x + bt * delRay.x;
	isect.y = inRay.p0.y + bt * delRay.y;

	inRay.p1 = isect;
	// now do reflect
	pointf2 perp{ bl->p0.y - bl->p1.y,bl->p1.x - bl->p0.x };
	pointf2 refl = reflect(delRay, perp);
	normalize2d(&refl, &refl);
	refl.x = -.25f*refl.x;
	refl.y = -.25f*refl.y;

	outRay = { isect,{isect.x+refl.x,isect.y+refl.y} };
	return true;
}

void calcoctmirror()
{
	// turn top line
	pointf2 p{ 0,1 }, p2;
	float ang = PIOVER180*angMirDeg;
	const float ang2 = ang;// +PI / 2;
	p2.x = halfLen * cos(ang2);
	p2.y = halfLen * sin(ang2);
	turnLine.p0.x = p.x + p2.x;
	turnLine.p0.y = p.y + p2.y;
	turnLine.p1.x = p.x - p2.x;
	turnLine.p1.y = p.y - p2.y;
	// calc rays
	rays.clear();
	aline aRay{ {0,0},startRay };
	S32 r = 0;
	maxRays = range(0, maxRays, 100);
	while (r < maxRays) {
		aline newRay;
		if (!bounceRay(aRay, newRay)) {
			// extend ray
			aRay.p1.x = beyond * aRay.p1.x - (beyond - 1)*aRay.p0.x;
			aRay.p1.y = beyond * aRay.p1.y - (beyond - 1)*aRay.p0.y;
			break;
		}
		rays.push_back(aRay);
		aRay = newRay;
		++r;
	}
	refRays = r;
	rays.push_back(aRay);
}

void drawoctmirror()
{
	for (S32 i = 0; i < OCT; ++i) {
		drawflinec(lines[i].p0, lines[i].p1,C32RED);
	}
	for (auto& ray : rays) {
		drawflinec(ray.p0, ray.p1, C32GREEN);
	}
}

#ifdef DONAMESPACE
} // end namespace octMirror
using namespace octMirror;
#endif

void plot2octmirrorinit()
{
	adddebvars("octmirror",octmirrordv,noctmirrordv);
	const float TWOOCTANG = PI / 8;
	halfLen = tan(TWOOCTANG);
	halfLen *= .95f;
	for (S32 i = 0; i < OCT; ++i) {
		const float ang = 2*PI*i / OCT;
		pointf2 p,p2;
		p.x = cos(ang);
		p.y = sin(ang);
		const float ang2 = ang + PI / 2;
		p2.x = halfLen*cos(ang2);
		p2.y = halfLen*sin(ang2);
		lines[i].p0.x = p.x + p2.x;
		lines[i].p0.y = p.y + p2.y;
		lines[i].p1.x = p.x - p2.x;
		lines[i].p1.y = p.y - p2.y;
	}
	plotter2init();
}

void plot2octmirrorproc()
{
	// interact with graph paper
	plotter2proc();
	// calc the image
	calcoctmirror();
}

void plot2octmirrordraw2d()
{
	// draw graph paper
	plotter2draw2d();
	drawoctmirror();
}

void plot2octmirrorexit()
{
	// free debvars, write out colors to plotter.bin
	plotter2exit();
	removedebvars("octmirror");
}
