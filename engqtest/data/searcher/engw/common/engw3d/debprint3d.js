var debprint = {};

debprint.vp = null; // debprint viewport
debprint.fontmodel; // debprint font model
debprint.fonttree; // debprint font tree
debprint.indebprint = true; // is it on or not
debprint.depth; // z
debprint.fontname = "font0.png";

debprint.idx = 0;
debprint.sclidx = 0;
debprint.strarr = null;

debprint.startline = 1;
debprint.totalines = 0;
debprint.ndrawlines = 20;

debprint.debugkey = "`".charCodeAt(0);
debprint.enable = true;

debprint.lastmwheel;
debprint.step = .25;

debprint.list = {
	main:[
		//{name:"debprint",key:"vp",obj:debprint},
		"debprint.vp",
		//{name:"debprint.fontmodel",key:"fcolor",obj:debprint.fontmodel.mat},
		"fpswanted",
		"frametimewanted",
		"frametimeactual",
		"fpsavg",
		"frametimeavg",
		"curfps",
		"proctimeactual",
		"proctimeavg",
		"frame",
		"testvec",
		"testobj",
		"testobj2",
	],
	sub:[
		"mainvp",
		"bool0",
		"bool1",
		"bool2",
	]
};

var bool0 = true;
var bool1 = false;
var bool2 = true;
var testvec = [2,[4,5],3,5.1,{six:6.1,seven:7.1},function(a){},null,undefined,true,false];
var testobj = {hey:null,ho:undefined,str:"astring",two:2,three:3.14,sub:{s1:1,s2:2},avec:testvec};
var testobj2 = {hiho:[1,2],subobj:{hum:"Here I come.",dinger:"What?"}};

debprint.addlist = function(name,list) {
	debprint.buildstrarr();
	debprint.idx = debprint.strarr.length;
	debprint.list[name] = list;
};
	
debprint.removelist = function(name) {
	delete debprint.list[name];
};

//("testvar from state10");
//	("fontmat",fml);
// take a primitive (not and object or array) and return a string
debprint.nicestr = function(obj) {
	if (obj === undefined || obj === null)
		return obj;
	switch(obj.constructor) {
	case Number:
		if ((obj%1) == 0) {
			return obj;
		} else {
			return obj.toFixed(5);
		}
		break;
	case String:
		return "\"" + obj + "\"";
		break;
	case Function:
		return "function";
		break;
	default:
		return obj;
		break;
	}
};

debprint.alter = function(parent,key,amount,clear) {
	if (!parent)
		return;
	var obj = parent[key];
	if (obj !== undefined && obj !== null) {
		switch(obj.constructor) {
			case Number:
				parent[key] += amount;
				if (clear)
					parent[key] = 0;
				break;
			case Boolean:
				parent[key] = !parent[key];
				if (clear)
					parent[key] = false;
				break;
			//case String:
			//case Function:
			default:
				break;
		}
	}
};

// returns an array of {name:name = val,parent:parent,key:key}
debprint.buildstr = function(name,parent,key) {
	var obj = parent[key];
	var j,an;
	var ret = [];
	var ele = {parent:parent,key:key};
	if (obj !== undefined && obj !== null) {
		switch(obj.constructor) {
		case Number:
		case String:
		case Boolean:
		case Function:
			ele.name = name;
			ret.push(ele);
			break;
		case Array:
			an = obj.length;
			for (j=0;j<an;++j) {
				var aval = obj[j];
				ret = ret.concat(debprint.buildstr(name + "[" + j + "]",obj,j));
			}
			break;
		default:
			if (typeof obj == "object") {
				var attr;
				for (attr in obj) {
					// pass over some members to avoid circular and too much data
					if (attr == "parent")
						continue;
					if (attr == "children")
						continue;
					if (attr == "possamp")
						continue;
					if (attr == "qrotsamp")
						continue;
					if (attr == "mod")
						continue;
					if (attr == "camattach")
						continue;
					if (attr == "lookat")
						continue;
					var oval = obj[attr];
					if (oval != undefined) {
						ret = ret.concat(debprint.buildstr(name + "." + attr,obj,attr));
					}
				}
			} else {
				// shouldn't get here
				alert("shouldn't get here");
			}
			break;
		}
	} else {
		ele.name = name;
		ret.push(ele);
	}
	return ret;
};
	
debprint.init = function() {
	// get size of texture
	var tex = Texture.createtexture(debprint.fontname);
	var glyw = tex.width/8; // 16
	var glyh = tex.height/16; // 32
	tex.glfree();
	debprint.depth = gl.viewportHeight/2;//*8/h;
	debprint.depth *= 2; // half pixel size still looks good, comment out for true 1 to 1 texel to pixel mapping
	debprint.vp = {
	   	// where to draw
		target:null,
		// clear
	//	clearflags:gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT,
	//	clearcolor:[0,.75,1,1],
		// orientation
		"trans":[0,0,-debprint.depth],
		"rot":[0,0,0],
		// frustum
		near:.002,
		far:10000.0,
		zoom:1,
		asp:gl.asp,
		isortho:true,
		ortho_size:debprint.depth,
	};
// build font model and tree for debprint
    debprint.fontmodel = new ModelFont("debmodel",debprint.fontname,"font2c",glyw,glyh,100,100);
    debprint.fontmodel.mat.fcolor = [0,1,0,1];
    debprint.fontmodel.mat.bcolor = [0,0,0,1];
    debprint.fontmodel.flags |= modelflagenums.NOZBUFFER; // always in front when drawn
	debprint.fontmodel.print("In Debprint");
	debprint.fonttree = new Tree2("debtree");
	debprint.fonttree.trans = [-debprint.depth,debprint.depth - glyh*debprint.startline,0];
	debprint.fonttree.setmodel(debprint.fontmodel);
	debprint.idx = 0;
	// build up the strings
	debprint.curline = 0;
	debprint.lastmwheel = input.mwheel;
	var fml = [
//		{name:"debprint.fontmodel",key:"mat",obj:debprint.fontmodel},
		"debprint.fontmodel.mat",
//		{name:"debprint.fontmodel.mat",key:"fcolor",obj:debprint.fontmodel.mat},
//		{name:"debprint.fontmodel.mat",key:"bcolor",obj:debprint.fontmodel.mat},
	];
	debprint.addlist("debprintfontmat",fml);
};

