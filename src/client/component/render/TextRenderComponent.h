#pragma once

#include <any>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "client/component/render/Renderable.h"

#include "engine/common.h"
#include "engine/component/Component.h"
#include "engine/events/Events.h"
#include "engine/scene/SceneObject.h"
#include "engine/filesystem/ProgramSettings.h"

namespace tdrp::component::render
{

class TextRenderComponent : public IRenderableComponent, public Component
{
	COMPONENT_ENABLE(TextRenderComponent)

public:
	TextRenderComponent() = default;
	~TextRenderComponent() override = default;

	TextRenderComponent(const TextRenderComponent& other) = delete;
	TextRenderComponent(TextRenderComponent&& other) = delete;
	TextRenderComponent& operator=(const TextRenderComponent& other) = delete;
	TextRenderComponent& operator=(TextRenderComponent&& other) = delete;
	bool operator==(const TextRenderComponent& other) = delete;

public:
	virtual void Initialize(ComponentEntity& owner) override;
	virtual void OnAttached(ComponentEntity& owner) override;
	virtual void OnDetached(ComponentEntity& owner) override;

public:
	Rectf GetBoundingBox() const override;
	void Render(sf::RenderTarget& window, const Rectf& viewRect, std::chrono::milliseconds elapsed) override;

protected:
	void load_font(const std::string& name);
	void load_text(const std::string& text);
	void property_update(uint16_t attribute_id);
	std::any provide_size() const;
	std::any provide_boundingbox() const;

protected:
	INJECT(settings::ProgramSettings, Settings);

protected:
	EventHandle m_handle_text_change;
	std::weak_ptr<SceneObject> m_owner;
	std::shared_ptr<sf::Font> m_font;
	std::optional<sf::Text> m_text;
	std::string m_font_name;
	bool m_good_font = false;
};

} // end namespace tdrp::component::render
