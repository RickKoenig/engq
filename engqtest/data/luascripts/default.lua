-- main lua state loaded when game starts

-- globals here

indialog = 0	-- someone's in a dialog

red = {1,0,0}
green = {0,1,0}
blue = {0,0,1}
black = {0,0,0}
white = {1,1,1}
yellow = {1,1,0}

-- list of npc's to load
local npcnames = { "npc1.lua" ,	"npc2.lua" , "npc3.lua" , "npc4.lua" }

local npchandles		-- script handles of npcnames
local levelhandle = nil 	-- current level handle

function init()
	OS.logger("in default.lua init\n")
	npchandles = {}
-- load the npcs
	i = 1
	while 1 do
		if type(npcnames[i]) == "nil" then 
			break
		end
		npchandles[i] = OS.addscript(npcnames[i])
		i = i + 1
	end
	levelhandle = OS.addscript("outside.lua")			-- load the first level
	OS.logger("default.lua: loaded level 'outside.lua' got handle %d\n",levelhandle)
end

function proc()
	OS.logger("in default.lua proc\n")
end

function exit()
	OS.logger("in default.lua exit\n")
	OS.removescript(levelhandle)
	levelhandle = nil
	i = 1
	while 1 do
		if type(npcnames[i]) == "nil" then
			break
		end
		OS.removescript(npchandles[i])
		i = i + 1
	end
	npchandles = nil 		-- clean up
--	return 1
end
