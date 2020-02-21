//#define INTERSTELLAR
#ifdef INTERSTELLAR
void interstellar()
{
	setfpuroundprec(FPUCW_RND_NEAR,FPUCW_PREC_80);
// conversions
	double fc = 299792458.0f;			// standard speed of light	m/s
	double vc = fc * 1;				// variable value for c
	double ly = 4.0f;				// light year in c*spy		m	
	double spy = 60*60*24*365.24f;	// seconds per year			s/y
// units in seconds
	double fly = ly * spy * fc;		// fixed light year in		s
	double vly = ly * spy * vc;		// variable light year in	s
	double g = 9.80665f;// / fc;		// 1/s
	//float t = 3;	// seconds of trip
//	double t = 1.775f * spy; // travel proper time in seconds, first quarter only (20 years)
//	double t = 10.0f * spy; // travel proper time in seconds, first quarter only (20 years)
	double t = 14.25f * spy; // travel proper time in seconds, first quarter only (20 years)
	double x = .5f*g*t*t;
	double gs = g/vc;
	double in = cosh(gs*t);
	//logger("in = %20g\n",in);
	double x2 = vc/gs*(in - 1);
	x = x / fc / spy; // convert meters to light years (fixed c)
	x2 = x2 / fc / spy;
	logger("time (years) = %9.8g, dist (fixed light years) = %9.8g\n",t/spy,x);
	logger("time (years) = %9.8g, dist2 (fixed light years) = %9.8g\n",t/spy,x2);
	defaultfpucontrol();
}
#endif

