-- log("Test")

Me.OnPlayerJoin = function(player)
	local scene = player:GetCurrentScene()
	log('OnPlayerJoin: scene: '..scene:GetName())
end

Me.OnPlayerLeave = function(player)
	log('OnPlayerLeave')
end

Me.OnServerTick = function(tick)
	-- log('OnServerTick: '..tick.."ms")
end
