#pragma once

#include <spriterengine/override/spriterfileattributewrapper.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace tdrp::loader::spriter
{

class JSONSpriterFileElementWrapper;

class JSONSpriterFileAttributeWrapper : public SpriterEngine::SpriterFileAttributeWrapper
{
public:
	JSONSpriterFileAttributeWrapper(JSONSpriterFileElementWrapper* parent_object, std::string name = "", json value = nullptr);

	std::string getName() override;

	bool isValid() override;

	SpriterEngine::real getRealValue() override;
	int getIntValue() override;
	std::string getStringValue() override;

	void advanceToNextAttribute() override;

private:
	json m_value;
	std::string m_name;
	JSONSpriterFileElementWrapper* m_parent_object;

	friend class JSONSpriterFileElementWrapper;
};

}
