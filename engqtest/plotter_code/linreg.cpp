#define LINREG
#ifdef LINREG
const S32 MAXPNTS = 5;
S32 npnts = 2;
pointf2 pnts[MAXPNTS];
float lrm,lrb;

void initlinreg()
{
	S32 i;
	for (i=0;i<MAXPNTS;++i) {
		pnts[i].x = (float)i;
		pnts[i].y = (float)i;
	}
}

void drawlinreg()
{
	npnts = range(0,npnts,MAXPNTS);
	S32 i;
	float minx = 1e20f;
	float maxx = -1e20f;
	float sx = 0.0f;
	float sy = 0.0f;
	float sxy = 0.0f;
	float sx2 = 0.0f;
	for (i=0;i<npnts;++i) {
		const pointf2& p = pnts[i];
		if (p.x > maxx)
			maxx = p.x;
		if (p.x < minx)
			minx = p.x;
		sx += p.x;
		sy += p.y;
		sxy += p.x*p.y;
		sx2 += p.x*p.x;
	}
	sx /= npnts;
	sy /= npnts;
	sxy /= npnts;
	sx2 /= npnts;
	lrm = (sxy - sx*sy)/(sx2 - sx*sx);
	lrb = sy - lrm*sx;
	// a little wider
	float dist = (maxx - minx)*.25f;
	maxx += dist;
	minx -= dist;
	pointf2x p1(minx,minx*lrm + lrb);
	pointf2x p2(maxx,maxx*lrm + lrb);
	for (i=0;i<npnts;++i) {
		drawfpoint(pointf2x(pnts[i]),C32RED);
	}
	drawflinec(p1,p2,C32BLACK);
}

#endif
