// returns 0<=ang<2*PI eventually
function normalangrad(rad) {
	if ((rad > 1000000) || (rad < -1000000))
		alert("normalangrad getting too big! " + rad);
	if (rad < 0.0)
		rad += 2*Math.PI;
	if (rad >= 2*Math.PI)
		rad -= 2*Math.PI;
	return rad;
}
