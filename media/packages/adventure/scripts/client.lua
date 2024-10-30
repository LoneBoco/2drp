vars = {
	timeout = {
		movement = 0,
		chat = 0
	}
}

func = {
	keys = {}
}

Me.OnCreated = function()
	log('Client OnCreated')
	vars.so_text = {}
end

Me.OnConnected = function()
	log('Client OnConnected')
	Me:ToggleUIContext("hud", true)
	Me:AssignDebugger("hud")
end

Me.OnClientFrame = function(tick)
	local so = Me.Player:GetCurrentControlledSceneObject()

	-- Timeouts.
	for k, v in pairs(vars.timeout) do
		if vars.timeout[k] > 0 then
			vars.timeout[k] = vars.timeout[k] - tick
			if vars.timeout[k] <= 0 then
				vars.timeout[k] = -1
			end
		end
	end

	-- Chat timeout.
	if vars.timeout.chat == -1 then
		vars.timeout.chat = 0
		if so ~= nil then so.Attributes["chat"] = "" end
	end

	-- No movement timeout.
	if vars.timeout.movement > 0 then return end
	if vars.timeout.movement == -1 then
		vars.timeout.movement = 0
	end

	if so == nil then return end

	-- UI open.
	if IsUIOpen() then
		if so.Animation == "hero_walk.gani" then
			so.Animation = "hero_idle.gani"
		end
		so.Velocity = Vector2df.new(0, 0)
		return
	end

	local domove = false
	local dir = 0
	local move = Vector2df.new(0, 0)

	-- Calculate movement.
	if (Me.keydown(Key.Up)) then
		move.Y = move.Y - 1; dir = 0; domove = true;
	elseif (Me.keydown(Key.Down)) then
		move.Y = move.Y + 1; dir = 2; domove = true;
	end

	if (Me.keydown(Key.Left)) then
		move.X = move.X - 1; dir = 1; domove = true;
	elseif (Me.keydown(Key.Right)) then
		move.X = move.X + 1; dir = 3; domove = true;
	end

	-- Update our position.
	if (domove == true) then
		-- Multiply our move vector by the distance we are going to move.
		-- Normalize it first if we don't want fast diagonal movement.
		-- move:normalize();
		move = move * 10
		so.Direction = dir
		so.Animation = "hero_walk.gani"
		so.Velocity = move
	else
		so.Animation = "hero_idle.gani"
		so.Velocity = Vector2df.new(0, 0)
	end
end

Me.OnKeyPress = function(key)
	if func.keys[key] ~= nil then
		return func.keys[key](key)
	end
end

Me.OnGainedOwnership = function(sceneobject)
	log('Gained ownership of scene object '..sceneobject.ID..' ('..sceneobject.ClassName..')')

	-- Set the camera to follow the player.
	-- Subscribe to the 'chat' attribute so we can update our chat text.
	if sceneobject.ClassName == "player" then
		Me.Player:SwitchControlledSceneObject(sceneobject)
		Me.Camera:FollowSceneObject(sceneobject, 0)
		Me.Camera.FollowOffset = Vector2di.new(24, 48)
		vars.chat_listener = sceneobject.Attributes:Get("chat"):Subscribe(func.chat)
	end

	-- Keep a reference to our chat scene object.
	if sceneobject.ClassName == "chat" then
		vars.so_text = sceneobject
	end
end

function IsUIOpen()
	local hud = rmlui.GetContext('hud')
	if hud == nil then return false end

	local chat = hud:GetDocument('chat')
	if chat ~= nil and chat.visible then return true end

	local inventory = hud:GetDocument('inventory')
	if inventory ~= nil and inventory.visible then return true end

	return false
end

-- Update our chat scene object with the value of our 'chat' attribute.
func.chat = function(attribute)
	local so = Me.Player:GetCurrentControlledSceneObject()
	local attached = vars.so_text
	if attached ~= nil then
		attached.Text = attribute:AsString()
		if attached.Text ~= "" then
			vars.timeout.chat = 1000 * 5
		end
	end
end

-- Grab.
func.keys[Key.A] = function(key)
end

-- Sword attack.
func.keys[Key.S] = function(key)
	local so = Me.Player:GetCurrentControlledSceneObject()
	if so == nil then return end

	so.Animation = "hero_sword.gani"
	so.Velocity = Vector2df.new(0, 0)
	vars.timeout.movement = 300

	local pos = Vector2df.new(so.Position.X + (1.5 * 16.0), so.Position.Y + (2.0 * 16.0))

	log("- sword: ("..so.Position.X..", "..so.Position.Y..") - ("..pos.X..", "..pos.Y..")")

	if (so.Direction == 0) then pos.Y = pos.Y - (1.0 * 16.0) end
	if (so.Direction == 2) then pos.Y = pos.Y + (0.5 * 16.0) end
	if (so.Direction == 1) then pos.X = pos.X - (0.5 * 16.0) end
	if (so.Direction == 3) then pos.X = pos.X + (0.5 * 16.0) end

	log("- w/ dir: ("..pos.X..", "..pos.Y..")")

	Me:SendEvent("sword", "", pos, 24)
