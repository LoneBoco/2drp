self.vars = {
	timeout = 0;
}

self:OnCreated = function ()
end

self:OnControlledActorChange = function(old, new)
end

self:OnClientFrameTick = function(tick)

	-- No movement timeout.
	if (timeout > 0) then
		timeout = timeout - tick;
		return;
	end

	-- Determine if we can move.
	local domove = false;
	local dir = 0;
	local move = vector2df.new(0, 0);
	if (Game.keydown(Key.Up)) then
		move.x = move.y - 1.0; dir = 0; domove = true;
	elseif (Game.keydown(Key.Left)) then
		move.x = move.x - 1.0; dir = 1; domove = true;
	elseif (Game.keydown(Key.Down)) then
		move.y = move.y + 1.0; dir = 2; domove = true;
	elseif (Game.keydown(Key.Right)) then
		move.x = move.x + 1.0; dir = 3; domove = true;
	end

	-- Update our position.
	if (domove == true) then
		-- Multiply our move vector by the distance we are going to move.
		-- Normalize it first if we don't want fast diagonal movement.
		-- move:normalize();
		move = move * 10.0;

		-- self.Physics.Velocity = move;
	else
		-- self.Physics.Velocity = vector2df.new(0, 0);
	end

	-- Show us walking.
	if (self:get_type() == ESOT.ANIMATED) then
		local aself = self:as_animated();
		if (aself ~= nil) then
			if (domove == true) then
				aself.Animation = "walk";
				aself.Direction = dir;
			else
				aself.Animation = "idle";
			end
		end
	end

end

self:OnKeyPress = function (key)

	if (timeout > 0) then return end

	local aself = self:as_animated();
	if (aself == nil) then return end

	if (key == KEY_KEY_M) then
		self.scale = vector2df.new(0.8, 0.8);
	end

	if (key == KEY_KEY_S or key == KEY_KEY_SPACE or key == KEY_NUMPAD_5) then
		aself.Animation = "sword";
		-- aself.Physics.Velocity = vector2df.new(0, 0);

		local pos = vector2df.new(aself.Position.x + (1.5 * 16.0), aself.Position.y + (2.0 * 16.0));
		if (aself.Direction == 0) then pos.y = pos.y - (1.5 * 16.0) end
		if (aself.Direction == 2) then pos.y = pos.y + (0.5 * 16.0) end
		if (aself.Direction == 1) then pos.x = pos.x - (2.5 * 16.0) end
		if (aself.Direction == 3) then pos.x = pos.x + (2.5 * 16.0) end

		local hits = Engine.send_event(self, "sword", "", pos, 16);
		Engine.log("hits: " + hits);

		timeout = 300;
	end

end

self:OnMouseWheel = function (x, y, delta)
	local scale = vector2df.new(self.Scale);
	scale.x = scale.x + (delta > 0 ? 0.1 : -0.1) * abs(delta);
	scale.y = scale.y + (delta > 0 ? 0.1 : -0.1) * abs(delta);
	if (scale.x < 0.5) then scale.x = 0.5 end
	if (scale.y < 0.5) then scale.y = 0.5 end
	if (scale.x > 2.0) then scale.x = 2.0 end
	if (scale.y > 2.0) then scale.y = 2.0 end
	self.Scale = scale;
end
