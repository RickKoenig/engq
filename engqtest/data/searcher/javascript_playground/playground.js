var /*frm,*/nam,col; // cookies
var out; // textareas
var appr={}; // playground default
var app={}; // playground
var rvn,wvn,wvv; // read and write variables

// setup a test app
appr.anarray = [3,4,{five:5},6];
appr.anobject = {first:{"one":"1st","One":"1ST"},second:"place",third:["show","bronze"]};
appr.astring = "hello";
appr.anumber = 3.14;
appr.anull = null;
appr.atrue = true;
appr.afalse = false;
appr.aundefined = undefined;
// app
appr.main = function () {
    var i;
    print("in main!\n");
    for (i = 0; i < 5; ++i) {
        print("fact(" + i + ") = " + app.afunfact(i) + "\n");
    }
    print("an array = '" + JSON.stringify(app.anarray) + "'\n");
    print("an object = '" + JSON.stringify(app.anobject) + "'\n");
    print("a string = '" + app.astring + "'\n");
    print("a number = '" + app.anumber + "'\n");
    print("a null = '" + app.anull + "'\n");
    print("a true = '" + app.atrue + "'\n");
    print("a false = '" + app.afalse + "'\n");
    print("an undefined = '" + app.aundefined + "'\n");
} 
// app
appr.afunfact = function (n) {
    	if (n == 0) {
        return 1;
    }
    return n * app.afunfact(n - 1);
} 

// html
function form2cookie() {
	writecookie("name",nam.value);
	writecookie("color",col.value);
}

// html
function cookie2form() {
	nam.value = readcookie("name");
	col.value = readcookie("color");
}

// html
function con2out() {
//	alert(con);
//	alert(out);
	out.value = wvv.value;
//	out.value = con.value;
}

// html
function clearout() {
	out.value = "Cleared output:\n\n";
}

// html
function proccondown(event) {
/*
//	var e = window.event ? event : e; //distinguish between IE's explicit event object (window.event) and Firefox's implicit.
	var e = window.event ? window.event : event;
	var unicode = e.keyCode;
	var k = String.fromCharCode(unicode);
//	if (k == "A")
//		document.body.style.fontSize="120%";
//	if (k == "Z")
//		document.body.style.fontSize="100%";
	//	out.value = "1\n\t2\n\t\t3\n\n"
//	out.value += k;
*/
}

// html
function procconup(event) {
/*
//	var e = window.event ? event : e; //distinguish between IE's explicit event object (window.event) and Firefox's implicit.
	var e = window.event ? window.event : event;
	var unicode = e.keyCode;
	var k = String.fromCharCode(unicode);
	if (k == "A")
		document.body.style.fontSize="120%";
	if (k == "Z")
		document.body.style.fontSize="100%";
	//	out.value = "1\n\t2\n\t\t3\n\n"
	out.value += k; 
*/
} 

// html
function enterhit(event) {
	var e = window.event ? window.event : event;
	var unicode = e.keyCode;
	var k = String.fromCharCode(unicode);
	return k == "\r";
}
	
// gets selected option into 'write variable name'
// html
function selectname() {
	if (rvn.selectedIndex>=0) {
		var chosenoption=rvn.options[rvn.selectedIndex] 
	//	if (chosenoption.value!="nothing"){
	 		wvn.value = chosenoption.value;
	 //	}
	 	readvar();
	} else {
		wvn.value = "";
		wvv.value = "";		
	}
}

// html
function writevar() {
	writevara(wvn.value,wvv.value);
	selectname();
}

// html
function writevarenter(event) {
	if (enterhit(event))
		writevar();
}

// html
function readvarenter(event) {
	if (enterhit(event))
		readvar();
}

// html
function listvars() {
	var key;
	print("app. variables\n\t");
	var ar = [];
	for (key in app) {
		if (hasOwnProperty.call(app,key)) {
//			print(key + " ");
			ar.push("'" + key + "'");
		}
	}
	ar.sort();
	print(ar);
	print("\n\n");
}

// html
function listvarsverbose() {
	var key;
	print("app. variables verbose\n");
	var ar = [];
 	for (key in app) {
		if (hasOwnProperty.call(app,key)) {
//			showvarnowvv(key);
			ar.push(key);
		}
	}
	ar.sort();
 	var i;
 	for (i=0;i<ar.length;++i) {
		showvarnowvv(ar[i]);
	}
	print("\n");
}

