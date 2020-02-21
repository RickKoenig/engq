#include <m_eng.h>
#include "testclassfactory.h"

// factory of classes given an enum
animalCreateFun animalFactory[NUM] = {
	Animal::create,
	Dog::create,
	Cat::create,
};

void testClassFactory()
{
	logger("########## test class factory ############\n");
	vector<Animal*> animals;
	U32 i;
	for (i=0;i<NUM;++i)
		animals.push_back(animalFactory[i]());
	vector<Animal*>::iterator it;
	for (it=animals.begin();it!=animals.end();++it)
		(*it)->speak();
	for (it=animals.begin();it!=animals.end();++it)
		delete(*it);
}
