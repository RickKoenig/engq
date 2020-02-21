// new approach, let javascript run the show, not the php

var messid;		// keep track of message traffic
var trackarr; 	// array of tracks
var curtrackname;	// current trackname
var newtrackname;

var curtrackdata;
var showresp = false;

// show last response  and convert to object
function responsephp(resp,id) {
	try {
		var obj = JSON.parse(resp);
	} catch(err) {
		var obj = 'JSON parse error';
	}
	if (showresp) {
		var objstr = JSON.stringify(obj);
		resp = escapehtml(objstr) + ", id = " + id + "<br/>";
		htmladd("responsephp","p",resp);
	}
	return obj;
}

function clearphp() {
	htmlclear("responsephp");
}

function disablephp() {
	showresp = !showresp;
	setupdisableresponsephp();
}

function testbin() {
	goAjaxBin("testbinbig.bin",responsebin);	//var oReq = new XDomainRequest();
	goAjaxBin("woodbridge01.bwo",unchunktest);
	goAjaxText("../sprites2/shaders/basic.ps",responsetext1);
	goAjaxImg("../sprites2/pics/panel.jpg",responseimg1);
}

function responsebin(ab,binname) { // ArrayBuffer
	var U8dataView = new Uint8Array(ab);
	var k = 0;
	var i;
	var ml = U8dataView.length;
	if (ml > 10)
		ml = 10;
	var pass = true;
	var i,j;
	for (j=0;j<256;++j) {
		for (i=0;i<256;++i) {
			v = U8dataView[k++];
			if (v!=255-j) {
				pass = false;
				break;
			}
			v = U8dataView[k++];
			if (v!=255-i) {
				pass = false;
				break;
			}
		}
		if (!pass) {
			break;
		}
	}
	if (pass) {
		for (j=0;j<256;++j) {
			for (i=0;i<256;++i) {
				v = U8dataView[k++];
				if (v!=255-i) {
					pass = false;
					break;
				}
				v = U8dataView[k++];
				if (v!=255-j) {
					pass = false;
					break;
				}
			}
			if (!pass) {
				break;
			}
		}
	}
	var datastr = pass ? "pass" : "fail";
	for (i=0;i<ml;++i)
		datastr += " " + U8dataView[i];
	htmladd("responsephp","p"," data = [" + datastr + " ]" + " datasize = '" + U8dataView.length + "'");
	htmladd("responsephp","p",binname);
}

function responsetext1(txt,txtname) {
	htmladd("responsephp","p",escapehtml(txt));
	htmladd("responsephp","p",txtname);
}

function responseimg1(img,imgname) {
	eleadd("responsephp",img);
	htmladd("responsephp","p",imgname);
}

function setupdisableresponsephp() {
	var ele = document.getElementById("responsedisable");
	document.getElementById("responsedisable").innerHTML = showresp ? "Disable" : "Enable";
}

//// a table from array of objects
function buildtable(tablename,arr,butname,butfuncname,butname2,butfuncname2) {
	var out = '';
	if (arr && arr.length > 0) {
		var attr;
		// title
		out += tablename + "<br/><br/>";
		out += "<table border='1'>";
		out += "<tr>";
		// header
		var obj = arr[0];
		for (attr in obj) {
			if (typeof obj[attr] != "function") {
				out += "<th>" + attr + "</th>";
			}
		}
		out += "</tr>";
		// data
		var j;
		for (j=0;j<arr.length;++j) {
			var obj = arr[j];
			out += "<tr>";
			for (attr in obj) {
				if (typeof obj[attr] != "function") {
					out += "<td>" + escapehtml(obj[attr]) + "</td>";
				}
			}
			// extra, some buttons
			if (butname) {
				out += '<td><button type="button" onclick=' + butfuncname + '(' + j + ')>' + butname + '</button></td>';
			}
			if (butname2) {
				out += '<td><button type="button" onclick=' + butfuncname2 + '(' + j + ')>' + butname2 + '</button></td>';
			}
			out += "</tr>";
		}
		out += "</table>";
		out += "<br/><br/>";
	}
	return out;
}

function checkoverwrite() {
	var n = trackarr.length;
	var i;
	for (i=0;i<n;++i)
		if (trackarr[i].name == newtrackname)
			break;
	if (i!=n)
		return confirm("Overwrite track '" + newtrackname + "' ?");
	else
		return true;
}
//////// login / register

