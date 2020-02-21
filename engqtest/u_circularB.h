

class CircularB {
	CircularA* ca;

public:
	void print()
	{
		logger("print B\n");
	}

	void setA(CircularA* cbA)
	{
		ca = cbA;
	}

	void printBoth()
	{
		logger("printBoth B\n");
		logger("self B\n");
		print();
		logger("other A\n");
		ca->print();
	}

};
