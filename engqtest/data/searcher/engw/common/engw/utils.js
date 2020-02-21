function range(a,b,c) {
	if (b<a)
		return a;
	if (b>c)
		return c;
	return b;
}

// returns a unique name
var uniqueid = 0;
function uniquestr() {
	return "unique" + uniqueid++;
}

/* function countProperties(obj) {
    var count = 0;

    for(var prop in obj) {
        if(obj.hasOwnProperty(prop))
            ++count;
    }

    return count;
} */

// running average class
function Runavg(nele) {
	this.nele = nele;
	this.arr = [];
	this.idx = 0;
	this.sum = 0;
}

Runavg.prototype.add = function(num) {
	if (this.arr.length == this.nele) { // array filled up
		this.sum -= this.arr[this.idx];
		this.arr[this.idx] = num;
		this.sum += num;
		++this.idx;
		if (this.idx == this.nele)
			this.idx = 0;
	} else { // building up array
		this.arr[this.idx] = num;
		this.sum += num;
		++this.idx;
		if (this.idx == this.nele)
			this.idx = 0;
	}
	return this.sum/this.arr.length;
};