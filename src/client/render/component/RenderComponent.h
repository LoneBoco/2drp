#pragma once

#include "SFML/Graphics.hpp"
#include "SFML/Audio.hpp"

#include "client/render/component/Renderable.h"

#include "engine/common.h"
#include "engine/component/Component.h"
#include "engine/resources/Resource.h"
#include "engine/scene/SceneObject.h"

namespace tdrp::render::component
{

class RenderComponent : public IRenderableComponent, public Component
{
	COMPONENT_ENABLE(RenderComponent)

public:
	RenderComponent() = default;
	~RenderComponent() = default;

	RenderComponent(const RenderComponent& other) = delete;
	RenderComponent(RenderComponent&& other) = delete;
	RenderComponent& operator=(const RenderComponent& other) = delete;
	RenderComponent& operator=(RenderComponent&& other) = delete;
	bool operator==(const RenderComponent& other) = delete;

public:
	virtual void Initialize(ComponentEntity& owner) override;
	virtual void OnAttached(ComponentEntity& owner) override;
	virtual void OnDetached(ComponentEntity& owner) override;

public:
	Rectf GetBoundingBox() const override;
	void Render(sf::RenderTarget& window) override;

protected:
	std::weak_ptr<SceneObject> m_owner;
	std::map<size_t, std::weak_ptr<sf::Texture>> m_textures;
	std::map<size_t, std::weak_ptr<sf::Sound>> m_sounds;
	sf::Sprite m_sprite;
};

} // end namespace tdrp::render::component
