#include "client/loader/spriter/pugixmlspriterfiledocumentwrapper.h"
#include "client/loader/spriter/pugixmlspriterfileelementwrapper.h"

#include "engine/common.h"
#include "client/game/Game.h"
#include "engine/filesystem/File.h"


namespace tdrp::loader::spriter
{

PugiXmlSpriterFileDocumentWrapper::PugiXmlSpriterFileDocumentWrapper()
{
}

void PugiXmlSpriterFileDocumentWrapper::loadFile(std::string fileName)
{
	auto game = BabyDI::Get<tdrp::Game>();

	auto file = game->Server.FileSystem.GetFile(fs::FileCategory::WORLD, fileName);
	if (file && file->Opened())
	{
		doc.load(*file);
	}
}

SpriterEngine::SpriterFileElementWrapper * PugiXmlSpriterFileDocumentWrapper::newElementWrapperFromFirstElement()
{
    return new PugiXmlSpriterFileElementWrapper(doc.first_child());
}

SpriterEngine::SpriterFileElementWrapper * PugiXmlSpriterFileDocumentWrapper::newElementWrapperFromFirstElement(const std::string & elementName)
{
    return new PugiXmlSpriterFileElementWrapper(doc.child(elementName.c_str()));
}

}
