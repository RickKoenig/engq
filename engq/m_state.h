struct state {
	void (*initfunc)();
	void (*procfunc)();
	void (*draw3dfunc)();
	void (*draw2dfunc)();
	void (*exitfunc)();
};

// user defines these
extern S32 numstates;
extern struct state states[];

// global functions
#define NOSTATE 0xffffffffU
void changestate(S32 newstate); // wow there's not much left!
void pushchangestate(S32 newstate);
void popstate();
void poporchangestate(S32 newstate); // only change state if stack empty

// internal
bool stateproc(); // returns 0 if all done
void statedraw3d();
void statedraw2d(); 
bool statehasdraw3d();
bool statehasdraw2d();
