#include <m_eng.h>

#include "u_trackpath.h"

trackpath_fo::trackpath_fo(const pointf3& startpos,float startrot,const PIECES* const pces,U32 numpces,float maxtimea,float piecesizea) :
  pieceset(pces),
  numpieceset(numpces),
  maxtime(maxtimea),
  piecesize(piecesizea)
{
	if (!numpieceset)
		errorexit("track size must be > 0 !!");
	// calculate the start pos and rot for each piece
	basepos = new pointf3[numpieceset];
	baserot = new float[numpieceset];
	pointf3 curpos = startpos;
	float currot = startrot;
	float halfrot;
	float ps2 = sqrtf(2.0f)*.5f*piecesize;
	for (U32 i=0;i<numpieceset;++i) {
		basepos[i] = curpos;
		baserot[i] = currot;
		switch(pieceset[i]) {
		case PAUSE:
			break;
		case STRAIGHT:
		curpos.x += piecesize*sinf(currot);
		curpos.z += piecesize*cosf(currot);
			break;
		case ROT_RIGHT:
			currot += PI/2.0f;
			currot = normalangrad(currot);
			break;
		case ROT_LEFT:
			currot -= PI/2.0f;
			currot = normalangrad(currot);
			break;
		case TURN_RIGHT:
			halfrot = currot + PI/4.0f;
			currot += PI/2.0f;
			currot = normalangrad(currot);
			curpos.x += ps2*sinf(halfrot);
			curpos.z += ps2*cosf(halfrot);
			break;
		case TURN_LEFT:
			halfrot = currot - PI/4.0f;
			currot -= PI/2.0f;
			currot = normalangrad(currot);
			curpos.x += ps2*sinf(halfrot);
			curpos.z += ps2*cosf(halfrot);
			break;
		}
	}
}
  
void trackpath_fo::operator()(float t,pointf3& pos,pointf3& rot)
{
	t *= numpieceset/maxtime; // now t is 0 to numpieceset, 1 unit for each piece
	float idxf;
	float tfract = modf(t,&idxf); // split t into fraction and integer parts, tfract 0 to 1, where on a piece
	U32 idx = U32(idxf); // piece index
	const pointf3& bp = basepos[idx];
	float br = baserot[idx];
	float brp,brm;
	float poff = tfract*piecesize; // where on piece we are, timelike
	float poff2 = sqrtf(2.0f)*.5f*poff;
	float ps2 = piecesize*.5f;
	float xo;// = -ps2*cosf(br); // origin for circle
	float zo;// = ps2*sinf(br);
	switch(pieceset[idx]) {
		case PAUSE:
			pos = bp;
			rot = pointf3x(0,br,0);
			break;
		case STRAIGHT:
			pos.x = bp.x + poff*sinf(br);
			pos.y = bp.y;
			pos.z = bp.z + poff*cosf(br);
			rot = pointf3x(0,br,0);
			break;
		case ROT_RIGHT:
			pos = bp;
			br += tfract*PI/2.0f;
			rot = pointf3x(0,br,0);
			break;
		case ROT_LEFT:
			pos = bp;
			br -= tfract*PI/2.0f;
			rot = pointf3x(0,br,0);
			break;
		case TURN_RIGHT:
			xo = ps2*cosf(br); // origin for circle
			zo = -ps2*sinf(br);
			brp = br + PI*.25f;
			br += tfract*PI/2.0f;
			pos.x = bp.x + xo - ps2*cosf(br);
			pos.y = bp.y;
			pos.z = bp.z + zo + ps2*sinf(br);
			rot = pointf3x(0,br,0);
			break;
		case TURN_LEFT:
			float xo = -ps2*cosf(br); // origin for circle
			float zo = ps2*sinf(br);
			brm = br - PI*.25f;
			br -= tfract*PI/2.0f;
			pos.x = bp.x + xo + ps2*cosf(br);
			pos.y = bp.y;
			pos.z = bp.z + zo - ps2*sinf(br);
			rot = pointf3x(0,br,0);
			break;
	}
}

trackpath_fo::~trackpath_fo()
{
	delete[] basepos;
	delete[] baserot;
}
