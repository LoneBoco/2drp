#include "client/loader/ui/UILoader.h"
#include "client/ui/UIManager.h"

#include "client/game/Game.h"
#include "engine/filesystem/File.h"
#include "engine/filesystem/FileSystem.h"
#include "engine/filesystem/Log.h"

#include <pugixml.hpp>

#include <boost/range/algorithm_ext.hpp>


namespace tdrp::loader
{

std::unique_ptr<ui::UIManager> UILoader::CreateUIManager()
{
	auto game = BabyDI::Get<Game>();
	auto manager = std::make_unique<ui::UIManager>();

	// Get our contexts.xml file.
	auto contexts = game->Server.FileSystem.GetFile("contexts.xml");
	if (!contexts) return nullptr;

	// Load our file.
	{
		pugi::xml_document infodoc;
		infodoc.load(*contexts);

		auto n_contexts = infodoc.child("contexts");
		if (n_contexts.empty()) return manager;

		// Determine level version.
		auto version = n_contexts.attribute("version").as_int(1);

		// Load our fonts.
		for (auto& fonts : n_contexts.children("font"))
		{
			std::string file = fonts.attribute("file").as_string();
			if (!file.empty())
			{
				auto fallback = fonts.attribute("fallback").as_bool(false);
				Rml::LoadFontFace(file, fallback);
			}
		}

		// Load our contexts.
		for (auto& context : n_contexts.children("context"))
		{
			// Don't load any contexts that have a load-on attribute set.
			auto load_on = context.attribute("load-on");
			if (!load_on.empty())
				continue;

			// Load the context.
			LoadContext(context, manager.get());
		}
	}

	return manager;
}

size_t UILoader::Load(ui::UIManager* manager, std::string_view load_on)
{
	size_t count = 0;

	// Get our contexts.xml file.
	auto game = BabyDI::Get<Game>();
	auto contexts = game->Server.FileSystem.GetFile("contexts.xml");
	if (!contexts) return 0;

	// Load our file.
	{
		pugi::xml_document infodoc;
		infodoc.load(*contexts);

		auto n_contexts = infodoc.child("contexts");
		if (n_contexts.empty()) return 0;

		// Determine level version.
		auto version = n_contexts.attribute("version").as_int(1);

		// Load our contexts.
		for (auto& context : n_contexts.children("context"))
		{
			// If we don't have a load-on or if it doesn't match, ignore.
			auto attrloadon = context.attribute("load-on");
			if (attrloadon.empty() || std::string_view{ attrloadon.as_string() } != load_on)
				continue;

			// Load the context.
			LoadContext(context, manager);
			++count;
		}
	}

	return count;
}

void UILoader::LoadContext(pugi::xml_node& node, ui::UIManager* manager)
{
	std::string name = node.attribute("name").as_string();
	if (name.empty()) return;

	auto context = manager->CreateContext(name);
	if (!context) return;

	// Load our data models here.
	// We have to do this before we load the document.
	if (!node.attribute("load-on").empty())
		manager->BindDataModels(context);

	// Load documents into the context.
	for (auto& node_document : node.children("document"))
	{
		std::string file = node_document.attribute("file").as_string();
		if (file.empty()) continue;

		auto document = context->LoadDocument(file);
		document->Show();

		log::PrintLine("[UI] Loaded document \"{}\".", file);
		if (!node_document.attribute("show").as_bool(false))
		{
			document->Hide();
		}
		else
		{
			log::PrintLine("[UI] Setting document to visible.");
		}
	}
}

} // end namespace tdrp::loader
