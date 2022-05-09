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
	int32_t version = 1;

	// Get our contexts.xml file.
	auto contexts = game->Server.FileSystem.GetFile("contexts.xml");
	if (!contexts) return nullptr;

	// Load our file.
	{
		pugi::xml_document infodoc;
		infodoc.load(*contexts);

		// Determine level version.
		auto n_contexts = infodoc.child("contexts");
		if (n_contexts)
			version = n_contexts.attribute("version").as_int(1);

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
			std::string name = context.attribute("name").as_string();
			if (name.empty())
				continue;

			auto c = manager->CreateContext(name);
			if (!c) continue;

			// Load documents into the context.
			for (auto& document : context.children("document"))
			{
				std::string file = document.attribute("file").as_string();
				if (file.empty()) continue;

				auto d = c->LoadDocument(file);
				d->Show();

				log::PrintLine("[UI] Loaded document \"{}\".", file);
				if (!document.attribute("show").as_bool(false))
				{
					d->Hide();
				}
				else
				{
					log::PrintLine("[UI] Setting document to visible.");
				}
			}
		}
	}

	return manager;
}

} // end namespace tdrp::loader
