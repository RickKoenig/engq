function a_init()
	p_log("INIT");
	cnt = 0
	sx = p_getEnv("screenWidth")
	sy = p_getEnv("screenHeight")
	p_log("screen resolution is " .. sx .. " by " .. sy);
	root = p_getEnv("root");
	devcode = p_getEnv("devCode")
	p_log("root = " .. root)
	p_log("devcode = " .. devcode)

	-- data = p_loadData(root .. "/smallfont.png")
	data = p_loadData("smallfont.png")
	fi,sfw,sfh = p_makeImage(data)
	sgw = sfw/8
	sgh = sfh/16
	sfont = p_makeFont(fi)
	
	-- urldata = p_loadURL("http://upload.wikimedia.org/wikipedia/commons/thumb/7/77/White_Mountain_CA.JPG/640px-White_Mountain_CA.JPG")
	-- urldata = p_loadURL("http://23.123.140.18/take0018.jpg")
	urldata = p_loadData("pics/take0000.jpg")
	ui,ux,uy = p_makeImage(urldata)

	data = p_loadData("startup.wav")
	snd0 = p_makeSound(data)
	
	data = p_loadData("applause.wav")
	snd1 = p_makeSound(data)

	data = p_loadData("pop2_wav.wav")
	snd2 = p_makeSound(data)
	v = 1
	
end

function a_tick()
	-- p_log("TICK");
	-- if cnt == 200 then
	--	p_action("reset")
	-- end
	fps = p_getEnv("fps")
	if cnt % 100 == 0 then
		p_log("count = " .. cnt)
	end
	if cnt % 300 == 0 then
		p_playSound(snd0,v)
	end
	if cnt % 300 == 100 then
		p_playSound(snd1,v)
	end
	if cnt % 300 == 200 then
		p_playSound(snd2,v)
	end
	if cnt % 100 == 0 then
		v = v - .05
	end
	cnt = cnt + 1
end

function a_draw()
	-- p_log("DRAW");
	-- p_drawImage(fi,sx-fw/2,sy-fh/2,1,0,0,0,.5,.5)
	p_drawImage(ui,0,8,1,cnt*math.pi/100,0,0,.25,.25)
	for i=0,20 do
		p_drawText("Count = " .. cnt*i,0,sy-sgh*(i+1),sfont)
	end
	p_drawText("fps = " .. fps,0,0,sfont)
end

function a_event(e,x,y)
	p_log("EVENT " .. e .. " " .. x .. " " .. y)
end

a_init()
