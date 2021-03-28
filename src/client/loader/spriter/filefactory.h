#pragma once

#include <spriterengine/override/filefactory.h>

#include "SFML/Graphics.hpp"

namespace tdrp::loader::spriter
{

class FileFactory : public SpriterEngine::FileFactory
{
public:
	FileFactory(sf::RenderWindow *validRenderWindow);
		
	SpriterEngine::ImageFile *newImageFile(const std::string &initialFilePath, SpriterEngine::point initialDefaultPivot, SpriterEngine::atlasdata atlasData) override;

	SpriterEngine::AtlasFile *newAtlasFile(const std::string &initialFilePath) override;

	SpriterEngine::SoundFile *newSoundFile(const std::string &initialFilePath) override;

	SpriterEngine::SpriterFileDocumentWrapper *newScmlDocumentWrapper() override;

	SpriterEngine::SpriterFileDocumentWrapper *newSconDocumentWrapper() override;

private:
	sf::RenderWindow *renderWindow;
};

}
