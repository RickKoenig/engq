/// call in order of bot,mid,top
var STAND_EPSILON = .0001;
/// brd2 base
var curnorm = vec3.create(); // keeps last norm remembered as mesh is generated
var curdir = vec3.create();
var tempnorm = vec3.create();

function strand_mid_surf(fun,rad) {
   	var b,bp,bm;
	var functor = function(p,q) {
		//p *= .5;
		//q *= .25;
		var v = [],n = [];
    	var h = STAND_EPSILON;
    	var qt = quat.create();
//    	const float hr=1/h;
		if (q == 0) { // calc curdir and curnorm
	    	b = fun(p);
	    	bp = fun(p+h/2);
	    	bm = fun(p-h/2);
			vec3.sub(curdir,bp,bm);
			vec3.normalize(curdir,curdir);
	//    	anorm=findanorm(dir);
	//		anorm=brd2norm;
			adjustnorm(curnorm,curdir);
		}
        q = 2*Math.PI*q;
		quat.setAxisAngle(qt,curdir,-q);
		vec3.transformQuat(n,curnorm,qt);
		vec3.scale(v,n,rad);
		vec3.add(v,v,b);
		return {"v":v,"n":n};
	};
	return functor;
}

function strand_bot_surf(fun,rad) {
   	var b;
	var functor = function(p,q) {
		var v = [],nc = [],n = [];
		vec3.scale(nc,curdir,-1);
    	var qt = quat.create();
    	b = fun(0.0);
        q = 2*Math.PI*q;
		quat.setAxisAngle(qt,curdir,-q);
		vec3.transformQuat(n,curnorm,qt);
		vec3.scale(v,n,p*rad);
		vec3.add(v,v,b);
		return {v:v,n:nc};
	};
	return functor;
}

function strand_top_surf(fun,rad) {
   	var b;
	var functor = function(p,q) {
		p = 1.0 - p;
		var v = [],nc = [],n = [];
		vec3.copy(nc,curdir);
    	var qt = quat.create();
    	b = fun(1.0);
        q = 2*Math.PI*q;
		quat.setAxisAngle(qt,curdir,-q);
		vec3.transformQuat(n,curnorm,qt);
		vec3.scale(v,n,p*rad);
		vec3.add(v,v,b);
		return {v:v,n:nc};
	};
	return functor;
}

/*
// 	logger("in braid2cap0 with p %f, q %f\n",p,q);
   	const float h=STAND_EPSILON;
//	const float hr=1/h;
	pointf3 b=fun(0.0f);
	pointf3 bp=fun(0.0f+h/2);
	pointf3 bm=fun(0.0f-h/2);
	pointf3 dir;
	pointf3 anorm;
	dir.x=bp.x-bm.x;
	dir.y=bp.y-bm.y;
	dir.z=bp.z-bm.z;
	normalize3d(&dir,&dir);
	anorm=findanorm(dir);
    p*=rad;
    q=2*PI*q;
	dir.w=-q;
	pointf3 qt;
	rotaxis2quat(&dir,&qt);
 	quatrot(&qt,&anorm,n);
	v->x=b.x+p*n->x;
	v->y=b.y+p*n->y;
	v->z=b.z+p*n->z;
	n->x=-dir.x;
	n->y=-dir.y;
	n->z=-dir.z;
	brd2norm=anorm; // remember last norm
}
*/
/*
template<typename T>
void strand_bot_surf<T>::operator()(float p,float q,pointf3* v,pointf3* n)
{
// 	logger("in braid2cap0 with p %f, q %f\n",p,q);
   	const float h=STAND_EPSILON;
//	const float hr=1/h;
	pointf3 b=fun(0.0f);
	pointf3 bp=fun(0.0f+h/2);
	pointf3 bm=fun(0.0f-h/2);
	pointf3 dir;
	pointf3 anorm;
	dir.x=bp.x-bm.x;
	dir.y=bp.y-bm.y;
	dir.z=bp.z-bm.z;
	normalize3d(&dir,&dir);
	anorm=findanorm(dir);
    p*=rad;
    q=2*PI*q;
	dir.w=-q;
	pointf3 qt;
	rotaxis2quat(&dir,&qt);
 	quatrot(&qt,&anorm,n);
	v->x=b.x+p*n->x;
	v->y=b.y+p*n->y;
	v->z=b.z+p*n->z;
	n->x=-dir.x;
	n->y=-dir.y;
	n->z=-dir.z;
	brd2norm=anorm; // remember last norm
} */
/*
/// brd2 top
template<typename T>
class strand_top_surf {
	T fun;
	float rad;
public:
    strand_top_surf(T funa,float rada) : fun(funa),rad(rada) {}
    void operator()(float p,float q,pointf3* v,pointf3* n)
    {
//		logger("in braid2cap1 with p %f, q %f\n",p,q);
    	const float h=STAND_EPSILON;
//    	const float hr=1/h;
    	pointf3 b=fun(1.0f);
    	pointf3 bp=fun(1.0f+h/2);
    	pointf3 bm=fun(1.0f-h/2);
		pointf3 dir;
		pointf3 anorm;
		dir.x=bp.x-bm.x;
		dir.y=bp.y-bm.y;
		dir.z=bp.z-bm.z;
    	normalize3d(&dir,&dir);
//    	anorm=findanorm(dir);
		anorm=brd2norm;
        p=(1.0f-p)*rad;
        q=2*PI*q;
    	dir.w=-q;
    	pointf3 qt;
    	rotaxis2quat(&dir,&qt);
     	quatrot(&qt,&anorm,n);
		v->x=b.x+p*n->x;
		v->y=b.y+p*n->y;
		v->z=b.z+p*n->z;
		*n=dir;
    }
};
*/

