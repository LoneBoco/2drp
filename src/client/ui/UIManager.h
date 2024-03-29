#pragma once

#include "engine/common.h"
#include "client/render/Window.h"

#include <unordered_set>
#include <functional>

#include <RmlUi/Core.h>
#include <sol/sol.hpp>

#include "client/ui/interface/RmlUi_Platform_SFML.h"
#include "client/ui/interface/RmlUi_Renderer_GL2.h"
#include "client/ui/interface/ShellFileInterface.h"

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

	UIManager(const UIManager&) = delete;
	UIManager(UIManager&&) = delete;
	UIManager& operator=(const UIManager&) = delete;
	UIManager& operator=(UIManager&&) = delete;
	bool operator==(const UIManager&) = delete;

public:
	Rml::Context* CreateContext(const std::string& name);
	Rml::Context* GetContext(const std::string& name);

public:
	void BindDataModels(Rml::Context* context);

public:
	void ToggleContextVisibility(const std::string& context, bool visible);
	void ToggleDocumentVisibility(const std::string& context, const std::string& document);
	void ReloadUI();
	void AssignDebugger(const std::string& context);
	void MakeItemsDirty();
	
public:
	void ScreenSizeUpdate();
	void Update();
	void Render();

public:
	script::ScriptPtr Script;
	std::unique_ptr<Rml::FileInterface> FileInterface = nullptr;
	Rml::SystemInterface* SystemInterface = nullptr;
	RenderInterface_GL2* RenderInterface = nullptr;

protected:
	INJECT(::tdrp::render::Window, window);
	INJECT(::tdrp::script::ScriptManager, script_manager);
	std::unordered_map<std::string, Rml::DataModelHandle> m_item_handles;
	std::unordered_set<std::string> m_visible_contexts;
	std::vector<Rml::Context*> m_managed_contexts;
	Rml::Context* m_debugger_host;
	Rml::Context* m_debugger_context;

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
