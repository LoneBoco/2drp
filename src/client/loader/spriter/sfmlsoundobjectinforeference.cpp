#include "client/loader/spriter/sfmlsoundobjectinforeference.h"

namespace tdrp::loader::spriter
{

SfmlSoundObjectInfoReference::SfmlSoundObjectInfoReference(sf::SoundBuffer &buffer)
{
	sound.setBuffer(buffer);
}

void SfmlSoundObjectInfoReference::playTrigger()
{
	if (getTriggerCount())
	{
		sound.play();
	}
}

void SfmlSoundObjectInfoReference::setPlaybackVolume()
{
	sound.setVolume(static_cast<float>(100 * getVolume()));
}

void SfmlSoundObjectInfoReference::setPlaybackPanning()
{
	sound.setPosition({ static_cast<float>(100 * getPanning()), 0, 0 });
}

}