/*function strand_mid_surf() {
	var wid = 3;
	var hit = 3;
	var startx = -wid;
	var stepx = 2*wid;
	var starty = hit;
	var stepy = -2*hit;
	var functor = function(p,q) {
		var v = [],n = [];
		n[0] = 0;
		n[1] = 0;
		n[2] = -1;
		v[0] = startx + p*stepx;
		v[1] = starty + q*stepy;
		v[2] = 0;
		return {"v":v,"n":n};
	}
	return functor;
}*/

var unitx = [1,0,0];
var unity = [0,1,0];
var unitz = [0,0,1];

function findnormaxis(out,p) {
	var a = Math.abs(p[0]);
	var b = Math.abs(p[1]);
	var c = Math.abs(p[2]);
	if (a<b) {
		if (a<c)
			return vec3.copy(out,unitx);
		else
			return vec3.copy(out,unitz);
			//return unitx;
	} else {
		if (b<c)
			return vec3.copy(out,unity);
		else
			return vec3.copy(out,unitz);
			//return unity;
	}
}

/// uses brd2norm and newdir returns newnorm
function adjustnorm(out,newdir) {
	vec3.copy(tempnorm,out); // save a copy
	var k = -vec3.dot(newdir,out);
	vec3.scale(out,newdir,k);
	vec3.add(out,out,tempnorm);
	vec3.normalize(out,out);
}

function findanorm(out,dir) {
	findnormaxis(out,dir);
	adjustnorm(out,dir);
}

function initbrd2norm(fun) {
	var b = fun(0);
	var h = STAND_EPSILON;
	var bp = fun(h/2);
	var bm = fun(-h/2);
	var anorm;
	vec3.sub(curdir,bp,bm);
	vec3.normalize(curdir,curdir);
	findanorm(curnorm,curdir);
}
	
function buildstrandbotmesh(strandfunc,pcapres,qres,pcapuv,quv,brad) {
	//initbrd2norm(strandfunc);
	return buildpatch(pcapres,qres,pcapuv,quv,strand_bot_surf(strandfunc,brad));
}

function buildstrandmodelbot(name,strandfunc,pcapres,qres,pcapuv,quv,brad,texname,shadername) {
	var mod = Model.createmodel(name);
	if (mod.refcount == 1) {
	    mod.setshader(shadername);
		var strandbotmesh = buildstrandbotmesh(strandfunc,pcapres,qres,pcapuv,quv,brad);
	    mod.setmesh(strandbotmesh);
	    mod.settexture(texname);
	    mod.commit();
	}
    return mod;
}

function buildstrandmidmesh(strandfunc,pres,qres,puv,quv,brad) {
	//initbrd2norm(strandfunc);
	return buildpatch(pres,qres,puv,quv,strand_mid_surf(strandfunc,brad));
}

function buildstrandmodelmid(name,strandfunc,pres,qres,puv,quv,brad,texname,shadername) {
	var mod = Model.createmodel(name);
	if (mod.refcount == 1) {
	    mod.setshader(shadername);
		var strandmidmesh = buildstrandmidmesh(strandfunc,pres,qres,puv,quv,brad);
	    mod.setmesh(strandmidmesh);
	    mod.settexture(texname);
	    mod.commit();
	}
    return mod;
}

