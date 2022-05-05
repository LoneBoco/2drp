#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "engine/common.h"
#include "engine/resources/Resource.h"
#include "engine/filesystem/FileSystem.h"
#include "engine/filesystem/File.h"

#include "client/game/Game.h"
#include "client/loader/SFMListream.h"

namespace tdrp::loader
{

inline
std::size_t LoadSound(const filesystem::path& file)
{
	auto resources = BabyDI::Get<tdrp::ResourceManager>();
	auto game = BabyDI::Get<tdrp::Game>();

	auto filename = file.filename().string();
	auto id = resources->FindId<sf::SoundBuffer>(filename);
	if (id == 0)
	{
		// Sound buffer doesn't exist yet, load it.
		auto f = game->Server.FileSystem.GetFile(file);
		if (f)
		{
			auto buffer = std::make_shared<sf::SoundBuffer>();

			SFMListream stream(*f);
			auto success = buffer->loadFromStream(stream);
			if (success)
			{
				id = resources->Add(filename, std::move(buffer));
			}
		}
	}

	return id;
}

inline
std::size_t LoadTexture(const filesystem::path& file)
{
	auto resources = BabyDI::Get<tdrp::ResourceManager>();
	auto game = BabyDI::Get<tdrp::Game>();

	auto filename = file.filename().string();
	auto id = resources->FindId<sf::SoundBuffer>(filename);
	if (id == 0)
	{
		// Sound buffer doesn't exist yet, load it.
		auto f = game->Server.FileSystem.GetFile(file);
		if (f)
		{
			auto texture = std::make_shared<sf::Texture>();

			loader::SFMListream stream(*f);
			auto success = texture->loadFromStream(stream);
			if (success)
			{
				id = resources->Add(filename, std::move(texture));
			}
		}
	}

	return id;
}

} // end namespace tdrp::loader
