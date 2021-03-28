#pragma once

#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/RectangleShape.hpp"

#include <spriterengine/objectinfo/boxinstanceinfo.h>

namespace tdrp::loader::spriter
{

class SfmlBoxInstanceInfo : public SpriterEngine::BoxInstanceInfo
{
public:
	SfmlBoxInstanceInfo(SpriterEngine::point initialSize, sf::RenderWindow *validRenderWindow);

	void render() override;

private:
	sf::RenderWindow *renderWindow;

	sf::RectangleShape rectangle;
};

}
