#include "client/loader/spriter/sfmlimagefile.h"

#include <spriterengine/global/settings.h>
#include <spriterengine/objectinfo/universalobjectinterface.h>

#include "client/loader/spriter/sfmlatlasfile.h"

#include "client/game/Game.h"
#include "client/loader/SFMListream.h"

#include "engine/common.h"
#include "engine/resources/Resource.h"
#include "engine/filesystem/File.h"

namespace tdrp::loader::spriter
{

SfmlImageFile::SfmlImageFile(std::string initialFilePath, SpriterEngine::point initialDefaultPivot, sf::RenderWindow *validRenderWindow) :
	ImageFile(initialFilePath,initialDefaultPivot),
	renderWindow(validRenderWindow)
{
	initializeFile();
}

void SfmlImageFile::initializeFile()
{
	auto game = BabyDI::Get<tdrp::Game>();
	auto resources = BabyDI::Get<tdrp::ResourceManager>();

	/*
	auto ps = path();
	auto atlasfile = ps.find('$');

	// If this is an atlas file, pull off the index.
	// The index is just to keep the image separated in the Spriter system.
	filesystem::path imagepath;
	if (atlasfile == std::string::npos)
		imagepath = ps;
	else imagepath = ps.substr(0, atlasfile);
	*/

	filesystem::path imagepath{ path() };

	// Set the texture.
	auto filename = imagepath.filename();
	auto id = resources->FindId<sf::Texture>(filename.string());
	if (id == 0)
	{
		auto base = game->Server.GetPackage()->GetBasePath();
		auto file = game->Server.FileSystem.GetFile(fs::FileCategory::ASSETS, filename);

		if (file)
		{
			auto texture = std::make_shared<sf::Texture>();

			loader::SFMListream stream(*file);
			auto success = texture->loadFromStream(stream);
			if (success)
			{
				id = resources->Add(filename.string(), std::move(texture));
			}
		}
	}
	if (id == 0)
	{
		SpriterEngine::Settings::Settings::error("SfmlImageFile::initializeFile - sfml texture unable to load file from path \"" + path() + "\"");
		return;
	}

	auto handle = resources->Get<sf::Texture>(id);
	if (auto h = handle.lock())
		sprite = std::make_optional<sf::Sprite>(*h);
	else SpriterEngine::Settings::Settings::error("SfmlImageFile::initializeFile - texture handle could not be locked");
}

void SfmlImageFile::renderSprite(SpriterEngine::UniversalObjectInterface * spriteInfo)
{
	if (!sprite.has_value())
		return;

	auto& sp = sprite.value();
	sp.setColor(sf::Color(255, 255, 255, static_cast<uint8_t>(255.0 * spriteInfo->getAlpha())));

	if(atlasFile) {
		// Adding of transformations is in the reverse order you would expect.
		sf::Transform t;
		if(atlasFrameData.rotated) {
			t.translate({ static_cast<float>(spriteInfo->getPosition().x), static_cast<float>(spriteInfo->getPosition().y) });
			t.rotate(sf::radians(static_cast<float>(spriteInfo->getAngle())));
			t.scale({ static_cast<float>(spriteInfo->getScale().x), static_cast<float>(spriteInfo->getScale().y) });
			t.translate({ static_cast<float>(-spriteInfo->getPivot().x * atlasFrameData.sourceSize.x), static_cast<float>(-spriteInfo->getPivot().y * atlasFrameData.sourceSize.y) });
			// Turn it because the image is rotated
			t.rotate(sf::degrees(-90.0f));
			// Translate the cutoff in atlas (spriteSourcePosition is the position of the atlas in the original == offset)
			// X and Y swapped
			// Also translate it the height (sourceSize.y) up so that the left-up corner of the original image is at the x0 y0.
			t.translate({ static_cast<float>(atlasFrameData.spriteSourcePosition.y - atlasFrameData.sourceSize.y), static_cast<float>(atlasFrameData.spriteSourcePosition.x) });
		}
		else {
			t.translate({ static_cast<float>(spriteInfo->getPosition().x), static_cast<float>(spriteInfo->getPosition().y) });
			t.rotate(sf::radians(static_cast<float>(spriteInfo->getAngle())));
			t.scale({ static_cast<float>(spriteInfo->getScale().x), static_cast<float>(spriteInfo->getScale().y) });
			// Translate the cutoff in atlas (spriteSourcePosition is the position of the atlas in the original == offset)
			// Also translate the pivot at onces (saves an extra instruction)
			t.translate({ static_cast<float>(atlasFrameData.spriteSourcePosition.x - spriteInfo->getPivot().x * atlasFrameData.sourceSize.x),
						  static_cast<float>(atlasFrameData.spriteSourcePosition.y - spriteInfo->getPivot().y * atlasFrameData.sourceSize.y) });
		}
		renderWindow->draw(sp, t);
	}
	else {
		sp.setPosition({ static_cast<float>(spriteInfo->getPosition().x), static_cast<float>(spriteInfo->getPosition().y) });
		sp.setRotation(sf::radians(static_cast<float>(spriteInfo->getAngle())));
		sp.setScale({ static_cast<float>(spriteInfo->getScale().x), static_cast<float>(spriteInfo->getScale().y) });
		sp.setOrigin({ static_cast<float>(spriteInfo->getPivot().x * texture.getSize().x), static_cast<float>(spriteInfo->getPivot().y * texture.getSize().y) });
		renderWindow->draw(sp);
	}
}

// Overwritten so we can create the sprite from the texture.
void SfmlImageFile::setAtlasFile(SpriterEngine::AtlasFile *initialAtlasFile, SpriterEngine::atlasframedata initialAtlasFrameData) {
	// First call baseclass implementation
	ImageFile::setAtlasFile(initialAtlasFile, initialAtlasFrameData);

	if (!sprite.has_value())
		return;
	auto& sp = sprite.value();

	// atlasFile should be a SfmlAtlasFile
	if(atlasFile && static_cast<SfmlAtlasFile*>(atlasFile)->loaded()) {
		const sf::Texture *texture = static_cast<SfmlAtlasFile*>(atlasFile)->getTexture();
		sp.setTexture(*texture);
		if(atlasFrameData.rotated) {
			// When rotated, the atlasdata framesize are relative to the original. Not the frame
			sp.setTextureRect({ { static_cast<int>(atlasFrameData.framePosition.x), static_cast<int>(atlasFrameData.framePosition.y) },
									{ static_cast<int>(atlasFrameData.frameSize.y), static_cast<int>(atlasFrameData.frameSize.x) } });
		}
		else {
			sp.setTextureRect({ { static_cast<int>(atlasFrameData.framePosition.x), static_cast<int>(atlasFrameData.framePosition.y) },
									{ static_cast<int>(atlasFrameData.frameSize.x), static_cast<int>(atlasFrameData.frameSize.y) } });
		}
	}
}

}
