#define D2_3D
#include <m_eng.h>
#include <m_perf.h>

viewport2 mainvp,lightvp;

viewport2x::viewport2x() {
	backcolor = C32LIGHTCYAN;
	zfront = .125f;
	zback = 1000;
	xstart = 0;
	ystart = 0;
	xres = WX;
	yres = WY;
	xsrc = WX;
	ysrc = WY;
	camtrans.z = 0;//-100;
	camtrans.x = 0;
	camtrans.y = 0;// 50;
	camrot = pointf3x(0, 0, 0);
	usev2w = false;
	camzoom = 1;//3.2f; // it'll getit from tree camattach if you have one
	flags = VP_CLEARBG | VP_CHECKER | VP_CLEARWB;
	isortho = false;
	useattachcam = false;
#if 0
	// bitmaps
	//	struct bitmap16 *backdrop; // where to draw
	//	S32 backdroptype;			// format of drawing surface
	//	struct bitmap16 *wbuffer; // wbuffer for draw
	//	mat4 w2c;
	C32 backcolor;
	// clipping
	float zfront, zback;
	S32 xstart, ystart; // used in projection
	S32 xres, yres; // used in projection
	S32 xsrc, ysrc;
	// matrix
	//	struct mat4 w2c; // derived from camattach,roottree,trans,rot,scale,wbscale,zoom,xsrc,ysrc,xres,yres
	//	struct tree* roottree;
	pointf3 camtrans, camrot;
	mat4 v2w;
	bool usev2w;
	//	float wbscale;
	float camzoom;
	// flags
	S32 flags;
	// for ortho
	bool isortho;
	float ortho_size;
	tree2* camattach;
	bool useattachcam;
#endif
}

void doflycam(viewport2* vp)
{
//	extern S32 mousemode;
	float leftright=0,foreback=0,updown=0;
	float mxc,mxr,myc,myr,rcx,rsx,rcy,rsy;
	if (wininfo.indebprint || wininfo.releasemode)
		return;
	if (KEY=='c') {
		treeinfo.flycam^=1;
		KEY=0; // so other viewports don't get a key..
	}
	if ((wininfo.mousemode) || (MX>=vp->xstart && MX<vp->xstart+vp->xres &&
		MY>=vp->ystart && MY<vp->ystart+vp->yres)) {
		if (KEY=='r') {
			vp->camtrans=vp->camrot=zerov;
			KEY=0;
		}
		if (treeinfo.flycam ) {
			if (KEY=='+')
				treeinfo.flycamspeed*=2.0f;
			if (KEY==K_NUMMINUS)
				treeinfo.flycamspeed*=1.0f/2.0f;
			if (wininfo.keystate[K_RIGHT]||wininfo.keystate[K_NUMRIGHT])
				leftright+=treeinfo.flycamspeed;
			if (wininfo.keystate[K_LEFT]||wininfo.keystate[K_NUMLEFT])
				leftright-=treeinfo.flycamspeed;
			if (wininfo.keystate[K_UP]||wininfo.keystate[K_NUMUP])
				foreback+=treeinfo.flycamspeed;
			if (wininfo.keystate[K_DOWN]||wininfo.keystate[K_NUMDOWN])
				foreback-=treeinfo.flycamspeed;
			if (MBUT==2)
				updown+=treeinfo.flycamspeed;
			if (MBUT==1)
				updown-=treeinfo.flycamspeed;
			if (wininfo.mousemode) {
				mxr=.35f*TWOPI/vp->xres;
				myr=.35f*TWOPI/vp->xres;
				if (treeinfo.flycamrevy)
					myr=-myr;
				vp->camrot.y=normalangrad(wininfo.dmx*mxr+vp->camrot.y);
				vp->camrot.x=snormalangrad(wininfo.dmy*myr+vp->camrot.x);
				vp->camrot.x=range(-PI/2,vp->camrot.x,PI/2);
			} else {
				mxc=vp->xstart+.5f*vp->xres;
				myc=vp->ystart+.5f*vp->yres;
				mxr=1.0f*2*TWOPI/vp->xres;
				myr=.5f*TWOPI/vp->yres;
				if (treeinfo.flycamrevy)
					myr=-myr;
				vp->camrot.y=normalangrad((MX-mxc)*mxr);
				vp->camrot.x=normalangrad((MY-myc)*myr);
			}
			rcx=cosf(vp->camrot.x);
			rsx=sinf(vp->camrot.x);
			rcy=cosf(vp->camrot.y);
			rsy=sinf(vp->camrot.y);
			vp->camtrans.x+=leftright*rcy;
			vp->camtrans.z-=leftright*rsy;
			vp->camtrans.x+=foreback*rcx*rsy;
			vp->camtrans.y+=-foreback*rsx;
			vp->camtrans.z+=foreback*rcx*rcy;
			vp->camtrans.x+=updown*rsx*rsy;
			vp->camtrans.y+=updown*rcx;
			vp->camtrans.z+=updown*rsx*rcy;
		}
	}
}

