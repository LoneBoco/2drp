#include <RmlUi/Lua.h>
#include <RmlUi/Debugger.h>
#include <RmlSolLua/RmlSolLua.h>

#include "client/ui/UIManager.h"
#include "client/ui/interface/RmlUi_Backend.h"

#include "client/game/Game.h"
#include "client/render/Window.h"

#include "client/script/Script.h"
#include "engine/script/modules/bind.h"


namespace tdrp::ui
{

	using item_container = decltype(server::Account::Items);
	using item_type = item_container::value_type;


	struct MapValueVariableDefinition : public Rml::VariableDefinition
	{
		MapValueVariableDefinition(VariableDefinition* underlying_definition)
			: Rml::VariableDefinition(Rml::DataVariableType::Array), underlying_definition(underlying_definition) {}

		int Size(void* ptr) override
		{
			return int(static_cast<item_container*>(ptr)->size());
		}

		Rml::DataVariable Child(void* void_ptr, const Rml::DataAddressEntry& address) override
		{
			item_container* ptr = static_cast<item_container*>(void_ptr);
			const int index = address.index;

			const int container_size = int(ptr->size());
			if (index < 0 || index >= container_size)
			{
				if (address.name == "size")
					return Rml::MakeLiteralIntVariable(container_size);

				Rml::Log::Message(Rml::Log::LT_WARNING, "Data array index out of bounds.");
				return Rml::DataVariable();
			}

			auto it = ptr->begin();
			std::advance(it, index);

			// Return the value of the map.
			void* next_ptr = it->second.get();
			return Rml::DataVariable(underlying_definition, next_ptr);
		}

	private:
		VariableDefinition* underlying_definition;
	};


	struct ItemInstanceVariableDefinition : public Rml::VariableDefinition
	{
		ItemInstanceVariableDefinition(Rml::DataTypeRegister& type_register)
			: Rml::VariableDefinition(Rml::DataVariableType::Struct)
		{
			auto* def_itemid = type_register.GetDefinition<ItemID>();
			auto* def_type = type_register.GetDefinition<item::ItemType>();
			auto* def_string = type_register.GetDefinition<std::string>();
			auto* def_sizet = type_register.GetDefinition<size_t>();
			auto* def_tags = type_register.GetDefinition<item::tag_container>();
			auto* def_attributes = type_register.GetDefinition<ObjectAttributes>();

			m_definitions.insert( {
				{ "id", def_itemid },
				{ "baseid", def_itemid },
				{ "type", def_type },
				{ "name", def_string },
				{ "description", def_string },
				{ "image", def_string },
				{ "tags", def_tags },
				{ "count", def_sizet },
				{ "attributes", def_attributes }
			});
		}

		Rml::DataVariable Child(void* ptr, const Rml::DataAddressEntry& address) override
		{
			const auto& name = address.name;
			if (name.empty())
			{
				Rml::Log::Message(Rml::Log::LT_WARNING, "Expected a struct member name but none given.");
				return Rml::DataVariable();
			}

			auto* instance = static_cast<item::ItemInstance*>(ptr);

			if (name == "id") return Rml::DataVariable(m_definitions[name], &instance->ID);
			else if (name == "baseid") return Rml::DataVariable(m_definitions[name], &instance->ItemBaseID);
			else if (name == "type") return Rml::DataVariable(m_definitions[name], &instance->Type);
			else if (name == "name") return Rml::DataVariable(m_definitions[name], &instance->ItemBase->Name);
			else if (name == "description") return Rml::DataVariable(m_definitions[name], &instance->ItemBase->Description);
			else if (name == "image") return Rml::DataVariable(m_definitions[name], &instance->ItemBase->Image);
			else if (name == "tags") return Rml::DataVariable(m_definitions[name], &instance->ItemBase->Tags);
			else if (name == "count")
			{
				if (auto* stackable = dynamic_cast<item::ItemStackable*>(instance); stackable)
					return Rml::DataVariable(m_definitions[name], &stackable->Count);
			}
			else if (name == "attributes")
			{
				if (auto* variant = dynamic_cast<item::ItemVariant*>(instance); variant)
					return Rml::DataVariable(m_definitions[name], &variant->VariantAttributes);
			}

			Rml::Log::Message(Rml::Log::LT_WARNING, "Member %s not found in data struct.", name.c_str());
			return Rml::DataVariable();
		}

