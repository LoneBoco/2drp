Me.OnCreated = function()
	local useable = Me:CreateUseable('bomb', 'block_sword.png', 'Set us up the bomb.')
	if useable == nil then return end

	useable.OnUsed = function()
		log('Bomb used, sending bomb event')
		local so = Me.Player:GetCurrentControlledSceneObject()
		Me:SendServerEvent(so, "bomb", "", so.Position, 0)
	end
end

Me.OnDestroyed = function()
	Me:DeleteUseable('bomb')
end

--[[Me.OnKeyPress = function(key)
	if key == Key.D then
		log('Attempting to use bomb')
		local bomb = Me.Useables["bomb"]
		if bomb ~= nil then
			log('Bomb found, using')
			bomb:Use()
		end
	end
end--]]
