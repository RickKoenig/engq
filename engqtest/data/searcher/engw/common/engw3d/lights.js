var lights = {
	"wlightdir":[0.0,0.0,0.0]
};

var dirlight = null; // this light will point along z

var amblight = null;

function dolights() {
	var wld4 = [0,0,1,0];
	var eld4 = vec4.create();
	if (dirlight && dirlight.mvm) {
		vec4.transformMat4(eld4,wld4,dirlight.mvm);
	} else {
		vec4.transformMat4(eld4,wld4,mvMatrix); // convert world space light dir vector to eye space
	}
	//wld = vec4.fromValues(lights.wlightdir[0],lights.wlightdir[1],lights.wlightdir[2],0.0);
	var eld3 = vec3.clone(eld4); // pare it down to 3 elements
	vec3.normalize(eld3,eld3); // normalize
	globalmat.elightdir = eld3;
}

function addlight(t) {
	if (t.flags & treeflagenums.LIGHT) {
		if (amblight == null && t.flags & treeflagenums.AMBLIGHT) {
			amblight = t;
		}
		if (dirlight == null && t.flags & treeflagenums.DIRLIGHT) {
			dirlight = t;
		}
	}
}

function removelight(t) {
	if (t == amblight)
		amblight = null;
	if (t = dirlight)
		dirlight = null;
}
