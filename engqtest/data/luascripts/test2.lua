-- a test

OS.Beep(1200,50)
local avar = 100
 
function init()
	OS.logger("in init test2.lua\n")
end

function proc()
	avar = avar + 3
	gvar = gvar + 5
	OS.logger("in proc of test2.lua, avar is now %d, gvar %d\n",avar,gvar)
	return avar
end

function exit()
	OS.logger("in exit test2.lua\n")
end
