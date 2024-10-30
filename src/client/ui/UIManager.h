#pragma once

#include "engine/common.h"
#include "client/render/Window.h"

#include <unordered_set>
#include <functional>

#include <RmlUi/Core.h>
#include <sol/sol.hpp>

#include "client/loader/ui/UILoader.h"
#include "client/ui/interface/RmlUi_Platform_SFML.h"
#include "client/ui/interface/RmlUi_Renderer_GL2.h"
#include "client/ui/interface/ShellFileInterface.h"

#include "engine/script/Script.h"


//namespace tdrp::render { class Window; }

namespace tdrp::ui
{

using ContextIterationFunc = std::function<bool(Rml::Context&)>;

struct UIContextDataDocument
{
	bool ShowOnLoad = false;
	std::string Filename;
};

struct UIContextData
{
	Rml::Context* Context = nullptr;
	std::string LoadOn;
	std::vector<UIContextDataDocument> Documents;
};

class UIManager
{
	friend class tdrp::loader::UILoader;

public:
	UIManager();
	~UIManager();

	UIManager(const UIManager&) = delete;
	UIManager(UIManager&&) = delete;
	UIManager& operator=(const UIManager&) = delete;
	UIManager& operator=(UIManager&&) = delete;
	bool operator==(const UIManager&) = delete;

public:
	void ToggleContextVisibility(const std::string& context, bool visible) noexcept;
	void ToggleDocumentVisibility(const std::string& context, const std::string& document);
	void ReloadUI();
	void AssignDebugger(const std::string& context);
	void MakeItemsDirty();
	
public:
	void ScreenSizeUpdate();
	void Update();
	void Render();

public:
	void LoadContext(std::string_view name);
	size_t LoadContextsFromEvent(std::string_view event);

protected:
	Rml::Context* createContext(std::string_view name);
	void bindDataModels(Rml::Context* context);
	UIContextData* getContextData(std::string_view name);

public:
	script::ScriptPtr Script;
	std::unique_ptr<Rml::FileInterface> FileInterface = nullptr;
	Rml::SystemInterface* SystemInterface = nullptr;
	RenderInterface_GL2* RenderInterface = nullptr;

protected:
	INJECT(::tdrp::render::Window, window);
	INJECT(::tdrp::script::ScriptManager, script_manager);
	std::unordered_map<std::string, Rml::DataModelHandle> m_item_handles;
	std::set<std::string> m_visible_contexts;
	std::map<std::string, UIContextData> m_contexts;
	Rml::Context* m_debugger_host;
	Rml::Context* m_debugger_context;

public:
	bool ForEachVisible(ContextIterationFunc func);
};

using UIManagerPtr = std::unique_ptr<UIManager>;

/////////////////////////////

inline UIContextData* UIManager::getContextData(std::string_view name)
{
	auto it = m_contexts.find(std::string{ name });
	if (it != m_contexts.end())
		return &it->second;

	return nullptr;
}

} // end namespace tdrp::ui
