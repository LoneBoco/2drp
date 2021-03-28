#include "client/loader/spriter/sfmlimagefile.h"

#include <spriterengine/global/settings.h>
#include <spriterengine/objectinfo/universalobjectinterface.h>

#include "client/loader/spriter/sfmlatlasfile.h"

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
	if (texture.loadFromFile(path()))
	{
		// texture.setSmooth(true);
		sprite.setTexture(texture);
	}
	else
	{
		SpriterEngine::Settings::Settings::error("SfmlImageFile::initializeFile - sfml texture unable to load file from path \"" + path() + "\"");
	}
}

void SfmlImageFile::renderSprite(SpriterEngine::UniversalObjectInterface * spriteInfo)
{
	sprite.setColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(255.0 * spriteInfo->getAlpha())));

	if(atlasFile) {
		// Adding of transformations is in the reverse order you would expect.
		sf::Transform t;
		if(atlasFrameData.rotated) {
			t.translate(static_cast<float>(spriteInfo->getPosition().x), static_cast<float>(spriteInfo->getPosition().y));
			t.rotate(static_cast<float>(SpriterEngine::toDegrees(spriteInfo->getAngle())));
			t.scale(static_cast<float>(spriteInfo->getScale().x), static_cast<float>(spriteInfo->getScale().y));
			t.translate(static_cast<float>(-spriteInfo->getPivot().x * atlasFrameData.sourceSize.x), static_cast<float>(-spriteInfo->getPivot().y * atlasFrameData.sourceSize.y));
			// Turn it because the image is rotated
			t.rotate(-90.0f);
			// Translate the cutoff in atlas (spriteSourcePosition is the position of the atlas in the original == offset)
			// X and Y swapped
			// Also translate it the height (sourceSize.y) up so that the left-up corner of the original image is at the x0 y0.
			t.translate(static_cast<float>(atlasFrameData.spriteSourcePosition.y - atlasFrameData.sourceSize.y), static_cast<float>(atlasFrameData.spriteSourcePosition.x));
		}
		else {
			t.translate(static_cast<float>(spriteInfo->getPosition().x), static_cast<float>(spriteInfo->getPosition().y));
			t.rotate(static_cast<float>(SpriterEngine::toDegrees(spriteInfo->getAngle())));
			t.scale(static_cast<float>(spriteInfo->getScale().x), static_cast<float>(spriteInfo->getScale().y));
			// Translate the cutoff in atlas (spriteSourcePosition is the position of the atlas in the original == offset)
			// Also translate the pivot at onces (saves an extra instruction)
			t.translate(static_cast<float>(atlasFrameData.spriteSourcePosition.x - spriteInfo->getPivot().x * atlasFrameData.sourceSize.x),
						static_cast<float>(atlasFrameData.spriteSourcePosition.y - spriteInfo->getPivot().y * atlasFrameData.sourceSize.y));
		}
		renderWindow->draw(sprite, t);
	}
	else {
		sprite.setPosition(static_cast<float>(spriteInfo->getPosition().x), static_cast<float>(spriteInfo->getPosition().y));
		sprite.setRotation(static_cast<float>(SpriterEngine::toDegrees(spriteInfo->getAngle())));
		sprite.setScale(static_cast<float>(spriteInfo->getScale().x), static_cast<float>(spriteInfo->getScale().y));
		sprite.setOrigin(static_cast<float>(spriteInfo->getPivot().x * texture.getSize().x), static_cast<float>(spriteInfo->getPivot().y * texture.getSize().y));
		renderWindow->draw(sprite);
	}
}

// Overwritten so we can create the sprite from the texture.
void SfmlImageFile::setAtlasFile(SpriterEngine::AtlasFile *initialAtlasFile, SpriterEngine::atlasframedata initialAtlasFrameData) {
	// First call baseclass implementation
	ImageFile::setAtlasFile(initialAtlasFile, initialAtlasFrameData);

	// atlasFile should be a SfmlAtlasFile
	if(atlasFile && static_cast<SfmlAtlasFile*>(atlasFile)->loaded()) {
		const sf::Texture *texture = static_cast<SfmlAtlasFile*>(atlasFile)->getTexture();
		sprite.setTexture(*texture);
		if(atlasFrameData.rotated) {
			// When rotated, the atlasdata framesize are relative to the original. Not the frame
			sprite.setTextureRect(sf::IntRect(static_cast<int>(atlasFrameData.framePosition.x), static_cast<int>(atlasFrameData.framePosition.y),
												static_cast<int>(atlasFrameData.frameSize.y), static_cast<int>(atlasFrameData.frameSize.x)));
		}
		else {
			sprite.setTextureRect(sf::IntRect(static_cast<int>(atlasFrameData.framePosition.x), static_cast<int>(atlasFrameData.framePosition.y),
												static_cast<int>(atlasFrameData.frameSize.x), static_cast<int>(atlasFrameData.frameSize.y)));
		}
	}
}

}
