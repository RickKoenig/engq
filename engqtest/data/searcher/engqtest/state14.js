var text14 = "WebGL: This state lets you swing around a pendulum with the mouse.";

var dampval = 0;
var dampstep = 1;//1.0/256.0;
var damparea;
var camz = 5;

//var pendpce0 = null;
var pendroot = null;

var pendpos;
var oldpendpos;
var pendvel;
var oldpendvel;
var pendaccel;
var pendrot;
var pendrotvel;

var dampval;
var damparr = [0,.00001,.00005,.0001,.001,.002,.005,.01,.05,.1,.2,.3,.4,.5];

var pendgrav = -.02;
var pendradius = 4;

function updatedamp() {
	printareadraw(damparea,"Damping : " + damparr[dampval].toFixed(5) + ", Rot : " + pendrot.toFixed(5) + ", Rotvel : " + pendrotvel.toFixed(5));
}
	
function lessdamp() {
	dampval -= dampstep;
	if (dampval < 0)
		dampval = 0;
	//updatedamp();
}

function moredamp() {
	dampval += dampstep;
	if (dampval >= damparr.length)
		dampval = damparr.length - 1;
	//updatedamp();
}

function resetdamp() {
	dampval = 1;
	//updatedamp();
}

function pendfric(v) {
	return (1-damparr[dampval])*v;
}
	
function load14() {
	if (!gl)
		return;
	preloadimg("pics/maptestnck.png");
	preloadimg("pics/panel.jpg");
}

function init14() {
	gl_mode(true);
	if (!gl)
		return;
	logger("entering webgl state14\n");
	
	pendpos = [0,0,0];
	oldpendpos = [0,0,0];
	pendvel = [0,0,0];
	oldpendvel = [0,0,0];
	pendaccel = [0,0,0];
	//pendrot = 0;//3*Math.PI/2;
	pendrot = 3*Math.PI/2+Math.PI/16; // a little off center
	pendrotvel = 0;//.04;
	
	
	// build the scene
	roottree = new Tree2("root");
	
	// ui
	setbutsname('pendu1');
	// less,more,reset for pendu1
	damparea = makeaprintarea('damp: ');
	makeabut("less damp",null,lessdamp);
	makeabut("more damp",null,moredamp);
	makeabut("reset damp",null,resetdamp,null,true);
	resetdamp();
	
	pendroot = new Tree2("pendroot");
	pendroot.trans = [0,0,0];
	pendroot.rot = [0,0,0];
	roottree.linkchild(pendroot);
	
	// part that you move
	var pendpce0 = buildsphere("pend1pce0",.2,"panel.jpg","diffusespecp");
	pendpce0.trans = [0,0,0];
	//pendpce0.rotvel = [.1,.5,0];
	//pendpce0.flags |= treeflagenums.ALWAYSFACING;
	pendroot.linkchild(pendpce0);
	
	// rod 
	var pendpce1 = buildcylinderxz("pend1pce1",.1,4,"panel.jpg","diffusespecp");
	pendpce1.trans = [0,0,0];
	//pendpce0.rotvel = [.1,.5,0];
	//pendpce0.flags |= treeflagenums.ALWAYSFACING;
	pendroot.linkchild(pendpce1);

	// bob 
	//var pendpce2 = buildcylinderxz("pend1pce2",.4,.2,"panel.jpg","diffusespecp");
	var pendpce2 = buildsphere3("pend1pce2",[.4,.1,.4],"panel.jpg","diffusespecp");
	pendpce2.trans = [0,4,0];
	//pendpce2.trans = [0,4,-.1];
	pendpce2.rot = [Math.PI/2,0,0];
	//pendpce0.rotvel = [.1,.5,0];
	//pendpce0.flags |= treeflagenums.ALWAYSFACING;
	pendroot.linkchild(pendpce2);

	// set the lights
	lights.wlightdir = vec3.fromValues(0,0,1);
	
	// set the camera
	//mainvp.trans = [0,0,-15]; // flycam
	mainvp.trans = [0,0,-camz]; // flycam
	mainvp.rot = [0,0,0]; // flycam

}

