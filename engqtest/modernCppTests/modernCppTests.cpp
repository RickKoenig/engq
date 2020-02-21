#define MODERNCPPTEST
#include <random>
#include <iostream>
#include <memory>
#include <functional>

//#include "test0.cpp"
//#include "test1.cpp"
//#include "test2.cpp"

#include "cpl4e1.cpp"
#include "cpl4e2.cpp"
#include "cpl4e3.cpp"

//stringstream cout;

void f(int n1, int n2, int n3, const int& n4, int n5)
{
	cout << n1 << ' ' << n2 << ' ' << n3 << ' ' << n4 << ' ' << n5 << '\n';
}

int g(int n1)
{
	return n1;
}

struct Foo {
	void print_sum(int n1, int n2)
	{
		cout << n1 + n2 << '\n';
	}
	int data = 10;
};

void modernCPPTest()
{
	cout << "\nmodernCPPTest\n";
	using namespace std::placeholders;  // for _1, _2, _3...

	// demonstrates argument reordering and pass-by-reference
	int n = 7;
	// (_1 and _2 are from std::placeholders, and represent future
	// arguments that will be passed to f1)
	auto f1 = std::bind(f, _2, _1, 42, std::cref(n), n);
	n = 10;
	f1(1, 2, 1001); // 1 is bound by _1, 2 is bound by _2, 1001 is unused
					// makes a call to f(2, 1, 42, n, 7)

	// nested bind subexpressions share the placeholders
	auto f2 = std::bind(f, _3, std::bind(g, _3), _3, 4, 5);
	f2(10, 11, 12); // makes a call to f(12, g(12), 12, 4, 5);

	// common use case: binding a RNG with a distribution
	std::default_random_engine e;
	std::uniform_int_distribution<> d(0, 10);
	auto rnd = std::bind(d, e); // a copy of e is stored in rnd
	for (int n = 0; n < 10; ++n)
		cout << rnd() << ' ';
	cout << '\n';

	// bind to a pointer to member function
	Foo foo;
	auto f3 = std::bind(&Foo::print_sum, &foo, 95, _1);
	f3(5);

	// bind to a pointer to data member
	auto f4 = std::bind(&Foo::data, _1);
	cout << f4(foo) << '\n';

	// smart pointers can be used to call members of the referenced objects, too
	cout << f4(std::make_shared<Foo>(foo)) << '\n'
		<< f4(std::make_unique<Foo>(foo)) << '\n';

	vector<float> fv = { 2,3,5,7,1'1 };
	for (auto f : fv)
		logger("f = %f\n", f);
	S32 b = 0b1001;
	logger("b = %d\n", b);

	S32* ia = new S32[10];
	//ia[-1] = 11;
	delete[] ia;
	//delete[] ia;

// chapters in some book C++ Programming Language, Fourth Edition
#if 0
		cout << "run some cpp tests from chapters in some book C++ Programming Language, Fourth Edition\n";
#ifdef TEST0
	test0();
#endif
#ifdef TEST1
	test1();
#endif
#ifdef TEST2
	test2();
#endif
#endif
	cout << "\nrun some cpp tests from chapters in some book C++ Programming Language, Fourth Edition\n";
#ifdef CHAP1
	chap1();
#endif
#ifdef CHAP2
	chap2();
#endif
#ifdef CHAP3
	chap3();
#endif
	cout << "\ndone all chapters!\n";

}
