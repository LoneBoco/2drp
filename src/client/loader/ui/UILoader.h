#pragma once

#include "engine/common.h"

#include "client/ui/UIManager.h"


namespace tdrp::loader
{

class UILoader
{
public:
	UILoader() = delete;
	~UILoader() = delete;

	UILoader(const UILoader& other) = delete;
	UILoader(UILoader&& other) = delete;
	UILoader& operator=(const UILoader& other) = delete;
	UILoader& operator=(UILoader&& other) = delete;

	/// <summary>
	/// Creates the UIManager and loads contexts without the load-on attribute.
	/// Does not do any model data-binding.
	/// </summary>
	static std::unique_ptr<ui::UIManager> CreateUIManager();

	/// <summary>
	/// Loads contexts which match the load_on attribute.
	/// </summary>
	/// <returns>How many contexts were loaded.</returns>
	static size_t Load(ui::UIManager* manager, std::string_view load_on);

private:
	static void LoadContext(pugi::xml_node& node, ui::UIManager* manager);
};

} // end namespace tdrp::loader
