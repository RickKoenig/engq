void	dx9_video_uninit();
void	dx9_paintwindow(U32 paintwindow);
S32		dx9_wnd_setupwindow(S32 x,S32 y);//,U32 bpp);
S32		dx9_fs_setupwindow(S32 x,S32 y);//,U32 bpp);
script* dx9_getnsubdrivers();
void	dx9_lock();
void	dx9_unlock();

struct vid3d {
//	D3DMATRIX matProjOrtho,matProj,matViewOrtho,matView;
//	D3DMATRIX matProj,matView; // set by beginscene
	enum _D3DZBUFFERTYPE zbuffertype;
//	D3DLIGHT9 lit;
//	float testopac;
	LPDIRECT3D9 d3d9;
	LPDIRECT3DDEVICE9 d3d9device;
//	bool fullscreen;
	S32 hiendmachine;
//	float camzoom,znear,zfar;
//	float camfov;
//	pointf3 campos,camrot;
//	pointf3 descampos,descamrot;
//	float desrate;
//	float mipmaplodbias;
//	S32 numlights;
//	pointf3 amblight;
//	S32 alphacutoff;
//	float tex01blend; // test texture blending
    D3DPRESENT_PARAMETERS pp;
	fogparms fog;
	S32 usemip;
	float miplodbias;
//	S32 alphacutoff; // see treeinfo in d2_tree.h
	float shadowepsilon;	 // for shadowmapping
//	bool inshadowdraw;
	D3DCAPS9 ddcaps9;
	S32 cullmode;
	S32 fillmode;
	float toonedgesize;
	float stanx;
	float stany;
	float edgesize;
	float edgezoff;
	float edgecosang;
	float normallength;
	S32 tooningame;
};

extern vid3d videoinfodx9;
