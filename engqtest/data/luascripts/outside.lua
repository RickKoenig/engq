-- a level

-- local avar

function init()
--	avar = 10 -- a local var
	OS.logger("in outside.lua init\n") -- , set avar to %d\n",avar)
--	return avar
end

function proc()
	OS.logger("in outside.lua proc\n")
end

function exit()
	OS.logger("in outside.lua exit\n")
--	return 1
end
