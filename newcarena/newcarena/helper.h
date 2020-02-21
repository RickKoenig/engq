class helper {
	vector<tree2*> sphs;
	tree2* rt;
	S32 nvis;
	static void dontdraw(tree2* t);
public:
	helper(tree2* rta); // root tree
	void resethelper();
	void addhelper(const pointf3& posa);
//	~helper(); // trees are in roottree, no need to free
};
