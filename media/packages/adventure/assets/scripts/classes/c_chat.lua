Me.OnCreated = function()
	local textso = SceneObject.As.Text(Me)
	if textso ~= nil then
		textso.Font = 'DejaVuSansCondensed.ttf'
		textso.FontSize = 15
		textso.Centered = true
	end
end