function setuptitle() {
	htmlclear("span1");
	var htmltitle = 'Login / Register';
	htmladd('span1','h4',htmltitle);
	
	var htmllogin = 
		'<form id="formlogin">' +
			'Login username: <input type="text" name="loginusername" id="loginusername" value="player"/><br/>' +
			//'Login age: <input type="text" name="loginuserage" id="loginuserage" value="22"/><br />' +
			'<input type="submit" value="LOGIN" />' +
		'</form>';// +
		//'<br />';
	htmladd('span1','p',htmllogin);
	document.getElementById("formlogin").onsubmit = submitlogin;

	var htmlregister = 
		'<form id="formregister">' +
		'	Register username: <input type="text" name="registerusername" id="registerusername" value="player"/><br />' +
		'	<input type="submit" value="REGISTER" />' +
		'</form> ';// +
		//'<br /> ';
	htmladd('span1','p',htmlregister);
	document.getElementById("formregister").onsubmit = submitregister;
}

function submitlogin() {
	// don't do regular submit, but do an ajax instead
	var yourname = document.getElementById("loginusername").value;
	var htmllogin = "login try with " + "'" + yourname + "'";
	setuptitle();
	htmladd("span1","p",htmllogin);
	//var yourage = document.getElementById("loginuserage").value;
	//yourage = parseInt(yourage);
	var sendobj = {
		"command":"login",
		"name":yourname,
		//"age":yourage,
		"obj":[{},
			{"x\"x":44,"y":"5j5"},{"x":32,"y":64}
		]
	};
	goAjax3("tracksmain.php",messid++,responselogin,sendobj,true);
	return false;
}

function responselogin(resp,id) {
	var obj = responsephp(resp,id);
	if (obj.result) {
		var htmllogindone = "login succeeded";
		trackarr = obj.list;
		setupmainmenu();
	} else {
		var htmllogindone = "login failed";
		setuptitle();
	}
	htmladd("span1","p",htmllogindone);
}

function submitregister() {
	// don't do regular submit, but do an ajax instead
	var yourname = document.getElementById("registerusername").value;
	var htmlregister = "register try with " + "'" + yourname + "'";
	setuptitle();
	htmladd("span1","p",htmlregister);
	var sendobj = {
		"command":"register",
		"name":yourname,
		"obj":[{},
			{"x\"x":44,"y":"5j5"},{"x":32,"y":64}
		]
	};
	goAjax3("tracksmain.php",messid++,responseregister,sendobj,true);
	return false;
}

function responseregister(resp,id) {
	var obj = responsephp(resp,id);
	if (obj.result) {
		var html = "register succeeded";
	} else {
		var html = "register failed";
	}
	setuptitle();
	htmladd("span1","p",html);
}

//////// mainmenu

function setupmainmenu() {
	htmlclear("span1");
	var htmlmainmenu = 'Main Menu';
	htmladd('span1','h4',htmlmainmenu);
	
	htmllogout = 
	'<form id="formlogout" style="margin:5px;padding:0;display:inline;">' +
		'<input type="submit" value="LOGOUT" />' +
	'</form>';// +
//	'<br />' +
//	'<br />';
	htmladd("span1","span",htmllogout);
	
	var ele = document.getElementById("formlogout");	
	ele.onsubmit = submitlogout;
	htmldisappear = 
		'<form id="formdisappear" style="margin:5px;padding:0;display:inline;">' +
			'<input type="submit" value="DISAPPEAR" />' +
		'</form>';// +
//		'<br />' +
//		'<br />';
	htmladd("span1","span",htmldisappear);
	document.getElementById("formdisappear").onsubmit = submitdisappear;

	htmltracklist = buildtable("Track list",trackarr,"Select","onclickselecttrack");
	htmladd("span1","p",htmltracklist);
	
	htmlnewtrack = 
		'<form id="formnewtrack">' +
			'<input type="text" id="newtrackname" maxlength="25" value="" />' +
			'<input type="submit" value="NEW TRACK" />' +
		'</form>';// +
//		'<br />' +
//		'<br />';
	htmladd("span1","p",htmlnewtrack);
	document.getElementById("formnewtrack").onsubmit = submitnewtrack;
}

function submitlogout() {
	setupmainmenu();
	var html = "logout try";
	htmladd("span1","p",html);
	var sendobj = {
		"command":"logout"
	};
	goAjax3("tracksmain.php",messid++,responselogoutdisappear,sendobj,true);
	return false;
}

function submitdisappear() {
	setupmainmenu();
	var html = "disappear try";
	htmladd("span1","p",html);
	var sendobj = {
		"command":"disappear"
	};
	goAjax3("tracksmain.php",messid++,responselogoutdisappear,sendobj,true);
	return false;
}

