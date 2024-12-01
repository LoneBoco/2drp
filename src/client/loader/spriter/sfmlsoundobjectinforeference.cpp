#include "client/loader/spriter/sfmlsoundobjectinforeference.h"

namespace tdrp::loader::spriter
{

SfmlSoundObjectInfoReference::SfmlSoundObjectInfoReference(sf::SoundBuffer &buffer)
{
	sound = std::make_optional(sf::Sound{ buffer });
}

void SfmlSoundObjectInfoReference::playTrigger()
{
	if (sound && getTriggerCount())
	{
		sound.value().play();
	}
}

void SfmlSoundObjectInfoReference::setPlaybackVolume()
{
	if (sound.has_value())
		sound.value().setVolume(static_cast<float>(100 * getVolume()));
}

void SfmlSoundObjectInfoReference::setPlaybackPanning()
{
	if (sound.has_value())
		sound.value().setPosition({ static_cast<float>(100 * getPanning()), 0, 0 });
}

}
