// test class factory
class Animal {
public:
	static Animal* create()
	{
		return new Animal();
	}
	virtual void speak()
	{
		logger("Animal noise\n");
	}
};

class Dog : public Animal {
public:
	static Animal* create()
	{
		return new Dog();
	}
	virtual void speak()
	{
		logger("Woof!\n");
	}
};

class Cat : public Animal {
public:
	static Animal* create()
	{
		return new Cat();
	}
	virtual void speak()
	{
		logger("Meow!\n");
	}
};

enum animalEnum {
	ANIMAL,
	DOG,
	CAT,
	NUM, // number of animal types
};

typedef Animal*(*animalCreateFun)();
void testClassFactory();