end

-- Useable.
func.keys[Key.D] = function(key)
	local itemid = Me.Flags.CurrentItem
	if itemid ~= nil then
		local item = Me.Player:GetItem(tonumber(itemid))
		if item ~= nil then item:Use() end
	end
end

-- Open the inventory.
func.keys[Key.Q] = function(key)
	Me:ToggleUIDocument('hud', 'inventory')
end

-- Activate our chat bar.
func.keys[Key.Tab] = function(key)
	local chat = rmlui.GetContext('hud'):GetDocument('chat')
	if chat.visible then
		chat:Hide()
	else
		chat:Show()
	end
end

-- Reload the UI.
func.keys[Key.F9] = function(key)
	Me:ReloadUI()
	-- for i,context in pairs(rmlui.contexts) do
	-- 	for _,document in pairs(context.documents) do
	-- 		log('Reloading style sheet '..document.id);
	-- 		document:ReloadStyleSheet()
	-- 	end
	-- end
end

-----------------------------
-- Testing

func.keys[Key.O] = function(key)
	Me:SendServerEvent("AddItem", "bomb")
end

func.keys[Key.P] = function(key)
	Me:SendServerEvent("RemoveItem", "bomb")
end

-----------------------------
-- Testing

func.keys[Key.Hyphen] = function(key)
	local so = Me.Player:GetCurrentControlledSceneObject()
	if so == nil then return end

	local hearts = so.Attributes["hearts"]
	local maxhearts = so.Attributes["maxhearts"]
	if hearts <= 1 then hearts = 1 else hearts = hearts - 1 end
	so.Attributes["hearts"] = hearts
	log('Hearts -1: ('..(hearts/2)..'/'..maxhearts..')')
end

func.keys[Key.Equal] = function(key)
	local so = Me.Player:GetCurrentControlledSceneObject()
	if so == nil then return end

	local hearts = so.Attributes["hearts"]
	local maxhearts = so.Attributes["maxhearts"]
	hearts = math.min(maxhearts * 2, hearts + 1)
	so.Attributes["hearts"] = hearts
	log('Hearts +1: ('..(hearts/2)..'/'..maxhearts..')')
end

func.keys[Key.LBracket] = function(key)
	local so = Me.Player:GetCurrentControlledSceneObject()
	if so == nil then return end

	local hearts = so.Attributes["hearts"]
	local maxhearts = so.Attributes["maxhearts"]
	if maxhearts <= 1 then maxhearts = 1 else maxhearts = maxhearts - 1 end
	so.Attributes["maxhearts"] = maxhearts
	log('Max hearts -1: ('..(hearts/2)..'/'..maxhearts..')')
end

func.keys[Key.RBracket] = function(key)
	local so = Me.Player:GetCurrentControlledSceneObject()
	if so == nil then return end

	local hearts = so.Attributes["hearts"]
	local maxhearts = so.Attributes["maxhearts"]
	maxhearts = maxhearts + 1
	so.Attributes["maxhearts"] = maxhearts
	log('Max hearts +1: ('..(hearts/2)..'/'..maxhearts..')')
end

-----------------------------

--[[
self:OnKeyPress = function (key)

	if (timeout > 0) then return end

	local aself = self:as_animated();
	if (aself == nil) then return end

	if (key == KEY_KEY_M) then
		self.scale = Vector2df.new(0.8, 0.8);
	end

	if (key == KEY_KEY_S or key == KEY_KEY_SPACE or key == KEY_NUMPAD_5) then
		aself.Animation = "sword";
		-- aself.Physics.Velocity = Vector2df.new(0, 0);

		local pos = Vector2df.new(aself.Position.X + (1.5 * 16.0), aself.Position.Y + (2.0 * 16.0));
		if (aself.Direction == 0) then pos.Y = pos.Y - (1.5 * 16.0) end
		if (aself.Direction == 2) then pos.Y = pos.Y + (0.5 * 16.0) end
		if (aself.Direction == 1) then pos.X = pos.X - (2.5 * 16.0) end
		if (aself.Direction == 3) then pos.X = pos.X + (2.5 * 16.0) end

		local hits = Engine.send_event(self, "sword", "", pos, 16);
		Engine.log("hits: " + hits);

		timeout = 300;
	end

end

self:OnMouseWheel = function (x, y, delta)
	local scale = Vector2df.new(self.Scale);
	scale.X = scale.X + (delta > 0 ? 0.1 : -0.1) * abs(delta);
	scale.Y = scale.Y + (delta > 0 ? 0.1 : -0.1) * abs(delta);
	if (scale.X < 0.5) then scale.X = 0.5 end
	if (scale.Y < 0.5) then scale.Y = 0.5 end
	if (scale.X > 2.0) then scale.X = 2.0 end
	if (scale.Y > 2.0) then scale.Y = 2.0 end
	self.Scale = scale;
end
]]
