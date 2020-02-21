class CircularB;

class CircularA {
	CircularB* cb;

public:
	void print()
	{
		logger("print A\n");
	}

	void setB(CircularB* cbA)
	{
		cb = cbA;
	}

	void printBoth();

};
