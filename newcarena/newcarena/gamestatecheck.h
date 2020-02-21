#define CHECKGAMESTATE

#ifdef CHECKGAMESTATE


class gamestate {
	vector<vector<hashi> > gamestates;
public:
	gamestate() : gamestates(gg->numplayers) {}
// add valid game state, calculate it's hash
	void writegamestate(const vector<keys>& ks);
// check new gamestates, compare gamestate hashes
	void readgamestate(U32 from,const vector<U8>& mess);
};

extern gamestate* gs;
#endif
