
-------------------------------------------------------
-- misc -----------------------------------------------
-------------------------------------------------------

-- print to log view and/or xcode console
function dbg(s)
	p_log(s)
end


-- return a number between 0 and n -1 inclusive 
function roll(n)
	return math.random(0, n - 1)
end

-- return true or false randomly
function coinFlip()
	return roll(1) == 0
end


-- return a unique integer
seqNum = 0;
function seq()
	seqNum = seqNum + 1
	return seqNum
end


function Stack()
	local pile = {}
	pile.push = function(t, o) 
		table.insert(t, o)
	end
	pile.pop = function(t) 
		return table.remove(t)
	end
	return pile
end


function Queue()
	local pile = {}
	pile.put = function(t, o) 
		table.insert(t, o)
	end
	pile.get = function(t) 
		return table.remove(t, 1)
	end
	pile.clear = function(t) 
		while table.remove(t, 1) ~= nil do
		end
	end
	return pile
end


-- duplicate object via a shallow copy
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


-------------------------------------------------------
-- Some math stuff
-------------------------------------------------------
sqrt = math.sqrt
sin = math.sin
cos = math.cos
floor = math.floor
abs = math.abs
atan2 = math.atan2
PI = math.pi
PI2 = PI * 2
PIH = PI / 2

-- return a number from 0.0 thru PI * for use as rot value
function azimuth(sx, sy, tx, ty)
    return atan2(ty - sy, tx - sx) + PIH
end

-- convert heading/angle to cartesion coords for distance 1.0
function cartes(az)
	az = az - PI
	return sin(az), -cos(az)
end



-------------------------------------------------------
-- sound
-------------------------------------------------------

-- play a sound with a volume (0.0 thru 1.0)
function play(snd, volume) 
	volume = volume or 1.0
	p_playSound(snd, volume);
end

