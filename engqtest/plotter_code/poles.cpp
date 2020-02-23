// complex analysis

#define POLES
#ifdef POLES

typedef std::complex<float> compf; // compf, typedef for a complex float

pointf2 base = {2,1};//{3,0};//{2,0};//{-1.25f,0};//{2,0};
pointf2 result = {6,9};
int poleiter = 2000;
float circsize = .0001f;
float fcircsize;
float radius = .5f;

struct pole {
	const char* val; // null is sentinal for arrays of poles
	pointf2 loc;
};

// some senarios


// z*z
compf sqfun(const compf& zin)
{
	return zin*zin;
}

const pole sqpoles[] = {
	0
};


// 1/z
compf invfun(const compf& zin)
{
//	return operator/(1.0f,zin); // test operator notation
	return 1.0f/zin;
}

const pole invpoles[] = {
	{"1",{0,0}},
	0
};


// 1/[(1-z)*(1+z)]
compf pole2mulfun(const compf& zin)
{
	return 1.0f/((1.0f - zin)*(1.0f + zin));
}

const pole pole2mulpoles[] = {
	{"1/2",{-1,0}},
	{"-1/2",{1,0}},
	0
};


// 1/(1-z)+1/(1+z)
compf pole2sumfun(const compf& zin)
{
	return 1.0f/(1.0f - zin) + 1.0f/(1.0f + zin);
}

const pole pole2sumpoles[] = {
	{"1",{-1,0}},
	{"-1",{1,0}},
	0
};


// 1/(z-1)+1/(z+1)
compf pole2sum2fun(const compf& zin)
{
	return 1.0f/(zin - 1.0f) + 1.0f/(zin + 1.0f);
}

const pole pole2sum2poles[] = {
	{"1",{-1,0}},
	{"1",{1,0}},
	0
};

// CFUN, typedef for a function pointer in compf out compf
typedef compf(*CFUN)(const compf&);

// full package
struct func {
	const char* name;
	const pole* poles;
	CFUN fun;
};

// tie it altogether, array of func
func funcs[] = {
	{"z*z",sqpoles,sqfun},
	{"1/z",invpoles,invfun},
	{"1/[(1-z)*(1+z)]",pole2mulpoles,pole2mulfun},
	{"1/(1-z)+1/(1+z)",pole2sumpoles,pole2sumfun},
	{"1/(z-1)+1/(z+1)",pole2sum2poles,pole2sum2fun},
};
#define NUMFUNCS NUMELEMENTS(funcs)

S32 curfunc;
S32 lastfunc;
string curfuncstr = "onetwo";
CFUN* curfun;

S32 poleint = 47;
string polestring = "pole string";
menuvar pole_debvar[] = {
	{"pole int",&poleint,D_INT},
	{"pole string",&polestring,D_STRING},
};
#define pole_ndebvar NUMELEMENTS(pole_debvar)

// walk in a circle
pointf2 cirwalk(float ang)
{
	pointf2 res;
	res.x = base.x + radius * cos(ang);
	res.y = base.y + radius * sin(ang);
	return res;
}

// walk in a circle delta
// del is 90 degrees from position relative to center
pointf2 cirwalkdel(float ang)
{
	pointf2 res;
	float scale = radius*TWOPI/poleiter;
	res.x = scale * -sin(ang);
	res.y = scale *  cos(ang);
	return res;
}

// call fun from and to pointf2 class
pointf2 zfunc(const pointf2& z)
{
	compf zc(z.x,z.y);
	compf resc = (*curfun)(zc);
	pointf2x res(resc.real(),resc.imag());
	return res;
}

pointf2 complexMultiplyPole(const pointf2& a,const pointf2& b)
{
	pointf2 ret;
	ret.x = a.x*b.x - a.y*b.y;
	ret.y = a.x*b.y + a.y*b.x;
	return ret;
}

void showpoles()
{
	const pole* p = funcs[curfunc].poles;
	while(p->val) {
		pointi2 pi=math2screen(p->loc);
		const string& val = p->val;
		outtextxybf32(B32,pi.x-8*(val.size()+1),pi.y-16,C32BLACK,C32WHITE,"%s",val.c_str());
		drawfpoint(p->loc,C32MAGENTA,2);
		++p;
	}
}

void testatoi()
{
	logger("test atoi\n");
	int i,n;
	n = 0;
	const char s[] = "784";
	for (i=0 ; s[i] >= '0' && s[i] <= '9' ; ++i)
		n = n*10 + s[i] - '0';
	logger("n = %d\n",n);
}

void testarg(S32 i = 3)
{
	logger("testarg i = %d\n",i);
}

void testvector()
{
	testarg(5);
	testarg();
	string s = "abcdef";
	s[3] = '3';
	logger("test string = '%s'\n",s.c_str());
	vector<S32> testv;
	for (U32 i=0;i<10;++i)
		testv.push_back(10*i);
	for (U32 i=0;i<10;++i)
		testv.push_back(10*i + 1);
	testv.erase(testv.begin() + 7);
	s = "";
	stringstream ss;
	for (U32 i=0;i<testv.size();++i) {
		ss << testv[i] << " ";
	}
	s = ss.str();
	if (!s.empty())
		s.resize(s.length() - 1); // remove space at end
	logger("test vector = '%s'\n",s.c_str());
}

void poles_init()
{
	logger("poles init\n");
	testatoi();
	testvector();
	curfunc = NUMFUNCS - 1; // last function
	//curfunc = 0; // first function
	lastfunc = -1;
	adddebvars("polevars",pole_debvar,pole_ndebvar);
}

// draw path and calculate everything
void poles_draw()
{
// ranges
	poleiter = range(1,poleiter,2000);
	circsize = range(.0001f,circsize,.0625f);
	curfunc = range(0,curfunc,(S32)NUMFUNCS - 1);
	if (curfunc != lastfunc) {
		curfuncstr = funcs[curfunc].name;
		curfun = &funcs[curfunc].fun;
		lastfunc = curfunc;
	}
// different circle sizes
	fcircsize = math2screen(circsize);
	float fcentersize = fcircsize*2.0f;
	float itersize = fcircsize*.75f;
// draw center point
	drawfpoint(base,C32MAGENTA,fcentersize);
// walk around the point, drawing lines
	int i;
	pointf2 last = cirwalk((poleiter - 1)*TWOPI/poleiter);
	result.x = result.y = 0;
	float dist = TWOPI/poleiter;
	for (i=0;i<poleiter;++i) {
		float ang = i*TWOPI/poleiter;
		pointf2 cp = cirwalk(ang);
#if 0
		pointf2 del;
		del.x = cp.x - last.x;
		del.y = cp.y - last.y;
#else
		pointf2 del = cirwalkdel(ang);
#endif
		pointf2 fun = zfunc(cp);
		pointf2 mul = complexMultiplyPole(fun,del);
		result.x += mul.x;
		result.y += mul.y;
		drawfline(cp,last,C32RED);
		drawfpoint(cp,C32RED,fcircsize);
		last = cp;
	}
	showpoles();
}

void poles_exit()
{
	logger("poles exit\n");
	removedebvars("polevars");
}


#endif