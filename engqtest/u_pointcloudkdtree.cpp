// engq API
#include <m_eng.h>

#include "u_pointcloudkdtree.h"
#include "u_median.h"

static bool lessthanpointsx(const pointf2& a,const pointf2& b)
{
	return a.x < b.x;
}

static bool lessthanpointsy(const pointf2& a,const pointf2& b)
{
	return a.y < b.y;
}

typedef bool lessthanfunc2d(const pointf2& a,const pointf2& b);
lessthanfunc2d *lessthanfunc2dlist[] = {
	lessthanpointsx,
	lessthanpointsy
};

static bool equalpoints2d(const pointf2& a,const pointf2& b)
{
	return a.x == b.x && a.y == b.y;
}



// 2D
void pc_kdtree2d::build()
{
	// bounding box
	boxmin = pointf2x( 10000, 10000);
	boxmax = pointf2x(-10000,-10000);
	//U32 select = 0;
	//pointf2x lastpoint;
	//U32 lasti = 0;
	U32 i,npoints = pnts.size();
	for (i=0;i<npoints;++i) {
		if (pnts[i].x < boxmin.x)
			boxmin.x = pnts[i].x;
		if (pnts[i].x > boxmax.x)
			boxmax.x = pnts[i].x;
		if (pnts[i].y < boxmin.y)
			boxmin.y = pnts[i].y;
		if (pnts[i].y > boxmax.y)
			boxmax.y = pnts[i].y;
	}
	//logger("bbox kdtree2d is boxmin = (%8.3f,%8.3f), boxmax = (%8.3f,%8.3f)\n",boxmin.x,boxmin.y,boxmax.x,boxmax.y);

	// find median and split to <, =, >
	median<pointf2> kdt(pnts,lessthanfunc2dlist[level%maxdim]);
	vector<pointf2> left,middle,right;
	kdt.splitmedian(left,middle,right);
	center = middle[0]; // might be some points in the middle that are the same. Pick the first one
	U32 n=middle.size();
	for (i=1;i<n;++i) { // eliminate same points
		const pointf2& tp = middle[i];
		if (!equalpoints2d(tp,center))
			left.push_back(tp); // point is the same on one axis, but different, put on left side
	}
	middle.resize(1); // just one in the middle
	if (level == 0) {
		left0 = left;
		middle0 = middle;
		right0 = right;
	}

	// now build children
	if (left.size())
		subleft = new pc_kdtree2d(left,level+1);
	if (right.size())
		subright = new pc_kdtree2d(right,level+1);
}

pc_kdtree2d::pc_kdtree2d(const vector<pointf2>& points,U32 levela) : subleft(0),subright(0),level(levela)
{
	pnts = points;
	build();

}

pc_kdtree2d::pc_kdtree2d(const pointf2* points,U32 npoints,U32 levela) : subleft(0),subright(0),level(levela)
{
	pnts.assign(points,points+npoints);
	build();
}

vector<pointf2> pc_kdtree2d::insidecirclebrute(const pointf2& tp,float rad) const // return list of points inside circle using brute force
{
	float rad2 = rad*rad;
	vector<pointf2> ret;
	U32 i,n=pnts.size();
	for (i=0;i<n;++i) {
		float d2 = dist2dsq(&tp,&pnts[i]);
		if (d2 <= rad2) {
			ret.push_back(pnts[i]);
		}
	}
	return ret;
}

#if 0
vector<pointf2> pc_kdtree2d::insidecircle(const pointf2& tp,float rad) const // return list of points inside circle using kdtrees
{
	vector<pointf2> ret;
	return ret;
}

#else

