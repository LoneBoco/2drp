#pragma once

#include <spriterengine/override/imagefile.h>

#include <SFML/Graphics.hpp>

namespace tdrp::loader::spriter
{

class SfmlImageFile : public SpriterEngine::ImageFile
{
public:
	SfmlImageFile(std::string initialFilePath, SpriterEngine::point initialDefaultPivot, sf::RenderWindow *validRenderWindow);

	void renderSprite(SpriterEngine::UniversalObjectInterface *spriteInfo) override;

	void setAtlasFile(SpriterEngine::AtlasFile *initialAtlasFile, SpriterEngine::atlasframedata initialAtlasFrameData) override;

private:
	sf::Texture texture;
	sf::Sprite sprite;

	sf::RenderWindow *renderWindow;

	void initializeFile();
};

}
