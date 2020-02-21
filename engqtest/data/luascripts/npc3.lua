-- npc3

local npchandle = nil

function init()
	OS.logger("in npc3.lua init\n")
	npchandle = OS.npcadd(300,232,15,"npc3")
	OS.npcsetvis(npchandle,1)
end

function proc()
	OS.logger("in npc3.lua proc\n")
	local np = OS.npcclicked()
--	OS.logger("npchandle is '%s' and clicked is '%s'\n",type(npchandle),type(np))
	if npchandle == np then
		OS.logger("npc3 clicked\n")
	end
end

function exit()
	OS.logger("in npc3.lua exit\n")
	if npchandle then
		OS.npcremove(npchandle)
		npchandle = nil
	end
end
