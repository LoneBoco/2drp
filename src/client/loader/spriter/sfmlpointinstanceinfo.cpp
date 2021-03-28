#include "client/loader/spriter/sfmlpointinstanceinfo.h"

#include <spriterengine/global/settings.h>

namespace tdrp::loader::spriter
{

SfmlPointInstanceInfo::SfmlPointInstanceInfo(sf::RenderWindow *validRenderWindow) :
	renderWindow(validRenderWindow),
	circle(10)
{
	circle.setFillColor(sf::Color::Red);
	circle.setOrigin(5, 5);
}

void SfmlPointInstanceInfo::render()
{
	if (SpriterEngine::Settings::renderDebugPoints)
	{
		circle.setPosition(static_cast<float>(getPosition().x), static_cast<float>(getPosition().y));
		circle.setRotation(static_cast<float>(SpriterEngine::toDegrees(getAngle())));
		renderWindow->draw(circle);
	}
}

}
