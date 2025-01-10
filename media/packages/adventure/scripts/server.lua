Me.OnStarted = function()
	log('Server started')
end

Me.OnPlayerJoin = function(player)
	local scene = Me:GetScene(player.Account.Scene or "") or Me:GetDefaultScene()
	log('OnPlayerJoin: scene: '..scene:GetName())

	Me:SwitchPlayerScene(player, scene)

	local x = player.Account.Flags.x or 0
	local y = player.Account.Flags.y or 0

	--
	local so = Me:CreateSceneObject(SceneObjectType.ANIMATED, "player")
	so.Position = Vector2df.new(x, y)
	so.Layer = 1
	so.Direction = 3
	so.Attributes["HEAD"] = "head_hero1.png"
	so.Attributes["BODY"] = "body_hero1.png"
	so.Attributes["SWORD"] = "sword_hero1.png"
	so.Attributes["SHIELD"] = "shield1.png"
	so.Animation = "hero_idle.gani"

	so.OnCreatePhysics = function()
		log('Creating physics')

		local config = PhysicsBody.new()
		config.Type = BodyType.HYBRID;
		config.FixedRotation = true;
		config:AddCollisionCircle(0.98, Vector2df.new(1.5, 2.0))

		so:SetPhysicsBody(config)
	end
	--

	local text = Me:CreateSceneObject(SceneObjectType.TEXT, "chat")
	text.Layer = 2
	text:AttachTo(so)
	text.Position = Vector2df.new(1.5 * 16, -1.0 * 16)

	Me:SwitchSceneObjectScene(so, scene)
	Me:SwitchSceneObjectOwnership(so, player)
	Me:SwitchSceneObjectOwnership(text, player)
end

Me.OnPlayerLeave = function(player)
	log('OnPlayerLeave')

	-- Save the player position.
	local so = player:GetCurrentControlledSceneObject()
	if so ~= nil then
		log('Saving player positional data.')
		player.Account.Flags.x = so.Properties.X
		player.Account.Flags.y = so.Properties.Y
	else
		log('Tried to save player position but they had no controlled scene object!')
	end

	Me:DeletePlayerOwnedSceneObjects(player)
end

Me.OnServerTick = function(tick)
	--log('OnServerTick: '..tick.."ms")
end

Me.OnServerEvent = function(scene, sender, name, data)
	if name == "AddScript" then
		Me:GiveClientScript(data, sender)
	elseif name == "RemoveScript" then
		Me:RemoveClientScript(data, sender)
	elseif name == "bomb" then
		log('boom')
	elseif name == "AddItem" then
		log('Adding bomb')
		Me:GiveItemToPlayer(sender, "bomb", ItemType.STACKABLE)
	elseif name == "RemoveItem" then
		log('Removing bomb')
		Me:RemoveItemsFromPlayer(sender, "bomb")
	end
end

-- Me.OnEvent = function(sender, name, data, origin, radius)
-- end
