#include <RmlUi/Lua.h>
#include <RmlUi/Debugger.h>
#include <RmlSolLua/RmlSolLua.h>

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

	log::PrintLine("[UI] Loaded Lua into UI.");

	// Load the UI bindings into the game's script state.
	Rml::SolLua::RegisterLua(&game->Script.GetLuaState());
	log::PrintLine("[UI] Loaded Lua into Game.");
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

	Rml::Debugger::Initialise(context);
	return context;
}

void UIManager::ToggleContextVisibility(const std::string& name, bool visible)
{
	if (visible)
		m_visible_contexts.insert(name);
	else
		m_visible_contexts.erase(name);
}

void UIManager::ToggleDocumentVisibility(const std::string& context, const std::string& document, bool visible)
{
	auto c = Rml::GetContext(context);
	if (c != nullptr)
	{
		auto d = c->GetDocument(document);
		if (d)
		{
			if (visible) d->Show();
			else d->Hide();
		}
	}
}

void UIManager::ReloadUI()
{
	// Turn off the debugger so we don't get a million error messages.
	//Rml::Debugger::Shutdown();

	// Reload document stylesheets.
	for (int c = 0; c < Rml::GetNumContexts(); ++c)
	{
		auto context = Rml::GetContext(c);
		for (int d = 0; d < context->GetNumDocuments(); ++d)
		{
			auto document = context->GetDocument(d);
			if (document)
			{
				document->ReloadStyleSheet();
				document->DispatchEvent(Rml::EventId::Load, Rml::Dictionary());
			}
		}

		// Add the debugger back in.
		//Rml::Debugger::Initialise(context);
	}
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

bool UIManager::ForEachVisible(ContextIterationFunc func)
{
	bool result = true;

	for (int i = 0; i < Rml::GetNumContexts(); ++i)
	{
		auto context = Rml::GetContext(i);
		if (!context) continue;

		if (m_visible_contexts.contains(context->GetName()))
			result &= func(*context);
	}

	return !result;
}

} // end namespace tdrp::ui
