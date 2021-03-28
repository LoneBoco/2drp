#include "client/loader/spriter/sfmlsoundfile.h"

#include <spriterengine/global/settings.h>

#include "client/loader/spriter/sfmlsoundobjectinforeference.h"

namespace tdrp::loader::spriter
{

SfmlSoundFile::SfmlSoundFile(std::string initialFilePath) :
	SoundFile(initialFilePath)
{
	initializeFile();
}

void SfmlSoundFile::initializeFile()
{
	if (!buffer.loadFromFile(path()))
	{
		SpriterEngine::Settings::error("SfmlSoundFile::initializeFile - sfml sound buffer unable to load file from path \"" + path() + "\"");
	}
}

SpriterEngine::SoundObjectInfoReference * SfmlSoundFile::newSoundInfoReference()
{
	return new SfmlSoundObjectInfoReference(buffer);
}

}
