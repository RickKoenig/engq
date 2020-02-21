#define RES3D // alternate shape class
#define D2_3D
#include <m_eng.h>
#include <l_misclibm.h>
#include "system/u_states.h"

#include "U_s_maze.h"
static shape* focus;
static shape* rl;
//static pbut* PRINT;
static pbut* LEVELPLUS;
static pbut* LEVELMINUS;
static pbut* LEVELPLUSPLUS;
static pbut* LEVELMINUSMINUS;
static pbut* WIDTHPLUS;
static pbut* WIDTHMINUS;
static pbut* HEIGHTPLUS;
static pbut* HEIGHTMINUS;
static text* TEXTHEIGHT;
static text* TEXTWIDTH;
static text* TEXTLEVEL;
static text* TEXTTITLE;
static pbut* QUIT; // quit

#define MAXLEVELS 100
#define MAXSIZEX 32
#define MAXSIZEY 32

static S32 curlevel=0;
static vector<class maze*> levels;

//static void updatetitle(S32 x,S32 y);
class maze {
public:
	static const float startx;// = 245.0f;
	static const float starty;// = 45;
	static float stepx;// = 16;
	static float stepy;// = 16;
	U32 dimx;
	U32 dimy;
	vector<vector<U8> > hazard;
	vector<vector<U8> > horzwall;
	vector<vector<U8> > vertwall;
	maze() : dimx(1),dimy(1)
	{
//		logger("in maze constructor\n");
		vector<U8> row(dimx,0);
		hazard.assign(dimy,row);
		row.clear();
		vertwall.assign(dimy,row);
	}
	maze(const maze& rhs)
	{
//		logger("in maze copy constructor\n");
		dimx = rhs.dimx;
		dimy = rhs.dimy;
		hazard = rhs.hazard;
		horzwall = rhs.horzwall;
		vertwall = rhs.vertwall;
	}
	maze(FILE* fp)
	{
//		logger("in maze file constructor\n");
		dimx = filereadU32LE(fp);
		dimy = filereadU32LE(fp);
//		U32 p = dimx*dimy;
		// hazard
		vector<U8> row(dimx,0);
		hazard.assign(dimy,row);
		U32 j;
		for (j=0;j<dimy;++j)
			fileread(fp,&hazard[j][0],dimx);
		// horz wall
		if (dimy>1) {
			horzwall.assign(dimy-1,row);
			for (j=0;j<dimy-1;++j)
				fileread(fp,&horzwall[j][0],dimx);
		}
		// vert wall
		row.assign(dimx-1,0);
		vertwall.assign(dimy,row);
		if (dimx>1)
			for (j=0;j<dimy;++j)
				fileread(fp,&vertwall[j][0],dimx-1);
	}
	void save(FILE* fp)
	{
//		logger("in maze file save\n");
		filewriteU32LE(fp,dimx);
		filewriteU32LE(fp,dimy);
		// hazard
		U32 j;
		for (j=0;j<dimy;++j)
			filewrite(fp,&hazard[j][0],dimx);
		// horz walls
		for (j=0;j<dimy-1;++j)
			filewrite(fp,&horzwall[j][0],dimx);
		// vert walls
		if (dimx>1)
			for (j=0;j<dimy;++j)
				filewrite(fp,&vertwall[j][0],dimx-1);
	}
	void incx()
	{
		if (dimx == MAXSIZEX)
			return;
		U32 j;
		// hazard
		for (j=0;j<dimy;++j) {
			hazard[j].push_back(0);
		}
		// horz walls
		for (j=0;j<dimy-1;++j) {
			horzwall[j].push_back(0);
		}
		// vert walls
		for (j=0;j<dimy;++j) {
			vertwall[j].push_back(0);
		}
		++dimx;
	}
	void decx()
	{
		if (dimx == 1)
			return;
		U32 j;
		// hazard
		for (j=0;j<dimy;++j) {
			hazard[j].pop_back();
		}
		// horz walls
		for (j=0;j<dimy-1;++j) {
			horzwall[j].pop_back();
		}
		// vert walls
		for (j=0;j<dimy;++j) {
			vertwall[j].pop_back();
		}
		--dimx;
	}
	void incy()
	{
		if (dimy == MAXSIZEY)
			return;
		vector<U8> row(dimx,0);
		hazard.push_back(row);
		horzwall.push_back(row);
		row.assign(dimx-1,0);
		vertwall.push_back(row);
		++dimy;
	}
	void decy()
	{
		if (dimy == 1)
			return;
		hazard.pop_back();
		horzwall.pop_back();
		vertwall.pop_back();
		--dimy;
	}
	void doui()
	{
		float sx = startx + stepx/4;
		float sy = starty + stepy/4;
		if (MX >= sx && MY >= sy) {
			float fx = fmodf(MX-sx,stepx);
			float fy = fmodf(MY-sy,stepy);
			U32 ix = U32((MX-sx)/stepx);
			U32 iy = U32((MY-sy)/stepy);
			if (ix<dimx && iy<dimy) { // hazard
				if (fx<.5f*stepx && fy<.5f*stepy) {
					hazard[iy][ix] ^= 1;
				}
			}
			if (ix<dimx && iy<dimy-1) { // horz walls
				if (fx<.5f*stepx && fy>.5625f*stepy && fy<.9375f*stepy) {
					horzwall[iy][ix] ^= 1;
				}
			}
			if (ix<dimx-1 && iy<dimy) { // vert walls
				if (fx>.5625f*stepx && fx<.9375f*stepx && fy<.5f*stepy) {
					vertwall[iy][ix] ^= 1;
				}
			}
//			updatetitle(ix,iy);
		}
#if 0
		// ui
		// hazards
		U32 i,j;
		for (j=0;j<dimy;++j) {
			for (i=0;i<dimx;++i) {
				if (hazard[j][i])
					video_sprite_draw(0,pointf3x(.75f,.25f,.25f),startx+i*stepx+stepx/4,starty+j*stepy+stepy/4,stepx/2,stepy/2);
			}
		}
		// horz walls
		for (j=0;j<dimy-1;++j) {
			for (i=0;i<dimx;++i) {
				if (horzwall[j][i])
					video_sprite_draw(0,pointf3x(),startx+i*stepx+2+2.5f,starty+j*stepy+2.5f+stepy-3*stepx/16,stepx-6*stepx/16,stepy/4);
			}
		}
		// vert walls
		for (j=0;j<dimy;++j) {
			for (i=0;i<dimx-1;++i) {
				if (vertwall[j][i])
					video_sprite_draw(0,pointf3x(),startx+i*stepx+2.5f+stepx-3*stepx/16,starty+j*stepy+2+2.5f,stepx/4,stepy-6*stepy/16);
			}
		}
#endif
	}
	void draw()
	{
		U32 i,j;
		video_sprite_draw(0,pointf3x(),startx           ,starty		      ,stepx*dimx+2.0f,2);
		video_sprite_draw(0,pointf3x(),startx           ,starty+stepy*dimy,stepx*dimx+2.0f,2);
		video_sprite_draw(0,pointf3x(),startx           ,starty		      ,2              ,stepy*dimy+2.0f);
		video_sprite_draw(0,pointf3x(),startx+stepx*dimx,starty		      ,2              ,stepy*dimy+2.0f);
#if 0
		// ui
		// hazards
		for (j=0;j<dimy;++j) {
			for (i=0;i<dimx;++i) {
				if (hazard[j][i])
					video_sprite_draw(0,pointf3x(.75f,.25f,.25f),startx+i*stepx+stepx/4,starty+j*stepy+stepy/4,stepx/2,stepy/2);
			}
		}
		// horz walls
		for (j=0;j<dimy-1;++j) {
			for (i=0;i<dimx;++i) {
				if (horzwall[j][i])
					video_sprite_draw(0,pointf3x(),startx+i*stepx+2+2.5f,starty+j*stepy+2.5f+stepy-3*stepx/16,stepx-6*stepx/16,stepy/4);
			}
		}
		// vert walls
		for (j=0;j<dimy;++j) {
			for (i=0;i<dimx-1;++i) {
				if (vertwall[j][i])
					video_sprite_draw(0,pointf3x(),startx+i*stepx+2.5f+stepx-3*stepx/16,starty+j*stepy+2+2.5f,stepx/4,stepy-6*stepy/16);
			}
		}
#else
		// draw
		// hazards
		for (j=0;j<dimy;++j) {
			for (i=0;i<dimx;++i) {
				video_sprite_draw(0,pointf3x(),startx+i*stepx,starty+j*stepy,2*stepx/16,2*stepy/16);
				if (hazard[j][i])
					video_sprite_draw(0,pointf3x(.75f,.25f,.25f),startx+i*stepx+3,starty+j*stepy+3,stepx-4,stepy-4);
			}
		}
		// horz walls
		for (j=0;j<dimy-1;++j) {
			for (i=0;i<dimx;++i) {
				if (horzwall[j][i])
					video_sprite_draw(0,pointf3x(),startx+i*stepx+1.5f*stepx/16,starty+j*stepy+(3+13)*stepy/16,stepx,2*stepx/16);
			}
		}
		// vert walls
		for (j=0;j<dimy;++j) {
			for (i=0;i<dimx-1;++i) {
				if (vertwall[j][i])
					video_sprite_draw(0,pointf3x(),startx+i*stepx+(3+13)*stepx/16,starty+j*stepy+1.5f*stepy/16,2*stepx/16,stepy);
			}
		}
#endif
	}
	void print(U32 cl)
	{
		if (dimx==1 && dimy==1)
			return;
		U32 i,j;
		logger_disableindent();
		logger("\n\t// level %u\n",cl);

		logger("\tstatic final int[][]  levelz%u = { // hazards\n",cl);
		for (j=0;j<dimy;++j) {
			logger("\t\t\t{");
			for (i=0;i<dimx;++i) {
				logger("%u,",hazard[j][i]);
			}
			logger("},\n");
		}
		logger("\t};\n");

		if (dimy<=1) {
			logger("\tstatic final int[][]  levelh%u = null; // horzwalls\n",cl);
		} else {
			logger("\tstatic final int[][]  levelh%u = { // horzwalls\n",cl);
			for (j=0;j<dimy-1;++j) {
				logger("\t\t\t{");
				for (i=0;i<dimx;++i) {
					logger("%u,",horzwall[j][i]);
				}
				logger("},\n");
			}
			logger("\t};\n");
		}

		if (dimx<=1) {
			logger("\tstatic final int[][]  levelv%u = null; // vertwalls\n",cl);
		} else {
			logger("\tstatic final int[][]  levelv%u = { // vertwalls\n",cl);
			for (j=0;j<dimy;++j) {
				logger("\t\t\t{");
				for (i=0;i<dimx-1;++i) {
					logger("%u,",vertwall[j][i]);
				}
				logger("},\n");
			}
			logger("\t};\n");
		}
		logger("\tstatic Maze level%u = new Maze(levelh%u,levelv%u,levelz%u);\n",cl,cl,cl,cl);
		logger("\n\n");
		logger_enableindent();
	}
#if 0
	// 4 by 4
	static final int[][]  maph4 = { // horizontal inside walls
			{1,1,0,0},
			{0,1,1,0},
			{0,0,0,0},
	};
	static final int[][]  mapv4 = { // vertical inside walls
		{0,0,0},
		{0,0,0},
		{0,0,0},
		{0,0,0},
	};
	static final int[][]  mapz4 = { // vertical inside walls
		{0,0,0,1},
		{0,0,0,1},
		{0,0,1,1},
		{1,0,0,0},
	};
	static Maze map4 = new Maze(maph4,mapv4,mapz4);

#endif
	~maze()
	{
//		logger("in maze destructor\n");
	}
	void setstep()
	{
		if (dimx>16 || dimy>16) {
			stepx = 16;
			stepy = 16;
		} else {
			stepx = 32;
			stepy = 32;
		}
	}

};
const float maze::startx = 245;
const float maze::starty = 45;
float maze::stepx = 32;
float maze::stepy = 32;
static void updatetext()
{
	C8 s[50];
	sprintf(s,"Level %u",curlevel);
	TEXTLEVEL->settname(s);
	sprintf(s,"Width %u",levels[curlevel]->dimx);
	TEXTWIDTH->settname(s);
	sprintf(s,"Height %u",levels[curlevel]->dimy);
	TEXTHEIGHT->settname(s);
}

