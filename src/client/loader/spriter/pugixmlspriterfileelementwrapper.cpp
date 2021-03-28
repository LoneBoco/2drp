#include "client/loader/spriter/pugixmlspriterfileelementwrapper.h"
#include "client/loader/spriter/pugixmlspriterfileattributewrapper.h"

namespace tdrp::loader::spriter
{

PugiXmlSpriterFileElementWrapper::PugiXmlSpriterFileElementWrapper(ElementPtr_t initialElement):
    element(initialElement)
{
}

std::string PugiXmlSpriterFileElementWrapper::getName()
{
    return element.name();
}

bool PugiXmlSpriterFileElementWrapper::isValid()
{
    return element;
}

void PugiXmlSpriterFileElementWrapper::advanceToNextSiblingElement()
{
    element = element.next_sibling();
}

void PugiXmlSpriterFileElementWrapper::advanceToNextSiblingElementOfSameName()
{
    element = element.next_sibling(element.name());
}

SpriterEngine::SpriterFileAttributeWrapper * PugiXmlSpriterFileElementWrapper::newAttributeWrapperFromFirstAttribute()
{
    return new PugiXmlSpriterFileAttributeWrapper(element.first_attribute());
}

SpriterEngine::SpriterFileAttributeWrapper * PugiXmlSpriterFileElementWrapper::newAttributeWrapperFromFirstAttribute(const std::string & attributeName)
{
    return new PugiXmlSpriterFileAttributeWrapper(element.attribute(attributeName.c_str()));
}

SpriterEngine::SpriterFileElementWrapper * PugiXmlSpriterFileElementWrapper::newElementWrapperFromFirstElement()
{
    return new PugiXmlSpriterFileElementWrapper(element.first_child());
}

SpriterEngine::SpriterFileElementWrapper * PugiXmlSpriterFileElementWrapper::newElementWrapperFromFirstElement(const std::string & elementName)
{
    return new PugiXmlSpriterFileElementWrapper(element.child(elementName.c_str()));
}

SpriterEngine::SpriterFileElementWrapper * PugiXmlSpriterFileElementWrapper::newElementWrapperFromNextSiblingElement()
{
    return new PugiXmlSpriterFileElementWrapper(element.next_sibling(element.name()));
}

SpriterEngine::SpriterFileElementWrapper * PugiXmlSpriterFileElementWrapper::newElementClone()
{
    return new PugiXmlSpriterFileElementWrapper(element);
}

PugiXmlSpriterFileElementWrapper::AttributePtr_t PugiXmlSpriterFileElementWrapper::findAttributeInElement(const std::string & name)
{
    return element.attribute(name.c_str());
}

}
