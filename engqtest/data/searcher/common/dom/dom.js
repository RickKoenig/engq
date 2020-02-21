// closure tests
function sumff(a) {
	return function(b) {
		return a + b;
	};
}
// end closure tests

function htmlclear(parent) {
	var ele = document.getElementById(parent);
	ele.innerHTML = '';
}

function htmladd(parent,celename,data) {
	var pele = document.getElementById(parent);
	var cele = document.createElement(celename);
	if (data != undefined)
		cele.innerHTML = data;
	pele.appendChild(cele);
}

function eleadd(parent,data) {
	var pele = document.getElementById(parent);
	pele.appendChild(data);
}

