

function dbg(s)
	p_log(s);
end



function makeSoundSet(file, num)
	-- local data = p_loadData(root .. "/" .. file);
	local data = p_loadData(file);
	local pf = function(self, v)
			local n = self.idx;
			p_playSound(self.snds[n], v);
			n = n + 1;
			if(n > #(self.snds)) then
				n = 1;
			end
			self.idx = n;
		end
	local set = {idx = 1, play = pf, snds = {} }
	for i = 1, num do
		set.snds[i] = p_makeSound(data);
	end
	return set;
end



-- return an object containing the img ref, and w, y
function getImage(file)
    	local url = file
	-- local url = root .. "/" .. file
	local i, width, height
	i, width, height = p_makeImage(p_loadData(url))
	dbg("getImage: " .. url .. " size=" .. width .. "," .. height)
    return { idata = i, w = width, h = height, x = 0, y = 0, r = 0.0}
end

function getSound(file)
	-- return p_makeSound(p_loadData( root .. "/" .. file))
	return p_makeSound(p_loadData( file))
end
function play(snd, volume) 
	if volume == nil then
		volume = 1.0;
	end
	p_playSound(snd, volume);
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


function hit(sq, hx, hy)
	local x, y = sq.x, sq.y
	local img = sq.img
	local w2 = img.w / 2
	local h2 = img.h / 2

	if(hx < x - w2) then return false; end
	if(hx > x + w2) then return false; end
	if(hy < y - h2) then return false; end
	if(hy > y + h2) then return false; end

	return true
end



root = p_getEnv('root')
dbg("root: " .. root)

sw = p_getEnv('screenWidth')
sh = p_getEnv('screenHeight')
dbg("screen: " .. sw .. "," .. sh)


bg = getImage("paper.jpg");
reset = getImage("reset.png");


chars = {"a", "b", "c"};
for i = 1, #chars do
	local s = chars[i]
	local img = getImage("char_" .. s .. ".png");
	chars[i] = img;
end

charDraw = function(sq)
	if(sq.img == nil) then
		return
	end

	rx, ry = rocket.x, rocket.y
	tx, ty = sq.x, sq.y

	dy = math.abs(ty - ry)
	if(dy < 150) then
		-- close
		wx = 150 - dy

		dx = math.abs(tx - rx)
		if(dx < 250) then

			m = 250 - dx

			wx = wx * (m / 250)

			if(tx <= rx) then
				tx = tx - wx
			else
				tx = tx + wx
			end
		end
	end

	p_drawImage(sq.img.idata, tx, ty, sq.a, sq.r, sq.px, sq.py, sq.sx, sq.sy);

end


mrg = sw * 0.07;
cx = mrg
cy = mrg * 2;
text = {};
for i = 1, 400 do

	local n = (i - 1) % 3
	local sq = mkSquid(chars[1 + n])

	sq.x = cx;
	sq.y = cy;
	cx = cx + 24; --2 + sq.img.w + 2;
	if(cx >= sw - mrg) then
		cy = cy + 45;
		cx = mrg;
	end

	sq.draw = charDraw

	sq.kill = function()
		sq.rv = 0.3;
		if(sq.id % 2 == 0) then
			sq.vx, sq.vy = 2, 2
		else
			sq.vx, sq.vy = -2, -2
		end
		sq.a = 0.3;
		sq.gy = 0.3
	end

	text[i] = sq;
end


img = getImage("fish.png");
fish = mkSquid(img, 100, 100)
fish.x = sw + img.w;
fish.y = sh * 0.65;
fish.vx = -1.5;
fish.vy = 0.0;
fish.sx = 1.0;
fish.sy = 1.0;
fish.rv = 0.0;
fish.tick = function(self)
	newton(self);
	wrap(self);
end


rocket = mkSquid(getImage("rocket.png"));
ground = sh - (rocket.img.h / 2);
rocket.x = sw / 2;
rocket.y = ground - 1;
rocket.sx = 1.0;
rocket.sy = 1.0;
rocket.vy = 0.0;
rocket.gy = 0;
rocket.vy = -5;
rocket.tick = function(self)

	if(rocket.y >= ground) then
		rocket.y = ground;
		rocket.vy = 0;
		rocket.gy = 0;
	else
		rocket.gy = 0.7;
		newton(self)
	end

end




shotImg = getImage('shot.png')
shots = {}


fishSet = makeSoundSet("pop2_wav.wav", 5);

play(getSound('startup.wav'));


function shotTick(sq)
	newton(sq)
	life = life - 1
	if(life <= 0) then
		shots[sq.idx] = nil;
	end

	local x, y = sq.x, sq.y
	for n = 1, #text do
		local txt = text[n]
		if(hit(txt, x, y)) then
			txt:kill();
			shots[sq.idx] = nil;
			return;
		end
	end
end

function shoot(tx, ty) 
	local sq = mkSquid(shotImg, rocket.x, rocket.y)
	local dx,dy = tx - sq.x, ty - sq.y	-- distance to target
	--sq.vx = (tx - sq.x) / 10
	sq.vy = -20 --(ty - sq.y) / 10
	life = 20;
	sq.tick = shotTick
	sq.idx = #shots + 1
	shots[sq.idx] = sq
end





function a_tick() 

	for i = 1, #text do text[i]:tick() end

	for i = 1, #shots do
		if(shots[i] ~= nil) then
			shots[i]:tick()
		end
	end

	fish:tick()

	rocket:tick();

end


bsx = sw / bg.w 
bsy = sh / bg.h 
function a_draw() 

    p_drawImage(bg.idata, 0, 0, 1.0, 0, 0, 0, bsx, bsy);

	for i = 1, #text do text[i]:draw() end

	for i = 1, #shots do
		if(shots[i] ~= nil) then
			shots[i]:draw()
		end
	end

	rocket:draw();

	fish:draw()

    p_drawImage(reset.idata, 5, 30, 1.0, 0, 0, 0, 1, 1);

end



function a_event(e, x, y)
    dbg("click " .. x .. "," .. y);

	if(hit(fish, x, y)) then
		fish.vx = fish.vx * -1
		fish.sx = fish.sx * -1.0;
		fishSet:play(0.2);
	elseif(hit(rocket, x, y)) then
		dbg("hit rocket")
		rocket.vy = rocket.vy - 5;
		if(rocket.y >= ground) then
			rocket.vy = rocket.vy - 5;
			rocket.y = rocket.y - 1;
		end
	else
		shoot(x, y)
	end
end


