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