#if 0
void pc_kdtree2d::insidecirclerec(const pointf2& tp,float inradr2,vector<pointf2>& rt) const
{
}
#else
// find all points within a given radius, pretty easy compared to find closest point
void pc_kdtree2d::insidecirclerec(const pointf2& tp,float inrad2,vector<pointf2>& ret) const // return list of points inside radius using kdtrees
{
	float currad2 = dist2dsq(&center,&tp);
	U32 funidx = level%maxdim;
	if (currad2 <= inrad2)
		ret.push_back(center); // add center point if inside radius
#if 1
	// early out
	if (!subleft && !subright)
		return;
#endif
	float tmc = tp.getmemberindex(funidx) - center.getmemberindex(funidx);
	float tmc2 = tmc*tmc;
	bool goleft = false,goright = false;
	if (tmc <= 0) { // go left
		goleft = true;
	}
	if (tmc >= 0) {
		goright = true;
	}
	// can we cross?
	if (inrad2 >= tmc2) {
		goleft = true;
		goright = true;
	}
	if (subleft && goleft) {
		subleft->insidecirclerec(tp,inrad2,ret);
	}
	if (subright && goright) {
		subright->insidecirclerec(tp,inrad2,ret);
	}
#if 0
	// descend tree to the closest leaf node or any leaf node if branches missing
	bool goleft = false,goright = false;
	if (subleft && subright) {
		if (lessthanfunc2dlist[funidx](tp,center)) { 
			// descend left
			goleft = true;
		} else {
			// descend right
			goright = true;
		}
	} else if (subleft) {
		// no right, descend left, wrong side, but..
		goleft = true;
	} else if (subright) {
		// no left, descend right, wrong side, but..
		goright = true;
	}

	if (goleft) {
		// descend left
		best = subleft->closestrec(tp,bestr2); 
		// compare against cur node
		if (curr2 < bestr2) {
			bestr2 = curr2;
			best = curnodepnt;
		}
		// prune this with a circle to line check.. TODO
		//bool crossover = true;
		float delta = center.getmemberindex(funidx) - tp.getmemberindex(funidx); // how big the radius has to be to crossover to the other side
		//float delta = center.getmemberindex(funidx) - best.getmemberindex(funidx); // how big the radius has to be to crossover to the other side
		if (delta < 0) {
			//errorexit("delta < 0");
			//logger("delta < 0");
			delta = 0; // fix wrong side
		}
		bool crossover = bestr2 >= delta*delta;//true;//lessthanfunc2dlist[funidx](tp,best);
		if (subright && crossover) {
			otherbest = subright->closestrec(tp,otherbestr2);
			if (otherbestr2 < bestr2) {
				bestr2 = otherbestr2;
				best = otherbest;
			}
		}
	} else if (goright) {
		// descend right
		best = subright->closestrec(tp,bestr2); 
		// compare against cur node
		if (curr2 < bestr2) {
			bestr2 = curr2;
			best = curnodepnt;
		}
		// prune this with a circle to line check.. TODO
		//bool crossover = true;
		//bool crossover = false;//!lessthanfunc2dlist[funidx](tp,best);
		float delta = tp.getmemberindex(funidx) - center.getmemberindex(funidx); // how big the radius has to be to crossover to the other side
		//float delta = best.getmemberindex(funidx) - center.getmemberindex(funidx); // how big the radius has to be to crossover to the other side
		if (delta < 0) {
			//errorexit("delta < 0");
			//logger("delta < 0");
			delta = 0; // fix wrong side
		}
		bool crossover = bestr2 >= delta*delta;//true;//lessthanfunc2dlist[funidx](tp,best);

		if (subleft && crossover) {
			otherbest = subleft->closestrec(tp,otherbestr2);
			if (otherbestr2 < bestr2) {
				bestr2 = otherbestr2;
				best = otherbest;
			}
		}
	} else {
		// at a leaf node
		best = center;
		bestr2 = dist2dsq(&center,&tp);
	}
	return best;
#endif
}
#endif

vector<pointf2> pc_kdtree2d::insidecircle(const pointf2& tp,float inrad) const // return points less than crad using kdtrees
{
	vector<pointf2> ret;
	float inrad2 = inrad*inrad;
	insidecirclerec(tp,inrad2,ret);
	return ret;
}

#endif

pointf2 pc_kdtree2d::closestbrute(const pointf2& tp,float& clrad) const // return closest point brute force
{
	float bestrad2 = 1e20f;
	U32 bestidx = -1;
	U32 i,n=pnts.size();
	for (i=0;i<n;++i) {
		float d2 = dist2dsq(&tp,&pnts[i]);
		if (d2 < bestrad2) {
			bestidx = i;
			bestrad2 = d2;
		}
	}
	pointf2 ret = pnts[bestidx];
	clrad = sqrtf(bestrad2);
	return ret;
}

pointf2 pc_kdtree2d::closestrec(const pointf2& tp,float& bestr2) const // return closest point using kdtrees
{
	// descend tree to the closest leaf node or any leaf node if branches missing
	pointf2x best;
	pointf2 otherbest;
	float otherbestr2;
	U32 funidx = level%maxdim;
	bool goleft = false,goright = false;
	if (subleft && subright) {
		if (lessthanfunc2dlist[funidx](tp,center)) { 
			// descend left
			goleft = true;
		} else {
			// descend right
			goright = true;
		}
	} else if (subleft) {
		// no right, descend left, wrong side, but..
		goleft = true;
	} else if (subright) {
		// no left, descend right, wrong side, but..
		goright = true;
	}

	pointf2 curnodepnt = center;
	float curr2 = dist2dsq(&curnodepnt,&tp);
	if (goleft) {
		// descend left
		best = subleft->closestrec(tp,bestr2); 
		// compare against cur node
		if (curr2 < bestr2) {
			bestr2 = curr2;
			best = curnodepnt;
		}
		// prune this with a circle to line check.. TODO
		//bool crossover = true;
		float delta = center.getmemberindex(funidx) - tp.getmemberindex(funidx); // how big the radius has to be to crossover to the other side
		//float delta = center.getmemberindex(funidx) - best.getmemberindex(funidx); // how big the radius has to be to crossover to the other side
		if (delta < 0) {
			//errorexit("delta < 0");
			logger("delta < 0");
			delta = 0; // fix wrong side
		}
		bool crossover = bestr2 >= delta*delta;//true;//lessthanfunc2dlist[funidx](tp,best);
		if (subright && crossover) {
			otherbest = subright->closestrec(tp,otherbestr2);
			if (otherbestr2 < bestr2) {
				bestr2 = otherbestr2;
				best = otherbest;
			}
		}
	} else if (goright) {
		// descend right
		best = subright->closestrec(tp,bestr2); 
		// compare against cur node
		if (curr2 < bestr2) {
			bestr2 = curr2;
			best = curnodepnt;
		}
		// prune this with a circle to line check.. TODO
		//bool crossover = true;
		//bool crossover = false;//!lessthanfunc2dlist[funidx](tp,best);
		float delta = tp.getmemberindex(funidx) - center.getmemberindex(funidx); // how big the radius has to be to crossover to the other side
		//float delta = best.getmemberindex(funidx) - center.getmemberindex(funidx); // how big the radius has to be to crossover to the other side
		if (delta < 0) {
			//errorexit("delta < 0");
			logger("delta < 0");
			delta = 0; // fix wrong side
		}
		bool crossover = bestr2 >= delta*delta;//true;//lessthanfunc2dlist[funidx](tp,best);

		if (subleft && crossover) {
			otherbest = subleft->closestrec(tp,otherbestr2);
			if (otherbestr2 < bestr2) {
				bestr2 = otherbestr2;
				best = otherbest;
			}
		}
	} else {
		// at a leaf node
		best = center;
		bestr2 = dist2dsq(&center,&tp);
	}
	return best;
}

