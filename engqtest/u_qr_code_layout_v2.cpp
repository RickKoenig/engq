// version 2
#include <m_eng.h>

// format info
pointi2 fc0[15] = {
	{8,0},
	{8,1},
	{8,2},
	{8,3},
	{8,4},
	{8,5},
	{8,7},
	{8,8},
	{7,8},
	{5,8},
	{4,8},
	{3,8},
	{2,8},
	{1,8},
	{0,8},
};
pointi2 fc1[15] = {
	{-1,8},
	{-2,8},
	{-3,8},
	{-4,8},
	{-5,8},
	{-6,8},
	{-7,8},
	{-8,8},
	{8,-7},
	{8,-6},
	{8,-5},
	{8,-4},
	{8,-3},
	{8,-2},
	{8,-1},
};
#if 0
// layout
static const pointi2 v2_d1[8] = {
	{23,21},
	{24,21},
	{23,22},
	{24,22},
	{23,23},
	{24,23},
	{23,24},
	{24,24},
};
static const pointi2 v2_d2[8] = {
	{23,17},
	{24,17},
	{23,18},
	{24,18},
	{23,19},
	{24,19},
	{23,20},
	{24,20},
};
static const pointi2 v2_d3[8] = {
	{23,13},
	{24,13},
	{23,14},
	{24,14},
	{23,15},
	{24,15},
	{23,16},
	{24,16},
};
static const pointi2 v2_d4[8] = {
	{23, 9},
	{24, 9},
	{23,10},
	{24,10},
	{23,11},
	{24,11},
	{23,12},
	{24,12},
};
static const pointi2 v2_d5[8] = {
	{21,12},
	{22,12},
	{21,11},
	{22,11},
	{21,10},
	{22,10},
	{21, 9},
	{22, 9},
};
static const pointi2 v2_d6[8] = {
	{21,16},
	{22,16},
	{21,15},
	{22,15},
	{21,14},
	{22,14},
	{21,13},
	{22,13},
};
static const pointi2 v2_d7[8] = {
	{21,20},
	{22,20},
	{21,19},
	{22,19},
	{21,18},
	{22,18},
	{21,17},
	{22,17},
};
static const pointi2 v2_d8[8] = {
	{21,24},
	{22,24},
	{21,23},
	{22,23},
	{21,22},
	{22,22},
	{21,21},
	{22,21},
};
static const pointi2 v2_d9[8] = {
	{19,21},
	{20,21},
	{19,22},
	{20,22},
	{19,23},
	{20,23},
	{19,24},
	{20,24},
};
static const pointi2 v2_d10[8] = {
	{19,12},
	{20,12},
	{19,13},
	{20,13},
	{19,14},
	{20,14},
	{19,15},
	{20,15},
};
static const pointi2 v2_d11[8] = {
	{17, 9},
	{18, 9},
	{19, 9},
	{20, 9},
	{19,10},
	{20,10},
	{19,11},
	{20,11},
};
static const pointi2 v2_d12[8] = {
	{17,13},
	{18,13},
	{17,12},
	{18,12},
	{17,11},
	{18,11},
	{17,10},
	{18,10},
};
static const pointi2 v2_d13[8] = {
	{17,22},
	{18,22},
	{17,21},
	{18,21},
	{17,15},
	{18,15},
	{17,14},
	{18,14},
};
static const pointi2 v2_d14[8] = {
	{15,23},
	{16,23},
	{15,24},
	{16,24},
	{17,24},
	{18,24},
	{17,23},
	{18,23},
};
static const pointi2 v2_d15[8] = {
	{15,17},
	{15,18},
	{15,19},
	{15,20},
	{15,21},
	{16,21},
	{15,22},
	{16,22},
};
static const pointi2 v2_d16[8] = {
	{16,12},
	{15,13},
	{16,13},
	{15,14},
	{16,14},
	{15,15},
	{16,15},
	{15,16},
};
static const pointi2 v2_d17[8] = {
	{16, 8},
	{15, 9},
	{16, 9},
	{15,10},
	{16,10},
	{15,11},
	{16,11},
	{15,12},
};
static const pointi2 v2_d18[8] = {
	{16, 3},
	{15, 4},
	{16, 4},
	{15, 5},
	{16, 5},
	{15, 7},
	{16, 7},
	{15, 8},
};
static const pointi2 v2_d19[8] = {
	{14, 0},
	{15, 0},
	{16, 0},
	{15, 1},
	{16, 1},
	{15, 2},
	{16, 2},
	{15, 3},
};
static const pointi2 v2_d20[8] = {
	{14, 4},
	{13, 3},
	{14, 3},
	{13, 2},
	{14, 2},
	{13, 1},
	{14, 1},
	{13, 0},
};
static const pointi2 v2_d21[8] = {
	{14, 9},
	{13, 8},
	{14, 8},
	{13, 7},
	{14, 7},
	{13, 5},
	{14, 5},
	{13, 4},
};
static const pointi2 v2_d22[8] = {
	{14,13},
	{13,12},
	{14,12},
	{13,11},
	{14,11},
	{13,10},
	{14,10},
	{13, 9},
};
static const pointi2 v2_d23[8] = {
	{14,17},
	{13,16},
	{14,16},
	{13,15},
	{14,15},
	{13,14},
	{14,14},
	{13,13},
};
static const pointi2 v2_d24[8] = {
	{14,21},
	{13,20},
	{14,20},
	{13,19},
	{14,19},
	{13,18},
	{14,18},
	{13,17},
};
static const pointi2 v2_d25[8] = {
	{12,24},
	{13,24},
	{14,24},
	{13,23},
	{14,23},
	{13,22},
	{14,22},
	{13,21},
};
static const pointi2 v2_d26[8] = {
	{12,20},
	{11,21},
	{12,21},
	{11,22},
	{12,22},
	{11,23},
	{12,23},
	{11,24},
};
static const pointi2 v2_d27[8] = {
	{12,16},
	{11,17},
	{12,17},
	{11,18},
	{12,18},
	{11,19},
	{12,19},
	{11,20},
};
static const pointi2 v2_d28[8] = {
	{12,12},
	{11,13},
	{12,13},
	{11,14},
	{12,14},
	{11,15},
	{12,15},
	{11,16},
};
static const pointi2 v2_d29[8] = {
	{12, 8},
	{11, 9},
	{12, 9},
	{11,10},
	{12,10},
	{11,11},
	{12,11},
	{11,12},
};
static const pointi2 v2_d30[8] = {
	{12, 3},
	{11, 4},
	{12, 4},
	{11, 5},
	{12, 5},
	{11, 7},
	{12, 7},
	{11, 8},
};
static const pointi2 v2_d31[8] = {
	{10, 0},
	{11, 0},
	{12, 0},
	{11, 1},
	{12, 1},
	{11, 2},
	{12, 2},
	{11, 3},
};
static const pointi2 v2_d32[8] = {
	{10, 4},
	{ 9, 3},
	{10, 3},
	{ 9, 2},
	{10, 2},
	{ 9, 1},
	{10, 1},
	{ 9, 0},
};
static const pointi2 v2_d33[8] = {
	{10, 9},
	{ 9, 8},
	{10, 8},
	{ 9, 7},
	{10, 7},
	{ 9, 5},
	{10, 5},
	{ 9, 4},
};
static const pointi2 v2_d34[8] = {
	{10,13},
	{ 9,12},
	{10,12},
	{ 9,11},
	{10,11},
	{ 9,10},
	{10,10},
	{ 9, 9},
};
static const pointi2 v2_d35[8] = {
	{10,17},
	{ 9,16},
	{10,16},
	{ 9,15},
	{10,15},
	{ 9,14},
	{10,14},
	{ 9,13},
};
static const pointi2 v2_d36[8] = {
	{10,21},
	{ 9,20},
	{10,20},
	{ 9,19},
	{10,19},
	{ 9,18},
	{10,18},
	{ 9,17},
};
static const pointi2 v2_d37[8] = {
	{ 8,16},
	{ 9,24},
	{10,24},
	{ 9,23},
	{10,23},
	{ 9,22},
	{10,22},
	{ 9,21},
};
static const pointi2 v2_d38[8] = {
	{ 8,12},
	{ 7,13},
	{ 8,13},
	{ 7,14},
	{ 8,14},
	{ 7,15},
	{ 8,15},
	{ 7,16},
};
static const pointi2 v2_d39[8] = {
	{ 5, 9},
	{ 7, 9},
	{ 8, 9},
	{ 7,10},
	{ 8,10},
	{ 7,11},
	{ 8,11},
	{ 7,12},
};
static const pointi2 v2_d40[8] = {
	{ 5,13},
	{ 4,12},
	{ 5,12},
	{ 4,11},
	{ 5,11},
	{ 4,10},
	{ 5,10},
	{ 4, 9},
};
static const pointi2 v2_d41[8] = {
	{ 3,16},
	{ 4,16},
	{ 5,16},
	{ 4,15},
	{ 5,15},
	{ 4,14},
	{ 5,14},
	{ 4,13},
};
static const pointi2 v2_d42[8] = {
	{ 3,12},
	{ 2,13},
	{ 3,13},
	{ 2,14},
	{ 3,14},
	{ 2,15},
	{ 3,15},
	{ 2,16},
};
static const pointi2 v2_d43[8] = {
	{ 1, 9},
	{ 2, 9},
	{ 3, 9},
	{ 2,10},
	{ 3,10},
	{ 2,11},
	{ 3,11},
	{ 2,12},
};
static const pointi2 v2_d44[8] = {
	{ 1,13},
	{ 0,12},
	{ 1,12},
	{ 0,11},
	{ 1,11},
	{ 0,10},
	{ 1,10},
	{ 0, 9},
};
const pointi2* qr_layout_v2[] = {
	 v2_d1, v2_d2, v2_d3, v2_d4, v2_d5, v2_d6, v2_d7, v2_d8, v2_d9,v2_d10,
	v2_d11,v2_d12,v2_d13,v2_d14,v2_d15,v2_d16,v2_d17,v2_d18,v2_d19,v2_d20,
	v2_d21,v2_d22,v2_d23,v2_d24,v2_d25,v2_d26,v2_d27,v2_d28,v2_d29,v2_d30,
	v2_d31,v2_d32,v2_d33,v2_d34,v2_d35,v2_d36,v2_d37,v2_d38,v2_d39,v2_d40,
	v2_d41,v2_d42,v2_d43,v2_d44
};
#endif
// draw a data blocker
void drawrect(vector<vector<bool> >& ds,S32 x0,S32 y0,S32 sizx,S32 sizy)
{
	S32 i,j;
	for (j=0;j<sizy;++j)
		for (i=0;i<sizx;++i)
			ds[j+y0][i+x0] = false;
}

