struct tsp {
	S32 nframe;
	struct bitmap32** frames;
	S32 texformat;
	S32 x,y;
	S32 curframe,lastframe; // user controlled, inited to 0,-1
	struct texture *atex;	// usercontrolled, inited to null
};
enum {TSP_USE_SOLID,TSP_USE_1ALPHA,TSP_USE_4ALPHA};

tsp* loadtspo(const C8* name,const C8* cname,S32 fmt,S32 texsizecheck);
tsp* duptsp(tsp* in);
void freetsp(tsp* t);
