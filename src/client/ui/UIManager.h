#pragma once

#include "engine/common.h"
#include "client/render/Window.h"

#include <set>
#include <functional>

#include <RmlUi/Core.h>
#include <sol/sol.hpp>

#include "client/ui/interface/RenderInterfaceSFML.h"
#include "client/ui/interface/ShellFileInterface.h"
#include "client/ui/interface/SystemInterfaceSFML.h"

#include "engine/script/Script.h"


//namespace tdrp::render { class Window; }

namespace tdrp::ui
{

using ContextIterationFunc = std::function<bool(Rml::Context&)>;

class UIManager
{
public:
	UIManager();
	~UIManager();

	UIManager(const UIManager& other) = delete;
	UIManager(UIManager&& other) = delete;
	UIManager& operator=(const UIManager& other) = delete;
	UIManager& operator=(UIManager&& other) = delete;
	bool operator==(const UIManager& other) = delete;

public:
	Rml::Context* CreateContext(const std::string& name);
	Rml::Context* GetContext(const std::string& name);

public:
	void ToggleContextVisibility(const std::string& context, bool visible);
	void ToggleDocumentVisibility(const std::string& context, const std::string& document, bool visible);
	void ReloadUI();
	
public:
	void ScreenSizeUpdate();
	void Update();
	void Render();

public:
	script::ScriptPtr Script;
	std::unique_ptr<ShellFileInterface> FileInterface = nullptr;
	std::unique_ptr<RmlUiSFMLRenderer> RenderInterface = nullptr;
	std::unique_ptr<RmlUiSFMLSystemInterface> SystemInterface = nullptr;

protected:
	INJECT(::tdrp::render::Window, Window);
	INJECT(::tdrp::script::ScriptManager, ScriptManager);
	std::set<std::string> m_visible_contexts;

public:
	bool ForEachVisible(ContextIterationFunc func);
};

using UIManagerPtr = std::unique_ptr<UIManager>;

/////////////////////////////

inline Rml::Context* UIManager::GetContext(const std::string& name)
{
	return Rml::GetContext(name);
}

} // end namespace tdrp::ui
