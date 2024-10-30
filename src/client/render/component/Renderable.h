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
	virtual ~IRenderableComponent() {}
	virtual Rectf GetBoundingBox() const = 0;
	virtual void Render(sf::RenderTarget& window, const Rectf& viewRect, std::chrono::milliseconds elapsed) = 0;
};

} // end namespace tdrp::render::component
