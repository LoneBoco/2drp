#pragma once

#include <type_traits>

#include "SFML/Graphics.hpp"

#include "engine/common.h"
#include "engine/component/Component.h"

namespace tdrp::render::component
{

class IRenderableComponent
{
public:
	virtual Rectf GetBoundingBox() const = 0;
	virtual void Render(sf::RenderTarget& window) = 0;
};

} // end namespace tdrp::render::component
