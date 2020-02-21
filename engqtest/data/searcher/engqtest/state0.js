// sprite demo

// init/build drawarea after a page load

// text keyup events, (stub of ajax)
function goAjax(e) {
	logger("goajax = " + e.value + ")\n");	
}

var text0 = "2D: A simple sprite demo using HTML5 canvas.";
var pa;
// state 0
function init0() {
	setbutsname('user');
	pa = makeaprintarea();
	// test text
	printareadraw(pa,"hey ho!");
	// test text input
	makeatext("Your Name","Goes here",goAjax);
	
}

function proc0() {
	// move stuff
	//curnsprites = 20;
	var cnt = 20;
	//Math.floor((Math.random()*10)+1); 
	//printareadraw(pa,"wheel = " + input.wheelPos);
	sprite_setsize(viewx,viewy);
	sprite_setframe(divint(frame,10)%20);
	sprite_draw(0,0,"take.jpg");
	for (i=0;i<cnt;++i) {
		//var spt = sprites[i];
		var x = 30*i-10;
		var y = -50 + .5*viewy*(1 + Math.sin(frame*.1 + i*.1));
		var w = 80;
		var h = 60;
		// var off = divint(frame,100)+i;
		var off = divint(i*frame,100);
		//var picname = gettake(off);
		sprite_setsize(w,h);
		sprite_setframe(off%20);
		//sprite_draw(x,y,w,h,"take.jpg",1,off%20);
		sprite_draw(x,y,"take.jpg");
	}
/*	for (i=0;i<32;++i) {
		//var spt = sprites[i];
		var x = 20*i;
		var y = 100 + Math.cos(frame*.1 + i*.1)*120;
		var w = 80;
		var h = 60;
		//var picname = gettake(i);
		sprite_draw(x,y,w,h,"take.png",1,i,1); // with rot
	}   */
	// sprite_draw(400,300,200,200,"takerot.png",frame%32);
	// sprite_draw(0,0,200,200,"take.png",Math.sin(Math.PI*2*frame/100)*.5+.5,frame%32,1);
}

function exit0() {
	clearbuts('user');
}
