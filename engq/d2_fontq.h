// new font class, using sprites
class fontq {
	textureb* glyphs[256];
	bool centerx,centery; // do center
	bool fixed;			// do fixed width
	float fixedwidth;	// fixed width value
	float scl;			// scale
	float spc;			// space width
	float sc;			// space between chars
	float leftmarg;		// left and right
	float topmarg;		// top and bot
	float maxxsize,maxysize;	// max x and y size
public:
	fontq(const C8* dir);
	void setcenter(bool ca) {centerx=ca;centery=ca;}
	void setcenterx(bool cax) {centerx=cax;}
	void setcentery(bool cay) {centery=cay;}
	void setfixedwidth(float fwa);
	void setscale(float sa) {scl=sa;}
	void setspace(float spca) {spc=spca;}
	void setcharspace(float sca) {sc=sca;}
	void setleftmarg(float lma) {leftmarg=lma;}
	void settopmarg(float tma) {topmarg=tma;}
	void print(float x,float y,float xs,float ys,const pointf3& colf,const C8* fmt,...);
	~fontq(); 
private:
	textureb* getglyph(U32 idx);
};

