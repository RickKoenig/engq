// attach cam
class camatt {
	bool newpos;
	tree2* const cat; // cam attach
	enum mode {TV,FOLLOW,FOLLOWPNT2ROAD,NUM};
	mode mo;
	pointf3x lastpos;
public:
	camatt(tree2* cata); // pass tree for (world) camera
	void proc(S32 uloop);
	void changemode();
	pointf3 getpos() const;
	pointf3 getrot() const;
	pointf3 getvel(S32 uloop) const;
	void bigchange() { newpos = true;}
};
