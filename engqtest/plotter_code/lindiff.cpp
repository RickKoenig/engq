#define LINDIFF

// y''(x) + p1*y'(x) + p0*y(x) = i*sin(w*x)
struct lindiff_params {
	// input
	float p1;
	float p0;
	float ypos;
	float yvel;
	// driven
	float amp;
	float w;
	// numerical analysis RK4
	float rkdeltime;
	float rkmaxx;
	bool operator==(const struct lindiff_params& rhs) const
	{
		return  p1 == rhs.p1 &&
				p0 == rhs.p0 &&
				ypos == rhs.ypos &&
				yvel == rhs.yvel &&
				amp == rhs.amp &&
				w == rhs.w &&
				rkdeltime == rhs.rkdeltime &&
				rkmaxx == rhs.rkmaxx;
	}
	bool operator!=(const struct lindiff_params& rhs) const
	{
		return !(*this == rhs);
	}
	// this is for runge kutta, change function
	// void (*changefunc)(float *os,float *delos,float time))
	void operator() (float* os,float* delos,float time)
	{
		delos[0] =  os[1];
		//delos[1] = -os[0];
		delos[1] = -p1*os[1] - p0*os[0] + amp*sinf(w*time);
	}
};

struct lindiff_results {
	// roots
	float r0,r1,i0,i1;
	int root_imag;
	int root_same;
	// is resonant
	bool resonant;
	// coefficients of homogeneous solution
	float c0,c1;
	//yh = e^(r0*x)*(c0*cos(k*x) + c1*sin(k*x)); // root_imag && !root_same
	//yh = c0*e^(r0*x) + c1*e^(r1*x);			// !root_imag && !root_same
	//yh = c0*e^(r0*x) + c1*x*e^(r0*x);			// root_same
	// particular solution of L(y) = a*sin(w*x)
	// coefficients of particular solution
	float u,v;
	// yp = u*cos(w*x)+v*sin(w*x) // !resonant
	// yp = u*x*cos(w*x) // resonant
	// for numerical solutions
	// solution of runge kutta
	vector<pointf2> f;
	float response; // non transient response amplitude to driven amplitude
	float phase; // in degrees
};

#if 0
lindiff_params ldp = {-7.0f,12.0f,0.0f,1.0f,0.0f,1.0f,
	.0625f,20};
#endif
#if 1
lindiff_params ldp = {.0625f,1.0f,0.0f,1.0f,0.0f,1.0f,
	.1f,20};
#endif
#if 0
lindiff_params ldp = {2.0f,0.0f,0.0f,0.0f,0.0f,1.0f};
	.1f,20};
#endif

lindiff_results ldr;

