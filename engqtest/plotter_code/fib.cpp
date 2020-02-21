#define FIB

class fibber {
	S64 a0,a1,q0,q1;
	S32 num;
	double b0,b1,c0,c1;
	double b,g,h;
	vector<S64> fibs;
	bool sameRoots;
	static const double eps;
public:
	fibber(S64 aa0,S64 aa1,S64 aq0,S64 aq1,S32 anum) :
		a0(aa0),a1(aa1),q0(aq0),q1(aq1),num(anum),fibs(anum),sameRoots(false)
	{
		// table for fibsum
		S32 i;
		for (i=0;i<num;++i) {
			if (i == 0)
				fibs[0] = a0;
			else if (i == 1)
				fibs[1] = a1;
			else
				fibs[i] = q0*fibs[i-1] + q1*fibs[i-2];
		}
		// roots for fibpow
		double d = q0*q0+4.0*q1;
		b0 = (q0 - sqrt(d))/2.0f;
		b1 = q0 - b0;

		if (abs(d) <= eps) {
			// found identical roots, spread them out
			b = b0;
			b0 -= eps;
			b1 += eps;
			sameRoots = true;
#if 0
			//c1 = (a1 - a0*(b - eps))/(2.0*eps);
			c1 = (a1 - a0*b + a0*eps)/(2.0*eps);
			//c0 = a0 - c1;
			//c0 = (2.0*eps*a0 - a1 + a0*b - a0*eps)/(2.0*eps);
			c0 = (-a1 + a0*b + a0*eps)/(2.0*eps);
#else
			g = a0/2.0;
			h = (-a1 + a0*b)/(2.0*eps);
			c0 = g + h;
			c1 = g - h;
#endif
		} else {
			c1 = (a1 - a0*b0)/(b1 - b0);
			c0 = a0 - c1;
		}
	}

	S64 fibsum(S32 i)
	{
		return fibs[i];
	}

	double fibpow(S32 i)
	{
//		if (false) {
		if (sameRoots) {
//			return 2.0*g*pow(b,double(i)) - 2.0*h*eps*double(i)*pow(b,double(i-1));
//			return a0*pow(b,double(i)) - 2.0*h*eps*double(i)*pow(b,double(i-1));
//			return a0*pow(b,double(i)) - (-a1 + a0*b)*double(i)*pow(b,double(i-1));
			return a0*pow(b,double(i)) + a1*double(i)*pow(b,double(i-1)) - a0*double(i)*pow(b,double(i));
		} else {
			return c0*pow(b0,double(i)) + c1*pow(b1,double(i));
		}
	}

	void showfibs()
	{
		logger("showfibs with a0 = %lld, a1 = %lld, q0 = %lld, q1 = %lld\n",a0,a1,q0,q1);
		logger("%f * %f ^ n + %f * %f ^ n\n",c0,b0,c1,b1);
		if (sameRoots)
			logger("SameRoots\n");
		S32 i;
		for (i=0;i<num;++i) {
			logger("fib of %2d is 'fibsum' %12lld or 'fibpow' %17.6f\n",i,fibsum(i),fibpow(i));
		}
	}
};

const double fibber::eps = .1;

void dofib()
{
	logger("doing fib!!!!!!!\n");
	setfpuroundprec(FPUCW_RND_NEAR,FPUCW_PREC_80);
	fibber f0(1,1,1,1,10);
	f0.showfibs();
	f0 = fibber(2,5,3,4,10);
	f0.showfibs();
	f0 = fibber(2,3,4,5,10);
	f0.showfibs();
	f0 = fibber(2,4,5,7,10);
	f0.showfibs();
	f0 = fibber(1,1,5,-6,10);
	f0.showfibs();
	// same root
	f0 = fibber(3,5,6,-9,10);
	f0.showfibs();
	//defaultfpucontrol();
}