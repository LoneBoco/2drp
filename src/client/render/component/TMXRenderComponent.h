#pragma once

#include "SFML/Graphics.hpp"
#include "SFML/Audio.hpp"

#include "client/render/component/Renderable.h"

#include "engine/common.h"
#include "engine/component/Component.h"
#include "engine/resources/Resource.h"
#include "engine/scene/SceneObject.h"

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
	~TMXRenderComponent() = default;

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
	void Render(sf::RenderTarget& window) override;

protected:
	std::vector<size_t> getVisibleChunks(const Recti& view) const;
	// const tmx::Tileset::Tile* getTileFromTilesets(uint32_t ID) const;
	Vector2di getTilePosition(const tmx::Vector2i& chunk_size, uint32_t index) const;
	void renderChunkToTexture(size_t chunk);

protected:
	std::weak_ptr<TMXSceneObject> m_owner;
	std::map<size_t, std::weak_ptr<sf::Texture>> m_textures;
	// std::map<size_t, std::weak_ptr<sf::Sound>> m_sounds;
	std::map<size_t, std::shared_ptr<sf::RenderTexture>> m_render_textures;
	std::map<size_t, sf::Sprite> m_sprites;
	std::shared_ptr<tmx::Map> m_tmx;
	sf::VertexArray m_chunk_vertices;
};

} // end namespace tdrp::render::component