/*static void updatetitle(S32 x,S32 y)
{
	C8 s[50];
	sprintf(s,"x = %d, y = %d",x,y);
	TEXTTITLE->settname(s);
}*/

void maze_init()
{
	logger("maze init\n");
	pushandsetdir("maze");
	levels.assign(100,(maze*)0);
	if (fileexist("maze.bin")) {
		FILE* fp = fopen2("maze.bin","rb");
		U32 i;
		for (i=0;i<MAXLEVELS;++i)
			levels[i] = new maze(fp);
		fclose(fp);
	} else {
		U32 i;
		for (i=0;i<MAXLEVELS;++i)
			levels[i] = new maze();
	}
	video_setupwindow(800,600);
//	rl=loadres("logonres.txt");
//	nsc=0;
//	sc=NULL;
/*
	factory2<shape> fact;
	script* sc=new script("logonres.txt");
	shape* rls=fact.newclass_from_handle(*sc);
	rl=dynamic_cast<shaperoot*>(rls);
	if (!rl)
		errorexit("can't find shaperoot");
	delete sc;
*/
	init_res3d(800,600);
	rl = res_loadfile("mazeres.txt");
//	LOGON=rl->find<pbut>("LOGON");
//	REGISTER=rl->find<pbut>("REGISTER");
	QUIT=rl->find<pbut>("PBUTQUIT");
//	PRINT=rl->find<pbut>("PBUTPRINT");
	LEVELPLUS=rl->find<pbut>("PBUTPLUSLEVEL");
	LEVELMINUS=rl->find<pbut>("PBUTMINUSLEVEL");
	LEVELPLUSPLUS=rl->find<pbut>("PBUTPLUSPLUSLEVEL");
	LEVELMINUSMINUS=rl->find<pbut>("PBUTMINUSMINUSLEVEL");
	WIDTHPLUS=rl->find<pbut>("PBUTPLUSWIDTH");
	WIDTHMINUS=rl->find<pbut>("PBUTMINUSWIDTH");
	HEIGHTPLUS=rl->find<pbut>("PBUTPLUSHEIGHT");
	HEIGHTMINUS=rl->find<pbut>("PBUTMINUSHEIGHT");
	TEXTHEIGHT=rl->find<text>("TEXTHEIGHT");
	TEXTWIDTH=rl->find<text>("TEXTWIDTH");
	TEXTLEVEL=rl->find<text>("TEXTLEVEL");
	TEXTTITLE=rl->find<text>("TEXTTITLE");
	updatetext();
	focus=0;
}

