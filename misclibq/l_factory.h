// the class factory
#define STARTCHILDREN "{"
#define ENDCHILDREN "}"
// had trouble with T*, don't understand , shape* worked, so did void*, so stuck with void* !?
// and it worked in VC7 enviornment
template <typename T>
class factory2 {
	typedef void*(*createfuni)(); // handy typedef for the create function
	typedef void*(*createfunh)(script&); // handy typedef for the create function
//	map<S32,createfuni> funmapi; // factory constructor fills this in
	map<string,createfunh> funmaph; // factory constructor fills this in
public:
//	T* newclass_from_id(S32 id);
	T* newclass_from_handle(script& sc);
	factory2();
};

/*template <typename T>
T* factory2<T>::newclass_from_id(S32 id)
{
	T* ret;
	map<S32,createfuni>::iterator found=funmapi.find(id); // T* should work here, !?
	if (found==funmapi.end())
		errorexit("unknown type id %d",id);
	ret=(T*)(*(found->second))();
	return ret;
}
*/
template <typename T>
T* factory2<T>::newclass_from_handle(script& sc)
{
	T* ret;
	const C8* tok=sc.read();
// see if children start early
	if (!strcmp(tok,STARTCHILDREN)) {
// yes, use base parent
		ret=T::createi();
		sc.backup(); // point back at STARTCHILDREN
	} else {
		map<string,createfunh>::iterator found=funmaph.find(tok);
		if (found==funmaph.end())
			errorexit("can't find type id '%s'",tok);
		ret=(T*)(*(found->second))(sc);
	}
// look for children
	tok=sc.read();
	if (strcmp(tok,STARTCHILDREN)) {
		sc.backup();
		return ret; // no children
	}
	while(1) {
		tok=sc.read();
		if (!strcmp(tok,ENDCHILDREN))
			break; // no more children
		sc.backup();
		T* c=newclass_from_handle(sc);
		ret->addchild(c);
	}
	return ret;
}

// handy function pointers
//typedef void*(*FPI)();
typedef void*(*FPH)(script&);

//#define RUNTIME_ERR // uncomment if you want a runtime message instead of a linker error
#ifdef RUNTIME_ERR
template <typename T>
factory2<T>::factory2<T>()
{
	errorexit("factory constructor '%s' needs specialization",typeid(T).name());
}
#endif
// end the class factory
