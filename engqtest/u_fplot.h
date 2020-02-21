class fplot {
	S32 xs0,ys0,xsw,ysh;
	float x0,y0,x1,y1;
	float mx,my,bx,by;
	const bitmap32* d;
	const C8* xlabel;
	const C8* ylabel;
	const C32* ycolors;
	bool lastline;
	float lx,ly;
	void f2s(float cxa,float cya,S32* cxsa,S32* cysa);
public:
	fplot(S32 xs0a,S32 ys0a, S32 xswa,S32 ysha, // coords of screen viewport
		float x0a,float y0a,float x1a,float y1a, // coords of data viewport
		const bitmap32* da,const C8* xlabela,const C8* ylabela,const C32* ycolorsa=0);
	void drawaxis();
	void drawlabels();
	void fline(float x0,float y0,float x1,float y1,C32 c);
	void startlinev();
	void flinev(float x,float y,C32 c);
};

class fplot3d {
	S32 xs0,ys0,xsw,ysh;
	float x0,y0,z0,x1,y1,z1;
	float mx,my,bx,by;
	const bitmap32* d;
	const C8* xlabel;
	const C8* ylabel;
	const C8* zlabel;
	bool lastline;
	float lx,ly,lz;
	float ps,pc,ys,yc;
	void f2s(float cxa,float cya,float cza,S32* cxsa,S32* cysa);
public:
	fplot3d(S32 xs0a,S32 ys0a, S32 xswa,S32 ysha, // coords of screen viewport
		float x0a,float y0a,float z0a,float x1a,float y1a,float z1a, // coords of data viewport
		const bitmap32* da,const C8* xlabela,const C8* ylabela,const C8* zlabela,
		float pitcha,float yawa);
	void drawaxis();
	void drawlabels();
	void fline(float x0,float y0,float z0,float x1,float y1,float z1,C32 c,float phasehilight=0.0f);
	void startlinev();
	void flinev(float x,float y,float z,C32 c,float phasehilight=0.0f);
};
