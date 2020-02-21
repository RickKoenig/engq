// albert's socks
// state 5
var state5 = {};
// drawer enums: 0 closed, 1 closed locked out, 2 empty, 3 sock
function resetdrawers() {
	state5.drawers = [[0,0,0],[0,0,0],[0,0,0]];
//	state5.drawers = [[0,1,2],[3,0,0],[0,0,0]];
	state5.cntr = 0;
	state5.done = false;
//	logger_str = "resetdrawers = ";
}

/* info about the drawers
// 0 no drawer open, 1 one drawer open HV, 2 two drawers open H, 3 two drawers open V, 4 3 drawers open H, 5 3 drawers open V
// returns object with 
{
	nopen; // number of open drawers
	dir; // 1 horz, 2 vert, 0 both
	nsocks; // number of socks in open drawers
}
*/
function drawersinfo() {
	var ret = {};
	ret.dir = 0;
	// if at least 3 drawers are empty or sock
	var dop = 0;
	var ns = 0;
	var i,j;
	var nh = [0,0,0];
	var nv = [0,0,0];
	for (j=0;j<3;++j) {
		for (i=0;i<3;++i) {
			var dv = state5.drawers[j][i];
			if (dv >= 2) {
				++dop;
				++nh[j];
				++nv[i];
			}
			if (dv == 3)
				++ns;
		}
	}
	for (i=0;i<3;++i) {
		if (nh[i] >= 2)
			ret.dir = 1;
		if (nv[i] >= 2)
			ret.dir = 2;
	}
	ret.nopen = dop;
	ret.nsocks = ns;
	return ret;
}

// return 2 or 3
function sockprob(p) {
	logger(" (sockprob = " + p.toFixed(4) + ")");
	return Math.random() < p ? 3 : 2;
}

// event
function clickdrawer(x,y) {
	var v = state5.drawers[y][x];
	if (v == 0) { // closed, not locked
		v = 2; // empty, add probable sock later
		state5.drawers[y][x] = v; // mark as empty for drawsinfo
		di = drawersinfo();
		switch (di.nopen) {
		case 1: // one drawer opened
			v = sockprob(1/3);
			break;
		case 2: // 2nd drawer opened
			//v = 3;
			if (di.dir == 1) { // H
				//v = 3;
				if (di.nsocks == 0) {
					v = sockprob(1/4);
				} else { // di.nsocks == 1
					v = sockprob(1/2);
				}
			} else { // di.dir == 2 // V
				//v = 3;
				if (di.nsocks == 0) {
					v = sockprob(1/2);
				} else { // di.nsocks == 1
					v = sockprob(0);
				}
			}
			break;
		case 3: // final drawer opened
			//v = 3;
			if (di.dir == 1) { // H
				//v = 3;
				if (di.nsocks == 0) {
					v = sockprob(0);
				} else if (di.nsocks == 1) {
					v = sockprob(1);
				} else { // di.nsocks == 2
					v = sockprob(0);
				}
			} else { // di.dir == 2 // V
				//v = 3;
				if (di.nsocks == 0) {
					v = sockprob(1);
				} else if (di.nsocks == 1) {
					v = sockprob(0);
				} else { // di.nsocks == 2 // can't happen
					undefined();
				}
			}
			state5.done = true;
			break;
		} 
		state5.drawers[y][x] = v; // drop in the sock
		// lock all drawers not in same row and column of pick
		var i,j;
		for (j=0;j<3;++j) {
			for (i=0;i<3;++i) {
				if (i!=x && j!=y && state5.drawers[j][i]==0)
					state5.drawers[j][i] = 1; // lock
			}
		}
	}
}
	
state5.noesc = true;
//var noesc5 = true; // disable escapehtml
state5.text = '2D: Albert\'s socks simulation, could use some better art. <br>' +
			'Another example of quantum weirdness. <br>' +
			'Rows always produce an even number of socks. <br>' +
			'Columns always produce and odd number of socks. <br>' +
			'Click <a href="http://motls.blogspot.com/2012/11/quantum-casino-less-than-zero-chance.html"> here</a> ' +
			'for a better explanation.';

state5.title = "Albert's socks";

state5.init = function() {
	//state5 = {};
	//state5.test = "this";
	resetdrawers();
	setbutsname('user');
	var pa = makeaprintarea();
	//state5.test = undefined;
	//delete state5.test;
	printareadraw(pa,"Albert's socks\nopen drawers horizontally or vertically");
};

state5.proc = function() {
	var n = 30;
	if (state5.done) { // game over show result for n ticks then start over
		++state5.cntr;
		if (state5.cntr == n) {
			resetdrawers();
		}
	}
	if (input.mclick[0] || input.mclick[1] || input.mclick[2]) { // which drawer was clicked upon
		var vx = input.mx/viewx;
		var vy = input.my/viewy;
		var divx = .3;
		var divy = .3;
		var idxx = Math.floor(vx/divx);
		var idxy = Math.floor(vy/divy);
		var idxxm = vx%divx;
		var idxym = vy%divy;
		if (idxxm > .1 && idxym > .1) {
			if (idxx >=0 && idxx <=2 && idxy >=0 && idxy <= 2) {
				clickdrawer(idxx,idxy);
			}
		}
	}
	sprite_setsize(viewx,viewy);
	sprite_draw(0,0,"take0014.jpg"); // background
	sprite_setsize(viewx/5,viewy/5);
	var i,j;
	for (j=0;j<3;++j) {
		for (i=0;i<3;++i) {
			switch(state5.drawers[j][i]) {
			case 0: // unlocked drawer
				sprite_draw(viewx*(3*i+1)/10,viewy*(3*j+1)/10,"take0015.jpg");
				break;
			case 1: // locked drawer
				sprite_setopac(.5);
				sprite_draw(viewx*(3*i+1)/10,viewy*(3*j+1)/10,"take0015.jpg");
				sprite_setopac(1);
				break;
			case 2: // empty
				break;
			case 3: // sock
				sprite_draw(viewx*(3*i+1)/10,viewy*(3*j+1)/10,"xpar.png");
				break;
			}
		}
	}
};

state5.exit = function() {
	clearbuts('user');
	//state5 = null;
};
