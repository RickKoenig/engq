#define HOUGHTEST
#ifdef HOUGHTEST

#define USENAMESPACE
#ifdef USENAMESPACE
namespace houghtest {
#endif

pointf2 hp;
float htheta;
pointf2 hq;
float hr;

#ifdef USENAMESPACE
} // namespace houghtest
using namespace houghtest;
#endif

void inithoughtest()
{
	hp = pointf2x(3.0f,2.0f);
	htheta = PI*(1.0f/16.0f);
}

float houghfunc(float a)
{
	return hp.x*cosf(a) + hp.y*sinf(a);
}

pointf2 houghparmfunc(float t)
{
	float a = t * PI;
	float r = houghfunc(a);
	return pointf2x(r*cosf(a),r*sinf(a));
}

void drawhoughtest()
{
	float st = sinf(htheta);
	float ct = cosf(htheta);
	hr = hp.x*ct + hp.y*st;
	hq.x = hr*ct;
	hq.y = hr*st;
	drawfpoint(hp,C32RED);
	drawfpoint(hq,C32CYAN);
	drawfline(pointf2x(0,0),hq,C32GREEN);
	pointf2 del = pointf2x(hp.x - hq.x,hp.y - hq.y);
	pointf2 bp0,bp1;
	float expand = 200.0f;
	bp0.x = hp.x + expand*del.x;
	bp0.y = hp.y + expand*del.y;
	bp1.x = hq.x - expand*del.x;
	bp1.y = hq.y - expand*del.y;
	drawfline(bp0,bp1,C32BLUE);
	drawfunctionrange(houghfunc,0,PI);
	drawfunction2(houghparmfunc);
	drawfpoint(pointf2x(htheta,hr),C32YELLOW);
}

void exithoughtest()
{
}

#endif // HOUGHTEST
