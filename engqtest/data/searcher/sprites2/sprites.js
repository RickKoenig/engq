// html 5 based sprite system

var NSPRITES = 1000;
var curnsprites = 0;

var badwait = 35;
var badimageframe = badwait*fpswanted; // 35 seconds

// integer log10 of v rounded down
function log10i(v) {
	var ret = -1;
	while(v>0) {
		v /= 10;
		v = Math.floor(v);
		++ret;
	}
	return ret;
}

// make a string of this string this many times
function repstr(s,t) {
	var ret = "";
	var i;
	for (i=0;i<t;++i) {
		ret += s;
	}
	return ret;
}

// put zeros in front to make number have len
function leftpad(number,len) {
	var log = log10i(number);
	if (log<0)
		log = 0;
	var rep = len - log - 1;
	if (rep<0)
		rep = 0;
	var str = repstr("0",rep);
	str += number;
	return str;
}

// for general sprites
var preloadedimages; // a map of images

function popcache(name)
{
	var img = preloadedimages[name];
//	if (!img)
//		return null;
	return img;
}

function pushcache(name,img)
{
	preloadedimages[name] = img;
}

function removecache(name)
{
	delete preloadedimages[name];
//	preloadedimages["pics/" + name] = undefined;
}

// clear out sprites and move images into an image cache
var edrawarea = null;
var ctx = null;

var spt_doscale;
var spt_wid;
var spt_hit;
var spt_rotatemode;
var spt_opac;
var spt_frm;
var spt_handx;
var spt_handy;
var spt_nfrm = 256;
var spt_u0;
var spt_v0;
var spt_u1;
var spt_v1;
var spt_douvs;
var spt_infont;

var spt_lastw;
var spt_lasth;

// build engine sprites, new, this just keeps track of images currently used by the sprites, nothing more
function sprites_init() {
	edrawarea = document.getElementById('mycanvas');
	viewx = edrawarea.width;
	viewy = edrawarea.height;
	//imagecacher = new Object();

	ctx=edrawarea.getContext("2d");
	if (!ctx)
		return;
	ctx.fillStyle="#FF0000";
/*	ctx.fillRect(0,0,150,75); */
}

function sprites_reset() {
	curnsprites = 0;
	spt_doscale = true;
	spt_wid = 1;
	spt_hit = 1;
	spt_rotatemode = false;
	spt_opac = 1;
	spt_frm = null;
	spt_handx = 0;
	spt_handy = 0;
	spt_nfrm = 256;
	spt_u0 = 0;
	spt_v0 = 0;
	spt_u1 = 1;
	spt_v1 = 1;
	spt_douvs = true;
}

function sprite_setscale(xs,ys) {
	spt_doscale = true;
	spt_wid = xs;
	spt_hit = ys;
}

function sprite_setsize(wid,hit) {
	spt_doscale = false;
	spt_wid = wid;
	spt_hit = hit;
}

function sprite_sethand(hx,hy) {
	spt_handx = hx;
	spt_handy = hy;
}

function sprite_setopac(opac) {
	spt_opac = opac;
	if (spt_opac > 1)
		spt_opac = 1;
	else if (spt_opac < 0)
		spt_opac = 0;
}

// frm of null turns off frame system
function sprite_setframe(frm) {
	spt_rotatemode = false;
	if (typeof frm == "number") {
		spt_frm = modint(frm+.5,spt_nfrm);	
	} else {
		spt_frm = null;
	}
	//spt_frm = 8;
	//logger_str += "(" + spt_frm + ")\n";
}

// frm of null turns off frame system
function sprite_setangle(ang) {
	if (typeof ang == "number") {
		spt_frm = ang;
		spt_rotatemode = true;
	} else {
		spt_frm = null;
		spt_rotatemode = false;
	}
	//spt_frm = 8;
	//logger_str += "(" + spt_frm + ")\n";
}

function sprite_setnframes(nfrm) { // change sprite angle resolution
	spt_nfrm = nfrm;	
}

function sprite_setuvs(u0,v0,u1,v1) {
	spt_u0 = u0;
	spt_v0 = v0;
	spt_u1 = u1;
	spt_v1 = v1;
	spt_douvs = true;
}

function sprite_resetuvs() {
	spt_u0 = 0;
	spt_v0 = 0;
	spt_u1 = 1;
	spt_v1 = 1;
	spt_douvs = false;
}

function img_ready(img) {
	if (!img)
		return false;
	return img.complete && img.naturalWidth>=1;
}

