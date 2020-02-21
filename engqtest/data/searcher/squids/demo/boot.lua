

-- return an object containing the img ref, and w, y
function getImage(file)
    -- local url = root .. "/" .. file
	url = file
	local i, width, height
	i, width, height = p_makeImage(p_loadData(url))
	p_log("getImage: " .. url .. " size=" .. width .. "," .. height)
    return { idata = i, w = width, h = height, x = 0, y = 0, r = 0.0}
end


-- return a unique integer
seqNum = 0;
function seq()
	seqNum = seqNum + 1
	return seqNum
end


-- duplicate via a shallow copy
function clone(proto)
  o = {}
  local inx = nil
  local val = nil
  repeat
    inx, val = next(proto, inx)
    if val then o[inx] = val end
  until (inx == nil)
  return o
end



-- apply some stupid semi-newtonian motion to a squid
function newton(sq)

	-- apply velocity
	sq.x = sq.x + sq.vx
	sq.y = sq.y + sq.vy
	-- apply gravity
	sq.vx = sq.vx + sq.gx
	sq.vy = sq.vy + sq.gy
	-- apply friction
	sq.vx = sq.vx - sq.fx
	sq.vy = sq.vy - sq.fy

	-- apply rotational velocity, acceleration, and friction 
	sq.r = sq.r + sq.rv
	sq.rv = sq.rv + sq.ra
	sq.rv = sq.rv - sq.rf

end

function wrap(sq)
	if(sq.x > sw + (sq.img.w / 2)) then sq.x = 0 end
	if(sq.x < 0) then sq.x = sw - (sq.img.w / 2) - 1 end
	if(sq.y > sh + (sq.img.h / 2)) then sq.y = 0 end
	if(sq.y < 0) then sq.y = sh + (sq.img.h / 2) - 1 end
end


-- just draws a squid with it's upper left corner at its x,y 
function justDraw(sq)
	if(sq.img) then
		p_drawImage(sq.img.idata, sq.x, sq.y, sq.a, sq.r, sq.px, sq.py, sq.sx, sq.sy);
	end
end



ProtoSquid = {

	name = "ProtoSquid",
	id = 0,

	img = nil,			-- default img?

	x = 0, y = 0,		-- position

	vx = 0, vy = 0,		-- velocity
	gx = 0, gy = 0,		-- gravity
	fx = 0, fy = 0,		-- friction

	sx = 1.0, sy = 1.0,	-- scale

	r = 0,				-- rotation
	rv = 0,				-- rotational velocity
	ra = 0,				-- rotational acceleration
	rf = 0,				-- rotational friction
	px = 0, py = 0,		-- pivot point

	a = 1.0,			-- alpha

	-- replaceable tick function
	tick = function(sq) newton(sq) end,

	-- replaceable draw function
	draw = function(sq) justDraw(sq) end,

}


function setScale(sq, sx, sy) 
	sq.sx = sx
	sq.sy = sy
end

-- create a new squid
function mkSquid(img, x, y)
	local o = clone(ProtoSquid);

	o.id = seq()
	o.name = "Squid-" .. o.id

	o.img = nil
	if(img) then
		o.img = img
		o.px = img.w / 2
		o.py = img.h / 2
	end

	o.x, o.y = x or 0, y or 0;

	return o;
end



root = p_getEnv('root')
p_log("root: " .. root)

sw = p_getEnv('screenWidth')
sh = p_getEnv('p_screenHeight')
p_log("screen: " .. sw .. "," .. sh)



rocks = {};
img = getImage("img/rock.png");
rock = mkSquid(img, 100, 100)
rock.vx = 0.3;
rock.vy = 0.5;
rock.sx = 0.5;
rock.sy = 0.5;
rock.tick = function(self)
	newton(self);
	wrap(self);
end
rocks[1] = rock
for i = 2, 400 do
	local sq = clone(rock);
	sq.vx = i * 0.025;
	sq.vy = i * 0.025;
	sq.rv = 0.02;
	sq.a = i * 0.01;
	sq.sx = i * 0.0020;
	sq.sy = i * 0.0020;
	rocks[i] = sq;
end



ufo = mkSquid(getImage("img/rocket.png"), sw / 2, sh / 2);
ufo.rv = 0.11;
ufo.sx = 0.3;
ufo.sy = 0.3;
ufo.vy = 0.0;
ufo.gy = 0.0;
ufo.tick = function(self)

	newton(self)

	local r, ra = ufo.r, ufo.ra
	r = r + ra;
	ufo.r, ufo.ra = r, ra

	if(ufo.x >= sw) then
		ufo.x = -140;
	end

	if(ufo.y >= sh) then
		ufo.y = -90;
	end

	if(ufo.y >= 400) then
		ufo.y = 400
		ufo.vy = (ufo.vy * -1) * 0.7
	end

end



function a_tick() 
	ufo:tick();
	for i = 1, #rocks do
		rocks[i]:tick()
	end
end


function a_draw() 
	for i = 2, #rocks do
		rocks[i]:draw()
	end
	ufo:draw();

end


function a_event(e, x, y)
    p_log("click " .. x .. "," .. y);
    touchx = x;
    touchy = y;
end


