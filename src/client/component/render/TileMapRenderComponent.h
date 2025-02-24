#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "client/component/render/Renderable.h"

#include "engine/common.h"
#include "engine/component/Component.h"
#include "engine/resources/Resource.h"
#include "engine/scene/SceneObject.h"

namespace tdrp::component::render
{

class TileMapRenderComponent : public IRenderableComponent, public Component
{
	COMPONENT_ENABLE(TileMapRenderComponent)

public:
	TileMapRenderComponent() = default;
	~TileMapRenderComponent() override = default;

	TileMapRenderComponent(const TileMapRenderComponent& other) = delete;
	TileMapRenderComponent(TileMapRenderComponent&& other) = delete;
	TileMapRenderComponent& operator=(const TileMapRenderComponent& other) = delete;
	TileMapRenderComponent& operator=(TileMapRenderComponent&& other) = delete;
	bool operator==(const TileMapRenderComponent& other) = delete;

public:
	virtual void Initialize(ComponentEntity& owner) override;
	virtual void OnAttached(ComponentEntity& owner) override;
	virtual void OnDetached(ComponentEntity& owner) override;

public:
	Rectf GetBoundingBox() const override;
	void Render(sf::RenderTarget& window, const Rectf& viewRect, std::chrono::milliseconds elapsed) override;

protected:
	std::weak_ptr<SceneObject> m_owner;
	std::unordered_map<size_t, std::weak_ptr<sf::Texture>> m_textures;
	std::unordered_map<size_t, std::weak_ptr<sf::Sound>> m_sounds;
	std::vector<std::shared_ptr<sf::RenderTexture>> m_render_textures;
	std::vector<sf::Sprite> m_sprites;
	//sf::VertexArray m_vertices;
	//sf::Shader m_shader;
};

} // end namespace tdrp::component::render
