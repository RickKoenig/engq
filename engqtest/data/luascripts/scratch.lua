-- a test

OS.Beep(1000,50) -- hello?
local avar = 10		-- how does this work?? It works but how??
-- gvar = 150		-- a global var
-- OS.logger("in beforeinit:  It would be good to have this work aye?? avar = %f, gvar = %f\n",avar,gvar);

--[
function init()
--	OS.logger("in init\n");
	local avar = 10 -- a local var
--	OS.logger("in init set avar to %f\n",avar);
end
]
function proc()
--	OS.logger("in proc\n");
	avar = avar + 1
	return avar
--	OS.logger("in proc, avar is now %f\n",avar);
end
--[
function exit()
--	OS.logger("in exit\n");
end
]