// solve second order LDE
class lindiff {
	lindiff_params p; // input
	lindiff_results r; // output
public:
	lindiff(const lindiff_params& a) :
	  p(a)
	{
		calc();
	}
	bool update(lindiff_params& a)
	{
		const float small = .0078125;
		if (a.rkdeltime < small)
			a.rkdeltime = small;
		if (a.rkmaxx < small)
			a.rkmaxx = small;
		if (a.rkmaxx > 10000.0f * a.rkdeltime)
			a.rkmaxx = 10000.0f * a.rkdeltime;
		if (a != p) {
			p = a;
			calc();
			return true;
		} else {
			return false;
		}
	}
	void showsol()
	{
		logger("\nshowsol with y'' + %6.3f*y' + %6.3f*y = 0\n",p.p1,p.p0);
		if (r.root_same) {
			logger("same roots y = c0*e^(%.3f*x) + c1*x*e^(%.3f*x)\n",r.r0,r.r0);
		} else if (r.root_imag) {
			//logger("imag roots y = c0*e^((%.3f%+.3f*I)*x) + c1*e^((%.3f%+.3f*I)*x)\n",r0,i0,r1,i1);
			logger("imag roots y = e^(%.3f*x)*(c0*cos(%.3f*x) + c1*sin(%.3f*x))\n",r.r0,r.i0,r.i1);
		} else { // !root_same && !root_imag
			logger("diff roots y = c0*e^(%.3f*x) + c1*e^(%.3f)\n",r.r0,r.r1);
		}
	}
	void draw()
	{
		drawfunction2(r.f); // show results of RK4
	}
	float driven(float x)
	{
		return p.amp*sin(p.w*x);
	}
	void getresults(lindiff_results& presults)
	{
		presults = r;
	}
	float fun(float x,bool transient)
	{
		float hom;
		if (r.root_same) {
			hom =expf(r.r0*x)*(r.c0 + r.c1*x);
		} else if (r.root_imag) {
			if (r.resonant) {
				hom = r.c0*cosf(r.i0*x) + r.c1*sinf(r.i0*x);
			} else {
				if (transient) {
					hom = expf(r.r0*x)*(r.c0*cosf(r.i0*x) + r.c1*sinf(r.i0*x));
				} else {
					hom = 0.0f; // r.c0*cosf(r.i0*x) + r.c1*sinf(r.i0*x);
				}
			}
		} else { // !root_same && !root_imag
			hom = r.c0*expf(r.r0*x) + r.c1*expf(r.r1*x);
		}
		float par;
		if (r.u == 0.0f && r.v == 0.0f) {
			par = 0.0f;
		} else if (r.resonant) {
			par = r.v*x*cos(p.w*x);
		} else {
			par = r.u*cos(p.w*x) + r.v*sin(p.w*x);
		}
		return hom + par;
		//return par;
		//return hom;
	}
private:
	// test with y'' + y = 0
	static void changefunc(float* os,float* delos,float t)
	{
		delos[0] = os[1];
		delos[1] = -os[0];
	}
	void calc()
	{
		// driven
		float cypos;
		float cyvel;
		r.resonant = false;
		r.response = 0.0f;
		r.phase = 0.0f;
		if (p.w == 0.0f || p.amp == 0.0f) { // no driven
			r.u = 0.0f;
			r.v = 0.0f;
			cypos = p.ypos;
			cyvel = p.yvel;
		} else if (p.w*p.w == p.p0 && p.p1 == 0.0f) { // resonant
			r.u = 0.0f;
			r.v = -p.amp/(2.0f*p.w);
			cypos = p.ypos;
			cyvel = -r.v + p.yvel;
			r.resonant = true;
		} else { // driven, non resonant
			float t0 = p.p0 - p.w*p.w;
			float t1 = p.p1*p.w;
			float den = t0*t0 + t1*t1;
			r.v = p.amp*t0/den;
			r.u = -p.amp*p.p1*p.w/den;
			cypos = -r.u + p.ypos;
			cyvel = -r.v*p.w + p.yvel;
			float bs = p.p0 - p.w*p.w;
			r.response = p.amp/sqrtf(p.p1*p.p1*p.w*p.w+bs*bs);
			r.phase = atan2f(-p.p1*p.w,t0)*PIUNDER180;
		}
		// roots for homogeneous
		float re = -p.p1*.5f;
		float d2 = p.p1*p.p1 - 4.0f*p.p0;
		if (d2 > 0.0f) { // 2 real roots, distinct
			float do2 = sqrtf(d2)*.5f;
			r.r0 = re + do2;
			r.r1 = re - do2;
			r.i0 = r.i1 = 0.0f;
			r.root_same = false;
			r.root_imag = false;
			float amb = 1.0f/(r.r0 - r.r1); // a - b
			r.c0 = (cyvel - r.r1*cypos)*amb;
			r.c1 = (r.r0*cypos - cyvel)*amb;
		} else if (d2 == 0.0f) { // 2 real roots, that are the same
			r.r0 = r.r1 = re;
			r.i0 = r.i1 = 0.0f;
			r.root_same = true;
			r.root_imag = false;
			r.c0 = cypos;
			r.c1 = cyvel - r.r0*cypos;
		} else { // d2 < 0.0f // 2 complex roots, conjugates of each other
			float do2i = sqrtf(-d2)*.5f;
			r.r0 = r.r1 = re;
			r.i0 = do2i;
			r.i1 = -do2i;
			r.root_same = false;
			r.root_imag = true;
			r.c0 = cypos;
			r.c1 = (cyvel - r.r0*cypos)/r.i0;
		}
		// numerical method
#if 0
		float y = 0.0f;
		float v = 0.0f;
		float a = 0.0f;
		float x = 0.0f;
		r.f.clear();
		int i;
 		for (i=0;i<=p.rknsteps;++i) {
			x = p.rkdeltime*float(i);
			y = x*3.0f;
			r.f.push_back(pointf2x(x,y));
		}
#else
		{
			r.f.clear();
			float x;
			const int no2 = 2;
			float os2[no2] = {p.ypos,p.yvel};
			int of2[no2] = {0,1};
			for (x=0.0f;x<=p.rkmaxx;x+=p.rkdeltime) {
				r.f.push_back(pointf2x(x,os2[0]));
				// dorungekutta(os2,of2,no2,x,p.rkdeltime,changefunc);
				dorungekutta(os2,of2,no2,x,p.rkdeltime,&p);
			}
		}
#endif
	}
};
// test 2

lindiff lin0(ldp);

