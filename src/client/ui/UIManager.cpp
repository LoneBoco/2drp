#include <RmlUi/Lua.h>
#include <RmlUi/Debugger.h>
#include <RmlSolLua.h>

#include "client/ui/UIManager.h"

#include "client/game/Game.h"
#include "client/render/Window.h"

#include "client/ui/interface/RenderInterfaceSFML.h"
#include "client/ui/interface/ShellFileInterface.h"
#include "client/ui/interface/SystemInterfaceSFML.h"

#include "client/script/Script.h"
#include "engine/script/modules/bind.h"


namespace tdrp::ui
{

UIManager::UIManager()
{
	FileInterface = std::make_unique<ShellFileInterface>();
	RenderInterface = std::make_unique<RmlUiSFMLRenderer>();
	SystemInterface = std::make_unique<RmlUiSFMLSystemInterface>();

	Rml::SetFileInterface(FileInterface.get());
	Rml::SetRenderInterface(RenderInterface.get());
	Rml::SetSystemInterface(SystemInterface.get());
	
	Lua = std::make_unique<sol::state>();
	Lua->open_libraries(sol::lib::base, sol::lib::string, sol::lib::table, sol::lib::math, sol::lib::jit);
	sol::set_default_exception_handler(*Lua);

	Rml::Initialise();
	Rml::SolLua::Initialise(Lua.get());

	script::modules::bind_events(*Lua);
	script::modules::bind_attributes(*Lua);
	script::modules::bind_player(*Lua);
	script::modules::bind_scene(*Lua);
	script::modules::bind_sceneobject(*Lua);
	script::modules::bind_server(*Lua);
	script::modules::bind_vector(*Lua);

	bind_globals(*Lua);
	bind_game(*Lua);
	bind_camera(*Lua);

	(*Lua)["log"] = [](const std::string& message) { log::PrintLine("{}", message); };
	(*Lua)["MODULENAME"] = "UI";

	auto game = BabyDI::Get<tdrp::Game>();
	(*Lua)["Game"] = game;

	log::PrintLine("[UI] Loaded LUA.");
}

UIManager::~UIManager()
{
	// Rml::Debugger::Shutdown();
	Rml::Shutdown();

	Lua = nullptr;
}

Rml::Context* UIManager::CreateContext(const std::string& name)
{
	// Convert to RmlUi size.
	Rml::Vector2i size{ Window->GetWidth(), Window->GetHeight() };

	auto context = Rml::CreateContext(name, size, RenderInterface.get());
	if (!context) return nullptr;

	//Rml::Debugger::Initialise(context);
	return context;
}

void UIManager::ToggleVisibility(const std::string& name, bool visible)
{
	if (visible)
		m_visible_contexts.insert(name);
	else
		m_visible_contexts.erase(name);
}

void UIManager::ScreenSizeUpdate()
{
	Rml::Vector2i size{ Window->GetWidth(), Window->GetHeight() };

	for (int i = 0; i < Rml::GetNumContexts(); ++i)
		Rml::GetContext(i)->SetDimensions(size);
}

void UIManager::Update()
{
	for (int i = 0; i < Rml::GetNumContexts(); ++i)
	{
		auto context = Rml::GetContext(i);
		if (!context) continue;

		if (m_visible_contexts.contains(context->GetName()))
			context->Update();
	}
}

void UIManager::Render()
{
	for (int i = 0; i < Rml::GetNumContexts(); ++i)
	{
		auto context = Rml::GetContext(i);
		if (!context) continue;

		if (m_visible_contexts.contains(context->GetName()))
			context->Render();
	}
}

void UIManager::ForEachVisible(ContextIterationFunc func)
{
	for (int i = 0; i < Rml::GetNumContexts(); ++i)
	{
		auto context = Rml::GetContext(i);
		if (!context) continue;

		if (m_visible_contexts.contains(context->GetName()))
			func(*context);
	}
}

} // end namespace tdrp::ui
