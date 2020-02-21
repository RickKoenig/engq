-- npc1
local npchandle = nil		-- display npc handle
local dialoghandle = nil	-- display dialog handle
local place = 0				-- where in the conversation you are

-- globals
visits = 0
-- test1 = 5
-- test2 = 7

local function trackvisits()
	OS.logger("in track visits\n")
	visits = visits + 1
end

-- test dialog v2
local dialogtext = {
		{"p1","Hi %d Sheriff","visits"},
		{"pg1","Hello %d Sheriff","hos","visits"},
--		{"if","test1","~=",13,"notnever"},
		{"if","visits","~=",0,"notnever"},
		{"n","I don't think we've met"},
		{"goto","merge"},
	{"label","notnever"},
		{"if","visits","~=",1,"notone"},
		{"n","I've seen you once before"},
		{"goto","merge"},
	{"label","notone"},
		{"n1","I've seen you %d times before","visits"},
--		{"ns1","I've seen you %d times before","synflute.wav","visits"},
	{"label","merge"},
		{"nop"},
		{"goto","skipstartover"},
	{"label","hos"},
		{"ns","Hello Player","synflute.wav"},
		{"goto","skipstartover"},
	{"label","startover"},
		{"n1","What do you want %d again?","visits"},
	{"label","skipstartover"},
		{"p","Can you help me?"},
		{"pg","Bye Sheriff","end"},
		{"goto","startover"},
	{"label","end"},
		{"n","Bye Player"},
		{"fun",trackvisits},
}

-- npc init/proc/exit

-- local wh,sh
function init()
	OS.logger("in npc1.lua init\n")
-- create npc display object
	npchandle = OS.npcadd(400,32,15,"npc1")
	OS.npcsetvis(npchandle,1)
end

function proc()
	OS.logger("in npc1.lua proc\n")
	local np = OS.npcclicked()
--	OS.logger("npchandle is '%s' and clicked is '%s'\n",type(npchandle),type(np))
	if npchandle == np then
		OS.logger("npc1 clicked\n")
-- if no dialog running and npc1 clicked make one
		if indialog == 0 then
			indialog = 1
			dialoghandle = OS.dialogadd(10,30,270,200)
			place = 0
		end
	end
	if dialoghandle then
-- run the dialog
		place = procdialog(dialoghandle,dialogtext,place)
-- if done talking then free dialog
		if place == -1 then
			OS.dialogremove(dialoghandle)
			dialoghandle = nil
			indialog = 0
		end
	end
end

-- exit game
function exit()
	OS.logger("in npc1.lua exit\n")
-- kill npc model
	if npchandle then
		OS.npcremove(npchandle)
		npchandle = nil
	end
-- kill dialog display if not already killed
	if dialoghandle then
		OS.dialogremove(dialoghandle)
-- clean up dialog walker
		doneprocdialog()	-- frees sound if necc.
		dialoghandle = nil
		indialog = 0
		place = 0
	end
end
