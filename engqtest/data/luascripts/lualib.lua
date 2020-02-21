-- handy functions

-- works just like the c function logger
function OS.logger(str,...)
	fstr = string.format(str,...)
	OS.logstring(fstr)
end

function OS.errorexit(str,...)
	fstr = string.format(str,...)
	OS.errorexitstring(fstr)
end

-- dialog walker v1

function walktext(t)
	local i = 1
	while 1 do
		local ts = type(t[i])
		if ts == "nil" then
			break
		elseif ts == "string" then
			OS.logger("text %d '%s'\n",i,t[i])
		elseif ts == "table" then
			OS.logger("text %d table\n",i)
			OS.loggerindent()
			walktext(t[i])
			OS.loggerunindent()
		else
			OS.logger("type %d '%s'\n",i,ts)
		end
		i = i + 1
	end
end

-- dialog walker v2

local function findgoto(dt,key)
	local i = 1
	while 1 do
		local pp = dt[i]
		local ts = type(pp)
		if ts~="table" then
			OS.logger("nil i = %d '%s'\n",i,ts);
			return 0
		end
--		OS.logger("i %d, pp1 = '%s', pp2 = '%s'\n",i,pp[1],pp[2])
		if pp[1] == "label" and pp[2]==key then
			return i + 1
		end
		i = i + 1
	end
	return 0
end

local dcounter = 0
local dwh,dsh
local maxcount = 10    -- npc delay in frames if no sound
local textoffx = 10		-- start x
local textoffy = 10     -- start y
local textspacey = 30   -- ypixels per line
local texty	= 20		-- fonty size

function procdialog(h,dt,p)
	local mx,my,mc = OS.dialogmouseinfo(h)
-- check click
	if mc~=0 then
		OS.logger("mouse click %d %d\n",mx,my)
	end
-- move place to 1 if 0
	if p==0 then
		p=1
		dcounter = 0
	end
-- get a line of script
	local pp = dt[p]
	if type(pp)~="table" then
		OS.errorexit("procdialog: not a table")
	end
-- get command from line
	local pp1 = pp[1]
	if type(pp1)~="string" then
		OS.errorexit("procdialog: command not a string")
	end
-- process player command
	if pp1=="p" or pp1=="p1" or pp1=="pg" or pp1=="pg1" then
-- build up choice strings
		OS.dialogclear(h)
		local i = 0
		while 1 do
			local ppi = dt[p+i]
			if  type(ppi)~="table" then
				break
			end
			if ppi[1]~="p" and ppi[1]~="pg" and ppi[1]~="p1" and ppi[1]~="pg1" then
				break
			end
			if ppi[1]=="p" or ppi[1]=="pg" then
				OS.dialogaddstr(h,textoffx,textoffy+textspacey*i,ppi[2],yellow)
			elseif ppi[1]=="p1" then
				local str = string.format(ppi[2],_G[ppi[3]])
				OS.dialogaddstr(h,textoffx,textoffy+textspacey*i,str,yellow)
			elseif ppi[1]=="pg1" then
				local str = string.format(ppi[2],_G[ppi[4]])
				OS.dialogaddstr(h,textoffx,textoffy+textspacey*i,str,yellow)
			end
			i = i + 1
		end
-- see if choice clicked
		if mc~=0 and my>=textoffy then
			local j
			j = (my-textoffy)/textspacey 
			j,f = math.modf(j)
			if j<i and f<texty/textspacey then
				OS.logger("j = %d\n",j)
				OS.logger("i = %d\n",i)
				local q = p + j
				local ppj = dt[q]
				if ppj[1]=="pg" or ppj[1]=="pg1" then
					p = findgoto(dt,ppj[3])
					if p==0 then
						OS.errorexit("procdialog: pg goto '%s' not found",_G[ppj[3]])
					end
				elseif ppj[1]=="p" or ppj[1]=="p1" then
					p = p + i
				end
			end
		end
-- process nonplayer command
	elseif pp1=="n" then
		OS.dialogclear(h)
		OS.dialogaddstr(h,textoffx,textoffy,pp[2],green)
		dcounter = dcounter + 1
		if dcounter == maxcount or mc~=0 then
			p = p + 1
			dcounter = 0
		end
	elseif pp1=="n1" then
		OS.dialogclear(h)
		local str = string.format(pp[2],_G[pp[3]])
		OS.dialogaddstr(h,textoffx,textoffy,str,green)
		dcounter = dcounter + 1
		if dcounter == maxcount or mc~=0 then
			p = p + 1
			dcounter = 0
		end
	elseif pp1=="ns" then
		OS.dialogclear(h)
		OS.dialogaddstr(h,textoffx,textoffy,pp[2],green)
		if not dwh then
			dwh = OS.waveload(pp[3])
			dsh = OS.waveplay(dwh)
		end
		if OS.soundstatus(dsh)==0 or mc~=0 then
			p = p + 1
			OS.waveunload(dwh)
			dwh = nil
			dsh = nil
		end
	elseif pp1=="ns1" then
		OS.dialogclear(h)
		local str = string.format(pp[2],_G[pp[4]])
		OS.dialogaddstr(h,textoffx,textoffy,str,green)
		if not dwh then
			dwh = OS.waveload(pp[3])
			dsh = OS.waveplay(dwh)
		end
		if OS.soundstatus(dsh)==0 or mc~=0 then
			p = p + 1
			OS.waveunload(dwh)
			dwh = nil
			dsh = nil
		end
-- process goto
	elseif pp1=="goto" then
		p = findgoto(dt,pp[2])
		if p==0 then
			OS.errorexit("procdialog: goto '%s' not found",pp[2])
		end
-- process if
	elseif pp1=="if" then
		local v = _G[pp[2]]
		local v2
		if type(pp[4])=="string" then
			v2 = _G[pp[4]]
		else
			v2 = pp[4]
		end
		OS.logger("if: %d %d\n",v,v2)
		local cnd = 0
		if pp[3]=="==" then
			if v==v2 then
				cnd = 1
			end
		elseif pp[3]=="~=" then
			if v~=v2 then
				cnd = 1
			end
		elseif pp[3]==">" then
			if v>v2 then
				cnd = 1
			end
		elseif pp[3]=="<" then
			if v<v2 then
				cnd = 1
			end
		elseif pp[3]==">=" then
			if v>=v2 then
				cnd = 1
			end
		elseif pp[3]=="<=" then
			if v<=v2 then
				cnd = 1
			end
		else
			OS.errorexit("procdialog: if '%s' condition not found",pp[3])
		end
		if cnd == 1 then
			OS.logger("if match: %d %s %d\n",v,pp[3],v2)
			p = findgoto(dt,pp[5])
			if p==0 then
				OS.errorexit("procdialog: if '%s' not found",pp[5])
			end
		else
			p = p + 1
		end
-- process nop
	elseif pp1=="nop" then
		p = p + 1
-- process label
	elseif pp1=="label" then
		p = p + 1
-- process function
	elseif pp1=="fun" then
		pp[2]()
--		OS.logger("lualib.lua: visits = %d\n",visits)
		p = p + 1
-- otherwise not a valid command
	else 
		OS.errorexit("bad command '%s'",pp1)
	end
-- get next command
	pp = dt[p]
-- return 'done' if no more commands
	if type(pp)=="nil" then
		p = -1
	end
-- return new place if more commands
	return p
end

function doneprocdialog()
	if dwh then
		OS.waveunload(dwh)
		dwh = nil
		dsh = nil
	end
end

