#define CPPTEST

// using int array
class bmp {
	int x;
	int y;
	unsigned int* data;
public:
	bmp();
	bmp(int x,int y);
	bmp(const bmp& b);
	bmp& operator=(const bmp& b);
	void show(const char* s);
	~bmp();
};

// default constructor
bmp::bmp() : x(0),y(0),data(0)
{
	logger("default bmp constructor 0,0\n");
}

// specific constuctor
bmp::bmp(int xa,int ya) : x(xa),y(ya)
{
	logger("bmp constructor with x = %d, y = %d\n",xa,ya);
	data = new unsigned int[x*y];
	int i,j;
	unsigned int* dp = data;
	for (j=0;j<y;++j)
		for (i=0;i<x;++i)
			*dp++ = i*j;
}

// copy constructor
bmp::bmp(const bmp& rhs)
{
	logger("bmp copy constructor with x = %d, y = %d\n",rhs.x,rhs.y);
	x = rhs.x;
	y = rhs.y;
	int p = x*y;
	data = new unsigned int[p];
	copy(rhs.data,rhs.data+p,data);
}

// assignment operator
bmp& bmp::operator=(const bmp& rhs)
{
	logger("assignment operator with %d %d\n",rhs.x,rhs.y);
	if (this == &rhs) {
		logger("same object!\n");
		return *this;
	}
	x = rhs.x;
	y = rhs.y;
	int p = x*y;
	delete[] data;
	data = new unsigned int[p];
	copy(rhs.data,rhs.data+p,data);
	return *this;
}

void bmp::show(const char* s)
{
	logger("bmp '%s': has %d, %d, and ptr %p\n",s,x,y,data);
	int i,j;
	unsigned int* dp = data;
	for (j=0;j<y;++j)
		for (i=0;i<x;++i)
			if (*dp++ != i*j)
				errorexit("bad show in bmp::show");
}

// destructor
bmp::~bmp()
{
	logger("bmp destructor with x = %d, y = %d\n",x,y);
	delete[] data;
}

// using int vector
class bmpv {
	int x;
	int y;
	vector<unsigned int> data;
public:
	bmpv();
	bmpv(int x,int y);
	void show(const char* s);
};

// default constructor
bmpv::bmpv() : x(0),y(0)
{
	logger("default bmpv constructor 0,0\n");
}

// specific constuctor
bmpv::bmpv(int xa,int ya) : x(xa),y(ya),data(x*y)
{
	logger("bmpv constructor with x = %d, y = %d\n",xa,ya);
	int i,j;
	// direct array access...
	unsigned int* dp = &data[0];
	for (j=0;j<y;++j)
		for (i=0;i<x;++i) {
			if (*dp != 0)
				errorexit("vector nonzero");
			*dp++ = i*j;
		}
}
void bmpv::show(const char* s)
{
	logger("bmpv '%s': has %d, %d, and vector size %d\n",s,x,y,data.size());
	int i,j;
	vector<unsigned int>::const_iterator dp = data.begin();
	for (j=0;j<y;++j)
		for (i=0;i<x;++i)
			if (*dp++ != i*j)
				errorexit("bad show in bmpv::show");
}

class bigger {
public:
	bigger();
	~bigger();
	void dostuff();
	void dostuffv();
private:
	bmp abmp;
	bmpv abmpv;

};

bigger::bigger()
{
	logger("bigger constructor\n");
}

bigger::~bigger()
{
	logger("bigger destructor\n");
}

void bigger::dostuff()
{
	logger("doing bigger stuff\n");
	abmp = bmp(30,40);
	bmp bmp0 = bmp(10,20);
	bmp bmp1 = bmp0;
	bmp1 = bmp0;
	bmp1 = bmp0;
	bmp1 = bmp1;
	bmp* bp0 = new bmp(35,45);
	bmp* bp1 = new bmp(55,65);
	*bp1 = *bp0;
	*bp1 = *bp1;
	abmp.show("abmp");
	bmp0.show("bmp0");
	bmp1.show("bmp1");
	bp0->show("bp0");
	bp0->show("bp1");
	delete bp0;
	delete bp1;
}

void bigger::dostuffv()
{
	logger("doing bigger stuffv\n");
	abmpv = bmpv(30,40);
	bmpv bmp0v = bmpv(10,20);
	bmpv bmp1v = bmp0v;
	bmp1v = bmp0v;
	bmp1v = bmp0v;
	bmp1v = bmp1v;
	bmpv* bp0v = new bmpv(35,45);
	bmpv* bp1v = new bmpv(55,65);
	*bp1v = *bp0v;
	*bp1v = *bp1v;
	abmpv.show("abmpv");
	bmp0v.show("bmp0v");
	bmp1v.show("bmp1v");
	bp0v->show("bp0v");
	bp0v->show("bp1v");
	delete bp0v;
	delete bp1v;
}

void do_cpptest()
{
	logger("doing cpptest!\n");
	memname("cpptest");
	bigger* abigger = new bigger;
	abigger->dostuff();
	abigger->dostuffv();
	delete abigger;
	memname("after");
}
