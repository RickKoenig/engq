var state = -1;
var newstate = -1;
var dochangestate = false;
var stateinited = false;

function changestate(news) {
	//if (!news)
	//	return;
	//while (ns >= nstates)
	//	ns -= nstates;
	//while (ns < 0)
	//	ns += nstates;
	newstate = news;
	dochangestate = true;
}

function showandgo(s) {
	var cbf = state[s];
	logger_str += "^^^^^^^^^^^ " + s + " ^^^^^^^^^^^\n";
//	if (s>=0 && s<nstates) {
	//var cbf = window[s];
	if (cbf) {
		cbf();
	}
	logger_str += "vvvvvvvvvvvvv " + s + " vvvvvvvvvvvv\n";
//		return true;	
//	} else {
//		return false;
//	} else {
//		return false;
//	}
}

function loadstate() {
//	var instele = document.getElementById('instructions');
//	instele.innerHTML = escapehtml("Hiho hiho\noff to work\nwe go.");
	//var s = "text" + state;
	//var noesc = "noesc" + state;
	var inst = state["text"];
	var noesc = state["noesc"];
	if (!inst)
		inst = "Default description";
	var instele = document.getElementById('instructions');
	if (instele) {
		if (noesc)
			instele.innerHTML = inst;
		else
			instele.innerHTML = escapehtml(inst);
	}
	//showandgo("load" + state);
	showandgo("load");
	preloadtime(2500,true);
	preloadtime(5000,true);
	setloaddonefunc(initstate);
}

function initstate() {
	loadstatus = 0;
	maindebugclear();
	maindebugsetbefore(); // but debug after user
	//showandgo("init" + state);
	showandgo("init");
	maindebugsetafter(); // but debug after user
	stateinited = true;
	resetframestep();
}

function procstate() {
	if (!myform) {
		if (input.key == 'n'.charCodeAt(0) /* && !myform */)
			nextstate();
		else if (input.key == 'p'.charCodeAt(0) /* && !myform */)
			prevstate();
	}
	if (dochangestate) {
		exitstate();
		state = newstate;
		loadstate();
		//initstate();
		dochangestate = false;
	}
	if (window.sprites_reset)
		sprites_reset();
	if (state) {
	//if (state>=0 && state<nstates) {
//		if (false) {
		if (stateinited) {
//		if (isloaded()) { // assets loaded, proc state
			//load_mode(false); // for webgl
			//window["proc" + state]();
			if (state.proc)
				state.proc();
			drawelements();
		} else { // assets loading, display loading progress screen
			//load_mode(true); // for webgl
			loadingproc();
		}
	}
}

function exitstate() {
	//var durl = edrawarea.toDataURL();
	if (stateinited) {
		//showandgo("exit" + state);
		showandgo("exit");
		stateinited = false;
	}
}

function nextstate() {
	var stateidx = statelist.indexOf(state);
	if (stateidx<0)
		alert("can't next state");
	++stateidx;
	if (stateidx >= statelist.length)
		stateidx -= statelist.length;
	changestate(statelist[stateidx]);
}

function prevstate() {
	var stateidx = statelist.indexOf(state);
	if (stateidx<0)
		alert("can't prev state");
	--stateidx;
	if (stateidx < 0)
		stateidx += statelist.length;
	changestate(statelist[stateidx]);
}

function selstate(sel) {
	var stateidx = statesel.selectedIndex;
	changestate(statelist[stateidx]);
}

function reloadstate() {
	changestate(state);
}

function getstatetitles() {
	var i;
	var ret = [];
	for (i=0;i<statelist.length;++i) {
		ret.push(statelist[i].title);
	}
	return ret;
}