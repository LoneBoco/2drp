#pragma once

#include <any>
#include <optional>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "client/component/render/Renderable.h"

#include "engine/common.h"
#include "engine/component/Component.h"
#include "engine/events/Events.h"
#include "engine/resources/Resource.h"
#include "engine/scene/SceneObject.h"
#include "engine/filesystem/ProgramSettings.h"

namespace tdrp::component::render
{

class StaticRenderComponent : public IRenderableComponent, public Component
{
	COMPONENT_ENABLE(StaticRenderComponent)

public:
	StaticRenderComponent() = default;
	~StaticRenderComponent() override = default;

	StaticRenderComponent(const StaticRenderComponent& other) = delete;
	StaticRenderComponent(StaticRenderComponent&& other) = delete;
	StaticRenderComponent& operator=(const StaticRenderComponent& other) = delete;
	StaticRenderComponent& operator=(StaticRenderComponent&& other) = delete;
	bool operator==(const StaticRenderComponent& other) = delete;

public:
	virtual void Initialize(ComponentEntity& owner) override;
	virtual void OnAttached(ComponentEntity& owner) override;
	virtual void OnDetached(ComponentEntity& owner) override;

public:
	Rectf GetBoundingBox() const override;
	void Render(sf::RenderTarget& window, const Rectf& viewRect, std::chrono::milliseconds elapsed) override;

protected:
	void load_image(const std::string& image);
	void image_property_update(uint16_t attribute_id);
	std::any provide_size() const;
	std::any provide_boundingbox() const;

protected:
	INJECT(settings::ProgramSettings, Settings);

protected:
	EventHandle m_handle_image_change;
	std::weak_ptr<SceneObject> m_owner;
	std::weak_ptr<sf::Texture> m_texture;
	std::optional<sf::Sprite> m_sprite;
};

} // end namespace tdrp::render::component
