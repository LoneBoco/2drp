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
	struct Chunk
	{
		std::optional<sf::Sprite> Sprite;
		//sf::VertexArray Vertices;
		Rectf Bounds;
		std::shared_ptr<sf::RenderTexture> RenderTexture;
	};

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
	void Render(sf::RenderTarget& window, const Rectf& viewRect, std::chrono::milliseconds elapsed) override;

public:
	void SetMaxChunks(size_t max);
	std::optional<Chunk> RenderChunkToTexture(uint32_t chunk_idx, std::shared_ptr<tmx::Map> tmx);
	void RenderChunkToTexture(uint32_t chunk_idx, const std::span<const uint32_t>& tiles);

protected:
	INJECT(settings::ProgramSettings, Settings);

protected:
	std::weak_ptr<TMXSceneObject> m_owner;
	//std::shared_ptr<tmx::Map> m_tmx;
	std::unordered_map<size_t, std::weak_ptr<sf::Texture>> m_textures;
	std::vector<std::optional<Chunk>> m_chunks;
	std::vector<uint32_t> m_chunks_to_render;
};

} // end namespace tdrp::render::component
