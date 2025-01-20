#include <set>
#include <boost/tokenizer.hpp>

#include "client/loader/gani/GaniLoader.h"
#include "client/game/Game.h"

#include "engine/filesystem/File.h"

namespace tdrp::loader
{

std::shared_ptr<GaniAnimation> GaniLoader::LoadGani(const filesystem::path& file)
{
	auto game = BabyDI::Get<tdrp::Game>();

	auto f = game->Server->FileSystem.GetFile(fs::FileCategory::ASSETS, file);
	if (f)
	{
		/*
		const std::string gani_version{ "GANI0001" };
		auto version_check = f->Read(8);
		if (gani_version.compare(0, std::string::npos, version_check.data(), 8) != 0)
			return nullptr;
		*/

		/*
		std::string SPRITES{ "sprites.png" };
		std::string HEAD{ "head0.png" };
		std::string BODY{ "body0.png" };
		std::string SWORD{ "sword0.png" };
		std::string SHIELD{ "shield0.png" };
		std::vector<std::string> ATTR{};
		ATTR.resize(20);
		*/

		auto animation = std::make_shared<GaniAnimation>();

		//int directions = 4;
		//bool is_looped = false;
		//bool is_continuous = false;

		//std::vector<SpriterEngine::Entity*> entities;
		//std::map<uint32_t, uint32_t> gani_ids;
		//std::vector<SSprite> sprites;
		//std::set<std::string> sounds;

		while (!f->Finished())
		{
			auto line = f->ReadLine();
			if (line.empty())
				continue;

			boost::char_separator<char> sep{ " " };
			boost::tokenizer tokens{ line, sep };

			auto token_iter = tokens.begin();
			auto definition = *token_iter;

			if (boost::iequals(definition, "SPRITE"))
			{
				// SPRITE index image x y w h

				std::istringstream str{ std::string{token_iter.base(), token_iter.end()} };

				uint32_t gani_index;
				std::string image;
				Recti source;

				str >> gani_index;
				str >> image;
				str >> source.pos.x;
				str >> source.pos.y;
				str >> source.size.x;
				str >> source.size.y;

				if (str.fail())
					continue;

				animation->Sprites[gani_index] = { image, source };

				// Record the image information so we can build an atlas later.
				/*
				uint32_t new_index = sprites.size();
				sprites.push_back({ new_index, gani_index, image, source });
				gani_ids[gani_index] = new_index;
				*/
			}
			else if (boost::iequals(definition, "LOOP"))
			{
				animation->IsLooped = true;
			}
			else if (boost::iequals(definition, "CONTINUOUS"))
			{
				animation->IsContinuous = true;
			}
			else if (boost::iequals(definition, "SINGLEDIRECTION"))
			{
				animation->Directions = 1;
			}
			else if (boost::iequals(definition, "SETBACKTO"))
			{
				animation->Setbackto = std::string{ token_iter.base(), token_iter.end() };
				boost::trim(animation->Setbackto);
				if (animation->Setbackto.find(".gani") == std::string::npos)
					animation->Setbackto += ".gani";
			}
			else if (boost::iequals(definition, "COLOREFFECT"))
			{
				// COLOREFFECT index r g b a
			}
			else if (boost::iequals(definition, "ZOOMEFFECT"))
			{
				// ZOOMEFFECT index value
			}
			else if (boost::iequals(definition, "SCRIPT"))
			{
				assert(true);
				// SCRIPTEND
			}
			else if (boost::iequals(definition, "DEFAULTHEAD"))
			{
				//HEAD = std::string{ token_iter.base(), token_iter.end() };
			}
			else if (boost::iequals(definition, "DEFAULTBODY"))
			{
				//BODY = std::string{ token_iter.base(), token_iter.end() };
			}
			else if (boost::iequals(definition, "DEFAULTSWORD"))
			{
				//SWORD = std::string{ token_iter.base(), token_iter.end() };
			}
			else if (boost::iequals(definition, "DEFAULTSHIELD"))
			{
				//SHIELD = std::string{ token_iter.base(), token_iter.end() };
			}
			else if (boost::icontains(definition, "DEFAULTATTR"))
			{
				/*
				std::string attrindex = definition.erase(0, sizeof("DEFAULTATTR"));
				uint32_t index = 1;

				std::istringstream str{ attrindex };
				str >> index;

				if (str.fail())
					continue;

				if (index > ATTR.size())
					continue;

				ATTR[index - 1] = std::string{ token_iter.base(), token_iter.end() };
				*/
			}
			else if (boost::iequals(definition, "ANI"))
			{
				// Break so we can read the rest of the animation data.
				break;
			}
		}

		//std::list<SFrame> frames;
		//SFrame* current_frame = nullptr;
		int current_direction = animation->Directions;
		std::set<uint32_t> used_sprites;

		// Read all the animation data.
		while (!f->Finished())
		{
			auto line = f->ReadLine();
			if (line.empty())
				continue;

			boost::char_separator<char> sep{ " ," };
			boost::tokenizer tokens{ line, sep };

			auto token_iter = tokens.begin();
			auto definition = *token_iter;

			if (boost::iequals(definition, "ANIEND"))
				break;

			else if (boost::iequals(definition, "WAIT"))
			{
				// WAIT periods

				// Ganis use delays in increments of 50ms.  The period is a number of how many delays to perform.
				// 0 = 50ms, 1 = 100ms, 2 = 150ms, ...

				std::istringstream str{ std::string{token_iter.base(), token_iter.end()} };

				int periods = 0;
				str >> periods;

				if (str.fail())
					continue;

				if (animation->Frames.empty())
					continue;

				animation->Frames.back().Delay = 50 * (periods + 1);
			}
			else if (boost::iequals(definition, "PLAYSOUND"))
			{
				// PLAYSOUND sound x y

				std::istringstream str{ std::string{token_iter.base(), token_iter.end()} };

				std::string sound;
				float x;
				float y;

				str >> sound;
				str >> x;
				str >> y;

				if (str.fail())
					continue;

				if (animation->Frames.empty())
					continue;

				animation->Frames.back().Sounds.emplace_back(sound, x * 16, y * 16);
				animation->Sounds.insert(sound);
			}
			// Animation data.
			else
			{
				if (current_direction >= animation->Directions)
				{
					animation->Frames.emplace_back(50);
					current_direction = 0;

					animation->Frames.back().Sprites.resize(animation->Directions);
				}

				uint32_t index;
				int x;
				int y;

				// Reset the tokenizer.
				tokens = boost::tokenizer{ line, sep };
				for (auto token = tokens.begin(); token != tokens.end(); )
				{
					std::istringstream str{ *token++ };
					str >> index;

					str.str(*token++);
					str.clear();
					str >> x;

					str.str(*token++);
					str.clear();
					str >> y;

					auto& current_frame_sprites = animation->Frames.back().Sprites[current_direction];
					current_frame_sprites.emplace_back(index, x, y);
					used_sprites.insert(index);
				}

				++current_direction;
			}
		}

		// Optimize - Remove unused sprites.
		std::erase_if(animation->Sprites,
			[&](decltype(animation->Sprites)::const_reference pair) -> bool
			{
				if (!used_sprites.contains(pair.first))
					return true;
				return false;
			}
		);

		return animation;
	}

	return nullptr;
}

} // end namespace tdrp::loader