// html
function runmain() {
	try {
		app.main();
	} catch (err) {
		print("Error running main.\n");
	}
	initoptions(rvn.value);
}

// html
function loadprogram() {
	loadprogramnsn();
	selectname();
}

// html
function saveprogram() {
	var str = " _app_begin\n";
	var key;
	var ar = [];
 	for (key in app) {
		if (hasOwnProperty.call(app,key)) {
			ar.push(key);
		}
	}
	ar.sort();
 	var i;
 	for (i=0;i<ar.length;++i) {
		var name = ar[i];
		var val = app[name];
		var res;
		if (typeof val == 'function') {
			// continue;
			res = serialize(val);
		} else {
			res = JSON.stringify(val,null,1);
		}
		str += name + " _app_colon " + res;
		if (i != ar.length - 1) {
			str += " _app_comma ";
		}
		str += "\n";
	}
	str += " _app_end\n";
	print("save string is = '" + str + "'\n");
	writecookie("app",str);
}

// html
function init() {
//	frm = document.getElementById("form1");
	nam = document.getElementById("name");
	rvn = document.getElementById("appreadvarname2");
	wvn = document.getElementById("appwritevarname");
	wvv = document.getElementById("appwritevarvalue");
	col = document.getElementById("color");
//	con = document.getElementById("console");
	out = document.getElementById("output");
	cookie2form();
	loadprogramnsn();
	if (!countProperties(app))
		resetprogram();
	initoptions("main");
	selectname();
// test regexp	
/*
	var str = "   \n     this is  text   \n    ";
	reg = /^\s*|\s*$/g;
	var str2 = str.replace(reg,"");
	print("test is '" + str + "' '" + str2 + "'\n"); 
*/
}

// html
function resetprogram() {
	app = clone(appr);
	initoptions("main");
	selectname();
}

/////////////////// derived functions
function loadprogramnsn() {
	app = new Object();
	var str = readcookie("app");
//	print("cookie app = '" + str + "'\n");

	var appbegin = "_app_begin";
	var appcolon = "_app_colon";
	var appcomma = "_app_comma";
	var append = "_app_end";
	
	var mid,end;
	// find start of 'name'
	mid = str.indexOf(appbegin) + appbegin.length;
	while(true) {
		// find end of 'name;
		end = str.indexOf(appcolon,mid);
		if (end == -1)
			break;
		var n = str.substring(mid,end);
		print("var name = '" + n + "'\n");
		
		// setup for 'value'
		mid = end + appcolon.length; // start of 'value'
		// find end of 'value'
		end = str.indexOf(appcomma,mid); // check comma
		var len = appcomma.length;
		var isend = false;
		if (end == -1) {
			end = str.indexOf(append,mid); // no, check end
			len = append.length;
			isend = true;
		}
		if (end == -1)
			break;
		var v = str.substring(mid,end);
		print("var value = '" + v + "'\n");
		
		reg = /^\s*|\s*$/g;
		var n2 = n.replace(reg,"");
		var v2 = v.replace(reg,"");
		writevara(n2,v2);
		if (isend)
			break;
		mid = end + len;
	}
/*	end=document.cookie.indexOf(";", begin);
	if (begin != -1) {
		var mid = begin + name.length + 1; // skip past 'name='
		if (end == -1) {
			 end=document.cookie.length;
		}
		val= unescape(document.cookie.substring(mid, end));
	} else {
		val = "---";
	} */
	initoptions("main");
}

function countProperties(obj) {
    var count = 0;

    for(var prop in obj) {
        if(obj.hasOwnProperty(prop))
                ++count;
    }

    return count;
}

function clone(obj){
    if(obj == null || typeof(obj) != 'object')
        return obj;

    var temp = obj.constructor(); // changed

    for(var key in obj)
        temp[key] = clone(obj[key]);
    return temp;
}

function writevara(name,value) {
	reg = /^function/;
	try {
		var res = reg.test(value); // look for a function
		if (res) {
			var v = eval('(' + value + ')');
			app[name] = v; // add or replace function
			initoptions(name);
		} else if (value == 'undefined') {
			var v = undefined;
			delete app[name]; // remove var
			initoptions("main");
		} else {
			var v = JSON.parse(value);
			app[name] = v; // add or replace var
			initoptions(name);
		}
//		selectname();
	} catch (err) {
		print("Syntax Error\n\n");
	}
}

function print(text) {
	out.value += text;
	out.scrollTop = out.scrollHeight;
}

