/*
// input: some variable
// output: a json string representing the variable
function serialize(obj) {
	var returnVal;
	if (obj != undefined) {
		switch(obj.constructor) {
		case Array:
			var vArr = "[";
			for(var i=0;i<obj.length;i++) {
				if (i>0) vArr += ",";
				vArr += serialize(obj[i]);
			}
			vArr += "]";
			return vArr;
		case String:
			returnVal = "\"" + obj + "\""; // cheap, should handle quotes better
			return returnVal;
		case Number:
			returnVal = isFinite(obj)?obj.toString():null;
			return returnVal;
		case Date:
			returnVal = "#" + obj + "#";
			return returnVal;
		default:
			if (typeof obj == "object") {
				var vobj = [];
				for (attr in obj) {
					if (typeof obj[attr] != "function") {
						vobj.push('"' + attr + '":' + serialize(obj[attr]));
					}
				}
				if (vobj.length > 0)
					return "{" + vobj.join(",") + "}";
				else
					return "{}";
			} else {
				return obj.toString();
			}
		}
	}
	return null;
}
*/
function testserial() {
	var s = "";
//	for (i=65;i<75;++i) {
	var i;
	for (i=32;i<127;++i) {
		s += String.fromCharCode(i);
		//s += String.fromCharCode(127+32-i-1);
	}
	var olist = [
		s,
		undefined,
		[3,4,5],
		["hi there",4,{}],
		"hi there \t\n\"'hey/hey",
		new Date(),
		{"x\"x":3,"y":4},
		true,
		false,
		null,
		"ain't",
	];
	try {
		var r = JSON.stringify(olist);
	} catch(err) {
		var r = null;
	}
	try {
		var p = JSON.parse(r);
	} catch(err) {
		var p = null;
		}
	var done = "done";
	return r;
}

function Cir(x,y,r) {
	this.x = x;
	this.y = y;
	this.r = r;
}

Cir.prototype.show = 
	function() {
		return "x = " + this.x.toString() + 
		", y = " + this.y.toString() + 
		", r = " + this.r.toString();
	}
;

function Rec(x,y,w,h) {
	this.x = x;
	this.y = y;
	this.w = w;
	this.h = h;
}

Rec.prototype.show = 
	function() {
		return "x = " + this.x.toString() + 
		", y = " + this.y.toString() + 
		", w = " + this.w.toString() + 
		", h = " + this.h.toString();
	}
;

// give object the specialized methods they need
function methodify(s) {
	var ts = typeof s;
	if (ts !== "object")
		return;
	if (s instanceof Array) { // array
		for (var i=0;i<s.length;++i)
			methodify(s[i]);
	} else { // object
		var keys = Object.keys(s);
		for (var i=0;i<keys.length;++i) {
			if (typeof s[keys[i]] === "object") {
				methodify(s[keys[i]]);
			}
		}
		if (s.stype) { // name of object, copy methods from object prototype
			var typ = window[s.stype];
			if (typ) {
				var prot = typ.prototype;
				if (prot) {
					var keys = Object.keys(prot);
					for (var i=0;i<keys.length;++i) {
						if (typeof prot[keys[i]] === "function") {
							s[keys[i]] = prot[keys[i]];
						}
					}
				}
			}
			//s.draw = window[s.stype].prototype.draw;
			//s.show = window[s.stype].prototype.show;
		}
	}
}

function testproto() {
	var o1 = new Cir(3,4,5);
	var o2 = new Rec(6,7,8,9);
	var o3 = new Cir(30,40,50);
	var o4 = new Rec(60,70,80,90);
	var s1 = o1.show();
	var s2 = o2.show();
	var s1 = o3.show();
	var s2 = o4.show();
	var po = {"a" : {"stype" : "Cir"}, "b" : {"stype" : "Rec"}, "c" : {"stype" : "Bar"}, "d" : {"what" : "Dank"}};
	methodify(po);
	var done = "done";
}

// convert < > & " ' ' \n to &lt; &gt; &amp; &quot; &nbsp; <br/>
function escapehtml(s) {
	if (typeof s !== 'string')
		return s;
	var r = "";
	var i;
	for (i=0;i<s.length;++i) {
		var c = s.charAt(i);
		if (c == '<')
			c = '&lt;';
		else if (c == '>')
			c = '&gt;';
		else if (c == '&')
			c = '&amp;';
		else if (c == '"')
			c = '&quot;';
		else if (c == ' ')
			c += '&nbsp;';
		else if (c == '\n')
			c += '<br/>';
		r += c;		
	}
	return r;
}

function cloneObject(obj) {
	var clone = {};
	for (var i in obj) {
		if (obj[i].constructor === Array) {
			clone[i] = obj[i].slice(0);
		} else {
			var objtype = typeof(obj[i]);
			if (objtype == "object")
				clone[i] = cloneObject(obj[i]);
			else
				clone[i] = obj[i];
		}
	}
	return clone;
}

function cloneObject1(obj) {
	var clone = {};
	for (var i in obj)
		clone[i] = obj[i];
	return clone;
}
