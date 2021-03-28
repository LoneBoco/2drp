#include "client/loader/spriter/sfmlboneinstanceinfo.h"

#include <spriterengine/global/settings.h>

namespace tdrp::loader::spriter
{

SfmlBoneInstanceInfo::SfmlBoneInstanceInfo(SpriterEngine::point initialSize, sf::RenderWindow *validRenderWindow) :
	BoneInstanceInfo(initialSize),
	renderWindow(validRenderWindow)
{
	boneShape.setPointCount(4);

	boneShape.setPoint(0, sf::Vector2f(0, static_cast<float>(initialSize.y/2.0)));
	boneShape.setPoint(1, sf::Vector2f(4, 0));
	boneShape.setPoint(2, sf::Vector2f(static_cast<float>(initialSize.x), static_cast<float>(initialSize.y/2.0)));
	boneShape.setPoint(3, sf::Vector2f(4, static_cast<float>(initialSize.y)));

	boneShape.setOrigin(0, static_cast<float>(initialSize.y/2.0));

	boneShape.setFillColor(sf::Color(0, 0, 255, 50));

	boneShape.setOutlineColor(sf::Color::White);
	boneShape.setOutlineThickness(1);
}

void SfmlBoneInstanceInfo::render()
{
	if (SpriterEngine::Settings::renderDebugBones)
	{
		boneShape.setPosition(static_cast<float>(getPosition().x), static_cast<float>(getPosition().y));
		boneShape.setRotation(static_cast<float>(SpriterEngine::toDegrees(getAngle())));
		boneShape.setScale(static_cast<float>(getScale().x), static_cast<float>(getScale().y));
		renderWindow->draw(boneShape);
	}
}

}