function proc14() {
	if (!gl)
		return;
    gl.clearColor(0,.25,0,1);                      // Set clear color to yellow, fully opaque
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

	
	if (input.mbut[0]) { // convert mouse to 3D space given a zoom factor of 1 and camera at 0,0,-camz
		var b = camz;
		var m = 2*camz/gl.viewportHeight;
		pendpos = [
			 m*input.mx - b*gl.viewportWidth/gl.viewportHeight,
			-m*input.my + b,
			0
		];
	}
///////// start physics /////////
	vec3.sub(pendvel,pendpos,oldpendpos);
	vec3.sub(pendaccel,pendvel,oldpendvel);
	pendaccel[1] -= pendgrav;
	vec3.copy(oldpendpos,pendpos);
	vec3.copy(oldpendvel,pendvel);
	
	// do cross product, get torque
	var torque = pendaccel[0]*Math.sin(pendrot) - pendaccel[1]*Math.cos(pendrot);

	var pendrotaccel = torque/pendradius;
	pendrotvel += pendrotaccel;
	pendrotvel = pendfric(pendrotvel);
	pendrot += pendrotvel;
/*	if (angd<0)
		angd+=2*PI;
	if (angd>2*PI)
		angd-=2*PI; */
///////// end physics /////////

	//pendrot += pendrotvel;
	pendrot = normang(pendrot);
	vec3.copy(pendroot.trans,pendpos);
	pendroot.rot = [0,0,pendrot-Math.PI/2];

	doflycam(mainvp); // modify the mvMatrix
	
	//pendpce0.trans = [0,0,0];
	roottree.proc();
	dolights(); // get some lights to eye space
	updatedamp();
	roottree.draw();
}

function exit14() {
	gl_mode(false);
	if (!gl)
		return;
	roottree.log();
	logrc();
	logger("after roottree glfree\n");
	roottree.glfree();
	logrc();
	roottree = null;
	logger("exiting webgl state14\n");
	clearbuts('pendu1');
}
/*
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <graph32/graph32.h>

#define NUMFRAMES 256
#define RADIUS 82	// measured from graphic
#define GRAV .1 //
#define PI 3.14159

double ftab[8]={.00001,.00005,.0001,.001,.002,.005,.01,0};
int fricvar=0;

double fric(double v)
{
return (1-ftab[fricvar])*v;
}

int ftointrnd(double f)
{
if (f>0)
	return f+.5;
return f-.5;
}

void main()
{
char str[80];
int key;
int mx,my,but;
int x,y,oldx=XSIZE/2,oldy=YSIZE/2;
int vx,vy,oldvx=0,oldvy=0;

double ax,ay;
double angv=0,angd=PI,anga;

double torque;

struct folder *f;
struct sprite s;
struct bitmap b,v;

mem_init();

alloc_bitmap(&b,XSIZE,YSIZE,-1);
make_video_bitmap(&v);
if (isolder("pend0001.lbm","pendu.spt")>0)
	f=foldersptread("pendu",0);
else
	{
	f=folderiffread("pend.lbm",0,0,255,-1,-1);
	foldersptwrite(f,"pendu");
	}
sptlink(&s,f,0); // hook up fld
sptmove(&s,160,100);
initgraph();
initmouse();	// mouseset at 160 100

while(1)
	{
	key=getkey();
	but=getmousexy(&mx,&my);
	if (but || key=='q' || key==K_ESC)
		break;
	if (key=='f')
		{
		fricvar++;
		fricvar&=7;
		}
	cliprect(&b,0,0,XSIZE-1,YSIZE-1,black);
///////// start physics /////////
	x=mx;
	y=my;
	vx=x-oldx;
	vy=y-oldy;
	ax=vx-oldvx;
	ay=vy-oldvy-GRAV;
	oldx=x;
	oldy=y;
	oldvx=vx;
	oldvy=vy;
	
	torque=ax*cos(angd)+ay*sin(angd);

	anga=torque/RADIUS;
	angv+=anga;
	angv=fric(angv);
	angd+=angv;
	if (angd<0)
		angd+=2*PI;
	if (angd>2*PI)
		angd-=2*PI;
///////// end physics /////////
	sptmove(&s,x,y);
	sptframe(&s,NUMFRAMES);
	sptshow(&s,&b);

//	sptframe(&s,NUMFRAMES+1);
//	sptmove(&s,10,10);
//	sptshow(&s,&b);

	sptmove(&s,x,y);
	sptframe(&s,ftointrnd(angd*NUMFRAMES/(2*PI))&(NUMFRAMES-1));
	sptshow(&s,&b);

	sprintf(str,"ax=%6.3f,ay=%6.3f,vx=%3d,vy=%3d",ax,ay,vx,vy);
	outtextxy(&b,0,0,str,white);
	sprintf(str,"angd=%6.3f,angv=%6.3f,anga=%8.5f",angd,angv,anga);
	outtextxy(&b,0,8,str,white);
	sprintf(str,"fric=%8.5f",ftab[fricvar]);
	outtextxy(&b,0,16,str,white);
	
	
	waitvb();
	clipblit(&b,&v,0,0,0,0,XSIZE,YSIZE);
	}
closegraph();
folderfree(f);				// free folder 
free_bitmap(&b);
}
*/