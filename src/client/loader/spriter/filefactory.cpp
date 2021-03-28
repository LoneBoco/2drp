#include "client/loader/spriter/filefactory.h"

#include <spriterengine/override/imagefile.h>
#include <spriterengine/override/soundfile.h>

#include "client/loader/spriter/pugixmlspriterfiledocumentwrapper.h"
#include "client/loader/spriter/jsonspriterfiledocumentwrapper.h"

#include "client/loader/spriter/sfmlimagefile.h"
#include "client/loader/spriter/sfmlatlasfile.h"
#include "client/loader/spriter/sfmlsoundfile.h"

namespace tdrp::loader::spriter
{

FileFactory::FileFactory(sf::RenderWindow *validRenderWindow) :
	renderWindow(validRenderWindow)
{
}

SpriterEngine::ImageFile * FileFactory::newImageFile(const std::string &initialFilePath, SpriterEngine::point initialDefaultPivot, SpriterEngine::atlasdata atlasData)
{
	return new SfmlImageFile(initialFilePath, initialDefaultPivot, renderWindow);
}

SpriterEngine::AtlasFile * FileFactory::newAtlasFile(const std::string &initialFilePath)
{
	return new SfmlAtlasFile(initialFilePath);

}

SpriterEngine::SoundFile * FileFactory::newSoundFile(const std::string & initialFilePath)
{
	return new SfmlSoundFile(initialFilePath);
}

SpriterEngine::SpriterFileDocumentWrapper * FileFactory::newScmlDocumentWrapper()
{
	return new PugiXmlSpriterFileDocumentWrapper();		
}

SpriterEngine::SpriterFileDocumentWrapper * FileFactory::newSconDocumentWrapper()
{
	return new JSONSpriterFileDocumentWrapper();
}

}
