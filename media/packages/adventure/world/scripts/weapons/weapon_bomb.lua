Me.OnCreated = function()
	log('Bomb created')
end

Me.OnUsed = function()
	log('Bomb used, sending bomb event')
	local so = Server.Player:GetCurrentControlledSceneObject()
	Server:SendServerEvent(so, "bomb", "", so.Position, 0)
end