// runge kutta
// test1 y' = y, y = c1*e^t c1 = 1, so y = e^t
void cf1(float* os,float* delos,float t)
{
	delos[0] = os[0];
}
// test2 y'' = -y, y = c1*cos(t) + c2*sin(t), c1 = 1 and c2 = 0, so y = cos(t)
void cf2(float* os,float* delos,float t)
{
	delos[0] = os[1];
	delos[1] = -os[0];
}

void rktest()
{
	logger("start rktest !!!!\n");

	float ts = .125f;
	float maxt = 2.0f;

// test1
	const int no1 = 1;
	float os1[no1] = { 1.0f };
	int of1[no1] = { 0 };
	float t = 0.0f;
	logger("\n");
	for (;t < maxt + maxt * .5f*ts;t += ts) {
		float y = expf(t);
		float rky = os1[0];
		logger("%t1 = %8.5f, y = %8.5f, rky = %8.5f, diff = %8.5f\n",t,y,rky,y-rky);
		dorungekutta(os1,of1,no1,t,ts,cf1);
	}

// test 2
	const int no2 = 2;
	float os2[no2] = { 1.0f, 0.0f};
	int of2[no2] = { 0 , 1};
	t = 0.0f;
	logger("\n");
	for (;t < maxt + maxt * .5f*ts;t += ts) {
		float y = cosf(t);
		float rky = os2[0];
		logger("%t2 = %8.5f, y = %8.5f, rky = %8.5f, diff = %8.5f\n",t,y,rky,y-rky);
		dorungekutta(os2,of2,no2,t,ts,cf2);
	}

	logger("finish rktest !!!!\n");

}

lindiff_params lindiff_scenarios[] = {
	{-7.0,12.0,0.0,0.0,0.0,0.0,.125f,10.0f},
	{-6.0,9.0,0.0,0.0,0.0,0.0,.125f,10.0f},
	{-1.0,9.0,0.0,0.0,0.0,0.0,.125f,10.0f},
	{0.0,9.0,0.0,0.0,0.0,0.0,.125f,10.0f},
};

const int NLDP = sizeof(lindiff_scenarios)/sizeof(lindiff_scenarios[0]);

// test functors
class funobj
{
public:
	funobj(float a) : factor(a) {}
	float operator() (float v) { return factor*3.0f*v; }
private:
	float factor;

};

float perimeter_square(float s)
{
	return 4.0f*s;
}

float perimeter_circle(float r)
{
	return TWOPI*r;
}

#if 1
template<typename F>
float perimeter(F fun,float v)
{
	return fun(v);
}
#else
float perimeter(float (*fun)(float),float v)
{
	return fun(v);
}
#endif

void dolindiff()
{
	int i;
// test functors
	float (*funs[2])(float) = {perimeter_square,perimeter_circle};
	float parms[2] = {3,4};
	for (i=0;i<2;++i)
		//logger("calling fun with %f, result %f\n",parms[i],funs[i](parms[i]));
		logger("calling fun with %f, result %f\n",parms[i],perimeter(funs[i],parms[i]));
	logger("calling fun with %f, result %f\n",11.0f,perimeter(perimeter_square,11.0f));
	funobj fo(1.1f);
	logger("calling functor with %f, result %f\n",19.0f,perimeter(fo,11.0f));

	rktest();
	logger("-|- -|- -|- begin lindiff!!!!!!!\n");
	for (i=0;i<NLDP;++i) {
		lin0.update(lindiff_scenarios[i]);
		lin0.showsol();
	}
	logger("-|- -|- -|- end lindiff!!!!!!!\n");
}

float linfun_trans(float x)
{
	return lin0.fun(x,true);
}

float linfun_notrans(float x)
{
	return lin0.fun(x,false);
}

float linfun_driven(float x)
{
	return lin0.driven(x);
}

void drawlindiff()
{
	if (lin0.update(ldp))
		lin0.showsol();
	lin0.getresults(ldr);
	C32 fcs = funccolor;
	drawfunction(linfun_trans);
	funccolor = C32BLUE;
	drawfunction(linfun_notrans);
	if (ldr.response != 0.0f) {
		pointf2x lf(-200.0f,ldr.response);
		pointf2x rt( 200.0f,ldr.response);
		drawfline(lf,rt,funccolor);
	}
	funccolor = C32GREEN;
	if (ldp.amp != 0.0f) {
		pointf2x lf(-200.0f,ldp.amp);
		pointf2x rt( 200.0f,ldp.amp);
		drawfline(lf,rt,funccolor);
	}
	drawfunction(linfun_driven);
	funccolor = C32RED;
	lin0.draw(); // numerical difference method
	funccolor = fcs;
}
