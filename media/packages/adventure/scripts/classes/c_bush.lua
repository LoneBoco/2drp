vars = {
	timeout = 0,
	timeUntilRespawn = 5000,
}

Me.OnCreated = function()
	Me.Layer = 1
end

Me.OnEvent = function(sender, name, data, position, radius)
	if name ~= "sword" then return end

	if (sender ~= Server.Player) then
		-- log("["..Me.ID.."] [Repl: "..tostring(Me.Replicated).."] Bush replication test - not event sender, ignoring event.")
		--return
	end

	local state = Me.Attributes["state"]
	if state ~= nil and state ~= 0 then return end

	if Me.Type == SceneObjectType.ANIMATED then
		local cut = Me.Attributes["ani_cut"]
		if cut ~= nil then
			Me.Animation = tostring(cut)
		end
	else
		-- Me.Visible = false
	end

	Me.Attributes["state"] = 1
	Physics.SceneObject.SetEnabled(Me, false)
	vars.timeout = vars.timeUntilRespawn
end

Me.OnUpdate = function(tick)
	if vars.timeout <= 0 then return end
	vars.timeout = vars.timeout - tick

	if vars.timeout <= 0 then
		if Me.Type == SceneObjectType.ANIMATED then
			Me.Attributes["state"] = 2

			local grow = Me.Attributes["ani_grow"]
			if grow ~= nil then
				Me.Animation = tostring(grow)
			else
				Me.OnAnimationEnd()
				return
			end
		else
			Me.Attributes["state"] = 0
			Physics.SceneObject.SetEnabled(Me, true)
		end
	end
end

Me.OnAnimationEnd = function(animation)
	local state = Me.Attributes["state"]
	if state ~= nil and state ~= 2 then return end

	Me.Attributes["state"] = 0
	Physics.SceneObject.SetEnabled(Me, true)

	local idle = Me.Attributes["ani_idle"]
	if idle ~= nil then
		Me.Animation = tostring(idle)
	end
end

Me.OnSolveContact = function(other, collision)
	if other.Velocity.X < 0 then
		collision.Enabled = false
	end
end
