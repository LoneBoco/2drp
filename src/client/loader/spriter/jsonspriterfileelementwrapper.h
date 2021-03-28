#pragma once

#include <spriterengine/override/spriterfileelementwrapper.h>
#include <nlohmann-json/json.hpp>

using json = nlohmann::json;

namespace tdrp::loader::spriter
{

class JSONSpriterFileAttributeWrapper;

class JSONSpriterFileElementWrapper : public SpriterEngine::SpriterFileElementWrapper
{
public:
	JSONSpriterFileElementWrapper(JSONSpriterFileElementWrapper* parent = nullptr, std::string name = "", json array = json::array(), int index = 0);

	std::string getName() override;

	bool isValid() override;

	void advanceToNextSiblingElement() override;
	void advanceToNextSiblingElementOfSameName() override;

	void advanceToNextAttribute(JSONSpriterFileAttributeWrapper* from_attribute);

private:
	SpriterEngine::SpriterFileAttributeWrapper *newAttributeWrapperFromFirstAttribute() override;
	SpriterEngine::SpriterFileAttributeWrapper *newAttributeWrapperFromFirstAttribute(const std::string & attributeName) override;

	SpriterEngine::SpriterFileElementWrapper *newElementWrapperFromFirstElement() override;
	SpriterEngine::SpriterFileElementWrapper *newElementWrapperFromFirstElement(const std::string & elementName) override;

	SpriterEngine::SpriterFileElementWrapper *newElementWrapperFromNextSiblingElement() override;

	SpriterEngine::SpriterFileElementWrapper *newElementClone() override;

	json m_array; // Array of elements with the same name
	int m_index; // Index of the current element
	std::string m_name;

	JSONSpriterFileElementWrapper *m_parent;
};

}
