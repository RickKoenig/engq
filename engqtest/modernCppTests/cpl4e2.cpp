#define CHAP2
#if 1

double square(double x)    // square a double precision floating-point number
{
	return x * x;
}

void print_square(double x)
{
	cout << "the square of " << x << " is " << square(x) << "\n";
}

template<typename T, unsigned int sz>
inline unsigned int lengthof(T(&)[sz]) // how does this work ??
{
	return sz;
}

class VectorTestC2 {

public:

	VectorTestC2(int s) :elem{ new double[s] }, sz{ s } { fill(elem, elem + s, 0); } // construct a Vector

	double& operator[](int i) { return elem[i]; } // element access: subscripting
	double operator[](int i) const { return elem[i]; } // element access: subscripting

	int size() const { return sz; }
	~VectorTestC2() { delete[] elem; }

private:

	double* elem; // pointer to the elements

	int sz;       // the number of elements

};

double read_and_sum(double arri[], unsigned int len)
{
	unsigned int lo = len;// lengthof(arri);
	double ret = 0;
	for (unsigned v = 0; v != lo; ++v)
		ret += arri[v];// arri)
		//ret += v;
	return ret;
}

#if 0
double read_and_sum(const VectorTestC2 v)
{

	//VectorTestC2 v(s);                     // make a vector of s elements

	//for (int i = 0; i != v.size(); ++i)

	//	cin >> v[i];                 // read into elements



	double sum = 0;

	for (int i = 0; i != v.size(); ++i)

		sum += v[i];                 // take the sum of the elements

	return sum;

}
#endif
void chap2()
{
	cout << "\nIn Chap2, code ready\n";
//}
//void test0()
//{
//	cout << "In test0\n";
	print_square(1.234);
#ifdef TEST0_0
	test0_0();
#endif
#ifdef TEST0_1
	test0_1();
#endif


	{

		int v[]{ 0,1,2,3,4,5,6,7,8,9 };



		for (auto x : v)            // for each x in v

			cout << x << '\n';



		for (auto x : { 10,21,32,43,54,65 })

			cout << x << '\n';

		// ...

	}

	int v1[] = { 0,1,2,3,4,5,6,7,8,9 };

	//int* v2 = v;

	cout << "lengthof = " << lengthof(v1) << std::endl;


	for (auto& x : v1)

		++x;
	for (auto x : v1)

		cout << x << '\n';

	// ...
	//auto sum = read_and_sum(10);
	double ras[] = { 3,5,7,11,13 };
	auto sum = read_and_sum(ras, lengthof(ras));
	cout << " numele = " << lengthof(ras) << endl;
	cout << "sum = " << sum << endl;
}
#endif
