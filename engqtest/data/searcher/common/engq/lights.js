var lights = {
	"wlightdir":[0.0,0.0,0.0]
};

function dolights() {
	var wld = vec4.fromValues(lights.wlightdir[0],lights.wlightdir[1],lights.wlightdir[2],0.0);
	var eld = vec4.create();
	vec4.transformMat4(eld,wld,mvMatrix); // convert world space light dir vector to eye space
	var eld3 = vec3.clone(eld); // pare it down to 3 elements
	vec3.normalize(eld3,eld3); // normalize
	globalmat.elightdir = eld3;
}
