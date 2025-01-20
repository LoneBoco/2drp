#include "jsonspriterfiledocumentwrapper.h"
#include "jsonspriterfileelementwrapper.h"

#include <spriterengine/global/settings.h>

#include <string>
#include <fstream>
#include <streambuf>

#include "engine/common.h"
#include "client/game/Game.h"
#include "engine/filesystem/File.h"


namespace tdrp::loader::spriter
{

JSONSpriterFileDocumentWrapper::JSONSpriterFileDocumentWrapper()
{
}

void JSONSpriterFileDocumentWrapper::loadFile(std::string fileName)
{
	auto game = BabyDI::Get<tdrp::Game>();

	auto file = game->Server->FileSystem.GetFile(fs::FileCategory::ASSETS, fileName);
	if (file && file->Opened())
	{
		m_json = json::parse(*file);
	}
}

SpriterEngine::SpriterFileElementWrapper * JSONSpriterFileDocumentWrapper::newElementWrapperFromFirstElement()
{
	// The root of the document should be an object
	if(m_json.is_object() && !m_json.empty()) {
		// We keep track of the elements with the same name as an array like it's structured in the scon file.
		// So if we don't have an array, we create one with one element.
		json array;
		array.push_back(m_json);
		return new JSONSpriterFileElementWrapper(nullptr, "spriter_data", array, 0);
	}
	SpriterEngine::Settings::error("JSON document doesn't contain an object");
	return new JSONSpriterFileElementWrapper();

}

SpriterEngine::SpriterFileElementWrapper * JSONSpriterFileDocumentWrapper::newElementWrapperFromFirstElement(const std::string & elementName)
{
	// The root has no name, but the scml has one (spriter_data), so we expect this here.
	assert(elementName.compare("spriter_data") == 0);
	return newElementWrapperFromFirstElement();
}

}
