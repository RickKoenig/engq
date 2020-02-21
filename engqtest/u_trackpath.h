enum PIECES {
	PAUSE,
	STRAIGHT,
	ROT_RIGHT,
	ROT_LEFT,
	TURN_RIGHT, // NYI
	//RIGHT = TURN_RIGHT,
	TURN_LEFT, // NYI
	//LEFT = TURN_LEFT,
};

class trackpath_fo {
	pointf3* basepos;
	float* baserot;
	const PIECES* const pieceset;
	const U32 numpieceset;
	const float maxtime;
	const float piecesize;
public:
	trackpath_fo(const pointf3& startpos,float startrot,const PIECES* const pces,U32 numpces,float maxtimea,float piecesizea = 12);
	void operator()(float t,pointf3& pos,pointf3& rot);
	~trackpath_fo();
};