function responselogoutdisappear(resp,id) {
	var obj = responsephp(resp,id);
	if (obj.result) {
		var html = 'logout/disappear succeeded';
	} else {
		var html = 'logout/disappear failed';
	}
	setuptitle();
	htmladd("span1","p",html);
}

function onclickselecttrack(row) {
	curtrackname = trackarr[row].name;
	setupmainmenu();
	var htmlselecttrack = "select try '" + curtrackname + "'";
	htmladd("span1","p",htmlselecttrack);
	var sendobj = {
		"command":"getdata",
		"name":curtrackname,
	};
	goAjax3("tracksmain.php",messid++,responseselecttrack,sendobj,true);
}

function responseselecttrack(resp,id) {
	try {
		var parseresp = JSON.parse(resp);
	} catch(err) {
		var parseresp = '"JSON parse error in responseselecttrack"';
	}
	var resp2 = JSON.stringify(parseresp);
	//resp2 = "\"a string\"";
	var robj = responsephp(resp2,id);
	var dobj = robj.data;
	if (dobj != undefined) {
		try {
			curtrackdata = JSON.parse(dobj);
			var jsonstring = JSON.stringify(curtrackdata);
		} catch(err) {
			var jsonstring = dobj + " (JSON parse error)";
		}
		setuptrackmenu();
		document.getElementById("textareadata").value = jsonstring;
		// var jshtml = "data = " + escapehtml(jsonstring) + "<br/>";
		// ele.value = jshtml;
		var htmlselecttrack = 'select track succeeded';
	} else {
		setupmainmenu();
		var htmlselecttrack = 'no \'data\' member in responseselecttrack';
	}
	htmladd("span1","p",htmlselecttrack);
}

function submitnewtrack() {
	curtrackname = newtrackname = document.getElementById("newtrackname").value;
	if (!checkoverwrite()) {
		setupmainmenu();
		var html = "don't overwrite '" + newtrackname + "'";
		htmladd("span1","p",html);
		return false;
	}
	curtrackdata = {"data":"newdata","mid * 10":messid * 10};
	var htmlnewtrack = "new track try " + "'" + curtrackname + "'";
	setupmainmenu();
	htmladd("span1","p",htmlnewtrack);
	var sendobj = {
		"command":"setdata",
		"name":curtrackname,
		"data":curtrackdata
		//"overwrite":false
	};
	goAjax3("tracksmain.php",messid++,responsenewtrack,sendobj,true);
	return false;
}

function responsenewtrack(resp,id) {
	var obj = responsephp(resp,id);
	if (obj.result) {
		trackarr = obj.list;
		setuptrackmenu();
		var ele = document.getElementById("textareadata").value = JSON.stringify(curtrackdata); // hope this matches the database
		var htmlnewtrack = 'new track succeeded ' + "'" + curtrackname + "'";
	} else {
		setupmainmenu();
		var htmlnewtrack = 'new track failed ' + "'" + curtrackname + "'";
	}
	htmladd("span1","p",htmlnewtrack);
}

//////// trackmenu

function setuptrackmenu() {
	htmlclear("span1");
	var htmltrackmenu = 'Track Menu';
	htmladd('span1','h4',htmltrackmenu);
	htmladd('span1','p',curtrackname);
	
	htmldata = 
	'<form id="formddata">' +
		'<textarea spellcheck="false" id="textareadata">track data</textarea>' +
	'</form>';// +
	//'<br />';
	htmladd("span1","p",htmldata);
	
	htmlsave = 
	'<form id="formsave" style="margin:5px;padding:0;display:inline;">' +
		'<input type="submit" value="SAVE" />' +
	'</form>';// +
	//'<br />';
	htmladd("span1","span",htmlsave);
	document.getElementById("formsave").onsubmit = submitsave;
	
	htmlsaveas = 
	'<form id="formsaveas" style="margin:5px;padding:0;display:inline;">' +
		'<input type="submit" value="SAVE AS" />' +
		'<input type="text" id="saveastrackname" maxlength="25" value="" />' +
	'</form>';// +
	//'<br />';
	htmladd("span1","span",htmlsaveas);
	document.getElementById("formsaveas").onsubmit = submitsaveas;
	document.getElementById("saveastrackname").value = curtrackname;
	
/*	html = 
	'<form id="formrename" style="margin:5px;padding:0;display:inline;">' +
		'<input type="submit" value="RENAME" />' +
		'<input type="text" id="renametrackname" maxlength="25" value="" />' +
	'</form>';// +
	//'<br />';
	htmladd("span1","span",html);
	document.getElementById("formrename").onsubmit = submitrename;
	document.getElementById("renametrackname").value = curtrackname;
*/	
	htmlnosave = 
	'<form id="formnosave" style="margin:5px;padding:0;display:inline;">' +
		'<input type="submit" value="NO SAVE" />' +
	'</form>';// +
	//'<br />';
	htmladd("span1","span",htmlnosave);
	document.getElementById("formnosave").onsubmit = submitnosave;
	
	htmldelete = 
	'<form id="formdelete" style="margin:5px;padding:0;display:inline;">' +
		'<input type="submit" value="DELETE" />' +
	'</form>' +
	'<br />';
	htmladd("span1","span",htmldelete);
	ele = document.getElementById("formdelete").onsubmit = submitdelete;
}

