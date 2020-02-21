// exponent ^ result = z3 where exponent and result and z3 are complex numbers
//#include <complex>

#define POWER
#ifdef POWER

pointf2 base = {4,4};//{2,0};//{-1.25f,0};//{2,0};
pointf2 exponent = {2,-4};//{0,-.5f};//{3,0};
pointf2 basesep = {2,2};
pointf2 exponentsep = {.3f,.3f};
pointf2 /*resulta,resultb,*/resultc/*,resultd*/;
const int pointiter = 1;
const int lineiter = 50;
const float linetest = 0;//.1f;
const float circsize = 4;

/*
// just (a) ^ (c) // real base to real expontent
pointf2 complexPowa(const pointf2& b,const pointf2& e)
{
	pointf2 ret = {0,0};
	ret.x = powf(b.x,e.x);
	return ret;
}

// just (a) ^ (id) , real base to compex exponent
pointf2 complexPowb(const pointf2& b,const pointf2& e)
{
	pointf2 ret = {0,0};
	if (fabs(b.x) <= EPSILON)
		return ret;
	// result = b ^ ix
	// result = e ^ ( (ln(b)*ix )
	float ang = logf(fabs(b.x))*e.y;
	ret.x = cosf(ang);
	ret.y = sinf(ang);
	if (b.x < 0) {
		float epang = expf(-PI*e.y);
		ret.x *= epang;
		ret.y *= epang;
	}
	return ret;
}
*/

pointf2 complexSum(const pointf2& a,const pointf2& b)
{
	pointf2 ret;
	ret.x = a.x + b.x;
	ret.y = a.y + b.y;
	return ret;
}

pointf2 complexMultiply(const pointf2& a,const pointf2& b)
{
	pointf2 ret;
	ret.x = a.x*b.x - a.y*b.y;
	ret.y = a.x*b.y + a.y*b.x;
	return ret;
}

// return e^(it)
pointf2 eiPow(float t)
{
	pointf2 ret;
	ret.x = cosf(t);
	ret.y = sinf(t);
	return ret;
}

// (a + bi) ^ (c + di) , everything, complex base to complex exponent
pointf2 complexPowc(const pointf2& b,const pointf2& e)
{
	pointf2 ret = {0,0};
	// result =  b.x+b.y ^  e.x+e.y
	// result = (a + bi) ^ (c + di)
	// result = (a + bi) ^ c   *   (a + bi) ^ (di)
	// r * e ^ (it) = a + bi
	// r = a*a + b*b
	// t = atan2(b,a)
	float r = sqrtf(b.x*b.x + b.y*b.y);
	if (r < EPSILON)
		return ret;
	float t = atan2f(b.y,b.x);
	// result = (r * (e ^ (it))) ^ c   *   (r * (e ^ (it))) ^ (di)
	// restult = r^c * e^(itc)   *   r^(di) * e^(-td)
	// restult = r^c * e^(itc)   *   e^(-td) * r^(di)
	// restult = r^c * e^(itc)   *   e^(-td) * e^(ln(r)*di)
	// result = e^((ln(r)*c) * e^(itc)   *   e^(-td) * e^(ln(r)*di)
	// result = e^((ln(r)*c + -td) * e^(i(tc + ln(r)*d))
	//float rc = powf(r,e.x);
	pointf2 eitcrdi = eiPow(t*e.x + logf(r)*e.y);
	//float etd = expf(-t*e.y);
	float rctd = expf(logf(r)*e.x - t*e.y);
	ret.x = rctd*eitcrdi.x;
	ret.y = rctd*eitcrdi.y;
	return ret;
}

pointf2 compfunc(const pointf2& a,const pointf2& b)
{
	//return complexSum(a,b);
	//return complexMultiply(a,b);
	return complexPowc(a,b);
}