pointf2 pc_kdtree2d::closest(const pointf2& tp,float& crad) const // return closest point using kdtrees
{
	float bestr2;
	pointf2 bst = closestrec(tp,bestr2);
	crad = sqrtf(bestr2);
	return bst;
}

vector<U32> pc_kdtree2d::insidecirclebrutei(const pointf2& tp,float rad) const // return list of points inside circle using brute force
{
	float rad2 = rad*rad;
	vector<U32> ret;
	U32 i,n=pnts.size();
	for (i=0;i<n;++i) {
		float d2 = dist2dsq(&tp,&pnts[i]);
		if (d2 < rad2) {
			ret.push_back(i);
		}
	}
	return ret;
}

vector<U32> pc_kdtree2d::insidecirclei(const pointf2& tp,float rad) const // return list of points inside circle using kdtrees
{
	vector<U32> ret;
	return ret;
}

U32 pc_kdtree2d::closestbrutei(const pointf2& tp,float& crad) const // return closest point brute force
{
	float bestrad2 = 1e20f;
	U32 bestidx = -1;
	U32 i,n=pnts.size();
	for (i=0;i<n;++i) {
		float d2 = dist2dsq(&tp,&pnts[i]);
		if (d2 < bestrad2) {
			bestidx = i;
			bestrad2 = d2;
		}
	}
	crad = sqrtf(bestrad2);
	return bestidx;
}

U32 pc_kdtree2d::closesti(const pointf2& tp,float& crad) const // return closest point using kdtrees
{
	U32 ret = 0;
	return ret;
}

/*const pointf2* pc_kdtree2d::getdata() const
{
	return 0;
}*/

void pc_kdtree2d::getsplitdata(vector<pointf2>& lefta,vector<pointf2>& middlea,vector<pointf2>& righta)
{
	lefta = left0;
	middlea = middle0;
	righta = right0;
}

pc_kdtree2d::~pc_kdtree2d()
{
	delete subleft;
	delete subright;
}

	
// 3D

void pc_kdtree3d::build()
{
	//logger("=== setup kdtree points ===\n");
	//logger("frame '0' kdtree points are size = %u\n",pnts.size());

	// bounding box
	boxmin = pointf3x( 10000, 10000, 10000);
	boxmax = pointf3x(-10000,-10000,-10000);
	U32 select = 0;
	pointf3x lastpoint;
	U32 lasti = 0;
	U32 i,npoints=pnts.size();
	for (i=0;i<npoints;++i) {
		if (pnts[i].x < boxmin.x)
			boxmin.x = pnts[i].x;
		if (pnts[i].x > boxmax.x)
			boxmax.x = pnts[i].x;
		if (pnts[i].y < boxmin.y)
			boxmin.y = pnts[i].y;
		if (pnts[i].y > boxmax.y)
			boxmax.y = pnts[i].y;
		if (pnts[i].z < boxmin.z)
			boxmin.z = pnts[i].z;
		if (pnts[i].z > boxmax.z)
			boxmax.z = pnts[i].z;
	}
	logger("bbox kdtree3d is boxmin = (%8.3f,%8.3f,%8.3f), boxmax = (%8.3f,%8.3f,%8.3f)\n",boxmin.x,boxmin.y,boxmin.z,boxmax.x,boxmax.y,boxmax.z);
}

pc_kdtree3d::pc_kdtree3d(const vector<pointf3>& points)
{
	pnts = points;
	build();
}

pc_kdtree3d::pc_kdtree3d(const pointf3* points,U32 npoints)
{
	pnts.assign(points,points+npoints);
	build();
}

pc_kdtree3d::~pc_kdtree3d()
{
	//delete subleft;
	//delete subright;
	logger("delete pc_kdtree3d\n");
}