function submitsave() {
	newtrackname = curtrackname;
	curtrackdata = document.getElementById("textareadata").value;
	setupmainmenu();
	var htmlsavetrack = "save track try " + "'" + curtrackname + "'";
	htmladd("span1","p",htmlsavetrack);
	var sendobj = {
		"command":"setdata",
		"name":curtrackname,
		"data":curtrackdata
		//"overwrite":true
	};
	goAjax3("tracksmain.php",messid++,responsesave,sendobj,true);
	return false;
}

function responsesave(resp,id) {
	responsesavesaveas(resp,id,"Save");
}

function submitsaveas() {
	newtrackname = document.getElementById("saveastrackname").value;
	curtrackdata = document.getElementById("textareadata").value;

	if (!checkoverwrite()) {
		setuptrackmenu();
		document.getElementById("textareadata").value = curtrackdata;
		var html = "don't overwrite '" + newtrackname + "'";
		htmladd("span1","p",html);
		return false;
	}

	setupmainmenu();
	var htmlsavetrack = "save as track try " + "'" + newtrackname + "'";
	htmladd("span1","p",htmlsavetrack);
	var sendobj = {
		"command":"setdata",
		"name":newtrackname,
		"data":curtrackdata
		//"overwrite":false
	};
	goAjax3("tracksmain.php",messid++,responsesaveas,sendobj,true);
	return false;
}

function responsesaveas(resp,id) {
	responsesavesaveas(resp,id,"Save as");
}

/*function submitrename() {
	oldtrackname = curtrackname;
	curtrackname = document.getElementById("renametrackname").value;
	var savedata = document.getElementById("textareadata").value;
	setupmainmenu();
	var htmlsavetrack = "rename track try " + "oldname '" + oldtrackname +  + "' new name '" + curtrackname + "'";
	htmladd("span1","p",htmlsavetrack);
	var sendobj = {
		"command":"rename",
		"name":curtrackname,
		"oldname":oldtrackname,
		"data":savedata
	};
	goAjax3("tracksmain.php",messid++,responsesave,sendobj,true);
	return false;
}*/

function responsesavesaveas(resp,id,mess) {
	var obj = responsephp(resp,id);
	if (obj.result) {
		var html = mess + ' track succeeded ' + "'" + newtrackname + "'";
		curtrackname = newtrackname;
		trackarr = obj.list;
		//curtrackname = '';
		setupmainmenu();
	} else {
		var html = mess + ' track failed ' + "'" + newtrackname + "'";
		setuptrackmenu();
		document.getElementById("textareadata").value = curtrackdata;
	}
	htmladd("span1","p",html);
}

function submitnosave() {
	setupmainmenu();
	var htmlnosave = 'nosave';
	htmladd("span1","p",htmlnosave);
	return false;
}

function submitdelete() {
	setupmainmenu();
	var htmldeltrack = "delete track try " + "'" + curtrackname + "'";
	htmladd("span1","p",htmldeltrack);
	var sendobj = {
		"command":"deletedata",
		"name":curtrackname,
	};
	goAjax3("tracksmain.php",messid++,responsedelete,sendobj,true);
	return false;
}

function responsedelete(resp,id) {
	var obj = responsephp(resp,id);
	if (obj.result) {
		var htmldeltrack = 'delete track succeeded ' + "'" + curtrackname + "'";
		trackarr = obj.list;
		//curtrackname = '';
	} else {
		var htmldeltrack = 'delete track failed ' + "'" + curtrackname + "'";
	}
	setupmainmenu();
	htmladd("span1","p",htmldeltrack);
}

//////// init

function setup() {
	document.getElementById("hostipaddr").innerHTML = "Server Address " + location.hostname;
	messid = 10000;
	document.getElementById("responseclear").onclick = clearphp;
	document.getElementById("responsedisable").onclick = disablephp;
	document.getElementById("testbin").onclick = testbin;
	setupdisableresponsephp();
	setuptitle();
}

window.onload = setup;
