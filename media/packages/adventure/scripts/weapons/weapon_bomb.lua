Me.OnCreated = function()
	log('Bomb created')
end

Me.OnAdded = function(item, count)
	log('Added '..count..' bombs from item instance '..item.ID)
end

Me.OnRemoved = function(item, count)
	log('Removed '..count..' bombs from item instance '..item.ID)
end

Me.OnUsed = function(item)
	log('Bomb used, sending bomb event')
	local so = Server.Player:GetCurrentControlledSceneObject()
	local scene = Server.Player:GetCurrentScene()
	Server:SendEvent(scene, "bomb", "", so.Position, 0)
end
