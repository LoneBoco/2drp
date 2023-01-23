Me.OnCreated = function()
	local textso = SceneObject.As.TEXT(Me)
	if textso ~= nil then
		textso.Font = 'DejaVuSansMono.ttf'
		textso.FontSize = 15
		textso.Centered = true
	end
end
