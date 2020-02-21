-- npc4

local npchandle = nil

function init()
	OS.logger("in npc4.lua init\n")
	npchandle = OS.npcadd(300,332,15,"npc4")
	OS.npcsetvis(npchandle,1)
end

function proc()
	OS.logger("in npc4.lua proc\n")
	local np = OS.npcclicked()
--	OS.logger("npchandle is '%s' and clicked is '%s'\n",type(npchandle),type(np))
	if npchandle == np then
		OS.logger("npc4 clicked\n")
	end
end

function exit()
	OS.logger("in npc4.lua exit\n")
	if npchandle then
		OS.npcremove(npchandle)
		npchandle = nil
	end
end
