#define ROTRECT
#ifdef ROTRECT

void do_doppler()
{
	logger("do doppler\n");
	float fc;
	float fe = 1;
	float c = 15;
	float vw;
	float vsep = 10;
	for (vw=0;vw<=10;vw+=.0625) {
		float ve = vw - vsep;
		fc = (c - vw)*(c + ve)/((c + vw)*(c - ve));
		logger("vw = %8.5f, fc = %8.5f\n",vw,fc);
	}
}

struct rotrect_parms {
	float width;
	float height;
	float rot;
	float diag;
	float factor; // how much to make square/rectangle bigger
	// right now just a square
};

rotrect_parms rrp;

void rotrect_init()
{
	// test some doppler
	do_doppler();
	rrp.width = 4;
	rrp.height = 4;
	rrp.rot = 0;
	rrp.factor = 1.5f;
}

pointf2 doRotate(pointf2& in,float rot)
{
	float rsin = sinf(rot);
	float rcos = cosf(rot);
	pointf2 out;
	out.x =  rcos*in.x + rsin*in.y;
	out.y = -rsin*in.x + rcos*in.y;
	return out;
}

void rotrect_draw()
{
	rrp.diag = sqrtf(float(rrp.width*rrp.width + rrp.height*rrp.height));
/*	pointf2x org; // 0,0
	pointf2x p0(3,4);
	pointf2x p1(1,7);
	drawflinec(org,p0,C32BLACK);
	drawflinec(org,p1,C32BLACK);
	drawflinec(p0,p1,C32BLACK); */

	pointf2x r0(rrp.width/2.0f,rrp.height/2.0f);
	pointf2x r1(rrp.width/2.0f,-rrp.height/2.0f);
	pointf2x r2(-rrp.width/2.0f,-rrp.height/2.0f);
	pointf2x r3(-rrp.width/2.0f,rrp.height/2.0f);

	r0 = doRotate(r0,rrp.rot);
	r1 = doRotate(r1,rrp.rot);
	r2 = doRotate(r2,rrp.rot);
	r3 = doRotate(r3,rrp.rot);

	// orignal rotated rectangle
	drawflinec(r0,r1,C32GREEN);
	drawflinec(r1,r2,C32GREEN);
	drawflinec(r2,r3,C32GREEN);
	drawflinec(r3,r0,C32GREEN);

	// figure out factor from rotation, 0 degrees factor 1, 45 degrees factor sqrt(2) etc.
	//float ifi = cosf(rrp.rot);
//#define RR_OLD
#define RR_NEW
#ifdef RR_OLD
	float ifi = fabs(cosf(rrp.rot)) + fabs(sinf(rrp.rot));
	rrp.factor = ifi;//1.0f / ifi;
	// stamp rectangle
	float largeWidth = rrp.factor * rrp.width;
	float largeHeight = rrp.factor * rrp.height;
#endif
#ifdef RR_NEW
	// stamp rectangle
	float largeWidth = sqrtf(rrp.width*rrp.width + rrp.height*rrp.height);
	float largeHeight = largeWidth;
#endif

	pointf2x stamp0(-largeWidth/2,-largeHeight/2);
	pointf2x stamp1(largeWidth/2,largeHeight/2);
	drawbox2(stamp0,stamp1,C32DARKGRAY);
}

#endif
