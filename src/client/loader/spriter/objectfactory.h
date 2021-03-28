#pragma once

#include "SFML/Graphics/RenderWindow.hpp"

#include <spriterengine/override/objectfactory.h>

namespace tdrp::loader::spriter
{

class ObjectFactory : public SpriterEngine::ObjectFactory
{
public:
	ObjectFactory(sf::RenderWindow *validRenderWindow);
		
	SpriterEngine::PointInstanceInfo *newPointInstanceInfo() override;

	SpriterEngine::BoxInstanceInfo *newBoxInstanceInfo(SpriterEngine::point size) override;

	SpriterEngine::BoneInstanceInfo *newBoneInstanceInfo(SpriterEngine::point size) override;

private:
	sf::RenderWindow *renderWindow;
};

}