function writecookie(name,value) {
	var when = new Date();
	// expire 24 hours from now
	//when.setTime(when.getTime() + 24 * 60 * 60 * 1000);
	// expire 30 years from now
	when.setFullYear(when.getFullYear() + 30);
	document.cookie = escape(name) + "=" + escape(value) +
	  ";expires=" + when.toGMTString();
	print("write cookie length = " + document.cookie.length + "\n")
}
	
function readcookie(name) {
	var begin,mid,end,val;
	
	print("read cookie length = " + document.cookie.length + "\n")
	begin=document.cookie.indexOf(name);
	end=document.cookie.indexOf(";", begin);
	if (begin != -1) {
		var mid = begin + name.length + 1; // skip past 'name='
		if (end == -1) {
			 end=document.cookie.length;
		}
		val= unescape(document.cookie.substring(mid, end));
	} else {
		val = "---";
	}
	return val;
}
	
function get_type(thing) {
    if(thing===null)
    	return "[object Null]"; // special case
    return Object.prototype.toString.call(thing);
}

function serialize(_obj)
{
	if (_obj === null)
		return 'null';
	if (_obj == undefined)
		return 'undefined';
/*   // Let Gecko browsers do this the easy way
   if (typeof _obj.toSource !== 'undefined' && typeof _obj.callee === 'undefined')
   {
      return _obj.toSource();
   }
*/
   // Other browsers must do it the hard way
   switch (typeof _obj)
   {
      // numbers, booleans, and functions are trivial:
      // just return the object itself since its default .toString()
      // gives us exactly what we want
      case 'number':
      case 'boolean':
      case 'function':
         return _obj;
         break;

      // for JSON format, strings need to be wrapped in quotes
      case 'string':
         return '"' + _obj + '"';
         break;

      case 'object':
         var str;
         if (_obj.constructor === Array || typeof _obj.callee !== 'undefined')
         {
            str = '['; // array
            var i, len = _obj.length;
            for (i = 0; i < len-1; i++) { str += serialize(_obj[i]) + ','; }
            str += serialize(_obj[i]) + ']';
         }
         else
         {
            str = '{'; // object
            var key;
            for (key in _obj) {
            	str += '"' + key + '":' + serialize(_obj[key]) + ',';
            }
            str = str.replace(/\,$/, '') + '}'; // get rid of last comma
         }
         return str;
         break;

      default:
         return 'UNKNOWN';
         break;
   }
}

function readvar() {
	showvar(rvn.value);
}

function showvar(name) {
	showvarnowvv(name);
	wvv.value = str;
	print("\n");
}

function showvarnowvv(name) {
	var val = app[name];
	if (typeof val == 'function')
		str = serialize(val);
	else
		str = JSON.stringify(val,null,1);
	print("'" + name + "' = '" + str + "' type = " + get_type(val) + "\n");
}

function addoption(text,val,sel) {
	var opt = new Option(text,val,sel,sel);
	try {
  		rvn.add(opt);
  	} catch (e) {
		// for IE earlier than version 8
		rvn.add(opt,null);
	}
}
	
function clearoptions() {
	while(rvn.length>0)
		rvn.remove(rvn.length-1); //removes last option within SELECT
}

function initoptions(selname) {
//	rvn.options = [];
//	var myselect=document.getElementById("sample")
//	try {
//		rvn.add(new Option("New Last Option", "4"), null) //add new option to end of "sample"
//		rvn.add(new Option("New First Option", "0"), myselect.options[0]) //add new option to beginning of "sample"
//	} catch(e) { //in IE, try the below version instead of add()
//		rvn.add(new Option("New Last Option", "thelast")); //add new option to end of "sample"
//		rvn.add(new Option("New First Option", "thefirst"), 0); //add new option to beginning of "sample"
//	}
	clearoptions();
/*	addoption("Kiwi","fruit");
	addoption("Carrot","veggie");
	addoption("Bread","grain",true);
	addoption("Penguin","bird");
//	removeoption("veggie");
	selectname(); */

	print("selname = '" + selname + "'\n");
	var key;
//	print("app. variables verbose\n");
	var ar = [];
 	for (key in app) {
		if (hasOwnProperty.call(app,key)) {
//			showvarnowvv(key);
			ar.push(key);
		}
	}
	ar.sort();
 	var i;
 	for (i=0;i<ar.length;++i) {
		addoption(ar[i],ar[i],ar[i]==selname);
	}
	print("\n");
}
