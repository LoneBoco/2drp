#pragma once 

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
	sf::SoundBuffer buffer;

	void initializeFile();
};

}
