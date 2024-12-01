#pragma once

#include <spriterengine/override/soundobjectinforeference.h>

#include "SFML/Audio.hpp"

namespace tdrp::loader::spriter
{

class SfmlSoundObjectInfoReference : public SpriterEngine::SoundObjectInfoReference
{
public:
	SfmlSoundObjectInfoReference(sf::SoundBuffer &buffer);

	void playTrigger() override;

private:
	void setPlaybackVolume() override;
	void setPlaybackPanning() override;

	std::optional<sf::Sound> sound;
};

}
