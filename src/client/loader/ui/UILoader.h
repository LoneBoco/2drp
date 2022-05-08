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

	static std::unique_ptr<ui::UIManager> CreateUIManager();

private:
};

} // end namespace tdrp::loader