	private:
		std::unordered_map<std::string, Rml::VariableDefinition*> m_definitions;
	};

} // end namespace tdrp::ui


namespace tdrp::ui
{

UIManager::UIManager()
{
	// Bind Rml.
	{
		Backend::Initialize(window->GetRenderWindow());

		FileInterface = std::make_unique<ShellFileInterface>();
		SystemInterface = Backend::GetSystemInterface();
		RenderInterface = dynamic_cast<RenderInterface_GL2*>(Backend::GetRenderInterface());

		Rml::SetFileInterface(FileInterface.get());
		Rml::SetSystemInterface(Backend::GetSystemInterface());
		Rml::SetRenderInterface(Backend::GetRenderInterface());

		Rml::Initialise();
	}

	// Bind Lua.
	{
		Script = script_manager->CreateScriptInstance("UI");
		Rml::SolLua::Initialise(&Script->GetLuaState(), "MODULENAME");

		// Bind the game scripts.
		auto game = BabyDI::Get<tdrp::Game>();
		Script->BindIntoMe(
			&bind_client,
			[this, &game](sol::state& lua) {
				lua["MODULENAME"] = "UI";
				lua["Game"] = game;
			}
		);

		log::PrintLine("[UI] Loaded Lua into UI.");

		// Load the UI bindings into the game's script state.
		Rml::SolLua::RegisterLua(&game->Script->GetLuaState());
		log::PrintLine("[UI] Loaded Lua into Game.");
	}

	// Create debugger context.
	{
		m_visible_contexts.emplace("debugger");
		Rml::Vector2i size{ window->GetWidth(), window->GetHeight() };
		m_debugger_host = Rml::CreateContext("debugger", size, RenderInterface);
		Rml::Debugger::Initialise(m_debugger_host);
	}
}

UIManager::~UIManager()
{
	// Garbage collect one final time to clean up any unique_ptrs sitting around.
	// If we don't do this, Rml crashes during shutdown.
	Script->CollectGarbage();

	// Pray.
	Rml::Shutdown();
	Backend::Shutdown();

	// Erase script instance.
	script_manager->EraseScriptInstance("UI");
}


Rml::Context* UIManager::CreateContext(const std::string& name)
{
	// Convert to RmlUi size.
	Rml::Vector2i size{ window->GetWidth(), window->GetHeight() };

	auto context = Rml::CreateContext(name, size, RenderInterface);
	if (!context) return nullptr;
	m_managed_contexts.push_back(context);

	return context;
}

void UIManager::BindDataModels(Rml::Context* context)
{
	// Bind Items.
	{
		if (auto constructor = context->CreateDataModel("items"))
		{
			auto mhandle = constructor.GetModelHandle();
			m_item_handles.insert(std::make_pair(context->GetName(), mhandle));

			constructor.RegisterScalar<item::ItemType>(
				[](const auto& type, auto& variant) { variant = static_cast<uint8_t>(type); },
				[](auto& type, const auto& variant) { type = static_cast<item::ItemType>(variant.Get<uint8_t>(0)); }
			);

			// Attributes.
			{
				if (auto sthandle = constructor.RegisterStruct<Attribute>())
				{
					sthandle.RegisterMember("id", &Attribute::ID);
					sthandle.RegisterMember("name", &Attribute::Name);
					sthandle.RegisterMember("integer", static_cast<int64_t(Attribute::*)()>(&Attribute::GetAs));
					sthandle.RegisterMember("int", static_cast<int64_t(Attribute::*)()>(&Attribute::GetAs));
					sthandle.RegisterMember("double", static_cast<double(Attribute::*)()>(&Attribute::GetAs));
					sthandle.RegisterMember("float", static_cast<double(Attribute::*)()>(&Attribute::GetAs));
					sthandle.RegisterMember("number", static_cast<double(Attribute::*)()>(&Attribute::GetAs));
					sthandle.RegisterMember("real", static_cast<double(Attribute::*)()>(&Attribute::GetAs));
					sthandle.RegisterMember("string", static_cast<std::string(Attribute::*)()>(&Attribute::GetAs));
				}
				constructor.RegisterArray<ObjectAttributes>();
			}

			// Item definitions.
			{
				constructor.RegisterArray<item::tag_container>();
				if (auto sthandle = constructor.RegisterStruct<item::ItemDefinition>())
				{
					sthandle.RegisterMember("baseid", &item::ItemDefinition::BaseID);
					sthandle.RegisterMember("name", &item::ItemDefinition::Name);
					sthandle.RegisterMember("description", &item::ItemDefinition::Description);
					sthandle.RegisterMember("image", &item::ItemDefinition::Image);
					sthandle.RegisterMember("tags", &item::ItemDefinition::Tags);
				}
			}

			// Item instance.
			{
				auto* type_register = constructor.GetTypeRegister();
				auto iteminstancedef = std::make_unique<ItemInstanceVariableDefinition>(*type_register);
				constructor.RegisterCustomDataVariableDefinition<item::ItemInstance>(std::move(iteminstancedef));
			}

			// Transforms.
			{
				constructor.RegisterTransformFunc("stackable", [](const Rml::VariantList& arguments) -> Rml::Variant
					{
						if (arguments.empty()) return {};
						auto* instance = arguments[0].Get<item::ItemInstance*>();
						if (const auto stackable = dynamic_cast<const item::ItemStackable*>(instance); stackable != nullptr)
							return Rml::Variant(stackable);
						return {};
					});

				constructor.RegisterTransformFunc("variant", [](const Rml::VariantList& arguments) -> Rml::Variant
					{
						if (arguments.empty()) return {};
						auto* instance = arguments[0].Get<item::ItemInstance*>();
						if (const auto variant = dynamic_cast<const item::ItemVariant*>(instance); variant != nullptr)
							return Rml::Variant(variant);
						return {};
					});
			}

			// Item map.
			{
				auto* type_register = constructor.GetTypeRegister();
				auto* item_instance_def = type_register->GetDefinition<item::ItemInstance>();

				auto mapvaluedef = std::make_unique<MapValueVariableDefinition>(item_instance_def);
				constructor.RegisterCustomDataVariableDefinition<item_container>(std::move(mapvaluedef));
			}

			auto game = BabyDI::Get<tdrp::Game>();
			constructor.Bind("items", &game->Server.GetPlayer()->Account.Items);
		}
	}
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
	}

