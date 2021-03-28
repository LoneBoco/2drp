#pragma once

#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/ConvexShape.hpp"

#include <spriterengine/objectinfo/boneinstanceinfo.h>

namespace tdrp::loader::spriter
{

class SfmlBoneInstanceInfo : public SpriterEngine::BoneInstanceInfo
{
public:
	SfmlBoneInstanceInfo(SpriterEngine::point initialSize, sf::RenderWindow *validRenderWindow);

	void render() override;

private:
	sf::RenderWindow *renderWindow;

	sf::ConvexShape boneShape;
};

}
