#pragma once

#include <spriterengine/override/spriterfiledocumentwrapper.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace tdrp::loader::spriter
{

class JSONSpriterFileDocumentWrapper : public SpriterEngine::SpriterFileDocumentWrapper
{
public:
	JSONSpriterFileDocumentWrapper();

	void loadFile(std::string fileName) override;

private:
	SpriterEngine::SpriterFileElementWrapper *newElementWrapperFromFirstElement() override;
	SpriterEngine::SpriterFileElementWrapper *newElementWrapperFromFirstElement(const std::string & elementName) override;

	json m_json;
};

}
