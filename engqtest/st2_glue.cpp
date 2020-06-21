#include <m_eng.h>
#include "st2_glue.h"

namespace st2 {
	TREE* loadlws(const char* name)
	{
		return new tree2(name);
	}

	TREE* findtreename(TREE* parent, const char* name)
	{
		return parent->find(name);
	}

	TREE *alloctree(int nsubs, const char *modelname)
	{
		const char* treeName = modelname ? modelname : "no model";
		return new tree2(treeName);
	}

	void unhooktree(TREE *t) {
		if (!t)
			errorexit("unhooktree: tree to unhook is NULL!!");
		t->unlink();
	}

}
