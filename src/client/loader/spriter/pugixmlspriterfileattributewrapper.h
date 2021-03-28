#pragma once

#include <pugixml.hpp>

#include <spriterengine/override/spriterfileattributewrapper.h>

namespace tdrp::loader::spriter
{

class PugiXmlSpriterFileAttributeWrapper : public SpriterEngine::SpriterFileAttributeWrapper
{
    typedef pugi::xml_attribute AttributePtr_t;

public:
    PugiXmlSpriterFileAttributeWrapper(AttributePtr_t initialAttribute);

    std::string getName() override;

    bool isValid() override;

    SpriterEngine::real getRealValue() override;
    int getIntValue() override;
    std::string getStringValue() override;

    void advanceToNextAttribute() override;

private:
    AttributePtr_t attribute;
};

}
