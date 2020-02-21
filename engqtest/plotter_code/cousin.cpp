#if 0
Given a general tree, write a function that returns the nth cousin of a node. A cousin to a node is any node at the same depth.  The nth cousin of a node is n cousin nodes away from it to the right in the tree.
 
From below:  if v == 6 and cousin == 2 then Findcousin = 9.  if v == 4 and cousin == 2 the Findcousin = Invalid
 
                         0
 
          /     /     /    \    \     \
 
        11     22     3    14    4    65
                   
      /             / \                      
 
    6              18  9           
                                              
class Node
{
    std::vector<Node> mChildren;
    Val            mVal;
};

int findepth(const Node& Root, Val v)
{
    static int depth = -1;
    static int level;
    if (level == 0)
        gotdepth = false;
    int i;
    for (i=0;i<Root.mChildren.size();++i)
        if (depth >= 0)
            break; // already found value
        if (v == Root[i].mVal]) {
            depth = level
        } else {
            ++level;
            int d = finddepth(Root[i],v);
            if (d >= 0)
                depth = d;
            --level;
        }
    }
    return depth;
    
}

// return a vector of all siblings at this level
void findcurlevel(vector<Val> vlist,const Node& Root,depth)
{
    static int level;
    int i;
    for (i=0;i<Root.size();++i) [
        if (level == depth)
            vlist.push_back(Root[i].mVal);
        ++level;
        findcurlevel(vlist,Root[i],depth);
        --level;
    }
 }


Val Findcousin(const Node& Root, Val v, uint cousin)
{
    int depth = finddepth(Root,v);
    if (depth < 0)
        return Invalid;
    vector<Val> vlist;
    // now find cousin
    findcurlevel(vlist,Root,depth);
    int a = alist.find(v);
    if (a < alist.size()-cousin)
        return alist[a+cousin];
    else
        return Invalid;  
}
#endif