void maze_proc()
{
// break
	if (KEY==K_ESCAPE) {
		poporchangestate(STATE_MAINMENU);
//		popstate();
	}
// ui
	shape* over=rl->getover();
	if (wininfo.mleftclicks) {
		levels[curlevel]->doui();
		focus=over;
	}
// call over or not over proc
	rl->over_no_over_proc(focus,over);
	if (focus) {
		S32 ret=focus->procfocus(); // focusproc returns -1 if nothing 0 or more when a hit occurs
		shape* focusa = ret>=0 ? focus : 0;
		if (focusa==LEVELPLUS) {
			if (curlevel!=MAXLEVELS-1) {
				++curlevel;
				updatetext();
			}
		} else if (focusa==LEVELMINUS) {
			if (curlevel!=0) {
				--curlevel;
				updatetext();
			}
		} else if (focusa==LEVELPLUSPLUS) {
			if (curlevel<MAXLEVELS-1-10)
				curlevel += 10;
			else
				curlevel = MAXLEVELS-1;
			updatetext();
		} else if (focusa==LEVELMINUSMINUS) {
			if (curlevel >= 10)
				curlevel -= 10;
			else
				curlevel = 0;
			updatetext();
		} else if (focusa==WIDTHPLUS) {
			levels[curlevel]->incx();
			updatetext();
		} else if (focusa==WIDTHMINUS) {
			levels[curlevel]->decx();
			updatetext();
		} else if (focusa==HEIGHTPLUS) {
			levels[curlevel]->incy();
			updatetext();
		} else if (focusa==HEIGHTMINUS) {
			levels[curlevel]->decy();
			updatetext();
/*		} else if (focusa==PRINT) {
			levels[curlevel]->print(curlevel);
			updatetext(); */
		} else if (focusa==QUIT) {
			poporchangestate(STATE_MAINMENU);
		}
	}
// end ui
	levels[curlevel]->setstep();
}

#define FSX 400
#define FSY 12
#if 0
void maze_draw2d()
{
//	cliprect32(B32,0,0,WX,WY,C32BLACK);
	rl->draw();
//	outtextxyf32(B32,WX/2-5*4,10,C32WHITE,"The Racing Network. TRN");
//	outtextxyf32(B32,0,0,C32WHITE,"focus = %p",focus);
}
#endif

void maze_draw3d()
{
	video_sprite_begin(
	800,600,	// intended resolution of screen (virtual screen space for video_sprite_draw)
	0,
	0);
	rl->draw3d();
	levels[curlevel]->draw();
	video_sprite_end();
}

void maze_exit()
{
	U32 i;
	FILE* fp = fopen2("maze.bin","wb");
	if (fp) {
		for (i=0;i<MAXLEVELS;++i) {
			levels[i]->save(fp);
			levels[i]->print(i);
			delete levels[i];
		}
		fclose(fp);
	}
	levels.clear();
	delete rl;
	popdir();
//	delete afont;
//	afont=0;
	exit_res3d();
}