/*
// (a + bi) ^ (c + di) , everything, complex base to complex exponent, using Complex library
pointf2 complexPowd(const pointf2& b,const pointf2& e)
{
	std::complex<float> Cb(b.x,b.y);
	std::complex<float> Ce(e.x,e.y);
	std::complex<float> Cret = std::pow(Cb,Ce);
	pointf2 ret = {Cret.real(),Cret.imag()};
	return ret;
}
*/

// draw a grid of points and lines
void drawgrid(const pointf2& center,const pointf2& sep,C32 color)
{
	pointf2 steppoint;
	pointf2 stepline;
	steppoint.x = sep.x/pointiter;
	steppoint.y = sep.y/pointiter;
	stepline.x = sep.x/lineiter;
	stepline.y = sep.y/lineiter;

	// draw lots of lines in x
	for (int j=-pointiter;j<=pointiter;++j) { // points
		pointf2 iter;
		iter.x = center.x + -lineiter*stepline.x;
		iter.y = center.y + j*steppoint.y;
		pointf2 last;
		for (int i=-lineiter+1;i<=lineiter;++i) { // lines
			last = iter;
			iter.x = center.x + i*stepline.x;
			pointf2 last2;
			last2.x = last.x;
			last2.y = last.y + linetest;
			drawfline(last2,iter,C32BLACK);
		}
	}

	// draw lots of lines in y
	for (int i=-pointiter;i<=pointiter;++i) { // points
		pointf2 iter;
		iter.y = center.y + -lineiter*stepline.y;
		iter.x = center.x + i*steppoint.x;
		pointf2 last;
		for (int j=-lineiter+1;j<=lineiter;++j) { // lines
			last = iter;
			iter.y = center.y + j*stepline.y;
			pointf2 last2;
			last2.x = last.x + linetest;
			last2.y = last.y;
			drawfline(last2,iter,C32BLACK);
		}
	}

	// draw some points 2D
	for (int j=-pointiter;j<=pointiter;++j) { // points
		pointf2 iter;
		iter.y = center.y + j*steppoint.y;
		for (int i=-pointiter;i<=pointiter;++i) { // points
			iter.x = center.x + i*steppoint.x;
			drawfpoint(iter,color,circsize);
		}
	}
}

