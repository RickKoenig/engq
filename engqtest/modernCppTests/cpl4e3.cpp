#define CHAP3

class VectorTestC3 {

public:

	VectorTestC3(int s) :elem{ new double[s] }, sz{ s } { fill(elem, elem + s, 0); } // construct a Vector
	VectorTestC3(std::initializer_list<double>);      // initialize with a list
	VectorTestC3() : elem{ nullptr }, sz{ 0 } {}// initialize with nothing
	double& operator[](int i) { return elem[i]; } // element access: subscripting
	double operator[](int i) const { return elem[i]; } // element access: subscripting

	int size() const { return sz; }
	~VectorTestC3() { delete[] elem; }

private:

	double* elem; // pointer to the elements

	int sz;       // the number of elements

};

VectorTestC3::VectorTestC3(std::initializer_list<double> lst) : elem{ new double[lst.size()] }
{
	//lst.size();
	copy(lst.begin(), lst.end(), elem);           // copy from lst into elem
	sz = lst.size();

}


//}

class Container {

public:

	virtual double& operator[](int) = 0;      // pure virtual function

	virtual int size() const = 0;             // const member function (§3.2.1.1)

	virtual ~Container() {}                   // destructor (§3.2.1.2)

};

class Vector_container : public Container {    // Vector_container implements Container

	VectorTestC3 v;

public:

	Vector_container(int s) : v(s) { }  // Vector of s elements
	Vector_container(std::initializer_list<double>);      // initialize with a list
	~Vector_container() {}



	double& operator[](int i) { return v[i]; }

	int size() const { return v.size(); }

};

Vector_container::Vector_container(std::initializer_list<double> lst) // initialize with a list
	: v{ lst }  {




}

class List_container : public Container {    // List_container implements Container

	list<double> ld;

public:

	List_container() { }  // empty list
	List_container(initializer_list<double> il) : ld{ il } {}      // initialize with a list
	~List_container() {}



	double& operator[](int i);

	int size() const { return ld.size(); }

};	

double& List_container::operator[](int i)

	{

		for (auto& x : ld) {

			if (i == 0) return x;

			--i;

		}

		throw out_of_range("List container");

	}




void use(Container& c)

{
	cout << "use\n";

	const int sz = c.size();



	for (int i = 0; i != sz; ++i)

		cout << c[i] << '\n';

}

struct Point {
	int x, y;
	Point(int xa = 0, int ya = 0) : x{ xa }, y{ ya } {}
};

class Shape {

public:

	virtual Point center() const = 0;      // pure virtual

	virtual void move(Point to) = 0;



	virtual void draw() const = 0;        // draw on current "Canvas"

	virtual void rotate(int angle) = 0;



	virtual ~Shape() {}                   // destructor

	 //...

};

void rotate_all(vector<Shape*>& v, int angle) // rotate v's elements by angle degrees{
{
	for (auto p : v)
		p->rotate(angle);
}

void draw_all(vector<Shape*>& v) // draw all
{
	for (auto p : v)
		p->draw();
}

class Triangle : public Shape {

public:

	Triangle(Point p0, Point p1, Point p2) {}          // constructor



	Point center() const { return 0; } // TODO, avg the 3 points

	void move(Point to) { /*x = to;*/ }



	void draw() const {}

	void rotate(int) {}               // nice simple algorithm

private:

	// TODO: define triangle members

};

class Circle : public Shape {

public:

	Circle(Point p, int rr);          // constructor



	Point center() const { return x; }

	void move(Point to) { x = to; }



	void draw() const {}

	void rotate(int) {}               // nice simple algorithm

private:

	Point x;   // center

	int r;     // radius

};

Circle::Circle(Point pa, int ra) : x{ pa }, r{ ra }{}

class Smiley : public Circle {  // use the circle as the base for a face

public:

	Smiley(Point p, int r) : Circle{ p,r }, mouth{ nullptr } { }



	~Smiley()

	{

		delete mouth;

		for (auto p : eyes) delete p;

	}

	void move(Point to) { Circle::move(to); }



	void draw() const;

	void rotate(int a) { Circle::rotate(a); }



	void add_eye(Shape* s) { eyes.push_back(s); }

	void set_mouth(Shape* s) { mouth = s; }

	virtual void wink(int i) {}     // wink eye number i



	 //...



private:

	vector<Shape*> eyes;          // usually two eyes

	Shape* mouth;

};

void Smiley::draw() const

{

	Circle::draw();

	for (auto p : eyes)

		p->draw();

	mouth->draw();

}

enum class Kind { circle, triangle, smiley };



Shape* read_shape(Kind k)        // read shape descriptions from input stream is

{

	//... read shape header from is and find its Kind k ...

	//Kind k = Kind::circle;
	Point p,p0,p1,p2;
	int r = 10;

	switch (k) {

	case Kind::circle:

		// read circle data {Point,int} into p and r

		return new Circle{ p,r };

	case Kind::triangle:

		// read triangle data {Point,Point,Point} into p1, p2, and p3

		return new Triangle{ p0,p1,p2 };

	case Kind::smiley:

		// read smiley data {Point,int,Shape,Shape,Shape} into p, r, e1 ,e2, and m

		Smiley* ps = new Smiley{ p,r };

		auto e1 = new Circle{ {2,1 }, 4 };
		Circle* e2 = new Circle{ {5,2 }, 3 };
		Circle* m = new Circle{ {4,5},4 };
		ps->add_eye(e1);

		ps->add_eye(e2);

		ps->set_mouth(m);

		return ps;

	}
	return nullptr;

}

void user()

{
	cout << "in user\n";

	std::vector<Shape*>v;

	v.push_back(read_shape(Kind::circle));
	v.push_back(read_shape(Kind::triangle));
	v.push_back(read_shape(Kind::smiley));
	//while (cin)

	//	v.push_back(read_shape(cin));

	//draw_all(v);                 // call draw() for each element

	rotate_all(v, 45);            // call rotate(45) for each element

	for (auto p : v) delete p;   // remember to delete elements

}

void chap3()
{
	cout << "\nIn Chap3\n";
	{
		//Vector_container vc{ 3 };
		Vector_container vc{ 10, 20, 30, 44 };
		vector<int> foo{ 3, 4, 5 };
		use(vc);
		List_container lc{ 2,3,5,7 };
		use(lc);
		user();
	}
}
