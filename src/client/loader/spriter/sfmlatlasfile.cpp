#include "client/loader/spriter/sfmlatlasfile.h"

#include <spriterengine/global/settings.h>

#include "client/game/Game.h"
#include "client/render/resource/SFMListream.h"

#include "engine/common.h"
#include "engine/resources/Resource.h"
#include "engine/filesystem/File.h"

namespace tdrp::loader::spriter
{

SfmlAtlasFile::SfmlAtlasFile(std::string initialFilePath) :
	AtlasFile(initialFilePath), m_loaded(false)
{
	initializeFile();
}

const sf::Texture* SfmlAtlasFile::getTexture() const
{
	if (auto t = texture.lock())
		return t.get();
	return nullptr;
}

void SfmlAtlasFile::initializeFile()
{
	m_loaded = false;

	auto game = BabyDI::Get<tdrp::Game>();
	auto resources = BabyDI::Get<tdrp::ResourceManager>();

	filesystem::path imagepath{ path() };

	// Set the texture.
	auto filename = imagepath.filename();
	auto id = resources->FindId<sf::Texture>(filename.string());
	if (id == 0)
	{
		auto base = game->Server.GetPackage()->GetBasePath();
		auto file = game->Server.FileSystem.GetFile(base / imagepath);
		if (file == nullptr)
			file = game->Server.FileSystem.GetFile(filename);

		if (file)
		{
			auto texture = std::make_shared<sf::Texture>();

			render::SFMListream stream(*file);
			texture->loadFromStream(stream);

			id = resources->Add(filename.string(), std::move(texture));
		}
	}
	if (id == 0)
	{
		SpriterEngine::Settings::Settings::error("SfmlImageFile::initializeFile - sfml texture unable to load file from path \"" + path() + "\"");
		return;
	}

	texture = resources->Get<sf::Texture>(id);
	m_loaded = true;
}

void SfmlAtlasFile::renderSprite(SpriterEngine::UniversalObjectInterface *spriteInfo, const SpriterEngine::atlasframedata data)
{
}

}
