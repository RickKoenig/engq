namespace mandlb {
extern const S32 MAXPAL;
class mandl {
	bitmap32* iter;
	bitmap32* work;
	S32 zf;
	S32 zoom;
	double izoom;
	S32 pal,paloff;
	C32* palette;
	bool dirty;
	S32 depth;
	S32 picisize;
	S32 ffastmand(S32 ix,S32 iy);
	S32 getmand(S32 x0,S32 y0,S32 ix,S32 iy);
	void getworkpicr(S32 x0,S32 y0,S32 xlo,S32 ylo, S32 xhi,S32 yhi);
public:
	static const S32 MAXDEPTH2 = 4096;
	mandl();
	~mandl();
	void setzoom(S32 zf);
	void setpicisize(S32 pcs);
	S32 setdepth(S32 d);
	void setpalette(S32 p);
//	S32 setpaletteoffset(S32 o);
	void rotatedac(S32 r);
	bitmap32* getpic(S32 xlo,S32 ylo, S32 xhi,S32 yhi);
	void getworkpic(S32 xlo,S32 ylo, S32 xhi,S32 yhi);
	void getworkpic2(S32 xlo,S32 ylo, S32 xhi,S32 yhi);
	bitmap32* getpicfromworkpic();
	bitmap32* getpici(S32 zm,S32 xi,S32 yi);
};

}

void mandl_task(S32 ps,const C8* fname);
