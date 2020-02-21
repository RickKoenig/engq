#define MAKEPENROSE
#ifdef MAKEPENROSE
bitmap32* ptfat,*ptthin;
const S32 PRX = 128;
const S32 PRY = 128;
const float angfat  = 72*PIOVER180; // 72 degrees
const float angthin = 36*PIOVER180; // 36 degrees
const S32 border = 3;
const float circlesize = 15;

struct circleinfo {
	bool right;
	bool bottom;
	bool big;
};
struct tileinfo {
	C32 backcolor;
	float ang;
	circleinfo cis[2];
	C8* fname;
};
tileinfo tilethin = {
	C32GREEN,
	angthin,
	{
		{true,false,false}, // blue
		{false,true,false} // red
	},
	"thin.png"
};
tileinfo tilefat = {
	C32LIGHTGREEN,
	angfat,
	{
		{false,false,false}, // blue
		{true,true,true} // red
	},
	"fat.png"
};

bitmap32* maketile(const tileinfo& ti)
{
	const S32 minn = 1;
	const S32 mind = 5;
	const S32 maxn = 4;
	const S32 maxd = 5;

	static const C32 circol[2] = {C32BLUE,C32RED};
	bitmap32* ret;
	S32 i,j;
	S32 maxj = (S32)(PRY*sinf(ti.ang));
	ret = bitmap32alloc(PRX*2,PRY*2,C32(0,0,0,0)); // background color
	S32 minim = (S32)(maxj/tanf(ti.ang));
	for (j=0;j<PRY;++j) {
		S32 mini = (S32)(j/tanf(ti.ang));
		S32 maxi = mini + PRX;
		S32 borderx = (S32)(border/sinf(ti.ang));
		for (i=0;i<PRX*2;++i) {
			if (j<maxj && i>= mini && i<maxi) {
				C32 c = ti.backcolor; // base tile color, maybe pass in
				if (j<border || j>=maxj-border)
					c = C32BLACK;
				if (i<mini+borderx || i>=maxi-borderx)
					c = C32BLACK;
				// do rules, blue and red circles
				int k;
				for (k=0;k<2;++k) {
					S32 tvi = i;
					S32 tvj = j;
					if (ti.cis[k].right) {
						tvi -= PRX;
					}
					if (ti.cis[k].bottom) {
						tvi -= minim;
						tvj -= maxj;
					}
					S32 d2 = tvi*tvi + tvj*tvj;
					S32 d2min;
					S32 d2max;
					if (ti.cis[k].big) {
						d2min = (S32)(maxn*PRX/maxd-circlesize/2);
						d2max = (S32)(maxn*PRX/maxd+circlesize/2);
					} else {
						d2min = (S32)(minn*PRX/mind-circlesize/2);
						d2max = (S32)(minn*PRX/mind+circlesize/2);
					}
					d2min *= d2min;
					d2max *= d2max;
	//				if (d2<PRX/4+circlesize/2 && d2>=PRX/4-circlesize/2)
					if (d2>=d2min && d2<d2max)
						c = circol[k];
					clipputpixel32(ret,i,j,c);
				}
			}
		}
	} 
	pushandsetdir("penrose");
	gfxwrite32(ti.fname,ret);
	popdir();
	return ret;
}

void makepenrose()
{
	ptthin = maketile(tilethin);
	ptfat = maketile(tilefat);
}

void drawpenrose()
{
	clipblit32(ptthin,B32,0,0,10,10,ptthin->size.x,ptthin->size.y);
	clipblit32(ptfat,B32,0,0,10+2*PRX+10,10,ptfat->size.x,ptfat->size.y);
}

void exitpenrose()
{
	bitmap32free(ptfat);
	bitmap32free(ptthin);
}
#endif
