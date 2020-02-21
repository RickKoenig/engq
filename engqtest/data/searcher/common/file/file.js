// pass in a arrayBuffer
function fopen(ab)
{
	var fh = {};
	fh.ab = ab;
	fh.offset = 0;
	return fh;
}

function freadI32(fh)
{
	if (fh.offset + 4 > fh.ab.byteLength)
		return null;
	var i32a = new Int32Array(fh.ab,fh.offset,1);
	fh.offset += 4;
	return i32a[0];
}

function freadF32(fh)
{
	if (fh.offset + 4 > fh.ab.byteLength)
		return null;
	var f32a = new Float32Array(fh.ab,fh.offset,1);
	fh.offset += 4;
	return f32a[0];
}

function freadI8v(fh,nele)
{
	if (fh.offset + nele > fh.ab.byteLength)
		return null;
	var i8a = new Int8Array(fh.ab,fh.offset,nele);
	fh.offset += nele;
	return i8a;
}

function freadI32v(fh,nele)
{
	var nb = nele*4;
	if (fh.offset + nb > fh.ab.byteLength)
		return null;
	var i32a = new Int32Array(fh.ab,fh.offset,nele);
	fh.offset += nb;
	return i32a;
}

function fskip(fh,nbytes)
{
	fh.offset += nbytes;
}

function fclose(fh)
{
	fh.ab = null;
}

var logger_str = "";

function logger(str) {
	//var estr = escapehtml(str);
	logger_str += str;
}

function spliturl(url) {
	var s = {};
	s.path = "";
	s.name = "";
	s.ext = "";
	var pidx = url.lastIndexOf("/");
	if (pidx>=0) {
		s.path = url.substr(0,pidx);
		url = url.substr(pidx+1);
	}
	var eidx = url.lastIndexOf(".");
	if (eidx >= 0) {
		s.name = url.substr(0,eidx);
		s.ext = url.substr(eidx+1);
	} else {
		s.name = url;
	}
	return s;
}

/*function getnamefromurl(url) {
	var idx = url.lastIndexOf(".");
	var name = url.substr(0,idx);
	return name;
}

function getextfromurl(url) {
	var idx = url.lastIndexOf(".");
	var ext = url.substr(idx+1);
	return ext;
}*/

function geturlfrompathnameext(path,name,ext) {
	var ret = name;
	if (path)
		ret = path + "/" + ret;
	if (ext)
		ret += "." + ext;
	return ret;
}
