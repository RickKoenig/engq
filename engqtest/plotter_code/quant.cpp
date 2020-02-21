#define QUANT
#ifdef QUANT

const float QEPS = 1e-10f;
vector<float> A;// = 1;
vector<float> B;// = 1;
//float oldB = -1;
vector<float> K;// = 1;
vector<float> W;// = 1;
float length = 1;
float oldlength = -1;
S32 qnumber = 0;
//S32 oldqnumber = -1;
float T0 = 6;
int showcalc = 0;
int showsol = 1;
vector<pointf2> ES;
float oldT0 = -1.0f;

#ifdef QUANT
void quantinit()
{
	// solve a simple integral P = sin^2(PI*x) from 0 to 1
	float sum = 0.0f;
	int i,n = 1000;
	float dx = 1.0f/n;
	for (i=0;i<n;++i) {
		float s = sinf(PI*i/n);
		sum += s*s * dx;
	}
	logger("QUANT: integral is %f\n",sum);
}
#endif

float aex(float x,S32 qn)
{
	return A[qn]*expf(K[qn]*x);
}

float aemx(float x,S32 qn)
{
	return A[qn]*expf(-K[qn]*x);
}

float maemx(float x,S32 qn)
{
	return -A[qn]*expf(-K[qn]*x);
}

float bcosx(float x,S32 qn)
{
	return B[qn]*cosf(W[qn]*x);
}

float bmsinx(float x,S32 qn)
{
	return -B[qn]*sinf(W[qn]*x);
}

float rhstan(float t)
{
	if (t < 0.0f)
		return -1.0f;
	float s = T0*T0/(t*t)-1.0f;
	if (s < 0.0f)
		return -1.0f;
	return sqrtf(s);
}

float rhscotan(float t)
{
	if (t < 0.0f)
		return 1.0f;
	float s = T0*T0/(t*t)-1.0f;
	if (s < 0.0f)
		return 1.0f;
	return -sqrtf(s);
}

float lhscotan(float t)
{
	return 1.0f/tanf(t);
}

float eigen_n(float x,S32 n)
{
	if (x < -length*.5f) // left
		return aex(x,n);
	if (x <= length*.5f) { // middle
		if (n&1) // odd
			return bmsinx(x,n);
		return bcosx(x,n); // even
	}
	// right
	if (n&1) // odd
		return maemx(x,n);
	return aemx(x,n); // even
}

float eigen_n2(float x,S32 n)
{
	float a = eigen_n(x,n);
	return a*a;
}

// see if base states are truly orthogonal
void calcarea()
{	
	U32 e0,e1;
	for (e1=0;e1<ES.size();++e1) {
		for (e0=0;e0<ES.size();++e0) {
			const S32 cnt = 10000;
			const float maxx = 10.0f * length;
			float sum = 0.0f;
			S32 j;
			for (j=-cnt;j<cnt;++j) { // just do right half
				float x = maxx * j / cnt;
				float a = eigen_n(x,e0);
				float b = eigen_n(x,e1);
				sum += a*b; // all real
			}
			//sum *= 2.0f;
			float stepsize = maxx / cnt;
			sum *= stepsize;
			logger("area = %f for e0 of %d, e1 of %d\n",sum,e0,e1);
		}
	}
}

// assume f(start) < g(start)
// assume f(end) > g(end)
// and only one crossing
float findintsect(float start,float end,float (*f)(float),float (*g)(float))
{
	float dist = fabsf(end - start);
	float lastdist = 0.0f;
	float x = 0.0f;
	do {
		x = (end + start) * .5f;
		float fx = f(x);
		float gx = g(x);
		float y = fx - gx;
		if (y > 0) {
			end = x;
		} else if (y < 0) {
			start = x;
		} else {
			//logger("findintsect: exact\n");
			return x;
		}
		lastdist = dist;
		dist = fabsf(end - start);

	} while(dist < lastdist);
	return x;
}

