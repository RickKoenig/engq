-- a test

OS.Beep(1000,50) -- hello?
local avar = 13		-- how does this work?? It works but how??
gvar = 150		-- a global var

-- OS.logger("in beforeinit:  It would be good to have this work aye?? avar = %d, gvar = %d\n",avar,gvar);

function init()
	OS.logger("in init test1.lua\n")
	avar = 10 -- a local var
	OS.logger("in init set avar to %d\n",avar)
end

function proc()
	OS.logger("in proc\n");
	avar = avar + 1
	gvar = gvar + 2
	OS.logger("in proc of test1.lua, avar is now %d, gvar %d\n",avar,gvar)
	return avar
end

function exit()
	OS.logger("in exit test1.lua\n")
end
