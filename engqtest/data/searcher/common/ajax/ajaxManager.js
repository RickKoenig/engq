var loadcnt = 0;
var reqcnt = 0;

var nwloadcnt = 0; // no wait
var nwreqcnt = 0;

// sends parsed object to the php server
function goAjax3(php,id,cbf,obj,post,delaytime,nowait) {
	if (nowait)
		++nwreqcnt;
	else
		++reqcnt;
	if (!delaytime) delaytime = 1500;
	var ajaxRequest=CreateRequestObject(); /* Cross-browser check;
	  Get a new XMLHttpRequest object */
	ajaxRequest.onload = function() {
		if (delaytime) {
			var timeoutfunc = function() {
				if (nowait)
					++nwloadcnt;
				else
					++loadcnt;
				cbf(ajaxRequest.responseText,id);
			};
			setTimeout(timeoutfunc,delaytime);
		} else {
			if (nowait)
				++nwloadcnt;
			else
				++loadcnt;
			cbf(ajaxRequest.responseText,id,nowait);
		}
	};  // End callback function
	// build up a request string from an object
	var vobj = [];
	for (attr in obj) {
		var val = obj[attr];
		if (typeof val != "function") {
			var sval = JSON.stringify(val);
			// let's remove the quotes from strings
			if (sval.charAt(0) == '"') {
				sval = sval.substring(1,sval.length-1);
			}
			vobj.push(attr + "=" + encodeURIComponent(sval));
		}
	}
	var sendstr = vobj.join("&");
	if (post) {
		// POST
		//ajaxRequest.open("POST", "tracks.php")
		ajaxRequest.open("POST", php);
		ajaxRequest.setRequestHeader("Content-type","application/x-www-form-urlencoded");
		ajaxRequest.send(sendstr);
	} else {
		// GET
		ajaxRequest.open("GET",
		  php + "?" + sendstr);
		ajaxRequest.send(null);
	}
} // End ajaxFunction()

/*
// sends multiple requests from an array of objects, callback receives array of results
function goAjax4(php,cbf,arr,post) {
	// implement nested closures ...
	var res = [];
	var cnt = 0;
	var len = arr.length;
	var i;
	for (i=0;i<len;++i) {
		cb = function(resp,i) {
			res[i] = resp;
			++cnt;
			if (cnt == len)
				cbf(res);
		}
		goAjax3(php,i,cb,arr[i],post);
	}
}
*/

function goAjaxText(filename,cbf,nowait) {
	//logger_str += " goAjaxText = " + filename + "\n";
	var s = spliturl(filename);
	var key = geturlfrompathnameext("",s.name,s.ext);
	preloadedtext[key] = "loading";
	if (nowait)
		++nwreqcnt;
	else
		++reqcnt;
	var oReq = new CreateRequestObject();
	oReq.open("GET", filename, true);
	oReq.onload = function () {
		if (nowait)
			++nwloadcnt;
		else
			++loadcnt;
		var txt = oReq.responseText;
		cbf(txt,key,nowait);
	};
	oReq.send(null);
}

var alertbin = false;
function goAjaxBin(filename,cbf,nowait) {
	//logger_str += " goAjaxBin = " + filename + "\n";
	var s = spliturl(filename);
	var key = geturlfrompathnameext("",s.name,s.ext);
	preloadedbin[key] = "loading";
	if (nowait)
		++nwreqcnt;
	else
		++reqcnt;
	var oReq = new CreateRequestObject();
	oReq.open("GET", filename, true);
	oReq.responseType = "arraybuffer";
	oReq.onload = function () {
		if (oReq.status == 404) { // file not found
			//alert("file not found '"+ filename + "'");
				alertbin = true;
			if (nowait)
				++nwloadcnt;
			else
				++loadcnt;
			cbf(null,filename,nowait);
		} else {
			var arrayBuffer = oReq.response; // Note: not oReq.responseText
			if (!arrayBuffer) {
				if (!alertbin) {
					alert("Browser problem was encountered, no Binary support!");
					alertbin = true;
				}
				//return;
			}
			if (nowait)
				++nwloadcnt;
			else
				++loadcnt;
			//if (arrayBuffer)
			cbf(arrayBuffer,filename,nowait);
	//		cbf(arrayBuffer,key,nowait);
		}
	};
	oReq.send(null);
}

var bwsinlightmap; // skip some image load error alerts in ajaxManager when loading a lightmap texture
function goAjaxImg(filename,cbf,nowait,data,keyname) {
	var inlightmap = bwsinlightmap; // closure
	//logger_str += " goAjaxImg = " + filename + "\n";
	var s = spliturl(filename);
	var key;
	if (keyname)
		key = keyname;
	else
		key = geturlfrompathnameext("",s.name,s.ext);
	var lext = s.ext.toLowerCase();
	var extalpha = false;
	if (lext == "dds" || lext == "tga" || lext == "png") {
		s.ext = "png";
		filename = geturlfrompathnameext(s.path,s.name,s.ext);
		extalpha = true;
	}
	if (nowait)
		++nwreqcnt;
	else
		++reqcnt;
	var img = new Image();
	preloadedimages[key] = img;
	img.hasalpha = extalpha;
	img.onload = function () {
		if (nowait)
			++nwloadcnt;
		else
			++loadcnt;
		cbf(img,key,nowait);
	};
	img.onerror = function () {
		if (s.ext == "jpg") { // try .png if .jpg fails to load
			var newsrc = geturlfrompathnameext(s.path,s.name,"png");
			img = new Image();
			preloadedimages[key] = img;
			img.hasalpha = false;
			img.onload = function() {
				if (nowait)
					++nwloadcnt;
				else
					++loadcnt;
				cbf(img,key,nowait);
			};
			img.onerror = function() {
				alert("image load error 2nd try: filename = " + filename);
				if (nowait)
					++nwloadcnt;
				else
					++loadcnt;
				cbf(null,key,nowait);
			};
			img.src = newsrc;
		} else {
			if (!inlightmap) // skip alerts if trying to load a lightmap
				alert("image load error: filename = " + filename);
			if (nowait)
				++nwloadcnt;
			else
				++loadcnt;
			cbf(null,key,nowait);
		}
	};
	if (data)
		img.src = data;
	else
		img.src = filename;
}

