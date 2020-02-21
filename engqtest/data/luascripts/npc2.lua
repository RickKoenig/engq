-- npc2

local npchandle = nil
local mousedialoghandle = nil 	-- mouse display (dumb dialog)
local dialoghandle = nil
local place = 0					-- where in the conversation you are

-- test dialog v2
local dialogtext = {
		{"n","Fee"},
		{"n","Fye"},
--		{"ns","Fye","vs_w1pxa1_01.wav"},
		{"n","Fo"},
		{"n","Fum"},
		{"if","visits","==",0,"end"},
		{"n","I see you met the sheriff"},
	{"label","end"}
}

-- npc init/proc/exit

-- local wh,sh
function init()
	OS.logger("in npc2.lua init\n")
-- create npc display object
	npchandle = OS.npcadd(300,132,15,"npc2")
	OS.npcsetvis(npchandle,1)
-- create dump mouse info dialog
	mousedialoghandle = OS.dialogadd(308,360,204,24)
end

function proc()
	OS.logger("in npc2.lua proc\n")
	OS.dialogclear(mousedialoghandle)
	mx,my,mc = OS.mouseinfo()
	OS.dialogaddstr(mousedialoghandle,10,8,string.format("mx %3d, my %3d, mclick %d",mx,my,mc))
	local np = OS.npcclicked()
--	OS.logger("npchandle is '%s' and clicked is '%s'\n",type(npchandle),type(np))
	if npchandle == np then
		OS.logger("npc2 clicked\n")
-- if no dialog running and npc2 clicked make one
		if indialog == 0 then
			indialog = 1
			dialoghandle = OS.dialogadd(10,334,250,50)
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
	OS.logger("in npc2.lua exit\n")
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
-- clean up mouse dialog
	if mousedialoghandle then
		OS.dialogremove(mousedialoghandle)
		mousedialoghandle = nil
	end
end