debprint.buildstrarr = function() {
	var i,j;
	var g = debprint.list.length;
	debprint.strarr = [];
	for (g in debprint.list) {
		var sublist = debprint.list[g];
		var n = sublist.length;
		var head = {header:g};
		debprint.strarr.push(head);
		for (i=0;i<n;++i) {
			var name = sublist[i];
			var st;
			if (name.constructor == String) { // global object
				var par = window;
				var sp = name.split(".");
				var sn = sp.length;
				for (j=0;j<sn-1;++j) {
					par = par[sp[j]];
				}
				//st = debprint.buildstr(name,window,name); // name,parent,key
				st = debprint.buildstr(name,par,sp[sn-1]); // name,parent,key
			} else { // object
				alert("old object method");
				st = debprint.buildstr(name.name + "." + name.key,name.obj,name.key); // name,parent,key
			}
			debprint.strarr = debprint.strarr.concat(st); // name,parent,key
		}
	}
};
	
debprint.proc = function() {
	// toggle debprint
	if (debprint.enable && input.key == debprint.debugkey) {
		debprint.indebprint = !debprint.indebprint;
		//input.key = 0;
	}
	//debprint.fonttree.proc(); // proc the tree, probably does nothing
	// convert list to printed list of variables and their values
	if (!debprint.indebprint)
		return;
	var i;
	debprint.buildstrarr();
	debprint.totallines = debprint.strarr.length;
	n = debprint.totallines;
	var d = debprint.ndrawlines;
	if (d > n)
		d = n;
	var ele;
	switch(input.key) {
	// down
	case keycodes.DOWN:
		++debprint.idx;
		input.key = 0;
		break;
	case keycodes.PAGEDOWN:
		debprint.idx += 5;
		input.key = 0;
		break;
	// up
	case keycodes.UP:
		--debprint.idx;
		input.key = 0;
		break;
	case keycodes.PAGEUP:
		debprint.idx -= 5;
		input.key = 0;
		break;
	// left
	case keycodes.LEFT:
		ele = debprint.strarr[debprint.idx];
		debprint.alter(ele.parent,ele.key,-debprint.step);
		input.key = 0;
		break;
	// right
	case keycodes.RIGHT:
		ele = debprint.strarr[debprint.idx];
		debprint.alter(ele.parent,ele.key,debprint.step);
		input.key = 0;
		break;
	// clear
	case keycodes.NUM5:
		var ele = debprint.strarr[debprint.idx];
		debprint.alter(ele.parent,ele.key,0,true);
		input.key = 0;
		break;
	// step
	case "-".charCodeAt(0):
		if (debprint.step > .0001)
			debprint.step *= .25;
		input.key = 0;
		break;
	case "+".charCodeAt(0):
	case "=".charCodeAt(0):
		if (debprint.step < 10000)
			debprint.step *= 4;
		input.key = 0;
		break;
	}
	// wheel
	debprint.idx -= input.wheelDelta;
	if (debprint.idx < 0)
		debprint.idx = 0;
	else if (debprint.idx >= n)
		debprint.idx = n - 1;
	// scrolling text
	debprint.sclidx = debprint.idx - Math.floor((d-1)/2);
	if (debprint.sclidx < 0)
		debprint.sclidx = 0;
	else if (debprint.sclidx > n - d)
		debprint.sclidx = n - d;
	var text = "";
	// build up the print strings
	for (i=debprint.sclidx;i<d+debprint.sclidx;++i) {
		var ext = "";
		if (debprint.idx == i)
			ext = "->";
		else
			ext = "  ";
		var ele = debprint.strarr[i];
		if (ele.header) {
			text += ext + "===== " + ele.header + " =====\n";
		} else {
			text += ext + ele.name + " = " + debprint.nicestr(ele.parent[ele.key]) + "\n";
		}
	}
	// display step
	//text += "\nstep = " + debprint.step.toFixed(5) + "\n";
	//text += "\nstep = " + debprint.step.toPrecision(5) + "\n";
	if (debprint.step >= .25)
		text += "\n      step = " + debprint.step + "\n";
	else
		text += "\n      step = " + debprint.step.toPrecision(3) + "\n";
/*	
	// checkerboard pattern
	var testalign = String.fromCharCode(127,127);
	//var testalign = String.fromCharCode(24,25);
	for (j=0;j<10;++j) {
		for (i=0;i<5;++i) {
			text += testalign;
		}
		text += "\n";
	}
*/
	debprint.fontmodel.print(text);
};

debprint.draw = function() {
	if (debprint.indebprint) {
		beginscene(debprint.vp);
		debprint.fonttree.draw();
	}
};

debprint.exit = function() {
	debprint.fonttree.glfree();
};
