#pragma once

#include <any>

#include "SFML/Graphics.hpp"
#include "SFML/Audio.hpp"

#include "client/render/component/Renderable.h"

#include "engine/common.h"
#include "engine/component/Component.h"
#include "engine/events/Events.h"
#include "engine/resources/Resource.h"
#include "engine/scene/SceneObject.h"

namespace tdrp::render::component
{

class SpriterRenderComponent : public IRenderableComponent, public Component
{
	COMPONENT_ENABLE(SpriterRenderComponent)

public:
	SpriterRenderComponent() = default;
	~SpriterRenderComponent() = default;

	SpriterRenderComponent(const SpriterRenderComponent& other) = delete;
	SpriterRenderComponent(SpriterRenderComponent&& other) = delete;
	SpriterRenderComponent& operator=(const SpriterRenderComponent& other) = delete;
	SpriterRenderComponent& operator=(SpriterRenderComponent&& other) = delete;
	bool operator==(const SpriterRenderComponent& other) = delete;

public:
	virtual void Initialize(ComponentEntity& owner) override;
	virtual void OnAttached(ComponentEntity& owner) override;
	virtual void OnDetached(ComponentEntity& owner) override;

public:
	Rectf GetBoundingBox() const override;
	void Render(sf::RenderTarget& window, std::chrono::milliseconds elapsed) override;

protected:
	void load_animation(const std::string& model, const std::string& entity = {}, const std::string& animation = {});
	//void property_update_image(uint16_t attribute_id);
	void property_update_entity(const uint16_t attribute_id);
	void property_update_animation(const uint16_t attribute_id);
	void attributes_update(const uint16_t attribute_id);
	std::any provide_size();

protected:
	//EventHandle m_handle_image;
	EventHandle m_handle_entity;
	EventHandle m_handle_animation;
	EventHandle m_handle_attributes;
	std::weak_ptr<SceneObject> m_owner;
	//std::map<size_t, std::weak_ptr<sf::Texture>> m_textures;
	//std::map<size_t, std::weak_ptr<sf::Sound>> m_sounds;
	//sf::Sprite m_sprite;
};

} // end namespace tdrp::render::component
