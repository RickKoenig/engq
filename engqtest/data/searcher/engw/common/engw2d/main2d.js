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
//var invfpswanted;
//var geominvfpswanted;
//var intervaltime = 1000/fpswanted; // 8 fps for now

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
		printareadraw(eoutstateman,"State " + statelist.indexOf(state));
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
				ks += i.toString(16) + " ";
		}
		//eoutkeystate.firstChild.nodeValue = ks;
		printareadraw(eoutkeystate,ks);
		
		// show input
		//eoutinputstate.firstChild.nodeValue = "inputstate = mx " + input.mx + " my " + input.my + " mbut " + input.mbut + " mclick " + input.mclick + " key " + input.key;
		printareadraw(eoutinputstate,"inputstate = mx " + input.mx + " my " + input.my + 
			" mbut " + input.mbut + " mwheel " + input.wheelPos + 
			" mclick0 " + input.mclick[0] + " mclick1 " + input.mclick[1] + " mclick2 " + input.mclick[2] + 
			" key " + input.key.toString(16));
		
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
		if (logmode)
			printareadraw(eoutlogger,logger_str);
		else {
			printareadraw(eoutlogger,"logger disabled");
			logger_str = "";
		}
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

var statesel = null;
var logmode = 0;
var logstrs = ["Log Disabled","Log Enabled"];
function maindebugsetbefore() {
	if (showstate) {
		// eoutstateman = document.getElementById('stateman');
		setbutsname('stateman');
		eoutstateman = makeaprintarea();
		makeabut('Prev State',prevstate);
		var statetitles = getstatetitles();
		statesel = makeaselect('State Select',statetitles,selstate);
		selectsetidx(statesel,statelist.indexOf(state));
		makeabut('Reload State',reloadstate);
		makeabut('Next State',nextstate);
		makeabut(logstrs[logmode],clearlog,null,null,true); // wide margins
		//makeahr();
	}
}

function clearlog() {
	inputevents = "inputevents 'clear' = ";	
	logger_str = "logger 'clear' = ";	
	logmode = 1 - logmode;
	this.value = logstrs[logmode];
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

var loadstatus = 0;
function mainload() {
	//preloadShaders();
	//preloadshaderlist("shaders/shaderlist.txt");
	//preloadtext("shaders/basic.ps");
	//preloadtext("shaders/basic.vs");
	//preloadimg("pics/panel.jpg");
	//preloadimg("/engw/common/sptpics/maptestnck.png"); // preload the default texture
	preloadimg("../common/sptpics/maptestnck.png"); // preload the default texture
//	preloadimg("pics/maptestnck.png"); // preload the default texture
	preloadimg("../common/sptpics/font3.png"); // preload the default texture
	preloadimg("../common/sptpics/take0007.jpg"); // preload the default texture
	preloadimg("../common/sptpics/coin_logo.png"); // preload the default texture
	//preloadimg("../tracks2/peelingwood.jpg");
	setloaddonefunc(maininit);
	document.oncontextmenu = function() {return false;};
}

function loadingproc() {
	sprite_setsize(viewx,viewy);
	sprite_draw(0,0,"take0007.jpg");
	sprite_setsize(15,15);
	sprite_drawfont(0,0    ,"font3.png","LOADING\nframe = " + frame + ", reqcnt = " + reqcnt + ", loadcnt " + loadcnt + 
		"\n nwreqcnt = " + nwreqcnt + ", nwloadcnt = " + nwloadcnt + "\n loadstatus " + loadstatus++);
//	sprite_drawfont(0,0    ,"fontbiggreen.png","frame = " + frame + ", reqcnt = " + reqcnt + ", loadcnt " + loadcnt);
//	sprite_drawfont(0,0    ,"font3.png","Hello World!\nABC\nDEF\nGHI");
}

function maininit() {
	loadstatus = 0;
	sprites_init();
	initinput();
	// get some nodes from html
	//maindebug();
	//initstate();
	changestate(startstate);
	//mainproc(); // do 1 proc right away
	//window.setInterval(mainproc,intervaltime);
	setframerate(mainproc,fpswanted);
}

function mainproc() {
	setframerate(mainproc,fpswanted);
	inputproc();
	//if (input.keystate[keycodes.down]) {
	//	clearlog();
	//}
	if (input.key)
		;//logger_str += "KB " + input.key + " ";
// proc
	//sprites_reset();
	//invfpswanted = 1.0/fpswanted;
	procstate();
// draw
	++frame;
}

// leaving page
function mainexit() {
	setframerate(null,0);
	//changestate(-1);
	exitstate();
}

// call something
// window.onload = maininit;
window.onload = mainload; // Start this whole thing off!
window.onunload = mainexit; // maybe save some cookies
