#include "client/loader/spriter/sfmlsoundfile.h"

#include <spriterengine/global/settings.h>

#include "client/loader/spriter/sfmlsoundobjectinforeference.h"

#include "client/game/Game.h"
#include "client/render/resource/SFMListream.h"

#include "engine/common.h"
#include "engine/resources/Resource.h"
#include "engine/filesystem/File.h"

namespace tdrp::loader::spriter
{

SfmlSoundFile::SfmlSoundFile(std::string initialFilePath) :
	SoundFile(initialFilePath)
{
	initializeFile();
}

void SfmlSoundFile::initializeFile()
{
	auto game = BabyDI::Get<tdrp::Game>();
	auto resources = BabyDI::Get<tdrp::ResourceManager>();

	filesystem::path imagepath{ path() };

	auto filename = imagepath.filename();
	auto id = resources->FindId<sf::SoundBuffer>(filename.string());
	if (id == 0)
	{
		auto base = game->Server.GetPackage()->GetBasePath();
		auto file = game->Server.FileSystem.GetFile(base / imagepath);
		if (file == nullptr)
			file = game->Server.FileSystem.GetFile(filename);

		if (file)
		{
			auto sound = std::make_shared<sf::SoundBuffer>();

			render::SFMListream stream(*file);
			sound->loadFromStream(stream);

			id = resources->Add(filename.string(), std::move(sound));
		}
	}
	if (id == 0)
	{
		SpriterEngine::Settings::Settings::error("SfmlSoundFile::initializeFile - sfml sound buffer unable to load file from path \"" + path() + "\"");
		return;
	}

	auto handle = resources->Get<sf::SoundBuffer>(id);
	if (auto h = handle.lock())
		buffer = h;
	else SpriterEngine::Settings::Settings::error("SfmlSoundFile::initializeFile - sound handle could not be locked");
}

SpriterEngine::SoundObjectInfoReference * SfmlSoundFile::newSoundInfoReference()
{
	return new SfmlSoundObjectInfoReference(*buffer);
}

}