function makeSoundSet(file, num)
	if num == nil then num = 1; end
	local data = p_loadData(file);
	local pf = function(self, v)
			local n = self.idx;
			play(self.snds[n], v);
			n = n + 1;
			if(n > #(self.snds)) then
				n = 1;
			end
			self.idx = n;
		end
	local set = {idx = 1, play = pf, snds = {} }
	for i = 1, num do
		local snd = p_makeSound(data)
		if snd then
			set.snds[i] = snd
		end
	end
	return set;
end


-------------------------------------------------------
-- gfx
-------------------------------------------------------

function getImage(file)
	local data = p_loadData(file)
	local i, width, height = p_makeImage(data)
	dbg( file .. ": " .. width .. "," .. height )
	--return { idata = i, w = width, h = height, x = 0, y = 0, r = 0.0 }
	return { idata = i, w = width, h = height }
end


function getFont(file, cw, ch)
	local img = getImage(file);
	local font = p_makeFont(img.idata);
	return font
end


-----------------------------------------------
-- squiddy stuff ------------------------------
-----------------------------------------------


function makeSquidSet(fCreate, num)
	if num == nil then
		num = 1
	end

	-- take out and return the next available squid from the set
	local get = function(self)
		local n = self.n;
		local sq = self.squids[n];
		if sq.alive then
			return nil
		end
		n = n + 1;
		if(n > #(self.squids)) then
			n = 1;
		end
		self.n = n
		return sq
	end

	-- tick all squids in the set
	local tick = function(self) 
		for i = 1, #self.squids do
			local sq = self.squids[i]
			if sq.alive then
				sq:tick()
			end
		end
	end

	-- draw all squids in the set
	local draw = function(self) 
		for i = 1, #self.squids do
			local sq = self.squids[i]
			if sq.alive then
				sq:draw()
			end
		end
	end

	-- kill all squids in the set
	local kill = function(self)
		for i = 1, #self.squids do
			self.squids[i].alive = false
		end
	end

	local set = { n = 1, get = get, squids = {}, tick = tick, draw = draw, kill = kill }

	for i = 1, num do
		set.squids[i] = fCreate();
	end

	return set;
end


function makeImgSet(base) 
	local set = {}
	for i = 1, 50 do 
		local f = base .. "-" .. i .. ".png"
		local img = getImage(f)
		if img.w == 0 then		-- xxx
			break
		end
		table.insert(set, img)
	end
	return set
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
	sq.r = sq.r + sq.vr 
	if(sq.r > PI2) then sq.r = sq.r - PI2 end
	if(sq.r < 0) then sq.r = sq.r + PI2 end
	sq.vr = sq.vr + sq.ra
	sq.vr = sq.vr - sq.rf

end

function screenWrap(sq)
	if(sq.x > sw + (sq.img.w / 2)) then sq.x = 0 end
	if(sq.x < 0) then sq.x = sw - (sq.img.w / 2) - 1 end
	if(sq.y > sh + (sq.img.h / 2)) then sq.y = 0 end
	if(sq.y < 0) then sq.y = sh + (sq.img.h / 2) - 1 end
end


quakex = 0
quakey = 0

function drawImage(idata, x, y, a, r, px, py, sx, sy)

	p_drawImage(idata, x + quakex, y + quakey, a, r, px, py, sx, sy);

end
function drawText(s, x, y, f, a)
	if f then
		p_drawText(s, x + quakex, y + quakey, f, a)
	--else
	--	dbg("****** drawText w/no font?");
	end
end


function addQuake(x, y)
	quakex = quakex + x;
	quakey = quakey + y;
end


-- just draws a squid with it's upper left corner at its x,y 
function justDraw(sq)
	if(sq.alive and sq.img) then
		drawImage(sq.img.idata, sq.x, sq.y, sq.a, sq.r, sq.px, sq.py, sq.sx, sq.sy);
	end
end


-- return a random enemy creation point
function randomPlace() 
	local x = roll(51) - 25
	if x > 0 then
		x = x + sw
	end
	local y = roll(51) - 25
	if y > 0 then
		y = y + sh
	end
	return x, y
end


-- this is the basic squid object
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
	vr = 0,				-- rotational velocity
	ra = 0,				-- rotational acceleration
	rf = 0,				-- rotational friction
	px = 0, py = 0,		-- pivot point

	a = 1.0,			-- alpha

	radius = 0,			-- for circular collisions

	alive = false,


	-- replaceable tick function
	tick = function(sq) newton(sq) end,

	-- replaceable draw function
	draw = function(sq) justDraw(sq) end,

}


-- return true if two squids, have overlapping radii
function hitting(sq1, sq2)
	if(sq1.alive and sq2.alive) then
		local rHit = (sq1.radius * sq1.radius) + (sq2.radius * sq2.radius)
		local xx = math.abs(sq2.x - sq1.x)
		local yy = math.abs(sq2.y - sq1.y)
		local rDist = (xx * xx) + (yy * yy)
		return rDist < rHit
	end
	return false
end



-- create a simple, basic squid with an image, and a position
function makeSquid(img, x, y, pct)
	local o = clone(ProtoSquid);

	o.id = seq()
	o.name = "Squid-" .. o.id

	o.img = nil
	if img ~= nil then
		o.img = img
		o.px = img.w / 2
		o.py = img.h / 2

		if pct ~= nil then
			scaleAgainstScreen(o, pct)
		end

	end

	o.x, o.y = x or 0, y or 0;

	return o;
end


-- return true if hx, hy is within the rect of a squids image.
function hitTest(sq, hx, hy)
	local x, y = sq.x, sq.y
	local img = sq.img
	if(img == nil) then return false; end
	local w2 = img.w / 2
	local h2 = img.h / 2
	if(hx < x - w2) then return false; end
	if(hx > x + w2) then return false; end
	if(hy < y - h2) then return false; end
	if(hy > y + h2) then return false; end
	return true
end



-----------------------------------------------
-- scaling support
-----------------------------------------------

function scaled(n) 
	return n * g_space
end

g_sw, g_sh = p_getEnv('screenWidth'), p_getEnv('screenHeight')
g_space = sqrt((g_sw * g_sw) + (g_sh * g_sh))

function scaleAgainstScreen(sq, pct)
	local w, h = sq.img.w, sq.img.h
	local imgDiag = sqrt( (w * w) + (h * h) )
	local n = g_space * pct
	local scl = n / imgDiag
	sq.sx, sq.sy = scl, scl
	return scl
end



-----------------------------------------------
-- Code for this particular game
-- everything above here is basically candidate for "lib" code of some sort
-----------------------------------------------


appStart = os.time()
gloop = 0
loop = 0

sw, sh = g_sw, g_sh
dbg("screen: " .. sw .. "," .. sh .. " " .. g_space)



font = getFont("font-small-green.png", 8, 14);
fontTmr = getFont("font-big-green.png", 20, 40);
fontMenu = getFont("font-small.png", 8, 14);
fontBtns = getFont("font-big.png", 20, 40);

bg = makeSquid(getImage("background1-grid.png"), 0, 0);
bg.sx = (sw * 1.1) / bg.img.w;
bg.sy = (sh * 1.1) / bg.img.h;
bg.x = (sw * 0.05) * -1
bg.y = (sh * 0.05) * -1


fader = makeSquid(getImage("fadeglass.png"), 0, 0);
fader.va = 0
fader.a = 0.0
fader.x = 0
fader.y = 0
fader.px = 0
fader.py = 0
fader.sx = sw / fader.img.w;
fader.sy = sh / fader.img.h;
fader.tick = function(sq)
	if sq.alive then

		--newton(sq)

		sq.a = sq.a + sq.va

		if sq.a >= 1.0 then
			if sq.pause > 0 then
				sq.pause = sq.pause - 1
				if sq.pause <= 0 then
					sq.va = -0.10
					if sq.doneFunc then
						sq.doneFunc()
					end
				end
			end
		end

		if sq.a <= 0.0 then
			sq.alive = false
		end

	end
end
fader.fadeThen = function(sq, func, pause, speedOut) 
	fader.a = 0.0
	if speedOut then
		sq.va = speedOut
	else
		sq.va = 0.10
	end
	sq.doneFunc = func
	sq.pause = 0
	if pause > 0 then
		sq.pause = pause
	end
	sq.alive = true
end





menu_btn = getImage("menu-button.png")
menu_btn_lit = getImage("menu-button-lit.png")

function mkButton(label, x, y, fn)

	sq = makeSquid(menu_btn, x, y)

	sq.pause = 0
	sq.clickFunc = fn

	sq.sx = 0.5; --menu_btn.w / bg.img.w
	sq.sy = 0.5; --menu_btn.w / bg.img.w

	sq.dim = menu_btn
	sq.lit = menu_btn_lit

	sq.lbl = label

	sq.hit = function(sq, x, y)
		local sx = sq.sx
		local sy = sq.sy
		local w = sq.img.w * sx
		local h = sq.img.h * sy
		local w2 = w * 0.5;
		local h2 = h * 0.5;
		if x < sq.x - w2 then return false end
		if y < sq.y - h2 then return false end
		if x > sq.x + w2 then return false end
		if y > sq.y + h2 then return false end
		return true
	end

	sq.click = function(sq) 
		if sq.pause > 0 then return end		-- bit o' the olde debounce!
		sq.img = sq.lit
		sq.pause = 10
		clickSet:play(0.5);
	end

	sq.tick = function(sq) 
		if sq.alive == false then return end
		newton(sq)
		if sq.pause > 0 then
			sq.pause = sq.pause - 1
			if sq.pause == 5 then
				sq.img = sq.dim
			end
			if sq.pause == 0 then
				if sq.clickFunc then
					sq.clickFunc()
				end
			end
		end
	end

	sq.draw = function(sq)
		if sq.alive == false then return end
		drawImage(sq.img.idata, sq.x, sq.y, sq.a, sq.r, sq.px, sq.py, sq.sx, sq.sy);
		drawText(sq.lbl, sq.x, sq.y - 14, fontBtns, 1.0);
	end

	return sq
end



function playGame()
	dbg('playGame');
	menu.alive = false
	tryAgain()
end

btnPlay = mkButton("Play", sw * 0.6, sh * 0.3, playGame)


function showIntro()
	dbg('showIntro');
	menu.alive = false

	--introScreens:put(makeIntroScreen("intro0.png"))
	introScreens:put(makeIntroScreen("intro1.png"))
	introScreens:put(makeIntroScreen("intro2.png"))
	intro = introScreens:get()
end

btnIntro = mkButton("Intro", sw * 0.6, sh * 0.43, showIntro)



menu = makeSquid(nil, 0, 0);
menu.px = 0;
menu.py = 0;
menu_bg = getImage("menu-bg.png")
menu.sx = sw / bg.img.w;
menu.sy = sh / bg.img.h;
menu.tick = function()
	if menu.alive == false then return end
	if menu.a < 1.0 then
		menu.a = menu.a + 0.2		-- quick fade in
	end
end
menu.draw = function()
	if menu.alive == false then return end
	drawImage(menu_bg.idata, 0, 0, menu.a, 0.0, 0, 0, menu.sx, menu.sy);
	drawText("SURVIVAL TIME TO BEAT", sw * 0.39, sh * 0.1, fontMenu, 1.0);
	drawText(secs2time(highScore), sw * 0.42, sh * 0.15, fontBtns, 1.0);
end
function showMenu()
	dbg("showMenu")
	menu.a = 0.0
	btnPlay.alive = true;
	btnIntro.alive = true;
	menu.alive = true
end




testFirings = 2;


introScreens = Queue()
introWait = 0

function introTick(sq)
	newton(sq)

	if introWait > 0 then
		introWait = introWait - 1
		if introWait <= 0 then
			-- next screen
			intro.gy = -2
		end
		return
	end

	if sq.vy == 0 then
		-- stationary == "fading in"
		if sq.a < 1.0 then
			sq.a = sq.a + 0.05
		else
			introWait = 300
		end
	else
		-- moving == "sliding offscreen"
		sq.a = sq.a - 0.05
		if sq.a <= 0 then
			sq.alive = false
			intro = introScreens:get()
			if intro == nil then
				showMenu()
			end
		end
	end
end
function makeIntroScreen(img) 
	local sq = makeSquid(getImage(img), 0, 0)
	sq.px = 0;
	sq.py = 0;
	sq.a = 0
	sq.sx = sw / sq.img.w;
	sq.sy = sh / sq.img.h;
	sq.tick = introTick
	sq.alive = true
	return sq
end

introScreens:put(makeIntroScreen("intro0.png"))
introScreens:put(makeIntroScreen("intro1.png"))
introScreens:put(makeIntroScreen("intro2.png"))

intro = introScreens:get()



function tickSmoke(sq)
	sq.sx = sq.sx + 0.020
	sq.sy = sq.sy + 0.020
	sq.a = sq.a - 0.01
	if sq.a <= 0 then
		sq.alive = false;
	end
end
imgSmoke = getImage("smoke.png");
function newSmoke()
	local sq = makeSquid(imgSmoke)
	sq.tick = tickSmoke
	return sq
end
smokeSet = makeSquidSet(newSmoke, 8)
function makeSmoke(x, y, a)
	local sq = smokeSet:get()
	if sq then
		sq.x = x
		sq.y = y
		sq.sx = 0.1
		sq.sy = 0.1
		sq.a = a
		sq.alive = true;
	end
end




function tickRedBoom(sq)
	sq.sx = sq.sx + 0.04
	sq.sy = sq.sy + 0.04
	sq.a = sq.a - 0.006
	if sq.a <= 0 then
		sq.alive = false;
	end
end
imgRedBoom = getImage("red-boom.png");
function newRedBoom()
	local sq = makeSquid(imgRedBoom)
	sq.tick = tickRedBoom
	return sq
end
redBoomSet = makeSquidSet(newRedBoom, 4)
function makeRedBoom(x, y, a)
	local sq = redBoomSet:get()
	if sq then
		sq.x = x
		sq.y = y
		sq.sx = 0.1
		sq.sy = 0.1
		sq.a = a
		sq.alive = true;
	end
end



function tickDebris(sq)
	newton(sq)

	if(sq.smoker) then
		if loop % 15 == 0 then
			makeSmoke(sq.x, sq.y, sq.a)
		end
	end

	sq.a = sq.a - 0.02
	if sq.a <= 0 then
		sq.alive = false;
	end

end
imgSetDebris = makeImgSet("debris");
function newDebris()
	local sq = makeSquid()
	sq.tick = tickDebris
	return sq
end
debrisSet = makeSquidSet(newDebris, 8)
function makeDebris(osq) --x, y, vx, vy)
	local sq = debrisSet:get()
	if sq then
		local img = imgSetDebris[roll(#imgSetDebris) + 1]
		sq.img = img
		sq.px = img.w / 2
		sq.py = img.h / 2
		sq.x = osq.x
		sq.y = osq.y
		sq.vx = osq.vx + ((roll(11) - 5) * 0.5)
		sq.vy = osq.vy + ((roll(11) - 5) * 0.5)
		sq.sx = roll(5) * 0.1
		sq.sy = roll(5) * 0.1
		sq.a = 1.0
		sq.vr = 0.1
		sq.alive = true;
		sq.smoker = roll(5) == 0
	end
end



turScale = 0.15

turretBase = makeSquid(getImage("base.png"), sw * 0.5, sh * 0.5, turScale);
turretBase.alive = true;


gun = makeSquid(getImage("base-star-tesla-cannon-default.png"), sw * 0.5, sh * 0.5, turScale);
gun.alive = true;
gun.imgNorm = gun.img;
gun.imgFire = getImage("base-star-tesla-cannon-firing2.png");
gun.draw = function(sq) 
	if(sq.alive and sq.img) then
		local ox, oy = cartes(turret.r)
		local dx, dy = sq.x + (ox * sq.recoil * 1.0), sq.y + (oy * sq.recoil * 1.0)
		drawImage(sq.img.idata, dx, dy, sq.a, turret.r, sq.px, sq.py, sq.sx, sq.sy);
	end
end
gun.recoil = 0;
gun.recoilv = 0;
gun.tick = function(sq)
	newton(sq)
	sq.recoil = sq.recoil + sq.recoilv
	sq.recoilv = sq.recoilv - 1
	if sq.recoil <= 0 then
		sq.recoil = 0
		sq.recoilv = 0
	end

	sq.heat = sq.heat - 1			-- cool down a little

end

gun.heat = 0
gun.fire = function(sq) 

	gun.heat = gun.heat + 10		-- gun can overheat and stop working

	if sq.recoil == 0 then
		--if gun.heat < 20 then
			if spawnShot(turret.tgtx, turret.tgty) then

				sq.recoilv = 3

				-- show the firing images briefly
				turret.imgTick = 4;

				turret.img = turret.imgFire
				gun.img = gun.imgFire

			end
		--else
		--	clickSet:play();
		--end
	end
end


turretRotateSet = makeSoundSet("mechanicaldoor.wav", 3);

turret = makeSquid(getImage("base-star-default.png"), sw * 0.5, sh * 0.5, turScale);
turret.imgNorm = turret.img;
turret.imgFire = getImage("base-star-firing.png");
turret.rc = 0
turret.imgTick = 0
trspd = 0.20
turret.radius = 25
turret.alive = true;
turret.tgtx = 0;
turret.tgty = 0;
turret.dying = 0;


turret.draw = function(sq) 
	justDraw(sq)
	gun:draw()
end

turret.tick = function(sq)

	newton(sq)

	if sq.alive == false then
		if sq.dying > 0 then
			if sq.dying > 50 and loop % 10 == 0 then
				makeRedBoom(sq.x + (roll(51) - 25), sq.y + (roll(51) - 25), 1)
			end
			sq.dying = sq.dying - 1
		end
		return
	end


	turretBase.r = PI2 - sq.r

	sq.vx = sq.vx * 0.9
	sq.vy = sq.vy * 0.9

	if(sq.imgTick > 0) then
		sq.imgTick = sq.imgTick - 1
		if(sq.imgTick <= 0) then
			sq.img = turret.imgNorm
			gun.img = gun.imgNorm
		end
	end


	if(sq.rc > 0) then
		-- turret is rotating
		sq.rc = sq.rc - 1
		if(sq.rc <= 0) then
			-- turret has reached firing azimuth
			
			gun:fire()

			sq.vr = 0		-- stop the rotation
			sq.r = sq.tgtAz

		end
	else
		-- turret is not rotating

		if gun.recoil == 0 then
			local o = shotQueue:get()
			if o ~= nil then
				-- a shot is in the queue

				-- note coordinates of where shot is destined
				sq.tgtx = o.x
				sq.tgty = o.y

				-- compute # of ticks needed to rotate to face shot dest
				local curAz = turret.r
				sq.tgtAz = azimuth(sq.x, sq.y, o.x, o.y)
				local diffAz = sq.tgtAz - curAz
				if(diffAz > PI2) then diffAz = diffAz - PI2 end
				if(diffAz < 0) then diffAz = diffAz + PI2 end

				-- start the turret rotating toward target
				if(diffAz == 0) then
					gun:fire()
				else
					turretRotateSet:play(0.3);
					if(diffAz < PI) then
						sq.vr = trspd
						sq.rc = abs(diffAz / trspd);
					else
						sq.vr = -trspd
						sq.rc = abs((PI2 - diffAz) / trspd);
					end
				end

			end
		end
	end

end

function youLose(hitter)

	dbg("game over")


	if score > highScore then
		store("highScore", ""..score)
		highScore = score
		dbg("high score!");
	end


	spawnBoom(turret)

	for i = 1, 7 do
		makeDebris(turret)
	end

	turrHitSet:play(0.2)
	turret.dying = 100
	turret.alive = false
	gun.alive = false

	shotQueue:clear()


	addQuake(12, 20);

	gameOver = true;

	--turretBase.vr = (roll(7) - 3) * 0.05
	turretBase.vr = turret.vr * (roll(5) / 10)
	turretBase.vx = hitter.vx * 0.05; --(roll(5) - 3) * 0.1
	turretBase.vy = hitter.vy * 0.05; --(roll(5) - 3) * 0.1

	tango.reset()

	fader:fadeThen(showMenu, 1, 0.0005)

end


clickSet = makeSoundSet("draw.wav", 3);
thumpSet = makeSoundSet("thump.wav", 3);
torpBoomSet = makeSoundSet("explode3.wav", 5);
turrHitSet = makeSoundSet("explode2.wav", 5);



boomSet = makeSoundSet("explode1.wav", 5);

boomAnim = {}
for i = 1, 25 do
	boomAnim[i] = getImage( "explosion1/" .. i .. ".png" );
end

MAX_BOOMS = 8
nextBoom = 1
booms = {}
function boomTick(sq)
	if sq.alive then
		newton(sq);
		sq.frm = sq.frm + 1;
		if sq.frm > #sq.anim * sq.slow then
			sq.alive = false;
			-- xxx debris ?
			return
		end
		sq.img = sq.anim[floor(sq.frm / sq.slow) + 1]

		sq.radius = sq.radius + 3

	end
end
for i = 1, MAX_BOOMS do
	local sq = makeSquid(boomAnim[1])
	sq.tick = boomTick
	sq.slow = 1
	sq.anim = boomAnim
	sq.frm = 1
	sq.alive = false
	booms[ #booms + 1 ] = sq
end
function spawnBoom(osq)
	local sq = booms[nextBoom];
	if sq.alive then
		-- clickSet:play();
	else
		sq.x = osq.x
		sq.y = osq.y
		sq.frm = 1
		sq.alive = true
		sq.radius = 4

		addQuake(0.5, 0.5);

		nextBoom = nextBoom + 1
		if nextBoom > MAX_BOOMS then
			nextBoom = 1
		end

	end
end

function each(t, fn)
	for i, boom in ipairs(booms) do
		if fn(boom) then break end
	end
end

function hitABoom(sq)

	local h = false
	each(booms, function(boom)
		if boom.alive then
			if hitting(sq, boom) then
				h = true
				return true	-- break loop
			end
		end
	end)
	return h

	--[[
	for booms do
	end
	for i,v in ipairs(booms) do
	end
	for i = 1, #booms do
		local boom = booms[i]
		if boom.alive then
			if hitting(sq, boom) then
			end
		end
	end
	return false
	]]
end




-------------------------------------------------
-- kryptos - the origina alien ships
-------------------------------------------------
kryptoSet = makeSoundSet("s_yabidl.wav", 8);

imgKrypto = getImage("green-alien-ship.png");
kryptoAnim = {}
for i = 1, 1 do
	kryptoAnim[i] = imgKrypto
end

MAX_KRYPTOS = 10
nextKrypto = 1
kryptos = {}
function kryptoTick(sq)
	if sq.alive then

		newton(sq);

		if sq.frm > #sq.anim * sq.slow then
			sq.frm = 1
		end
		sq.img = sq.anim[floor(sq.frm / sq.slow) + 1]

		for i = 1, #booms do
			local boom = booms[i]
			if boom.alive then
				if hitting(sq, boom) then
					spawnBoom(sq) -- xxx 

					for i = 1, 7 do
						makeDebris(sq)
					end

					sq.alive = false
					boomSet:play(0.2);
					break
				end
			end
		end

		if hitting(sq, turret) then
			sq.alive = false;
			youLose(sq);

		end

	end
end
for i = 1, MAX_KRYPTOS do
	local sq = makeSquid(kryptoAnim[1], 0, 0, 0.04)
	sq.tick = kryptoTick
	sq.slow = 4
	sq.anim = kryptoAnim
	sq.frm = 1
	sq.alive = false
	sq.radius = 18
	kryptos[ #kryptos + 1 ] = sq
end
function spawnKrypto()

	if testFirings > 0 then
		return
	end

	local sq = kryptos[nextKrypto];
	if sq.alive then
		-- clickSet:play();
	else
		local x = roll(sw * 2)
		if x < sw + 20 then
			x = (x * -1) - 20
		end
		local y = roll(sh * 2)
		if y < sh + 20 then
			y = (y * -1) - 20
		end
		sq.x = x
		sq.y = y
		--sq.sx = 0.4
		--sq.sy = 0.4
		sq.frm = 1

		local dx = turret.x - x
		local dy = turret.y - y
		sq.vx = dx / 500
		sq.vy = dy / 500

		sq.r = azimuth(turret.x, turret.y, x, y)

		sq.alive = true

		kryptoSet:play();

		nextKrypto = nextKrypto + 1
		if nextKrypto > MAX_KRYPTOS then
			nextKrypto = 1
		end
	end
end



-------------------------------------------------
-- medusa
-------------------------------------------------
medusaImg = getImage("yellow-alien-fighter.png")	-- xxx replace with anim loop at some point
medusa = {

	-- create a pool of available squids
	pool = makeSquidSet( function()
		local sq = makeSquid(medusaImg, 0, 0)

		-- modify physical size of squid by amt
		sq.grow = function(sq, amt) 
			if sq.sx < 1.0 and sq.sy < 1.0 then
				sq.sx = sq.sx + amt
				sq.sy = sq.sy + amt
			end
		end

		-- return true if squid is capable of reproducing
		sq.fertile = function(sq)
			return sq.gen < 2
		end

		-- explode and die
		sq.die = function(sq)
			sq.alive = false
			spawnBoom(sq)
			for i = 1, 7 do
				makeDebris(sq)
			end
			boomSet:play(0.2);
		end

		sq.tick = function(sq)

			newton(sq)
			screenWrap(sq)

			-- get older
			sq.age = sq.age + 1
			if sq.age % 300 == 0 then
				sq:grow(0.1)				-- get bigger
				if sq:fertile() then
					sq.pregnant = true		-- get pregnant
				else
					sq.gen = sq.gen - 1		-- get fertile
				end
			end

			
			if sq.pregnant == true then
				if abs(sq.vr) < 0.8 then
					sq.vr = sq.vr * 1.10	-- spin faster
				else
					-- reached max spin rate; give birth
					sq.pregnant = false
					sq.vr = 0;
					medusa.spawn(sq) 
				end
			else
				-- random wandering
				if sq.age % 40 == 0 then
					sq.vr = sq.vr + (roll(7) - 3) * 0.02	-- alter turning speed
				end
				if sq.age % 5 == 0 then
					sq.vr = sq.vr * 0.9				-- reduce turning speed a bit
				end
			end

			-- set x,y velocities based on azimuth
			local ox, oy = cartes(sq.r)
			sq.vx = ox * 2.5
			sq.vy = oy * 2.5


			if hitABoom(sq) then
				-- collided with an explosion; blow up and die
				sq:die()
				return
			end

			if hitting(sq, turret) then
				-- collided with turret
				sq:die()
				youLose(sq);		-- game over
			end

		end

		return sq
	end, 12 ),

	spawn = function(mom)

		-- medusa
		if wave < 3 then
			return nil
		end

		-- defaults for first generation squids (if mom == nil)
		local gen = 0
		local x, y = randomPlace()
		local scl = 0.4;

		if mom ~= nil then
			if mom:fertile() == false then return nil end -- the line of descent dies out
			gen = mom.gen + 1
			scl = scl - (gen * 0.1)
			x = mom.x
			y = mom.y
		end

		local sq = medusa.pool:get(); 
		if sq == nil then return nil end	-- max medusas already living

		-- breathe life into child
		sq.age = 0
		sq.gen = gen
		sq.x = x
		sq.y = y
		sq.birth = loop
		sq.vr = 0
		sq.sx = scl
		sq.sy = scl
		if mom ~= nil then
			-- shrink the mother 
			mom.sx = scl
			mom.sy = scl
		end
		sq.pregnant = false
		sq.alive = true

		return sq
	end,
}




-------------------------------------------------
-- red devil
-------------------------------------------------
imgDevil = getImage("red-alien-fighter.png");
tickDevil = function(sq)
	if sq.alive then
		newton(sq)
		screenWrap(sq)
		local ox, oy = cartes(sq.r)
		sq.vx = ox * 4.5
		sq.vy = oy * 4.5
		if loop % 40 == 0 then
			sq.vr = sq.vr + (roll(7) - 3) * 0.05
		end
		if loop % 10 == 0 then
			sq.vr = sq.vr * 0.9
		end

		for i = 1, #booms do
			local boom = booms[i]
			if boom.alive then
				if hitting(sq, boom) then
					sq.alive = false
					spawnBoom(sq) -- xxx 
					for i = 1, 7 do
						makeDebris(sq)
					end
					boomSet:play(0.2);
					break
				end
			end
		end

		if hitting(sq, turret) then
			sq.alive = false
			youLose(sq);
		end
	end
end
function spawnDevil()

	if wave < 2 then
		return nil
	end

	if testFirings > 0 then
		return 
	end

	if devil ~= nil and devil.alive then
		return
	end
	local x = roll(51) - 25
	if x > 0 then
		x = x + sw
	end
	local y = roll(51) - 25
	if y > 0 then
		y = y + sh
	end
	devil = makeSquid(imgDevil, x, y)
	devil.tick = tickDevil
	devil.vr = 0
	devil.alive = true
	devil.sx = 0.5
	devil.sy = 0.5
	--dbg('devilspawn @'..x..","..y)
end
--spawnDevil()









shotAnim = {}
for i = 1, 1 do
	shotAnim[i] = getImage("torpedo/blue-torpedo-frame" .. i .. ".png");
end

MAX_SHOTS = 4
nextShot = 1
shots = {}
function shotTick(sq)
	if sq.alive then
		newton(sq);

		sq.frm = sq.frm + 1;
		if sq.frm > #sq.anim * sq.slow then
			sq.frm = 1
		end
		sq.img = sq.anim[floor(sq.frm / sq.slow) + 1]

		sq.fuse = sq.fuse - 1
		if sq.fuse <= 0 then
			spawnBoom(sq) -- xxx 
			sq.alive = false	-- xxx debris
			torpBoomSet:play(0.5);
		end

		--[[sq.steps = sq.steps - 1
		if sq.steps <= 0 then
			spawnBoom(sq) -- xxx 
			sq.alive = false	-- xxx debris
			torpBoomSet:play(0.5);
		end
		]]
	end
end
for i = 1, MAX_SHOTS do
	local sq = makeSquid(shotAnim[1])
	sq.tick = shotTick
	sq.slow = 4
	sq.anim = shotAnim
	sq.frm = 1
	sq.alive = false
	sq.radius = 0
	sq.sx = g_space * 0.0006
	sq.sy = g_space * 0.0006
	sq.vr = 0.2
	shots[ #shots + 1 ] = sq
end
function spawnShot(x, y) 

	local sq = shots[nextShot];
	if sq.alive then
		-- clickSet:play();
		return false
	end

	-- allocated a shot

	thumpSet:play(0.2);

	-- place shot at dead center of turret
	sq.x = turret.x;
	sq.y = turret.y;

	-- find distance from shot to its destination
	local xd = x - sq.x
	local yd = y - sq.y
	local hd = sqrt((xd * xd) + (yd * yd))		-- absolute distance to target

	local v = (g_space * 0.01)	-- torpedo absolute velocity
	local fuse = hd / v			-- torpedo explodes after this # of ticks


	local r = azimuth(x, y, sq.x, sq.y)
	local ox, oy = cartes(r)
	sq.vx = ox * v
	sq.vy = oy * v

	-- adjust shot location from center of turret to location of gun muzzle
	sq.fuse = fuse - 5
	sq.x = sq.x + (sq.vx * 5)
	sq.y = sq.y + (sq.vy * 5)

	sq.alive = true

	nextShot = nextShot + 1
	if nextShot > MAX_SHOTS then
		nextShot = 1
	end

	return true

end


MAX_QUEUE_SHOTS = 4
shotQueue = Queue()
function shoot(x, y)
	if #shotQueue < MAX_QUEUE_SHOTS then
		shotQueue:put({x = x, y = y})
	else
		clickSet:play();
	end

	if testFirings > 0 then
		testFirings = testFirings - 1
	end
end


digits = {}
for i = 1, 10 do
	digits[i] = getImage("digit-" .. (i - 1) .. ".png");
end

sqGameOver = makeSquid(getImage("gameover.png"))
sqGameOver.x = sw * 0.5
sqGameOver.y = sh * 0.4
sqGameOver.aAdd = 0.1
sqGameOver.tick = function(sq)
	if sq.alive then
		newton(sq);
		sq.a = sq.a + sq.aAdd
		if(sq.a >= 1) then
			sq.aAdd = -0.01
		end
		if(sq.a <= 0) then
			sq.aAdd = 0.01
		end
	end
end


-- tango is army's friend
tango = makeSquid(nil, sw * 0.1, sh * 0.5);
tango.draw = function(sq)
	if tango.alive == false then return end
	if tango.msg == nil then return end
	drawText(tango.msg, tango.x, tango.y, font, 0.7);
end
tango.tick = function(sq) 
	if tango.alive == false then return end

	local t = loop - tango.start

	local msg = tango.msg;

	if t == 400 then
		msg = "TX 81X18 81X09 33200.0\n";
		msg = msg .. "81X18 DO YOU READ?\n";
	end
	if t == 600 then
		msg = msg .. "81X18 DO YOU READ?\n";
	end
	if t == 700 then
		msg = msg .. "ARMY, TANGO U SHOW OFFLINE?\n";
	end
	if t == 850 then
		msg = msg .. "OH GOD #@&% ARMY\n";
	end
	if t == 880 then
		msg = msg .. "81X18, TANGO CONDITION REPORT\n";
	end
	if t == 900 then
		msg = msg .. "#@&% MAN - NOT YOU TOO\n";
	end
	if t == 915 then
		msg = msg .. "ARMY, TANGO RESPOND\n";
	end
	if t == 930 then
		msg = msg .. "#@&% MAN NO.\n";
	end
	if t == 1000 then
		msg = msg .. "ARMY APOD DISPATCHED\n";
	end
	if t == 1100 then
		msg = msg .. "HANG ON ARMY\n";
	end
	if t == 1200 then
		msg = msg .. "DONT U F#%&N LEAVE ME MAN\n";
	end
	if t == 1300 then
		msg = msg .. "GO#@%&IT ARMY, TANGO RESPOND \n";
	end
	if t == 1400 then
		msg = msg .. "ARMY, TANGO RESPOND?\n";
	end
	if t == 1500 then
		msg = msg .. "ARMY, TANGO RESPOND?\n";
	end
	if t == 1600 then
		msg = msg .. "ARMY, TANGO RESPOND?\n";
	end
	if t == 1700 then
		msg = msg .. "ARMY, TANGO RESPOND?\n";
	end

	tango.msg = msg;

end
tango.reset = function() 
	tango.alive = true
	tango.start = loop
	tango.msg = nil
end


function secs2time(t)
	local secs = (t % 60)
	if(secs < 10) then secs = "0"..secs end
	t = floor(t / 60)
	local mins = t % 60
	if(mins < 10) then mins = "0"..mins end
	t = floor(t / 60)
	local hrs = t % 24
	if(hrs < 10) then hrs = "0"..hrs end
	local s = hrs..":"..mins..":"..secs
	return s
end


function store(key, val) 
	p_saveLocal(key, p_makeData(val))
end

function fetch(key, val) 
	return p_makeText(p_loadLocal(key))
end

utsStart = os.time()

highScore = tonumber(fetch('highScore')) or 0;
score = 0

gameOver = false;



--makeSoundSet('startup.wav'):play();

PRESSURE_RESET = 90
pressure = PRESSURE_RESET
wave = 1


function a_tick2() 

	gloop = gloop + 1		-- loops since app start

	loop = loop + 1			-- loops since play start

	if menu.alive then
		menu:tick()
		btnPlay:tick()
		btnIntro:tick()
		return
	end

	if fader.alive then
		fader:tick()
	end


	if intro then
		intro:tick();
		return
	end

	if gameOver then
		tango:tick();
	else
		score = os.time() - utsStart
	end

	sqGameOver.alive = gameOver;

	if loop == 300 then
		testFirings = 0;
	end

	bg:tick()
	turret:tick()
	turretBase:tick()
	gun:tick()
	for i = 1, #shots do shots[i]:tick() end
	for i = 1, #booms do booms[i]:tick() end
	for i = 1, #kryptos do kryptos[i]:tick() end
	smokeSet:tick();
	redBoomSet:tick();
	debrisSet:tick();

	if devil then
		devil:tick();
	end

	medusa.pool:tick();

	sqGameOver:tick();


	if loop % 100 == 0 then
		pressure = pressure - 1
		if pressure <= 10 then
			pressure = PRESSURE_RESET
			-- PRESSURE_RESET = PRESSURE_RESET + 20;
			wave = wave + 1
		end
	end

--
--if loop == 10 then medusa.spawn(); end

	if loop % pressure == 0 then
		if roll(5) == 0 then
			if roll(2) == 0 then
				medusa.spawn()
			else
				spawnDevil()
			end
		else
			spawnKrypto()
		end
	end

	quakex = (quakex * -1) * 0.9
	quakey = (quakey * -1) * 0.9

end


ticksPerDraw = 1
tickBacklog = 1


function a_tick() 

	if gloop == 0 then
		a_tick2()
		return
	end

	local elapsed = os.time() - appStart	-- # of secs elapsed since app start
	local ideal	= elapsed * 30				-- # of ticks we ought to have performed
	local actual = gloop 					-- # of ticks we've actually performed
	ticksPerDraw = ( ideal / gloop )		-- # ticks per draw we need to do (not integer)

	while tickBacklog >= 1 do
		a_tick2()
		tickBacklog = tickBacklog - 1
	end


	--[[a_tick2()	-- do one tick normal tick

	while true do
		local elapsed = os.time() - appStart		-- # of secs since app start
		local ideal	= elapsed * 30	-- # of ticks we've ideally performed
		local actual = gloop 		-- # of ticks we've actuall performed
		if actual < ideal then
			a_tick2()
		else
			break
		end
	end
	]]

	--if gloop % 300 == 0 then
	--dbg('ticksPerDraw='..ticksPerDraw)
	--end

	--[[local m = makeupTicks
	if m > 0 then
		while m > 0 do
			a_tick2()
			m = m - 1
		end
	end
	]]

end


function drawNumber(n, x, y)
	local dy = y
	local dx = x
	local i = n % 10
	local img = digits[i + 1]
	drawImage(img.idata, dx, dy, 1, 0, 0, 0, 0.5, 0.5);
	n = floor(n / 10)
	i = n % 10
	img = digits[i + 1]
	dx = dx - 20
	drawImage(img.idata, dx, dy, 1, 0, 0, 0, 0.5, 0.5);
end



function tryAgain() 
	dbg("tryAgain()");

	loop = 0

	for i = 1, MAX_KRYPTOS do
		kryptos[i].alive = false;
	end

	if devil then
		devil.alive = false
	end

	if medusa then
		medusa.pool:kill();
		--medusa.alive = false
	end

	-- make a turret.reset() func (same for many other squids)
	turret.rc = 0;
	turret.imgTick = 0
	turret.tgtx = 0;
	turret.tgty = 0;
	turret.dying = 0;
	turret.alive = true
	turret.vr = 0
	turret.img = turret.imgNorm;

	gun.vr = 0
	gun.img = gun.imgNorm;
	gun.alive = true
	utsStart = os.time()
	pressure = PRESSURE_RESET
	wave = 1
	turret.r = 0

	smokeSet:kill()
	redBoomSet:kill()
	debrisSet:kill()

	shotQueue:clear()

	turretBase.x =  sw * 0.5
	turretBase.y =  sh * 0.5
	turretBase.vx = 0;
	turretBase.vy = 0;
	turretBase.vr = 0;

	tango.alive = false

	gameOver = false;

	makeSoundSet('startup.wav'):play();

end



function a_draw() 

	tickBacklog = tickBacklog + ticksPerDraw

	if menu.alive then
		menu:draw()
		btnPlay:draw()
		btnIntro:draw()
		return
	end


	if intro then
		intro:draw();
		return
	end

	drawImage(bg.img.idata, bg.x, bg.y, 1.0, 0, 0, 0, bg.sx, bg.sy)


	turretBase:draw()
	turret:draw()

	for i = 1, #shots do shots[i]:draw() end
	for i = 1, #booms do booms[i]:draw() end
	for i = 1, #kryptos do kryptos[i]:draw() end

	if devil then
		devil:draw();
	end

	medusa.pool:draw()

	smokeSet:draw()
	redBoomSet:draw()
	debrisSet:draw()

	sqGameOver:draw()

	tango:draw();

	--drawTimer()
	drawText(secs2time(score), sw * 0.25, sh * 0.05, fontTmr, 0.7);

	--drawNumber(pressure, 30, sh * 0.95)

	fader:draw()


end




function a_event(e, x, y)

    --dbg("click " .. x .. "," .. y);

	if intro then
		intro.gy = -2;
		introWait = 0
		return
	end

	if menu.alive then
		if btnPlay:hit(x, y) then
			btnPlay:click()
			return
		end
		if btnIntro:hit(x, y) then
			btnIntro:click()
			return
		end
		return
	end

	if fader.alive then
		fader.alive = false
		showMenu()
		return
	end

	--[[if gameOver then
		if turret.dying == 0 then
			fader:fadeThen(tryAgain, 20)
		end
		return
	end]]


	shoot(x, y)

end



dbg("boot.lua finished executing");

