#pragma once

#include <pugixml.hpp>

#include <spriterengine/override/spriterfileelementwrapper.h>

namespace tdrp::loader::spriter
{

class PugiXmlSpriterFileElementWrapper : public SpriterEngine::SpriterFileElementWrapper
{
    typedef pugi::xml_node ElementPtr_t;
    typedef pugi::xml_attribute AttributePtr_t;

public:
    PugiXmlSpriterFileElementWrapper(ElementPtr_t initialElement);

    std::string getName() override;

    bool isValid() override;

    void advanceToNextSiblingElement() override;
    void advanceToNextSiblingElementOfSameName() override;

private:
    SpriterEngine::SpriterFileAttributeWrapper *newAttributeWrapperFromFirstAttribute() override;
    SpriterEngine::SpriterFileAttributeWrapper *newAttributeWrapperFromFirstAttribute(const std::string & attributeName) override;

    SpriterEngine::SpriterFileElementWrapper *newElementWrapperFromFirstElement() override;
    SpriterEngine::SpriterFileElementWrapper *newElementWrapperFromFirstElement(const std::string & elementName) override;

    SpriterEngine::SpriterFileElementWrapper *newElementWrapperFromNextSiblingElement() override;

    SpriterEngine::SpriterFileElementWrapper *newElementClone() override;

    AttributePtr_t findAttributeInElement(const std::string & name);

    ElementPtr_t element;
};

}