function goAjaxTimer(cbf,timv,nowait) {
	if (nowait)
		++nwreqcnt;
	else
		++reqcnt;
	var timf = function() {
		if (nowait)
			++nwloadcnt;
		else
			++loadcnt;
		if (cbf)
			cbf(timv,nowait);
	};
	window.setTimeout(timf,timv);
}

var preloadedtext = {};
var preloadedbin = {};
var preloadedimages = {};

var loaddonefunc = null;

function isloaded() {
	return loadcnt == reqcnt;
}

function setloaddonefunc(ldf) {
	loaddonefunc = ldf;
	if (isloaded()) // nothing to preload ?
		loaddonefunc();
}

function preloadtext(resname,nowait) {
	var s = spliturl(resname);
	var key = geturlfrompathnameext("",s.name,s.ext);
	var t = preloadedtext[key];
	if (t)
		return;
	goAjaxText(resname,resppreloadtxt,nowait);
}

function resppreloadtxt(txt,txtname,nowait) {
	var s = spliturl(txtname);
	var key = geturlfrompathnameext("",s.name,s.ext);
	preloadedtext[key] = txt;
	if (isloaded() && !nowait) {
		loaddonefunc();
	}
}

function preloadbin(binname,nowait) {
	var s = spliturl(binname);
	var key = geturlfrompathnameext("",s.name,s.ext);
	var t = preloadedbin[key];
	if (t)
		return;
	goAjaxBin(binname,resppreloadbin,nowait);
}

function resppreloadbin(bin,binname,nowait) {
	if (!bin) {
		if (isloaded() && !nowait) {
			if (loaddonefunc) {
				loaddonefunc();
			}
		}
		return;
	}
	var s = spliturl(binname);
	var key = geturlfrompathnameext("",s.name,s.ext);
	preloadedbin[key] = bin;
	if (isloaded() && !nowait) {
		if (loaddonefunc) {
			loaddonefunc();
		}
	}
}

// return true if image has some transparency
function imghasalpha(img) {
	var spriteCanvas = document.createElement('canvas');
	spriteCanvas.width = img.naturalWidth;
	spriteCanvas.height = img.naturalHeight;
	var spriteContext = spriteCanvas.getContext('2d');
	spriteContext.drawImage(img,0,0);
	var imageData = spriteContext.getImageData(0,0,img.naturalWidth,img.naturalHeight);
	var i;
	for (i=0;i<imageData.data.length;i+=4) {
		var aval = imageData.data[i+3];
		if (aval < 240) {
			img.hasalpha = true;
			return;
		}
	}
	img.hasalpha = false; // all pixels have too much opacity
}
	
function preloadimg(imgname,nowait,imgdatab64) {
	var s = spliturl(imgname);
	if (s.ext.length == 0) { // it's a folder of 6 files
		var sixname1 = ["posx.jpg","posy.jpg","posz.jpg","negx.jpg","negy.jpg","negz.jpg"];
		var sixname2 = ["POSX","POSY","POSZ","NEGX","NEGY","NEGZ"];
		var i;
		for (i=0;i<6;++i) {
			var key = imgname + "/" + sixname1[i];
			var keyname = sixname2[i] + "_" + s.name + ".jpg"; // name becomes like 'POSX_fname.jpg'
			var t = preloadedimages[keyname];
			if (t)
				return;
			goAjaxImg(key,resppreloadimg,nowait,imgdatab64,keyname);
		}
	} else {
		var key = geturlfrompathnameext("",s.name,s.ext);
		var t = preloadedimages[key];
		if (t)
			return;
		goAjaxImg(imgname,resppreloadimg,nowait,imgdatab64);
	}
}

function resppreloadimg(img,key,nowait) {
	//if (!keyname)
	//	keyname = imgname;
	if (!img) {
		img = preloadedimages["maptestnck.png"];
		img.err = true;
	} else {
		img.err = false;
	}
	if (img.hasalpha)
		imghasalpha(img); // add a hasalpha member to the image, test image for transparency, check only alpha bitmaps like png
//	preloadedimages[keyname] = img;
	
	if (isloaded() && !nowait) {
		if (loaddonefunc) {
			loaddonefunc();
		}
	}
}

function preloadtime(tim,nowait) {
	goAjaxTimer(resppreloadtime,tim,nowait);
}

function resppreloadtime(timv,nowait) {
/*	if (nowait)
		logger_str += "nwresptime " + timv + "\n";	
	else
		logger_str += "resptime " + timv + "\n";	*/
	if (isloaded() && !nowait) {
		if (loaddonefunc) {
			loaddonefunc();
		}
	}
}

function preloadshaderlist(shaderlistname,nowait) {
	var t = preloadedtext[shaderlistname];
	if (t)
		return;
	goAjaxText(shaderlistname,resppreloadshaderlist,nowait);
}

function resppreloadshaderlist(shaderlistj,shaderlistname,nowait) {
	if (!shaderlistj) {
		alert("no shaderlist '" + shaderlistname + "'");
	}
	shaderlist = JSON.parse(shaderlistj);
	//preloadedtext[shaderlistname] = shaderlist;
	preloadShaders();
	if (isloaded() && !nowait) {
		if (loaddonefunc) {
			loaddonefunc();
		}
	}
}


// encodeURIComponent 
