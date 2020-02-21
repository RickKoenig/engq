// javascript for main.html

var inputevents = "inputevents = ";
var logger_str = "logger = ";

// globals
//var serverip = ""; // engine sprites
var serverip = ""; // engine sprites 'searcher' web server
var serverip2 = ""; // engine sprites2 'apache' web server
//var serverip = "http://99.108.140.134:8080/"; // engine sprites 'searcher' web server
//var serverip2 = "http://99.108.140.134/"; // engine sprites2 'apache' web server

var eoutstateman;
var escroll;
var eoutkeystate;
var eoutinputstate;
var eoutinputevents;
var eoutlogger;

var fpswanted = 20;
var invfpswanted;
//var geominvfpswanted;
var intervaltime = 1000/fpswanted; // 8 fps for now

var viewx;// = 600;
var viewy;// = 500;
var frame = 0;

var showstate = true;
//var showscroll = true;
var showprint = true;

//var showstate = false;
var showscroll = false;
//var showprint = false;

// debug on the elements
function drawelements() {
	// show state
	if (showstate) {
		//eoutstateman.firstChild.nodeValue = "State " + state;
		printareadraw(eoutstateman,"State " + state);
	}
	
	// show scroll
	if (showscroll) {
		//escroll.firstChild.nodeValue = 
		//	" sclvel = " + scrollvelx + "," + scrollvely + " scl = " + scrollx + "," + scrolly;
		printareadraw(escroll,"sclvel = " + scrollvelx + "," + scrollvely + " scl = " + scrollx + "," + scrolly);
	}
		
	// show much stuff
	if (showprint) {
		var maxsize = 300;
		// show keystate
		var ks = "keystate = ";
		var len = input.keystate.length;
		var i;
		for (i=0;i<len;++i) {
			if (input.keystate[i])
				ks += i + " ";
		}
		//eoutkeystate.firstChild.nodeValue = ks;
		printareadraw(eoutkeystate,ks);
		
		// show input
		//eoutinputstate.firstChild.nodeValue = "inputstate = mx " + input.mx + " my " + input.my + " mbut " + input.mbut + " mclick " + input.mclick + " key " + input.key;
		printareadraw(eoutinputstate,"inputstate = mx " + input.mx + " my " + input.my + " mbut " + input.mbut + " mwheel " + input.wheelPos + " mclick0 " + input.mclick[0] + " mclick1 " + input.mclick[1] + " mclick2 " + input.mclick[2] + " key " + input.key);
		
		// show inputevents
		if (inputevents.length > maxsize) {
			inputevents = inputevents.substr(inputevents.length-maxsize,maxsize);
		}
		//eoutinputevents.firstChild.nodeValue = inputevents;
		printareadraw(eoutinputevents,inputevents);
		
		// show logger
/*		if (logger_str.length > maxsize) {
//			logger_str = logger_str.substr(logger_str.length-maxsize,maxsize);
		} */
		//eoutlogger.outerHTML = "Howdy!";
		//eoutlogger.firstChild.nodeValue = logger_str;
		printareadraw(eoutlogger,logger_str);
	}
	
	// show image cache
/*	var str = "";
	for (var nam in imagecacher) {
		str += "C[" + nam + "] = " + imagecacher[nam].size+ ",";	
	} */
}

function maindebugclear() {
	clearbuts('debprint');
	clearbuts('scroller');
	clearbuts('stateman');
}

function maindebugsetbefore() {
	if (showstate) {
		// eoutstateman = document.getElementById('stateman');
		setbutsname('stateman');
		eoutstateman = makeaprintarea();
		makeabut('Prev State',prevstate);
		makeabut('Next State',nextstate);
		makeabut('Clear Log',clearlog,null,null,true); // wide margins
		//makeahr();
	}
}

function nextstate() {
	changestate(state+1);
}

function prevstate() {
	changestate(state-1);
}

function clearlog() {
	inputevents = "inputevents 'clear' = ";	
	logger_str = "logger 'clear' = ";	
}

function maindebugsetafter() {
	if (showscroll || showprint)
		;//makeahr();
	if (showscroll) {
		scrollerinit();
	}
	if (showprint) {
		setbutsname('debprint');
		// eoutkeystate = document.getElementById('keystate');
		eoutkeystate = makeaprintarea();
		// eoutinputstate = document.getElementById('inputstate');
		eoutinputstate = makeaprintarea();
		// eoutinputevents = document.getElementById('inputevents');
		eoutinputevents = makeaprintarea();
		// eoutlogger = document.getElementById('logger');
		eoutlogger = makeaprintarea();
	}
}

function mainload() {
	//preloadShaders();
	preloadshaderlist("shaders/shaderlist.txt");
	//preloadtext("shaders/basic.ps");
	//preloadtext("shaders/basic.vs");
	//preloadimg("pics/panel.jpg");
	preloadimg("pics/maptestnck.png"); // preload the default texture
	preloadimg("pics/smallfont.png"); // preload the default texture
	preloadimg("pics/take0007.jpg"); // preload the default texture
	preloadimg("pics/coin_logo.png"); // preload the default texture
	//preloadimg("../tracks2/peelingwood.jpg");
	setloaddonefunc(maininit);
	document.oncontextmenu = function() {return false;};
}

var defaultimage;

function maininit() {
	defaultimage = preloadedimages["maptestnck.png"];
	sprites_init();
	gl_init();
	initinput();
	// get some nodes from html
	//maindebug();
	//initstate();
	changestate(startstate);
	//mainproc(); // do 1 proc right away
	window.setInterval(mainproc,intervaltime);
}

function mainproc() {
	inputproc();
	if (input.keystate[keycodes.down]) {
		clearlog();
	}
	if (input.key)
		;//logger_str += "KB " + input.key + " ";
// proc
	//sprites_reset();
	//gl_reset();
	invfpswanted = 1.0/fpswanted;
	procstate();
// draw
	++frame;
}

// leaving page
function mainexit() {
	changestate(-1);
	exitstate();
	gl_exit();
}

// call something
// window.onload = maininit;
window.onload = mainload;
window.onunload = mainexit; // maybe save some cookies