void drawgridfunc(const pointf2& base,const pointf2& basesep,const pointf2& exponent,const pointf2& exponentsep,C32 color)
{
	pointf2 basesteppoint;
	pointf2 basestepline;
	pointf2 expsteppoint;
	pointf2 expstepline;
	basesteppoint.x = basesep.x/pointiter;
	basesteppoint.y = basesep.y/pointiter;
	basestepline.x = basesep.x/lineiter;
	basestepline.y = basesep.y/lineiter;
	expsteppoint.x = exponentsep.x/pointiter;
	expsteppoint.y = exponentsep.y/pointiter;
	expstepline.x = exponentsep.x/lineiter;
	expstepline.y = exponentsep.y/lineiter;
#if 1
	// draw lines in exp
	for (int j=-pointiter;j<=pointiter;++j) { // points base.y
		pointf2 biter;
		biter.y = base.y + j*basesteppoint.y;
		for (int i=-pointiter;i<=pointiter;++i) { // points base.x
			biter.x = base.x + i*basesteppoint.x;
			for (int n=-pointiter;n<=pointiter;++n) { // points in exp.y
				pointf2 eiter;
				eiter.x = exponent.x + -lineiter*expstepline.x;
				eiter.y = exponent.y + n*expsteppoint.y;
				pointf2 func = compfunc(biter,eiter);
				pointf2 last;
				for (int m=-lineiter+1;m<=lineiter;++m) { // lines in exp.x
					last = func;
					eiter.x =  exponent.x + m*expstepline.x;
					pointf2 last2;
					last2.x = last.x + linetest;
					last2.y = last.y + linetest;
					func = compfunc(biter,eiter);
					drawfline(last2,func,C32BLACK);
				}
			}
			for (int m=-pointiter;m<=pointiter;++m) { // points in exp.x
				pointf2 eiter;
				eiter.x = exponent.x + m*expsteppoint.x;
				eiter.y = exponent.y + -lineiter*expstepline.y;
				pointf2 func = compfunc(biter,eiter);
				pointf2 last;
				for (int n=-lineiter+1;n<=lineiter;++n) { // lines in exp.y
					last = func;
					eiter.y =  exponent.y + n*expstepline.y;
					pointf2 last2;
					last2.x = last.x + linetest;
					last2.y = last.y + linetest;
					func = compfunc(biter,eiter);
					drawfline(last2,func,C32BLACK);
				}
			}
		}
	}
#endif
#if 1
	// draw lines in base
	for (int j=-pointiter;j<=pointiter;++j) { // points exp.y
		pointf2 eiter;
		eiter.y = exponent.y + j*expsteppoint.y;
		for (int i=-pointiter;i<=pointiter;++i) { // points exp.x
			eiter.x = exponent.x + i*expsteppoint.x;
			for (int n=-pointiter;n<=pointiter;++n) { // points in base.y
				pointf2 biter;
				biter.x = base.x + -lineiter*basestepline.x;
				biter.y = base.y + n*basesteppoint.y;
				pointf2 func = compfunc(biter,eiter);
				pointf2 last;
				for (int m=-lineiter+1;m<=lineiter;++m) { // lines in base.x
					last = func;
					biter.x =  base.x + m*basestepline.x;
					pointf2 last2;
					last2.x = last.x + linetest;
					last2.y = last.y + linetest;
					func = compfunc(biter,eiter);
					drawfline(last2,func,C32BLACK);
				}
			}
			for (int m=-pointiter;m<=pointiter;++m) { // points in base.x
				pointf2 biter;
				biter.x = base.x + m*basesteppoint.x;
				biter.y = base.y + -lineiter*basestepline.y;
				pointf2 func = compfunc(biter,eiter);
				pointf2 last;
				for (int n=-lineiter+1;n<=lineiter;++n) { // lines in base.y
					last = func;
					biter.y =  base.y + n*basestepline.y;
					pointf2 last2;
					last2.x = last.x + linetest;
					last2.y = last.y + linetest;
					func = compfunc(biter,eiter);
					drawfline(last2,func,C32BLACK);
				}
			}
		}
	}
#endif
	// draw some points 4D
	for (int j=-pointiter;j<=pointiter;++j) { // points
		pointf2 biter;
		biter.y = base.y + j*basesteppoint.y;
		for (int i=-pointiter;i<=pointiter;++i) { // points
			biter.x = base.x + i*basesteppoint.x;
			//drawfpoint(biter,color);
			for (int n=-pointiter;n<=pointiter;++n) { // points
				pointf2 eiter;
				eiter.y = exponent.y + n*expsteppoint.y;
				for (int m=-pointiter;m<=pointiter;++m) { // points
					eiter.x = exponent.x + m*expsteppoint.x;
					pointf2 result;
					// func
					result = compfunc(biter,eiter);
					drawfpoint(result,color,circsize);
				}
			}
		}
	}
}

void power_draw()
{
	//resulta = complexPowa(base,exponent);
	//resultb = complexPowb(base,exponent);
	resultc = complexPowc(base,exponent);
	//resultd = complexPowd(base,exponent);
	//drawfpoint(base,C32RED);
	//drawfpoint(exponent,C32GREEN);
	//drawfcircle(resulta,C32BLUE,10);
	//drawfcircle(resultb,C32CYAN,20);
	//drawfcircle(resultc,C32MAGENTA,30);
	//drawfpoint(resultc,C32MAGENTA);
	//drawfcircle(resultd,C32BROWN,40);

	drawgrid(base,basesep,C32RED);
	drawgrid(exponent,exponentsep,C32GREEN);
	drawgridfunc(base,basesep,exponent,exponentsep,C32MAGENTA);
}

#endif
