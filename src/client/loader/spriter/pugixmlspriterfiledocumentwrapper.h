#pragma once

#include <pugixml.hpp>

#include <spriterengine/override/spriterfiledocumentwrapper.h>

namespace tdrp::loader::spriter
{

class PugiXmlSpriterFileDocumentWrapper : public SpriterEngine::SpriterFileDocumentWrapper
{
    typedef pugi::xml_document Document_t;

public:
    PugiXmlSpriterFileDocumentWrapper();

    void loadFile(std::string fileName) override;

private:
    SpriterEngine::SpriterFileElementWrapper *newElementWrapperFromFirstElement() override;
    SpriterEngine::SpriterFileElementWrapper *newElementWrapperFromFirstElement(const std::string & elementName) override;

    Document_t doc;
};

}
