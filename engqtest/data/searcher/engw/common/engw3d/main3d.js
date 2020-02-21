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
		printareadraw(eoutinputstate,"inputstate = mx " + input.mx + " my " + input.my + " mbut " + input.mbut + " mwheel " + 
		input.wheelPos + " mclick0 " + input.mclick[0] + " mclick1 " + input.mclick[1] + " mclick2 " + 
		input.mclick[2] + " key " + input.key.toString(16) + " keybufflen " + input.keybuff.length);
		
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
	if (showstate && myform) {
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

function mainload() {
	//preloadShaders();
	preloadshaderlist("shaders/shaderlist.txt");
	//preloadtext("shaders/basic.ps");
	//preloadtext("shaders/basic.vs");
	//preloadimg("pics/panel.jpg");
	//preloadimg("/engw/common/sptpics/maptestnck.png"); // preload the default texture
	preloadimg("../common/sptpics/maptestnck.png"); // preload the default texture
//	preloadimg("pics/maptestnck.png"); // preload the default texture
	preloadimg("../common/sptpics/font0.png"); // preload the default texture
	preloadimg("../common/sptpics/font3.png"); // for debprint, hmm..
	preloadimg("../common/sptpics/take0007.jpg"); // preload the default texture
	preloadimg("../common/sptpics/coin_logo.png"); // preload the default texture
	//preloadimg("../tracks2/peelingwood.jpg");
	setloaddonefunc(maininit);
	document.oncontextmenu = function() {return false;};
}

var loadingvp = null;
var loadingmodelfont = null;
var loadingtreefont = null;
var loadstatus = 0;

function loadingproc() {
	if (!loadingvp) {
		// get size of texture
		var tex = Texture.createtexture("font0.png");
		var glyw = tex.width/8; // 16
		var glyh = tex.height/16; // 32
		tex.glfree();
		var debprint_depth = gl.viewportHeight/2;
		//debprint_depth *= 2; // half pixel still looks good
		loadingvp = {
	    	// where to draw
			target:null,
			// clear
			clearflags:gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT,
		//	clearcolor:[0,1,.75,1],
			clearcolor:[0,0,0,1],
		//	mat4.create();
			// orientation
			"trans":[0,0,-debprint_depth],
			"rot":[0,0,0],
		//	"scale":[1,1,1],
			// frustum
			near:.002,
			far:10000.0,
			zoom:1,
			asp:gl.asp,
			//isortho:true,
			ortho_size:debprint_depth*2,
			// optional target (overrides rot)
			inlookat:false,
			//lookat:null,
			incamattach:false
			//camattach:null
		};
	// build model f0, test font model0
	    loadingmodelfont = new ModelFont("amodf0","font0.png","tex",glyw,glyh,30,20);
		loadingmodelfont.print("Loading...");
		loadingtreefont = new Tree2("amod0");
		loadingtreefont.trans = [-debprint_depth,debprint_depth,0];
		loadingtreefont.setmodel(loadingmodelfont);
	}
//	logger_str += "loadingproc()\n";
	//gl.clearColor(Math.random(),Math.random(),Math.random(),1);
    //gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
    var percent = (loadcnt*100/reqcnt).toFixed(2);
    var loadingstr = 
	"Loading " + percent + "%\n\n" +
	"frame = " + frame + "\nreqcnt = " + reqcnt + "\nloadcnt = " + loadcnt + 
		"\nnwreqcnt = " + nwreqcnt + "\nnwloadcnt = " + nwloadcnt + "\nloadstatus = " + loadstatus++ + "\n";
	var i,j;
	var testalign = String.fromCharCode(127,127);
	for (j=0;j<10;++j) {
		for (i=0;i<5;++i) {
			loadingstr += testalign;
		}
		loadingstr += "\n";
	}
	loadingmodelfont.print(loadingstr);
	beginscene(loadingvp);
	loadingtreefont.draw();
}

var defaultimage;

function maininit() {
	loadstatus = 0;
	defaultimage = preloadedimages["maptestnck.png"];
	//sprites_init();
	initinput();
	gl_init();
	// get some nodes from html
	//maindebug();
	//initstate();
	if (gl)
		changestate(startstate);
	//mainproc(); // do 1 proc right away
	//intervalid = window.setInterval(mainproc,intervaltime);
	resetframestep();
	setframerate(mainproc,fpswanted);
	debprint.init();
}

function mainproc() {
	setframerate(mainproc,fpswanted);
	inputproc();
	//if (input.keystate[keycodes.DOWN]) {
	//	clearlog();
	//}
	if (input.key)
		;//logger_str += "KB " + input.key + " ";
// proc
	//sprites_reset();
	//invfpswanted = 1.0/fpswanted;
	debprint.proc();
	procstate();
	debprint.draw();
// draw
	measureproctime();
	++frame;
}

// leaving page
function mainexit() {
	setframerate(null,0);
	//window.clearInterval(intervalid);
	//intervalid = null;
	//changestate(-1);
	debprint.exit();
	exitstate();
	gl_exit();
}

// call something
// window.onload = maininit;
window.onload = mainload;
window.onunload = mainexit; // maybe save some cookies
