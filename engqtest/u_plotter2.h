namespace u_plotter2 {

void plotter2init(),plotter2proc(),plotter2draw2d(),plotter2exit();

extern C32 funccolor;

pointi2 math2screen(const pointf2& p);
float math2screen(float p);
pointf2 screen2math(const pointi2& i);
float screen2math(float i);
pointf2 newcenter(const pointi2& i,const pointf2& p);
pointf2 getminvisxy();
pointf2 getmaxvisxy();
void drawfpoint(const pointf2& c,C32 clr,float rad = 4);
void drawfcircle(const pointf2& c,C32 clr,S32 r);
void drawfcirclef(const pointf2& c,C32 clr,float r);
void drawfhair(const pointf2& c,C32 clr);
void drawfline(const pointf2& a,const pointf2& b,C32 c);
void drawflinec(const pointf2& a,const pointf2& b,C32 c);
void drawflineh(const pointf2& a,const pointf2& b,C32 c);
void drawbox2(const pointf2& a,const pointf2& b,C32 c);
void drawfunction(float (*f)(float));
void drawfunction_n(float (*f)(float,S32),S32 n,C32 col);
void drawfunctionrange(float (*f)(float),float start,float end,C32 col = funccolor);
void drawfunctionrange_n(float (*f)(float,S32),float start,float end,S32 n);
void drawfunction2(pointf2 (*f)(float t));
void drawfunction2(const vector<pointf2>& f);
void drawfunctionrk2d(float* objs, S32* offs,S32 noffs,
					void (*initfunc)(float *os),
					void (*changefunc)(float *os,float *newos,float time));
// user functions
float sinfun(float a);
float expe(float a);
float exp10(float a);
float fc(float a);
float fs(float a);
float root3_0(float c);
float root3_1(float c);
float root3_2(float c);
float xs(float c);
float beercan(float x);
float quintic(float x);
float seventh(float x);
pointf2 circ(float t);

extern float lzoom;

} // end namespace u_plotter2