function sprite_draw(x,y,picname) {
	if (!ctx)
		return;
	spt_lastw = 0;
	spt_lasth = 0;
	if (curnsprites >= NSPRITES)
		return;
	if (!spt_opac && spt_opac!=0)
		spt_opac = 1;
	else if (spt_opac > 1)
		spt_opac = 1;
	else if (spt_opac < 0)
		spt_opac = 0;
	if (spt_opac < .01)
		return;
	var hasframe = false;
	// animated sprites
	if (!spt_rotatemode && (typeof spt_frm == "number") && (Math.floor(spt_frm) === spt_frm) && spt_frm >= 0) {
		hasframe = true;
		var idx = picname.lastIndexOf(".");
		var ext;
		if (idx>=0) {
			ext = picname.substr(idx);
			picname = picname.substr(0,idx);
		} else {
			ext = "";
		}
		picname += leftpad(spt_frm,4) + ext;
	}
	++curnsprites;
	var img = popcache(picname); // get old image if cached
	if (img) {
		if (!img_ready(img) && img.frame + badimageframe <= frame) {
			removecache(picname); // will cause a reload of image
			img = null;
		}
	}
	if (!img) { // get new image from server
		img = document.createElement("img");
		img.src = serverip + "pics/" + picname;
//				img.src = picname;
		// img.src = serverip + "/" + picname;
		img.alt = picname;
		img.className = "absol";
		img.frame = frame;
		img.ondragstart = doondragstart;
		pushcache(picname,img);
	}
	if (!img_ready(img))
		return;	// not good, nothing to draw
	
	// got a valid image, let's draw it	
	var w = spt_wid;
	var h = spt_hit;
	spt_lastw = spt_wid;
	spt_lasth = spt_hit;
	var dimw,dimh;
	dimw = img.naturalWidth;
	dimh = img.naturalHeight;
	if (spt_doscale) {
		w *= dimw;
		h *= dimh;
		if (spt_infont) { // 8 by 16 glyphs
			w *= .125;
			h *= .0625;
		}
	}
// take care of clip expand, it's alot like uv's	
	spt_lastw = w;
	spt_lasth = h;
	var dc = spt_douvs; // for font
	var cl = spt_u0 * dimw;
	var ct = spt_v0 * dimh;
	var cr = spt_u1 * dimw;
	var cb = spt_v1 * dimh;
//	if (spt_opac!=1) {
//	}
	ctx.globalAlpha = spt_opac;
	if (spt_rotatemode) {
		ctx.save();
		ctx.translate(x,y);
		ctx.rotate(spt_frm);
		ctx.drawImage(img,-spt_handx*w,-spt_handy*h,w,h);
		ctx.restore();
	} else {
		if (dc) { // something needs to be clipped, font or viewport
		x -= w*spt_handx;
		y -= h*spt_handy;
			ctx.drawImage(img,cl,ct,cr-cl,cb-ct,x,y,w,h);
		} else {
			ctx.drawImage(img,x,y,w,h);
		}
	}
/*	img.setAttribute("style",stylestr);
	img.onclick = imgonclick;
	edrawarea.appendChild(img); */
	//ctx.fillRect(x,y,5,5);
}

// uses 16 rows, 8 column images
function sprite_drawfont(x,y,pic,text) {
	// save previous sprite state
	var rotatemodesave = spt_rotatemode;
	var frmsave = spt_frm;
	var handxsave = spt_handx;
	var handysave = spt_handy;
	var douvssave = spt_douvs;
	var u0save = spt_u0;
	var v0save = spt_v0;
	var u1save = spt_u1;
	var v1save = spt_v1;
	
	// setup for font sprite state
	spt_rotatemode = false;
	spt_frm = false;
	spt_handx = 0;
	spt_handy = 0;
	spt_douvs = true;
	
	// draw text as font sprites
	spt_infont = true;
	var i;
	var n = text.length;
	var xs = x;
	for (i=0;i<n;++i) {
		var cc =	text.charCodeAt(i);
		if (cc >= 128)
			continue;
		if (cc == 32) { // space
			x += spt_lastw;
			continue;
		}
		if (cc == 10) { // \n
			x = xs;
			y += spt_lasth;
			continue;
		}
		var r = cc>>3;
		var c = cc&7;
		var fx = .025; // fudge in, helps prevent wrap around effects
		var fy = .025;
		sprite_setuvs(c/8.0,r/16.0,(c+1)/8.0-fx/8.0,(r+1)/16.0-fy/16.0);
//		sprite_setuvs(c/8.0+fx/8.0,r/16.0+fy/16.0,(c+1)/8.0-fx/8.0,(r+1)/16.0-fy/16.0);
		sprite_draw(x,y,pic);
		x += spt_lastw;
		// y += spt_lasth;
	}
	spt_infont = false;
	
	// restore previous sprite state
	spt_rotatemode = rotatemodesave;
	spt_frm = frmsave;
	spt_handx = handxsave;
	spt_handy = handysave;
	spt_douvs = douvssave;
	spt_u0 = u0save;
	spt_v0 = v0save;
	spt_u1 = u1save;
	spt_v1 = v1save;
}

// event
function imgonclick(e) {
	inputevents += "(Iclick " + getxcode(e) + "," + getycode(e) + ") ";
	//++mclick;
}

// event
function doondragstart(e) {
	inputevents += "(Mdrag " + getxcode(e) + "," + getycode(e) + ") ";
	return false;
}

