#pragma once

#include <any>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "engine/common.h"
#include "engine/component/Component.h"
#include "engine/events/Events.h"
#include "engine/resources/Resource.h"
#include "engine/scene/SceneObject.h"
#include "engine/filesystem/ProgramSettings.h"

#include "client/render/component/Renderable.h"
#include "client/render/component/Animation/IAnimationSystem.h"

namespace tdrp::render::component
{

class AnimationRenderComponent : public IRenderableComponent, public Component
{
	COMPONENT_ENABLE(AnimationRenderComponent)

public:
	AnimationRenderComponent() = default;
	~AnimationRenderComponent() = default;

	AnimationRenderComponent(const AnimationRenderComponent& other) = delete;
	AnimationRenderComponent(AnimationRenderComponent&& other) = delete;
	AnimationRenderComponent& operator=(const AnimationRenderComponent& other) = delete;
	AnimationRenderComponent& operator=(AnimationRenderComponent&& other) = delete;
	bool operator==(const AnimationRenderComponent& other) = delete;

public:
	virtual void Initialize(ComponentEntity& owner) override;
	virtual void OnAttached(ComponentEntity& owner) override;
	virtual void OnDetached(ComponentEntity& owner) override;

public:
	Rectf GetBoundingBox() const override;
	void Render(sf::RenderTarget& window, const Rectf& viewRect, std::chrono::milliseconds elapsed) override;

protected:
	void load_animation();
	void property_update_image(uint16_t attribute_id);
	void property_update_entity(const uint16_t attribute_id);
	void property_update_animation(const uint16_t attribute_id);
	void attributes_update(const uint16_t attribute_id);
	std::any provide_size();
	std::any provide_boundingbox();
	std::any provide_animation();

protected:
	INJECT(settings::ProgramSettings, Settings);

protected:
	EventHandle m_handle_image;
	EventHandle m_handle_entity;
	EventHandle m_handle_animation;
	EventHandle m_handle_attributes;
	std::weak_ptr<SceneObject> m_owner;
	std::unique_ptr<animation::IAnimationSystem> m_animation;
	//std::map<size_t, std::weak_ptr<sf::Texture>> m_textures;
	//std::map<size_t, std::weak_ptr<sf::Sound>> m_sounds;
	//sf::Sprite m_sprite;
};

} // end namespace tdrp::render::component
