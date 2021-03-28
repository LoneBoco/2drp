#pragma once

#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/CircleShape.hpp"

#include <spriterengine/objectinfo/pointinstanceinfo.h>

namespace tdrp::loader::spriter
{

class SfmlPointInstanceInfo : public SpriterEngine::PointInstanceInfo
{
public:
	SfmlPointInstanceInfo(sf::RenderWindow *validRenderWindow);

	void render() override;

private:
	sf::RenderWindow *renderWindow;

	sf::CircleShape circle;
};

}
