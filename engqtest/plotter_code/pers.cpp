#define PERS

// world space
pointf2 p0 = {-4.0,4.0};
pointf2 p1 = { 3.0,3.0};
pointf2 p;
float t = .25f;
// screen space
pointf2 r0 = {0.0,1.0};
pointf2 r1 = {0.0,1.0};
pointf2 r = {0.0,1.0};
float s = .25f;


void initpers()
{
}

void drawpers()
{
	pointf2x org; // 0,0
	drawflinec(org,p0,C32BLACK);
	drawflinec(org,p1,C32BLACK);
	drawflinec(p0,p1,C32BLACK);
	p.x = p0.x + t*(p1.x - p0.x);
	p.y = p0.y + t*(p1.y - p0.y);
	drawflinec(org,p,C32LIGHTGREEN);
	r0.x = p0.x/p0.y;
	r1.x = p1.x/p1.y;
	drawflinec(r0,r1,C32BLACK);
	//s = ((p0.x + (p1.x - p0.x)*t)/(p0.y + (p1.y - p0.y)*t) - p0.x/p0.y)/(p1.x/p1.y - p0.x/p0.y);
	//s = t;
	//s = t*p1.y/(p0.y + t*(p1.y - p0.y));
	t = p0.y*s/(p1.y + s*(p0.y - p1.y));
	r.x = r0.x + s * (r1.x - r0.x);
	drawflinec(org,r,C32GREEN);
}

void exitpers()
{
}
