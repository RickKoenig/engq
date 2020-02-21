// based on probabilities at XY and YZ 99%
// what kind of probabilites does XZ have?
#define BELL

#ifdef BELL

namespace bell {
S32 fu = FLOATUP/256;

float D = .01f; // how many we want different between XY and YZ
float S = 1 - D; // same 99%

// these range from 0 to 1
float a = 1; // seed
float b;
float c;
float d;
float e;
float f;
float g;
float h;

float Pone; // want this to be 1
float Pxz,Pyz; // want these to be S 99%
float Pxy; // result, maybe always >= 2S - 1 or 98%
//float Pxz2; // check algebra

S32 constrain = 1;

void constrain01(float& v)
{
	v = range(0.0f,v,1.0f);
}

void calc_bell()
{
	// constrain probabilities
	constrain01(D);
	constrain01(a);
	constrain01(b);
	constrain01(c);
	constrain01(d);
	constrain01(e);
	constrain01(f);
	constrain01(g);
	constrain01(h);
	S = 1 - D;

	// constrain total probability to 1, 8 DOF
	Pone = a + b + c + d + e + f + g + h;
	if (constrain) {
		if (abs(Pone) > EPSILON) {
			float inv = 1.0f/Pone;
			a *= inv;
			b *= inv;
			c *= inv;
			d *= inv;
			e *= inv;
			f *= inv;
			g *= inv;
			h *= inv;
		}
	}

	// constrain XY probability to S
	Pxy = a + b + e + f;
	if (constrain) {
		if (abs(Pxy) > EPSILON) {
			float inv = S/Pxy;
			a *= inv;
			b *= inv;
			e *= inv;
			f *= inv;
		}
	}
	// constrain YZ probability to S
	Pyz = a + d + f + g;
	if (constrain) {
		if (abs(Pyz) > EPSILON) {
			float inv = S/Pyz;
			a *= inv;
			d *= inv;
			f *= inv;
			g *= inv;
		}
	}
	// show probability XZ
	Pxz = a + c + f + h;
}

} // namespace bell

#endif // BELL
