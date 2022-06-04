#include <RmlUi/Lua.h>
#include <RmlUi/Debugger.h>
#include <RmlSolLua/RmlSolLua.h>

#include "client/useable/Useable.h"

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
	// Bind Rml.
	{
		FileInterface = std::make_unique<ShellFileInterface>();
		RenderInterface = std::make_unique<RmlUiSFMLRenderer>();
		SystemInterface = std::make_unique<RmlUiSFMLSystemInterface>();

		Rml::SetFileInterface(FileInterface.get());
		Rml::SetRenderInterface(RenderInterface.get());
		Rml::SetSystemInterface(SystemInterface.get());

		Rml::Initialise();
	}

	// Bind Lua.
	{
		Script = script_manager->CreateScriptInstance("UI");
		Rml::SolLua::Initialise(&Script->GetLuaState(), "MODULENAME");

		// Bind the game scripts.
		Script->BindIntoMe(
			&bind_client
		);

		auto& state = Script->GetLuaState();
		state["MODULENAME"] = "UI";

		auto game = BabyDI::Get<tdrp::Game>();
		state["Game"] = game;

		log::PrintLine("[UI] Loaded Lua into UI.");

		// Load the UI bindings into the game's script state.
		Rml::SolLua::RegisterLua(&game->Script->GetLuaState());
		log::PrintLine("[UI] Loaded Lua into Game.");
	}
}

UIManager::~UIManager()
{
	// Rml::Debugger::Shutdown();
	Rml::Shutdown();
}

Rml::Context* UIManager::CreateContext(const std::string& name)
{
	// Convert to RmlUi size.
	Rml::Vector2i size{ window->GetWidth(), window->GetHeight() };

	auto context = Rml::CreateContext(name, size, RenderInterface.get());
	if (!context) return nullptr;

	// Bind Useables.
	{
		if (auto constructor = context->CreateDataModel("useables"))
		{
			auto mhandle = constructor.GetModelHandle();
			m_useable_handles.insert(std::make_pair(name, mhandle));

			if (auto uhandle = constructor.RegisterStruct<useable::Useable>())
			{
				uhandle.RegisterMember("name", &useable::Useable::Name);
				uhandle.RegisterMember("description", &useable::Useable::Description);
				uhandle.RegisterMember("image", &useable::Useable::Image);
			}

			constructor.RegisterArray<UseablesList>();

			auto game = BabyDI::Get<tdrp::Game>();
			constructor.Bind("useables", &game->GetUseables());
		}
	}

	return context;
}

void UIManager::ToggleContextVisibility(const std::string& name, bool visible)
{
	if (visible)
		m_visible_contexts.insert(name);
	else
		m_visible_contexts.erase(name);
}

void UIManager::ToggleDocumentVisibility(const std::string& context, const std::string& document)
{
	auto c = Rml::GetContext(context);
	if (c != nullptr)
	{
		auto d = c->GetDocument(document);
		if (d)
		{
			if (d->IsVisible()) d->Hide();
			else d->Show();
		}
	}
}

void UIManager::ReloadUI()
{
	// Turn off the debugger so we don't get a million error messages.
	Rml::Debugger::SetContext(nullptr);

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
		Rml::Debugger::SetContext(context);
	}
}

void UIManager::AssignDebugger(const std::string& context)
{
	auto c = Rml::GetContext(context);
	if (c == nullptr)
		return;

	Rml::Debugger::Shutdown();
	Rml::Debugger::Initialise(c);
}

void UIManager::MakeUseablesDirty()
{
	for (auto& [k, v] : m_useable_handles)
	{
		v.DirtyVariable("useables");
	}
}

void UIManager::ScreenSizeUpdate()
{
	Rml::Vector2i size{ window->GetWidth(), window->GetHeight() };

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
