#include "client/loader/ui/UILoader.h"
#include "client/ui/UIManager.h"

#include "client/game/Game.h"
#include "engine/filesystem/File.h"
#include "engine/filesystem/FileSystem.h"
#include "engine/filesystem/Log.h"

#include <pugixml.hpp>

#include <boost/range/algorithm_ext.hpp>


namespace tdrp::loader::helper
{
	static void loadContext(pugi::xml_node& node, ui::UIManager* manager, std::vector<ui::UIContextDataDocument>& documents)
	{
		// Load documents into the context.
		for (auto& node_document : node.children("document"))
		{
			std::string file = node_document.attribute("file").as_string();
			if (file.empty()) continue;

			bool show = node_document.attribute("show").as_bool(false);
			documents.emplace_back(ui::UIContextDataDocument{ .ShowOnLoad = show, .Filename = file });

			log::PrintLine("         Document: {}", file);
		}
	}
} // end namespace tdrp::loader::helper


namespace tdrp::loader
{

std::unique_ptr<ui::UIManager> UILoader::CreateUIManager()
{
	return std::make_unique<ui::UIManager>();
}

size_t UILoader::Load(ui::UIManager* manager)
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

		// Determine file version.
		auto version = n_contexts.attribute("version").as_int(1);

		log::PrintLine("[UI] Gathering UI data...");

		// Load our fonts.
		for (auto& fonts : n_contexts.children("font"))
		{
			std::string file = fonts.attribute("file").as_string();
			if (!file.empty())
			{
				auto fallback = fonts.attribute("fallback").as_bool(false);
				Rml::LoadFontFace(file, fallback);
				log::PrintLine("       Loaded font: {}", file);
			}
		}

		// Load our contexts.
		for (auto& context : n_contexts.children("context"))
		{
			std::string name = context.attribute("name").as_string();
			if (name.empty()) continue;

			ui::UIContextData data = {};
			if (auto attr = context.attribute("load-on"); !attr.empty())
				data.LoadOn = attr.as_string();

			log::PrintLine("       Registered context: {}", name);

			// Load the context.
			helper::loadContext(context, manager, data.Documents);

			manager->m_contexts.emplace(std::make_pair(name, std::move(data)));
			++count;
		}
	}

	return count;
}

} // end namespace tdrp::loader
