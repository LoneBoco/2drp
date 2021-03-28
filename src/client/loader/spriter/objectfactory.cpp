#include "client/loader/spriter/objectfactory.h"

#include "client/loader/spriter/sfmlpointinstanceinfo.h"
#include "client/loader/spriter/sfmlboxinstanceinfo.h"
#include "client/loader/spriter/sfmlboneinstanceinfo.h"
#include "client/loader/spriter/sfmlsoundobjectinforeference.h"

namespace tdrp::loader::spriter
{

ObjectFactory::ObjectFactory(sf::RenderWindow *validRenderWindow) :
	renderWindow(validRenderWindow)
{
}

SpriterEngine::PointInstanceInfo * ObjectFactory::newPointInstanceInfo()
{
	return new SfmlPointInstanceInfo(renderWindow);
}

SpriterEngine::BoxInstanceInfo * ObjectFactory::newBoxInstanceInfo(SpriterEngine::point size)
{
	return new SfmlBoxInstanceInfo(size, renderWindow);
}

SpriterEngine::BoneInstanceInfo * ObjectFactory::newBoneInstanceInfo(SpriterEngine::point size)
{
	return new SfmlBoneInstanceInfo(size, renderWindow);
}

}