void qcalc()
{
	if (ES.empty())
		return;
	A.clear();
	B.clear();
	K.clear();
	W.clear();
	U32 i,j;
	for (i=0;i<ES.size();++i) {
		B.push_back(1.0f);
		float e = 4*ES[i].x*ES[i].x/length/length;
		float e0 = 4*T0*T0/length/length;
		W.push_back(sqrtf(e));
		K.push_back(sqrtf(e0-e));
		//B = 1;
		if (i&1) { // odd
			A.push_back(B[i]*sinf(W[i]*length*.5f)*expf(K[i]*length*.5f));
		} else { // even
			A.push_back(B[i]*cosf(W[i]*length*.5f)*expf(K[i]*length*.5f));
		}
//#define QINF
#ifdef QINF 
		A[i] = 0;
		K[i] = 0;
		W[i] = (i+0)*PI/length;
#endif
		// normalize
		const S32 cnt = 10000;
		const float maxx = 10.0f * length;
		float sum = 0.0f;
		for (j=0;j<cnt;++j) { // just do right half
			float x = maxx * j / cnt;
			float a = eigen_n(x,i);
			sum += a*a;
		}
		sum *= 2.0f; // just the right half
		float stepsize = maxx / cnt;
		sum *= stepsize; // this is the area
		sum = sqrtf(sum);
		A[i] /= sum;
		B[i] /= sum;
	}
}

void drawquant()
{
	static bool first = true;
	if (T0 != oldT0) {
		ES.clear();
		S32 k = 0;
		while(true) {
			bool bail = false;
			float left = k*PI + QEPS;
			float right = k*PI + .5f*PI - QEPS;
			if (left > T0)
				break;
			if (right > T0) {
				right = T0 - QEPS;
				bail = true;
			}
			float ix = findintsect(left,right,tanf,rhstan);
			pointf2x p = pointf2x(ix,tanf(ix));
			ES.push_back(p);
			if (bail)
				break;
			left = k*PI + .5f*PI + QEPS;
			right = k*PI + PI - QEPS;
			if (left > T0)
				break;
			if (right > T0) {
				right = T0 - QEPS;
				bail = true;
			}
			ix = findintsect(right,left,lhscotan,rhscotan); // a trick with the right and left
			p = pointf2x(ix,lhscotan(ix));
			ES.push_back(p);
			if (bail)
				break;
			++k;
		}
	}
	qnumber = range(0,qnumber,(S32)ES.size()-1);
	if (/* oldqnumber != qnumber || */oldlength != length || oldT0 != T0 /*|| oldB != B*/) {
		qcalc();
		//oldqnumber = qnumber;
		oldlength = length;
		oldT0 = T0;
		//oldB = B;
	}
	if (first) {
		calcarea();
#if 0
		float f;
		for (f=0;f<=length*10.0f;f+=.1f) {
			float a = eigen_n(f,0);
			logger("eigen_n(%f,0) = %f\n",f,a);
		}
#endif
		first = false;
	}
	if (showcalc) {
		//drawfunction(aex);
		// evens
		drawfunction(tanf);
		drawfunction(rhstan);
		// odds
		drawfunction(lhscotan);
		drawfunction(rhscotan);
	//	drawfpoint(pointf2x(3.0f,4.0f),C32GREEN);
		S32 i,n = ES.size();
		for (i=0;i<n;++i) {
			S32 rad;
			if (i == qnumber) {
				rad = 10;
			} else {
				rad = 3;
			}
			drawfcircle(ES[i],C32CYAN,rad);
			drawfcircle(pointf2x(ES[i].x,0.0f),C32RED,rad);
		}
		for (i=-100;i<=100;++i) {
			pointf2x p0(i*PI*.5f,.25f);
			pointf2x p1(i*PI*.5f,-.25f);
			pointf2x pt(i*PI*.5f,0.0f);
			drawfline(p0,p1,C32RED);
			pointi2 pi = math2screen(pt);
			outtextxybf32(B32,pi.x-12,pi.y+16,C32BLACK,C32LIGHTCYAN,"%3.1fPI",i*.5f);
		}
	}
	if (showsol && ES.size()) {
		drawfline(pointf2x(-length*.5f,-5),pointf2x(-length*.5f,5),C32GREEN);
		drawfline(pointf2x(length*.5f,-5),pointf2x(length*.5f,5),C32GREEN);

		drawfunction_n(eigen_n,qnumber,C32RED);
		drawfunction_n(eigen_n2,qnumber,C32LIGHTBLUE);
	}
}

#endif
