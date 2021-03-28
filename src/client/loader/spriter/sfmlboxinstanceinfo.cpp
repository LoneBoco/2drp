#include "client/loader/spriter/sfmlboxinstanceinfo.h"

#include <spriterengine/global/settings.h>

namespace tdrp::loader::spriter
{

SfmlBoxInstanceInfo::SfmlBoxInstanceInfo(SpriterEngine::point initialSize, sf::RenderWindow *validRenderWindow) :
	BoxInstanceInfo(initialSize),
	renderWindow(validRenderWindow),
	rectangle(sf::Vector2f(static_cast<float>(initialSize.x), static_cast<float>(initialSize.y)))
{
}

void SfmlBoxInstanceInfo::render()
{
	if (SpriterEngine::Settings::renderDebugBoxes)
	{
		rectangle.setPosition(static_cast<float>(getPosition().x), static_cast<float>(getPosition().y));
		rectangle.setRotation(static_cast<float>(SpriterEngine::toDegrees(getAngle())));
		rectangle.setScale(static_cast<float>(getScale().x), static_cast<float>(getScale().y));
		rectangle.setOrigin(static_cast<float>(getPivot().x * getSize().x), static_cast<float>(getPivot().y * getSize().y));
		renderWindow->draw(rectangle);
	}
}

}