vector<vector<pointi2> > qr_layout;

const S32 BLOCKCOL = 6;

bool isup(S32 col)
{
	if (col>=BLOCKCOL)
		--col;
	return (col&2) != 0;
}

bool isright(S32 col)
{
	if (col>=BLOCKCOL)
		--col;
	return (col&1) != 0;
}

// for versions 1 through 6
void buildlayout(S32 ver)
{
	S32 modu = 4*ver + 17; // 25 for ver==2
// first build non data sections
	vector<vector<bool> > datasect(modu);
	logger("\nbuildlayout\n");
	S32 i,j;
	for (j=0;j<modu;++j) {
		datasect[j].assign(modu,true);
	}
	drawrect(datasect,0,0,9,9);
	drawrect(datasect,modu-8,0,8,9);
	drawrect(datasect,0,modu-8,9,8);
	if (ver>1)
		drawrect(datasect,modu-9,modu-9,5,5); // draw the 4th smaller target
	drawrect(datasect,9,BLOCKCOL,modu-17,1); // BLOCKCOL == BLOCKROWj
	drawrect(datasect,BLOCKCOL,9,1,modu-17);
#if 0
	for (j=0;j<modu;++j) {
		for (i=0;i<modu;++i) {
			C8 c = datasect[j][i] ? '#' : '.'; // # 0 light,  . 1 dark
			logger("%c",c);
		}
		logger("\n");
	}
	logger("\n");
#endif
	// build layout
	qr_layout.clear();
	i = j = modu - 1; // start in lower right corner
	S32 bit = 7;
	vector<pointi2> word(8);
	S32 wat = 0; // watchdog
	while(i>=0 && j>=0) {
		++wat;
		if (wat>5000)
			break;
		// logger("at %3d,%3d\n",i,j);
//		if (true) {
		if (datasect[j][i]) {
			word[bit].x = i;
			word[bit].y = j;
			--bit;
			if (bit<0) {
				qr_layout.push_back(word);
				bit = 7;
			}
		}
		// move around
		if (isright(i)) {
			--i;
		} else if (isup(i)) {
			++i;
			--j;
			if (j < 0) {
				++j;
				i -= 2;
				if (i == BLOCKCOL)
					--i;
			}
		} else { // going down
			++i;
			++j;
			if (j>=modu) {
				--j;
				i -= 2;
			}
		}
	}
	logger("codewords = %d, remainder = %d\n",qr_layout.size(),7-bit);
	// check codewords against manually built v2 layout (manual/auto)
#if 0
	if (ver == 2) {
		logger("version2 check\n");
		if (qr_layout.size() != 44) {
			logger("wrong data size\n");
		} else {
			for (j=0;j<44;++j) {
				for (i=0;i<8;++i) {
					if (qr_layout[j][i].x != qr_layout_v2[j][i].x && qr_layout[j][i].y != qr_layout_v2[j][i].y)
						logger("diff at i = %d,  j = %d\n",i,j);
				}
			}
		}
		logger("done version2 check\n");
	}
#endif
	logger("done buildlayout\n");
}
