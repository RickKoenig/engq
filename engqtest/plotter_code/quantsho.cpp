#define QUANTSHO
#ifdef QUANTSHO

#if 0
typedef float QF;
#define factQ factf
#define powQ powf
#define sqrtQ sqrtf
#define expQ expf
#else
typedef double QF;
#define factQ factd
#define powQ pow
#define sqrtQ sqrt
#define expQ exp
#endif
#ifdef QUANTSHO
void calcherm();

#if 0
extern U8 fontdata[];

// compare 2 different fonts
void testFonts()
{
	logger("testing fonts\n");
	S32 match = 0;
	S32 mismatch = 0;
	S32 fudge = 0; // offset to generate more mismatch errors
	U8* rawFont = fontdata;
	bitmap32* bitmapFont = video3dinfo.sysfont;
	// run through chars 32 to 127
	for (S32 ch=32;ch<128;++ch) {
		S32 bx = ch & 0xf; // % 16
		S32 by = ch >> 4; // / 16
		bx *= 8;
		by *= 8;
		by -= fudge;
		U8* cp = rawFont + (ch - 32)*64;
		for (S32 y=0;y<8;++y) {
			for (S32 x=0;x<8;++x) {
				C32 bv = clipgetpixel32(bitmapFont,bx + x,by + y);
				U8 rv = *cp++;
				//logger("bitmap %d %d %d %d, raw %d\n",bv.r,bv.g,bv.b,bv.a,rv);
				if ((bv.c32 != 0) == (rv != 0)) {
					++match;
				} else {
					++mismatch;
				}
			}
		}
	}
	logger("mismatch = %d\n",mismatch);
	logger("match = %d\n",match);
	logger("total = %d\n",mismatch + match);
}
#endif
void quantshoinit()
{
	calcherm();
	//calcarea();
	//testFonts(); // compare fonts
}
#endif

S32 fact(S32 n)
{
	S32 i;
	S32 f = 1;
	for (i=1;i<=n;++i)
		f *= i;
	return f;
}

float factf(S32 n)
{
	S32 i;
	float f = 1;
	for (i=1;i<=n;++i)
		f *= i;
	return f;
}

double factd(S32 n)
{
	S32 i;
	double f = 1;
	for (i=1;i<=n;++i)
		f *= i;
	return f;
}

S32 qnumber = 0;
const U32 ESsize = 58;
QF hp[ESsize][ESsize];
void calcherm()
{
	S32 i,j;
	hp[0][0] = 1.0f;
	hp[1][0] = 0.0f;
	hp[1][1] = 2.0f;
	for (j=1;j<ESsize-1;++j) {
		hp[j+1][0] = -2*j*hp[j-1][0];
		for (i=1;i<=j+1;++i) {
			hp[j+1][i] = 2*hp[j][i-1] - 2*j*hp[j-1][i];
		}
	}
}

#if 1
QF herm(QF x,U32 n)
{
	U32 j;
	QF r = 0.0f;
	const QF* hpn = hp[n];
	for (j=0;j<=n;++j) {
		r *= x;
		r += hpn[n-j];
	}
	return r;
}
#else
QF herm(QF x,U32 n)
{
	switch(n) {
	case 0:
		return 1.0f;
	case 1:
		return 2.0f*x;
	case 2:
		return 4.0f*x*x - 2.0f;
	case 3:
		return x*(8.0f*x*x - 12.0f);
	case 4:
		return x*x*(16.0f*x*x - 48.0f) + 12.0f;
	default:
		errorexit("bad n for herm %d",n);
	}
	return 0.0f;
}
#endif

float eigen_n(float x,S32 n)
{
	QF v = powQ((QF)PI,(QF)-.25f)*expQ(-x*x*.5f)*herm(x,n)/sqrtQ(powQ((QF)2.0f,QF(n))*factQ(n));
	return (float)v;
}

/*QF teigen_n(QF x,S32 n)
{
	QF v = .5f + powf(PI,-.25f)*expf(-x*x*.5f)*therm(x,n)/sqrtf(powf(2.0f,QF(n))*factf(n));
	return v;
}
*/
float eigen_n2(float x,S32 n)
{
	QF a = eigen_n(x,n);
	return (float)(a*a);
}

// see if base states are truly orthogonal
void calcarea()
{	
	//calcherm();
	const float maxx = 20.0f;
	U32 e0,e1;
	for (e1=0;e1<ESsize;++e1) {
		for (e0=0;e0<ESsize;++e0) {
			const S32 cnt = 20000;
			float sum = 0.0f;
			S32 j;
			for (j=-cnt;j<cnt;++j) {
				float x = maxx * j / cnt;
				float a = (float)eigen_n(x,e0);
				float b = (float)eigen_n(x,e1);
				sum += a*b; // all real
			}
			//sum *= 2.0f;
			float stepsize = maxx / cnt;
			sum *= stepsize;
			logger("area = %f for e0 of %d, e1 of %d\n",sum,e0,e1);
		}
		logger("\n");
	}
}

void drawquantsho()
{
	qnumber = range(0,qnumber,(S32)ESsize-1);
	drawfunction_n(eigen_n,qnumber,C32RED);
//	drawfunction_n(teigen_n,qnumber,C32RED);
	drawfunction_n(eigen_n2,qnumber,C32LIGHTBLUE);
#if 0
	LARGEFONT->outtextxybf32(B32,200,200,C32GREEN,C32BLACK,"Hello world!");
	SMALLFONT->outtextxybf32(B32,300,300,C32GREEN,C32BLACK,"Hello world!");
	clipblit32(video3dinfo.sysfont2,B32,0,0,400,400,B32->size.x,B32->size.y);
	drawtextque_string_foreback(LARGEFONT,600,600,F32GREEN,F32MAGENTA,"textque");
#endif
}

#endif