	// Add the debugger back in.
	if (m_debugger_context != nullptr)
		Rml::Debugger::SetContext(m_debugger_context);
}

void UIManager::AssignDebugger(const std::string& context)
{
	auto c = Rml::GetContext(context);
	if (c == nullptr)
		return;

	m_debugger_context = c;
	Rml::Debugger::SetContext(m_debugger_context);
}

void UIManager::MakeItemsDirty()
{
	for (auto& [k, v] : m_item_handles)
	{
		v.DirtyVariable("items");
	}
}

void UIManager::ScreenSizeUpdate()
{
	Rml::Vector2i size{ window->GetWidth(), window->GetHeight() };

	for (int i = 0; i < Rml::GetNumContexts(); ++i)
		Rml::GetContext(i)->SetDimensions(size);

	RenderInterface->SetViewport(size.x, size.y);
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
	assert(RenderInterface);

	RenderInterface->BeginFrame();

	// Render back to front.
	// We want the debugger to render last.
	for (int i = Rml::GetNumContexts(); i > 0; --i)
	{
		auto context = Rml::GetContext(i - 1);
		if (!context) continue;

		if (m_visible_contexts.contains(context->GetName()))
			context->Render();
	}

	RenderInterface->EndFrame();
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

		if (!result)
			break;
	}

	return !result;
}

} // end namespace tdrp::ui
