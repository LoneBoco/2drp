#pragma once 

#include <memory>

#include <spriterengine/override/soundfile.h>

#include "SFML/Audio.hpp"

namespace tdrp::loader::spriter
{

class SfmlSoundFile : public SpriterEngine::SoundFile
{
public:
	SfmlSoundFile(std::string initialFilePath);

	SpriterEngine::SoundObjectInfoReference * newSoundInfoReference();

private:
	std::shared_ptr<sf::SoundBuffer> buffer;

	void initializeFile();
};

}
