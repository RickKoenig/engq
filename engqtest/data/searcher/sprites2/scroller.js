var scrollx = 87;
var scrolly = -60;
var scrollvelx = 0;
var scrollvely = 0;

var dirs = {
	"UL":{"x":-1,"y":-1},
	"U":{"x":0,"y":-1},
	"UR":{"x":1,"y":-1},
	"L":{"x":-1,"y":0},
	"C":{"x":0,"y":0},
	"R":{"x":1,"y":0},
	"DL":{"x":-1,"y":1},
	"D":{"x":0,"y":1},
	"DR":{"x":1,"y":1},
};

function scrollerinit() {
	// escroll = document.getElementById('scroll');
	setbutsname('scroller');
	makeahr();
	makeabut('UL',null,scrollerrep,scrollerup);
	makeabut('U',null,scrollerrep,scrollerup);
	makeabut('UR',null,scrollerrep,scrollerup);
	makeabr();
	makeabut('L',null,scrollerrep,scrollerup);
	makeabut('C',scrollerclickc,scrollerrep,scrollerup);
	makeabut('R',null,scrollerrep,scrollerup);
	makeabr();
	makeabut('DL',null,scrollerrep,scrollerup);
	makeabut('D',null,scrollerrep,scrollerup);
	makeabut('DR',null,scrollerrep,scrollerup);
	escroll = makeaprintarea();
}

function scrollerrep(e) {
	if (e.value == "C") {
		logger_str += "(Center)\n";
		scrollx = 0;
		scrolly = 0;
	}
	var dir = dirs[e.value];
	if (dir==null || dir== undefined) {
		return;
	}
	scrollvelx = dir.x;
	scrollvely = dir.y;	
	scrollx += scrollvelx;
	scrolly += scrollvely;
	//logger_str += "(srep)\n"
}

// clear out the logger
function scrollerclickc(e) {
	inputevents = "inputevents = ";	
	logger_str = "logger = ";	
}

function scrollerup(e) {
	//logger_str += "(sup)\n"
	scrollvelx = 0;
	scrollvely = 0;
}
