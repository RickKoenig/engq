#include <m_eng.h>

#define ORDER 5
static struct matn* mn;
static struct vecn* vn,*outpu;
static struct con32* cn,*mcon;
// static U32 count;

#if 0
#define MAXVAL 1.0f
static U32 iternvec(struct vecn* b)
{
	U32 pos=0;
	while(pos<b->order) {
		if (b->data[pos]<MAXVAL) {
			++(b->data[pos]);
			return 0;
		}
		b->data[pos]=0.0f;
		++pos;
	}
	return 1;
}

static U32 iternmat(struct matn* m)
{
	U32 posx=0,posy=0;
	while(posy<m->order) {
		while(posx<m->order) {
			float val=matngetcolrow(m,posx,posy);
			if (val<MAXVAL) {
				++val;
				matnsetcolrow(m,posx,posy,val);
				return 0;
			}
			matnsetcolrow(m,posx,posy,0.0f);
			++posx;
		}
		posx=0;
		++posy;
	}
	return 1;
}

static U32 iternmatnvec(struct matn* m,struct vecn* v)
{
	U32 ret;
	ret=iternvec(v);
	if (!ret)
		return 0;
	ret=iternmat(m);
	return ret;
}
#endif

static void dosolve()
{
	U32 result;
	gcon=mcon;
	matnvecnlogger(mn,vn,"mnvn");
	gcon=cn;
	result=matnsolve(mn,vn,outpu); // returns 0 no, 1 one, 2 many
}

void s_mattest_init()
{
	video_setupwindow(1024,768);
	cn=con32_alloc(WX/2,WY,C32WHITE,C32BLUE);
	mcon=con32_alloc(WX/2,WY/2,C32WHITE,C32BLUE);
	mn=matncreate(ORDER);
	vn=vecncreate(ORDER);
	outpu=vecncreate(ORDER);
	matnzero(mn); // ident to zero
	matnsetcolrow(mn,0,0,1.0f);
	vecnsetele(vn,0,5);
#if ORDER>=2
	vecnsetele(vn,1,7);
	matnsetcolrow(mn,1,0,2.0f);
	matnsetcolrow(mn,0,1,4.0f);
	matnsetcolrow(mn,1,1,5.0f);
#endif
#if ORDER>=3
	vecnsetele(vn,2,11);
	matnsetcolrow(mn,0,2,7.0f);
	matnsetcolrow(mn,1,2,8.0f);
	matnsetcolrow(mn,2,2,9.0f);
	matnsetcolrow(mn,2,0,3.0f);
	matnsetcolrow(mn,2,1,6.0f);
#endif
//	matnzero(mn); // ident to zero
//	vecnzero(vn);
//	do {
//		gcon_printf("==========================================================\n");
		dosolve();
//		gcon_printf("\n");
//	} while(!iternmatnvec(mn,vn)); 
}

static void procbut()
{
	S32 x,y;
	x=MX/72;
	y=MY/8-1;
	if (x>=0 && x<ORDER && y>=0 && y<ORDER) {
		if (wininfo.mleftclicks)
			matnsetcolrow(mn,x,y,matngetcolrow(mn,x,y)+wininfo.mleftclicks);
		if (wininfo.mrightclicks)
			matnsetcolrow(mn,x,y,matngetcolrow(mn,x,y)-wininfo.mrightclicks);
		if (wininfo.mmiddleclicks)
			matnsetcolrow(mn,x,y,0.0f);
	}
	if (x>=ORDER && x<=ORDER+1 && y>=0 && y<ORDER) {
		if (wininfo.mleftclicks)
			vecnsetele(vn,y,vecngetele(vn,y)+wininfo.mleftclicks);
		if (wininfo.mrightclicks)
			vecnsetele(vn,y,vecngetele(vn,y)-wininfo.mrightclicks);
		if (wininfo.mmiddleclicks)
			vecnsetele(vn,y,0.0f);
	}
}

void s_mattest_proc()
{
	S32 i;
	if (KEY==K_ESCAPE)
		popstate();
	if (wininfo.mleftclicks || wininfo.mrightclicks || wininfo.mmiddleclicks) {
		con32_clear(mcon);
		con32_clear(cn);
		procbut();
		dosolve();
	}
	if (wininfo.justdropped && wininfo.args)
//		for (i=0;i<wininfo.argc;++i)
//			con32_printf(cn,"drop %d, '%s'\n",i,wininfo.argv[i]);
		for (i=0;i<wininfo.args->num();++i) {
			const C8* s=(*wininfo.args).idx(i).c_str();
			con32_printf(cn,"drop %d, '%s'\n",i,s);
		}
}

void s_mattest_draw2d()
{
//	video_lock();
	cliprect32(B32,0,0,WX,WY,C32BLACK);
	clipblit32(con32_getbitmap32(cn),B32,0,0,WX/2,0,WX,WY);
	clipblit32(con32_getbitmap32(mcon),B32,0,0,0,0,WX,WY);
//	video_unlock();
}

void s_mattest_exit()
{
	matnfree(mn);
	vecnfree(vn);
	vecnfree(outpu);
	gcon=0;
	con32_free(cn);
	con32_free(mcon);
}
