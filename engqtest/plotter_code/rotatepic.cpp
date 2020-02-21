//#define ROTATEPIC
#ifdef ROTATEPIC
const S32 NROT1 = 256;
void rotatepic()
{
	pushandsetdir("c:/srcw32/engq/engqtest/data/searcher");
	bitmap32* o = gfxread32("take0000.jpg");
	gfxwrite32("take.png",o);
//#if 0
	S32 ix = (S32)sqrtf((float)o->size.x*o->size.x+o->size.y*o->size.y);
	if (ix%2)
		++ix; // make even
	S32 iy = ix;
	float handinx = o->size.x/2.0f;
	float handiny = o->size.y/2.0f;
	float handoutx = ix/2.0f;
	float handouty = iy/2.0f;
	int i,j,k;
	for (k=0;k<NROT1;++k) {
		bitmap32* so = bitmap32alloc(ix,iy,0); // xpar black
		float a = k*TWOPI/NROT1;
		for (j=0;j<iy;++j) {
			for (i=0;i<ix;++i) {
				float fx = i - handoutx + .5f;
				float fy = j - handouty + .5f;
				float rfx =  fx*cosf(a) + fy*sinf(a);
				float rfy = -fx*sinf(a) + fy*cosf(a);
				C32 v = clipgetpixel32(o,S32(rfx + handinx),S32(rfy + handiny));
				clipputpixel32(so,i,j,v);
			}
		}
		char str[50];
		sprintf(str,"takerot%04d.png",k);
		gfxwrite32(str,so);
		bitmap32free(so);
	}
//#endif
	bitmap32free(o);
	popdir(); // back to data/plotter
}
#endif
//#define ROTATEPIC2
#ifdef ROTATEPIC2
#define NROT2 256
void rotatepic2()
{
	pushandsetdir("c:/dosprog/dpaint2");
	bitmap32* o = gfxread32("brush1.pcx");
	popdir();
	pushandsetdir("c:/srcw32/engq/engqtest/data/searcher");
	gfxwrite32("brush1.png",o);
	S32 ix = (S32)sqrtf((float)o->size.x*o->size.x+o->size.y*o->size.y);
	if (ix%2)
		++ix; // make even
	S32 iy = ix;
	float handinx = o->size.x/2.0f;
	float handiny = o->size.y/2.0f;
	float handoutx = ix/2.0f;
	float handouty = iy/2.0f;
	int i,j,k;
	for (k=0;k<NROT2;++k) {
		bitmap32* so = bitmap32alloc(ix,iy,0); // xpar black
		float a = k*TWOPI/NROT2;
		for (j=0;j<iy;++j) {
			for (i=0;i<ix;++i) {
				float fx = i - handoutx + .5f;
				float fy = j - handouty + .5f;
				float rfx = fx*cosf(a) + fy*sinf(a);
				float rfy = -fx*sinf(a) + fy*cosf(a);
				C32 v = clipgetpixel32(o,S32(rfx + handinx),S32(rfy + handiny));
				clipputpixel32(so,i,j,v);
			}
		}
		char str[50];
		sprintf(str,"brush1rot%04d.png",k);
		gfxwrite32(str,so);
		bitmap32free(so);
	}
	bitmap32free(o);
	popdir(); // back to data/plotter
}
#endif
//#define ROTATEPIC3
#ifdef ROTATEPIC3
#define NROT3 256
void rotatepic3()
{
	static char* pics[] = {
		"plank1",
		"plank2",
		"plank3",
		"plank4",
		"plank5",
		"plank6",
		"ball1",
		"ball2",
		"ball3",
		"ball4",
		"ball5",
	};
	const S32 npics = sizeof(pics)/sizeof(pics[0]);
	S32 p;
	for (p=0;p<npics;++p) {
		pushandsetdir("c:/dosprog/dpaint2");
		C8 fname[100];
		strcpy(fname,pics[p]);
		strcat(fname,".pcx");
		bitmap32* o = gfxread32(fname);
		popdir();
		pushandsetdir("c:/srcw32/engq/engqtest/data/searcher");
		strcpy(fname,pics[p]);
		strcat(fname,".png");
		gfxwrite32(fname,o);
		S32 ix = (S32)sqrtf((float)o->size.x*o->size.x+o->size.y*o->size.y);
		if (ix%2)
			++ix; // make even
		S32 iy = ix;
		float handinx = o->size.x/2.0f;
		float handiny = o->size.y/2.0f;
		float handoutx = ix/2.0f;
		float handouty = iy/2.0f;
		int i,j,k;
		for (k=0;k<NROT3;++k) {
			bitmap32* so = bitmap32alloc(ix,iy,0); // xpar black
			float a = k*TWOPI/NROT3;
			for (j=0;j<iy;++j) {
				for (i=0;i<ix;++i) {
					float fx = i - handoutx + .5f;
					float fy = j - handouty + .5f;
					float rfx = fx*cosf(a) + fy*sinf(a);
					float rfy = -fx*sinf(a) + fy*cosf(a);
					C32 v = clipgetpixel32(o,S32(rfx + handinx),S32(rfy + handiny));
					clipputpixel32(so,i,j,v);
				}
			}
			sprintf(fname,"%srot%04d.png",pics[p],k);
			gfxwrite32(fname,so);
			bitmap32free(so);
		}
		bitmap32free(o);
		popdir(); // back to data/plotter
	}
}
#endif
