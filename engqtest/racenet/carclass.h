// handles the car and it's hsv, maybe later the regpoints also...
class carclass
{
//	tree2* rt;
	tree2* cartree;
	bitmap32* carbasepic;
	bitmap8* carpalpic;
	bitmap32* carbasepic2;
	bitmap8* carpalpic2;
	bitmap32* cargraypic;
	bitmap32* cargraypic2;
	textureb* cartex;
	textureb* cartex2;
public:
	enum colortype {COLNONE,COLHSV,COLRGB};
private:
	colortype canhsv; // has the necc. files
public:
	carclass(const C8* carname);
	void changehsv(const hsv* curhsvs);
	colortype getcanhsv() { return canhsv; }
	~carclass(); // make sure this gets called 'before' delete roottree...
	tree2* getcartree_passownership() { tree2* tt=cartree; cartree=0; return tt; } // pass/return ownership of cartree
	tree2* getcartree() { return cartree; } // keep ownership of cartree in carclass
	void changerims(S32 rimnum);
};
