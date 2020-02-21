#define CUBIC
#ifdef CUBIC
#define MAXROOTS 5
#define MAXPERMS 120
// roots and symmetry
pointf2 roots[5] = {
	{.25f,0},
	{2.25f,0},
	{.5f,0},
	{.75f,0},
	{.95f,0}
};

pointf2 res[MAXROOTS][MAXPERMS];

pointf2 respow[MAXROOTS][MAXPERMS];

float rootsep = .02f;
S32 permnum = -1;
S32 resnum = -1;
S32 drawpow = 1;
S32 nroots = 3;
S32 nperms;
S32 resp = 3; // a power of resolvents
S32 drawroots = 1;
S32 drawres = 1;

U32 ffact(U32 n)
{
	U32 i;
	U32 p = 1;
	for (i=2;i<=n;++i)
		p *= i;
	return p;
}

void calccubic()
{
	nroots = range(0,nroots,MAXROOTS);
	nperms = ffact(nroots);
	complex w[MAXROOTS];
	S32 i,j,k;
	for (i=0;i<nroots;++i) {
		w[i] = complex(cosf(i*TWOPI/nroots),sinf(i*TWOPI/nroots));
	}
//	complex w0 = 1;
//	complex w1(-.5f,sqrtf(3.0f)*.5f);
//	complex w2 = w1.conjugate();
//	static bool once;
//	if (!once) {
//		roots[0] = w0;
//		roots[1] = w1;
//		roots[2] = w2;
//		once = true;
//	}
	S32 perm[MAXROOTS];
	for (i=0;i<nroots;++i)
		perm[i] = i;
	j = 0;
	do {
		for (k=0;k<nroots;++k) {
			complex sum = 0;
			for (i=0;i<nroots;++i) {
				sum = sum + w[(i*k)%nroots]*roots[perm[i]];
			}
			res[k][j] = sum;
		}
		++j;
	} while (next_permutation(perm,perm+nroots));
/*
// e
	res[0][0] = (complex)roots[0] +    roots[1] +    roots[2];
	res[1][0] = (complex)roots[0] + w1*roots[1] + w2*roots[2];
	res[2][0] = (complex)roots[0] + w2*roots[1] + w1*roots[2];
// ab
	res[0][1] = (complex)roots[1] +    roots[0] +    roots[2];
	res[1][1] = (complex)roots[1] + w1*roots[0] + w2*roots[2];
	res[2][1] = (complex)roots[1] + w2*roots[0] + w1*roots[2];
// ac
	res[0][2] = (complex)roots[2] +    roots[1] +    roots[0];
	res[1][2] = (complex)roots[2] + w1*roots[1] + w2*roots[0];
	res[2][2] = (complex)roots[2] + w2*roots[1] + w1*roots[0];
// bc
	res[0][3] = (complex)roots[0] +    roots[2] +    roots[1];
	res[1][3] = (complex)roots[0] + w1*roots[2] + w2*roots[1];
	res[2][3] = (complex)roots[0] + w2*roots[2] + w1*roots[1];
// abc
	res[0][4] = (complex)roots[1] +    roots[2] +    roots[0];
	res[1][4] = (complex)roots[1] + w1*roots[2] + w2*roots[0];
	res[2][4] = (complex)roots[1] + w2*roots[2] + w1*roots[0];
// acb
	res[0][5] = (complex)roots[2] +    roots[0] +    roots[1];
	res[1][5] = (complex)roots[2] + w1*roots[0] + w2*roots[1];
	res[2][5] = (complex)roots[2] + w2*roots[0] + w1*roots[1]; */
	for (i=0;i<nperms;++i) {
		for (j=0;j<nroots;++j) {
			complex p = 1;
			for (k=0;k<resp;++k)
				p = p * (complex)res[j][i];
			respow[j][i] = p;
		}
	}
}
void drawcubic()
{
#define NCOLORS 6
	C32 colors[NCOLORS]={C32(255,205,155),C32LIGHTRED,C32LIGHTGREEN,C32LIGHTBLUE,C32LIGHTCYAN,C32LIGHTMAGENTA};
	pointf2x zerop;
	S32 i;
	if (drawroots)
		for (i=0;i<nroots;++i) {
			drawflinec(roots[i],zerop,C32DARKGRAY);
		}
	for (i=0;i<nperms;++i) {
//		if (permnum>=0 && permnum!=i)
//			continue;
		C32 c1,c2,c3;
		c1 = c2 = c3 = colors[i%NCOLORS];
		c1.r /= 2;
		c1.g /= 2;
		c1.b /= 2;
		c2.r /= 4;
		c2.g /= 4;
		c2.b /= 4;
		c2.r *= 3;
		c2.g *= 3;
		c2.b *= 3;
		float rs = permnum>=0 ? 0 : rootsep;
		S32 j;
		for (j=0;j<nroots;++j) {
			if (resnum<0 || resnum==j) {
				if (permnum<0 || permnum==i) {
					if (/*j>0 && */drawpow)
						drawflinec(pointf2x(respow[j][i].x+rs*i,respow[j][i].y+rs*i),zerop,c3);
					if (drawres)
						drawflinec(pointf2x(res[j][i].x+rs*i,res[j][i].y+rs*i),zerop,c3);
				}
			}
		}
	}
	drawfpoint(pointf2(),C32WHITE);
#if 0
	const float MAXXY=20;
	const S32 NSEG=300;
// draw points
	S32 i;
	for (i=0;i<MAXPNTS;++i) {
		drawfpoint(pnts[i],C32BLACK);
		pointi2 pi;
//		pi=math2screen(pnts[i]);
//		outtextxybf32(B32,pi.x-8,pi.y+10,C32BLACK,C32YELLOW,"P%c",'A'+i);
	}
// draw line from pa to pb
/*	pointf2 p0,p1;
	const float enlarge=1.5f;
	const float diff=(pnts[1].x-pnts[0].x)*.5f;
	const float sum= (pnts[1].x+pnts[0].x)*.5f;
	p0.x=-diff*enlarge+sum;
	p1.x=diff*enlarge+sum;
	p0.y=linm*p0.x+linb;
	p1.y=linm*p1.x+linb; 
	drawfline(p0,p1,C32BLACK);*/
	drawfline(pnts[0],pnts[1],C32BLACK);
#endif
//	drawfunction(sinfun);
//	drawfunction(expe);
//	drawfunction(exp10);
//	drawfunction(exp10le);
//	drawfunction(root3_0);
//	drawfunction(root3_1);
//	drawfunction(root3_2);
//	drawfunction(xs);
//	drawfunction(beercan);
//	drawfunction(quintic);
//	drawfunction(seventh);
//	drawfunction(sinf);
//	drawfunction2(circ);
//	drawfunctionrk2d(objstate,offsets,2,initf,changef);
//	drawfunction(tant);
#if 0
	drawfline(pointf2x(0.0f,0.0f),pointf2x(v1start,0.0f),C32RED);
	drawfline(pointf2x(v1start,0.0f),pointf2x(v1start,v1*vscale),C32RED);
	drawfline(pointf2x(v1start,v1*vscale),pointf2x(v1end,v1*vscale),C32RED);
	drawfline(pointf2x(v1end,v1*vscale),pointf2x(v1end,0.0f),C32RED);
	drawfline(pointf2x(v1end,0.0f),pointf2x(1.0f,0.0f),C32RED);
	drawfunctionrk2d(objstate,offsets,3,initfs,changefs);
#endif
}
#endif
