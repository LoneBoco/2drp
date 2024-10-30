#pragma once

#include "engine/common.h"

namespace tdrp::ui
{
	class UIManager;
}

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
	/// Creates the UIManager.
	/// Does not do any model data-binding.
	/// </summary>
	static std::unique_ptr<ui::UIManager> CreateUIManager();

	/// <summary>
	/// Loads contexts from the contexts.xml file.
	/// </summary>
	/// <returns>How many contexts were loaded.</returns>
	static size_t Load(ui::UIManager* manager);
};

} // end namespace tdrp::loader
