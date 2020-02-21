void loadscoreline();
void updatescoreline(struct ol_playerdata *of);
void freescoreline();
void flattenmodel(struct tree *t,float zval);

/*
namespace scrline {
	extern pointf2 energymeterlocs[MAXENERGIES];
}
*/

/*extern float emetertest;
extern pointf2 trip5[3];
extern uv triuv5[3];
extern pointf2 trip2[3];
extern uv triuv2[3];
*/
extern pointf2 trip[3];
extern uv triuv[3];
extern pointf2 hudlocs[];
extern float etest,pcx,pcy,picx[],picy[];
extern float sizemul;
extern float speedcx;
extern float speedcy;
extern float speedpicx;
extern float speedpicy;
extern float speedstartang;
extern float speedendang;
extern float speedtest;

extern float track_topx;
extern float track_topy;

extern float track_topsclx;
extern float track_topscly;
extern float track_topoffx;
extern float track_topoffy;
extern float track_toprot;