function buildstrandtopmesh(strandfunc,pcapres,qres,pcapuv,quv,brad) {
	//initbrd2norm(strandfunc);
	return buildpatch(pcapres,qres,pcapuv,quv,strand_top_surf(strandfunc,brad));
}

function buildstrandmodeltop(name,strandfunc,pcapres,qres,pcapuv,quv,brad,texname,shadername) {
	var mod = Model.createmodel(name);
	if (mod.refcount == 1) {
	    mod.setshader(shadername);
		var strandtopmesh = buildstrandtopmesh(strandfunc,pcapres,qres,pcapuv,quv,brad);
	    mod.setmesh(strandtopmesh);
	    mod.settexture(texname);
	    mod.commit();
	}
    return mod;
}

function buildstrand(name,strandfunc,pres,qres,pcapres,puv,quv,pcapuv,brad,texname,shadername) {
	var ret = new Tree2(name);
	
	initbrd2norm(strandfunc);
	
	var modbot = buildstrandmodelbot(name+"_bot",strandfunc,pcapres,qres,pcapuv,quv,brad,texname,shadername);
	var bot = new Tree2("bot");
	bot.setmodel(modbot);
	ret.linkchild(bot);
	
	var modmid = buildstrandmodelmid(name+"_mid",strandfunc,pres,qres,puv,quv,brad,texname,shadername);
	var mid = new Tree2("mid");
	mid.setmodel(modmid);
	ret.linkchild(mid);
	
	var modtop = buildstrandmodeltop(name+"_top",strandfunc,pcapres,qres,pcapuv,quv,brad,texname,shadername);
	var top = new Tree2("top");
	top.setmodel(modtop);
	ret.linkchild(top);
	
	return ret;
}

function changestrandmesh(tree,strandfunc,pres,qres,pcapres,puv,quv,pcapuv,brad) {
	initbrd2norm(strandfunc);
	
	var modbot = tree.children[0].mod;
	var strandbotmesh = buildstrandbotmesh(strandfunc,pcapres,qres,pcapuv,quv,brad);
	modbot.changemesh({verts:strandbotmesh.verts,norms:strandbotmesh.norms});
	
	var modmid = tree.children[1].mod;
	var strandmidmesh = buildstrandmidmesh(strandfunc,pres,qres,puv,quv,brad);
	modmid.changemesh({verts:strandmidmesh.verts,norms:strandmidmesh.norms});
	
	var modtop = tree.children[2].mod;
	var strandtopmesh = buildstrandtopmesh(strandfunc,pcapres,qres,pcapuv,quv,brad);
	modtop.changemesh({verts:strandtopmesh.verts,norms:strandtopmesh.norms});
	
}

/*
/// build the braid root with body and caps
template <typename T>
tree2* buildstrand(const T& func,int pres,int qres,int pcapres,float brad2,const C8* matname)
{
	tree2* bt=new tree2("brdtree");
	
	
/// bottom cap
	modelb* modcc0b=model_create(unique());
	if (model_getrc(modcc0b)==1) {
//        pushandsetdir("gfxtest");
		buildpatch2t(modcc0b,pcapres,qres,(float)pcapres,(float)qres,strand_bot_surf<T>(func,brad2),"maptest.tga","bark.tga",matname);
//	    popdir();
	}
	tree2* brdtreec0=new tree2("brdtreec0");
	brdtreec0->setmodel(modcc0b);
//	delete brdtreec0;
	bt->linkchild(brdtreec0); 
	
	
/// body
	modelb* modcmb=model_create(unique());
//	pointf3 dirb={0,1,0,0};
	if (model_getrc(modcmb)==1) {
//        pushandsetdir("gfxtest");
		buildpatch2t(modcmb,pres,qres,(float)pres,(float)qres,strand_mid_surf<T>(func,brad2),"maptest.tga","rengst.jpg",matname);
//	    popdir();
	}
	tree2* brdtreem=new tree2("brdtreem");
	brdtreem->setmodel(modcmb);
 	bt->linkchild(brdtreem);
 	
 	
/// top cap
	modelb* modcc1b=model_create(unique());
	if (model_getrc(modcc1b)==1) {
//        pushandsetdir("gfxtest");
		buildpatch2t(modcc1b,pcapres,qres,(float)pcapres, (float)qres,strand_top_surf<T>(func,brad2),"maptest.tga","bark.tga",matname);
//	    popdir();
	}
	tree2* brdtreec1=new tree2("brdtreec1");
	brdtreec1->setmodel(modcc1b);
	bt->linkchild(brdtreec1); 
	
	
	return bt;
}

*/