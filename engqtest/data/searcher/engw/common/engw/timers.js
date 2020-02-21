var intervalid = null;
var curfps = 0;
var frametimewanted = 0; // from fpswanted
var frametimeactual = 0; // from start of 1 frame to start of next frame
var frametimeavg = 0;
var fpsavg = 0;
var frametime = 0; // used for time for the frame, animation etc.
var proctimeactual = 0;
var proctimeavg = 0;
var mslastnowtime = 0; // internal for deltas
var autofps = false; // adjust framerate depending on performance
var autofpswanted = null;
var rfs = false;
var autofrmcnt = 0;
var nautofrmcnt = 100;
var autoframeratio = 1.001;

function resetframestep() {
	rfs = true;
}
var avgsize = 200;
var avgframeclass = new Runavg(avgsize);
var avgprocclass = new Runavg(avgsize);
var testrunavg = false;
function dotestrunavg() {
	var arunavg = new Runavg(5);
	var res;
	res = arunavg.add(7);
	res = arunavg.add(4);
	res = arunavg.add(9);
	res = arunavg.add(6);
	res = arunavg.add(3);
	res = arunavg.add(2);
	res = arunavg.add(1);
	res = arunavg.add(1);
	res = arunavg.add(5);
	res = arunavg.add(5);
	res = arunavg.add(5);
	res = arunavg.add(5);
}

// called at start of proc
function setframerate(fun,fps) {
	if (fps == 0)
		fps = 1000;
	if (testrunavg) {
		dotestrunavg();
		testrunavg = false;
	}
	if (autofps) {
		if (!autofpswanted) {
			autofpswanted = fps;
		} else {
			fps = autofpswanted;
		}
	}
	if (fps > 1000)
		fps = 1000;
	if (fps > 0)
		frametimewanted = 1.0/fps;
	else
		frametimewanted = 0.0;
	var now;
	if (window.performance && window.performance.now) {
		var msnowtime = performance.now();
		if (rfs) {
			mslastnowtime = msnowtime;
			//frametime = 0;
			rfs = false;
		}
		frametimeactual = .001*(msnowtime-mslastnowtime);
		mslastnowtime = msnowtime;
	} else {
		frametimeactual = frametimewanted;
	}
	frametimeavg = avgframeclass.add(frametimeactual);
	fpsavg = 1.0/frametimeavg;
//	frametime = frametimewanted;
	frametime = frametimeactual;
	if (autofps) {
		if (autofrmcnt == 0) {
			if (frametimewanted * autoframeratio > frametimeavg) {
				++autofpswanted;
			} else {
				--autofpswanted;
			}
			autofrmcnt = nautofrmcnt - 1;
		} else {
			--autofrmcnt;
		}
		fps = autofpswanted;
	}
	if (fps != curfps) {
		if (intervalid) {
			window.clearInterval(intervalid);
			intervalid = null;
		}
		if (fps)
			intervalid = window.setInterval(fun,Math.floor(1000*frametimewanted));
		curfps = fps;
	}
}

// called at end of proc
function measureproctime() {
	if (window.performance && window.performance.now) {
		var msendproctime = performance.now();
		proctimeactual = .001*(msendproctime - mslastnowtime);
		proctimeavg = avgprocclass.add(proctimeactual);
	}
}
