// handle buttons and some printarea thing
var mbutcur = [0,0,0];
var mbuthold = [0,0,0];
var lastrepbut = null;
var repdelay = 0;

var myform;

var curname;

var nchilds = 0;

// event
function buthandleclick_this() {
	buthandleclick(this);
}
function buthandleclick(but) {
	if (but.onclicknotthis) {
		but.onclicknotthis(but);
		return;
	}
}

// event
function buthandledown_this() {
	buthandledown(this);
}
function buthandledown(but) {
	repdelay = 0;
	lastrepbut = but;
	buthandledownrep(but);
}
function buthandledownrep(but) {
	if (but.repfunc) {
		but.repfunc(but);
		return;
	}
}

// event
function buthandleupout_this() {
	buthandleupout(this);
}
function buthandleupout(but) {
	lastrepbut = null;
	if (but.upfunc) {
		but.upfunc(but);
		return;
	}
}

// event
function buthandleovermove() {
	var but = this;
}

function makeabut(name,clickfunc,repfunc,upfunc,widemargins) {
	if (!myform)
		return;
	var bn = document.createElement('input');
	bn.type = 'button';
	bn.name = curname;
	bn.value = name;	
	if (widemargins)
		bn.setAttribute('class','centerbwm');
	else
		bn.setAttribute('class','centerb');
	bn.onclick = buthandleclick_this;
	bn.onclicknotthis = clickfunc;
	bn.onmousedown = buthandledown_this;
	bn.onmouseup = buthandleupout_this;
	bn.onmouseout = buthandleupout_this;
	bn.repfunc = repfunc;
	bn.upfunc = upfunc;
	myform.appendChild(bn);
	++nchilds;
}

function upfunc_this () {
	if (this.onkeyuptext) {
		this.onkeyuptext(this);
	}
}

function makeatext(name,text,upfunctext) {
	if (!myform)
		return;
	var pa = makeaprintarea();
	printareadraw(pa,name);
	var bn = document.createElement('input');
	bn.type = 'text';
	bn.name = curname;
	bn.value = text;	
	bn.setAttribute('class','centerb');
	bn.onkeyup = upfunc_this;
	bn.onkeyuptext = upfunctext;
	myform.appendChild(bn);
	++nchilds;
}

function makeabr() {
	if (!myform)
		return;
	var bn = document.createElement('br');
	bn.name = curname;
	myform.appendChild(bn);
	++nchilds;
}

function makeahr() {
	if (!myform)
		return;
	if (nchilds == 0)
		return;
	var bn= document.createElement('hr');
	bn.name = curname;
	myform.appendChild(bn);
	++nchilds;
}

function makeaprintarea(val) {
	if (!myform)
		return;
	makeahr();
	var bn = document.createElement('p');
	bn.name = curname;
//	bn2= document.createElement('code');
	if (val)
//		bn2.innerHTML = val;
		bn.innerHTML = val;
//	bn.appendChild(bn2);
	myform.appendChild(bn);
	++nchilds;
	return bn;
}

function printareadraw(node,text) {
	if (!myform)
		return;
	node.innerHTML = escapehtml(text);
/*	var n = node.childNodes.length;
	for (i=n-1;i>=0;--i) { // go backwards and hope for the best
		var e = node.childNodes[i];
		node.removeChild(e);
	}
	var bn1;
	while(true) {
		var nli = text.indexOf("\n");
		if (nli >= 0) {
			var lefttext = text.substring(0,nli);
			text = text.substring(nli+1);
			bn1 = document.createElement('p');
			bn1.innerHTML = lefttext;
			node.appendChild(bn1);
		} else {
			break;
		}
	}
	bn1 = document.createElement('p');
	bn1.innerHTML = text;
	node.appendChild(bn1);
//	var bn2 = document.createElement('p');
//	bn2.innerHTML = text;
//	node.appendChild(bn2);
	
//	node.firstChild.innerHTML = text;
//	node.nodeValue = text;
//	node.innerText = text;
//	node.nodeValue = "text";
//	eoutstateman.firstChild.nodeValue = "State " + state; */
}

function setbutsname(nam) {
	curname = nam;
}

function clearbuts(nam) {
	if (!myform)
		return;
	var n = myform.childNodes.length;
	var cnt = 0;
	for (i=n-1;i>=0;--i) { // go backwards and hope for the best
		var e = myform.childNodes[i];
		if (e.name == nam) {
			myform.removeChild(e);
			++cnt;
			--nchilds;
		}
	}
	// logger_str += "(rem " + cnt + " buts)\n";
	// logger_str += "(nchilds " + nchilds +  ")\n";
}

function butinit() {
//	var i;
//	var n;
	// try to add new buttons
	myform = document.getElementById('myform');
	// add event handlers to all buttons
/*	if (!myform)
		return;
	n = myform.childNodes.length;
	for (i=0;i<n;++i) {
		var e = myform.childNodes[i];
		if (e.type == 'button') {
		//	++bts;
//		if (e.type == 'button' && e.name != 'user') {
			e.onclick = buthandleclick_this;
			e.onmousedown = buthandledown_this;
			e.onmouseup = buthandleupout_this;
			e.onmouseout = buthandleupout_this;
			//inputevents += e.value + ' ';
		}
	}
	//logger_str += "(added " + bts + " buts)\n"; */
}

function butproc() {
	// button repeat
	if (repdelay > 5) {
		if (lastrepbut)
			buthandledownrep(lastrepbut);
	}
	++repdelay;
}
