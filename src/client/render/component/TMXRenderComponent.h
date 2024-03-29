#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "client/render/component/Renderable.h"

#include "engine/common.h"
#include "engine/component/Component.h"
#include "engine/resources/Resource.h"
#include "engine/scene/SceneObject.h"
#include "engine/filesystem/ProgramSettings.h"

namespace tmx
{
	class Map;
}

namespace tdrp::render::component
{

class TMXRenderComponent : public IRenderableComponent, public Component
{
	COMPONENT_ENABLE(TMXRenderComponent)

public:
	TMXRenderComponent() = default;
	~TMXRenderComponent();

	TMXRenderComponent(const TMXRenderComponent& other) = delete;
	TMXRenderComponent(TMXRenderComponent&& other) = delete;
	TMXRenderComponent& operator=(const TMXRenderComponent& other) = delete;
	TMXRenderComponent& operator=(TMXRenderComponent&& other) = delete;
	bool operator==(const TMXRenderComponent& other) = delete;

public:
	virtual void Initialize(ComponentEntity& owner) override;
	virtual void OnAttached(ComponentEntity& owner) override;
	virtual void OnDetached(ComponentEntity& owner) override;

public:
	Rectf GetBoundingBox() const override;
	void Render(sf::RenderTarget& window, std::chrono::milliseconds elapsed) override;

protected:
	Vector2di getTilePosition(const tmx::Vector2i& chunk_size, uint32_t index) const;
	void renderToTexture();

protected:
	INJECT(settings::ProgramSettings, Settings);

protected:
	std::weak_ptr<TMXSceneObject> m_owner;
	std::unordered_map<size_t, std::weak_ptr<sf::Texture>> m_textures;
	std::shared_ptr<sf::RenderTexture> m_render_texture;
	std::shared_ptr<tmx::Map> m_tmx;
	sf::Sprite m_sprite;
	sf::VertexArray m_chunk_vertices;
};

} // end namespace tdrp::render::component
