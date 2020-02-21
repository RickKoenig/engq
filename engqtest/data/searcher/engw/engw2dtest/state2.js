// rotating sprite demo
var ang;

var state2 = {};

state2.text = "2D: Demo of sprite package with\n" + 
			"font, opacity, animation, scaling, handle.";
state2.title = "advanced sprite demo";

state2.init = function() {
	ang = 0;//Math.PI/4;
};

state2.proc = function() {
	// background
	var angf = Math.PI * frame / 400 + Math.PI*3/4;
	sprite_setsize(viewx,viewy);
	sprite_draw(0,0,"take0007.jpg");
//	sprite_setsize(10,10);
	// font
	var x = 250+Math.cos(angf)*300;
	var y = 270+Math.sin(angf)*320;
	//sprite_setscale(2,2);
	sprite_setsize(10,10);
	//x = 100;
	//y = 100;
	sprite_drawfont(x,y    ,"fontbiggreen.png","Hello World!\nABC\nDEF\nGHI");
	sprite_drawfont(x,y+200,"smallfont.png"   ,"Hello World!\nABC\nDEF\nGHI");
 
	//sprite_setscale(1,1);
	sprite_setscale(.5,.5);
	//sprite_setsize(390,98);

	// rotating sprite
//	sprite_sethand(.5,.5);
//	sprite_sethand(1,1);
	sprite_sethand(.75,.25);
	//sprite_setrotatemode(true);
	sprite_setangle(ang);
	sprite_draw(viewx/2,viewy/2,"brush1.png");
	
	sprite_sethand(.5,.5);
	//sprite_setrotatemode(true);
	sprite_setangle(ang);
	var i;
	var pics = ["plank1.png","plank2.png","plank3.png","plank4.png","plank5.png","plank6.png"];
	var npics = pics.length;
	sprite_setopac(Math.cos(angf*5)*.5+.5);
	for (i=0;i<npics;++i) {
		sprite_draw(75*i+100,3*viewy/4+10*i,pics[i]);
	}
	var pics2 = ["ball1.png","ball2.png","ball3.png","ball4.png","ball5.png"];
	var npics2 = pics2.length;
	for (i=0;i<npics2;++i) {
		var s = (i+1)*.5;
		sprite_draw(75*i+100,1*viewy/4+10*i,pics2[i]);
	}	
	sprite_setopac(1);

	// simple sprite
	//sprite_setrotatemode(false);
	sprite_setframe(null);
	sprite_sethand(.75,.25);
	sprite_setscale(.5,.5);
	sprite_draw(viewx/2,viewy/2,"brush1.png");

	// show center of viewport
	sprite_setsize(3,3);
	sprite_draw(viewx/2,viewy/2,"take0001.jpg");
	//logger_str += "proc2\n";
	ang -= 2*Math.PI/64;
	if (ang < 0)
		ang += 2*Math.PI;
};

// exit function, dummy implied
