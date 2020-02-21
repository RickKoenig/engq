////////////// generic class factory routines
// the factory
// factory makes T*'s or their derived classes from the handle H, can handle heirarchy
// requires specialization
template <typename T,typename H>
class factory {
public:
// makes 1 object from handle, alters state of handle, whole heirarchy or whatever
	T* newclass_from_handle(H h);
//	T* newclass_from_name(const char* name);
	factory(); // specialize to eliminate linker errors
};

// specialized factory class for script&
template<typename T>
class factory<T,script> {
	typedef void* (*createfun2)(script& sc); // handy typedef for the create function
//	typedef T* (*createfun2)(script* sc); // handy typedef for the create function
	map<string,createfun2> funmap2; // factory constructor fills this in
public:
	T* newclass_from_handle(script&);
	T* newclass_from_name(string s,S32 arg);
	factory(); // specialize to eliminate linker errors
};

/// read in a T type heirarchy from a file, assumes factory constructor has been written for type T,
/// see tree1family2.h
/// uses specialized script& template
/// script parser
template <typename T>
T* factory<T,script>::newclass_from_handle(script& sc)
{
	T* ret=0;
	const char* idname=sc.read();
	if (!idname || idname[0]=='\0')
		errorexit("bad eof in '%s'",sc.getfname().c_str());
	else { /// read main node, this is where the factory does its work
		map<string,createfun2>::iterator found=funmap2.find(idname); // find the create function, given the id
		if (found==funmap2.end())
			errorexit("unknown shape %s in reading '%s' file",script2printc(idname).c_str(),sc.getfname().c_str());
		createfun2 s=found->second;
		ret=(T*)(*s)(sc);
//		ret=(T*)(*(found->second))(sc); // create the right class
	}
/// check for children
	idname=sc.read();
	if (!idname)
		return ret;
	else if (/*strcmp(idname,"child") && */ strcmp(idname,"{")) {
		sc.backup(); /// no children
		return ret;
	}
	while(1) {
		idname=sc.read();
		if (!idname)
			return ret;
		if (/*!strcmp(idname,"endchild") || */ !strcmp(idname,"}"))
			return ret;
		sc.backup();
		T* retc=newclass_from_handle(sc);
		ret->linkchild(retc);
	}
	return 0;
}

/// build T* from id
/*template <typename T>
T* factory<T,script>::newclass_from_name(string s,S32 arg)
{
	T* ret;
	map<string,createfun2>::iterator found=funmap2.find(s);
	if (found==funmap2.end())
		errorexit("unknown shape '%s'",s.c_str());
	ret=(*(found->second))(arg); // create the right class, passing a 0 to create fun, calls default constructor
	return ret;
}
*/
/// don't need a factory to write out the data, assumes data has a save or better
/*template<typename T>
void newclass_to_handle(const T* s,script& sc)
{
	s->savet(sc);
	if (s->chlist.empty())
		return;
	const int child=SID_CHILD;
	const int endchild=SID_ENDCHILD;
	list<chorelist*>::const_iterator i = s->chlist.begin();
//	fwrite(&child,1,4,fp);
	sc.addscript("child");
	while(i != s->chlist.end()) {
		newclass_to_handle(*i,sc);
		++i;
	}
//	fwrite(&endchild,1,4,fp);
	sc.addscript("endchild");
}
*/
