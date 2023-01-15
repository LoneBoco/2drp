#include <tmxlite/Map.hpp>
#include <tmxlite/TileLayer.hpp>

#include "client/game/Game.h"
#include "client/render/component/TMXRenderComponent.h"
#include "client/loader/SFMListream.h"
#include "client/loader/ResourceLoaders.h"

#include "engine/filesystem/File.h"
#include "engine/scene/SceneObject.h"


namespace tdrp::render::component
{

TMXRenderComponent::~TMXRenderComponent()
{
}

void TMXRenderComponent::Initialize(ComponentEntity& owner)
{
	if (auto p_so = dynamic_cast<SceneObject*>(&owner))
	{
		// Re-acquire our scene object from the server so it is properly reference counted by shared_ptr.
		auto game = BabyDI::Get<tdrp::Game>();
		auto so = game->Server.GetSceneObjectById(p_so->ID);
		if (so->GetType() == SceneObjectType::TMX)
		{
			auto so_tmx = std::dynamic_pointer_cast<TMXSceneObject>(so);
			m_owner = so_tmx;
			m_tmx = so_tmx->TmxMap;
			if (!m_tmx)
			{
				log::PrintLine("** ERROR: TMX scene objects should be non-replicated.");
				throw "TMX scene objects should be non-replicated.";
			}
		}
	}
}

void TMXRenderComponent::OnAttached(ComponentEntity& owner)
{
	auto game = BabyDI::Get<tdrp::Game>();
	auto resources = BabyDI::Get<tdrp::ResourceManager>();

	m_textures.clear();
	// m_sounds.clear();

	if (auto so = m_owner.lock())
	{
		// Load the images.
		{
			const auto& tilesets = m_tmx->getTilesets();
			for (const auto& tileset : tilesets)
			{
				filesystem::path image = tileset.getImagePath();

				auto id = loader::LoadTexture(image);
				if (id != 0)
				{
					auto handle = resources->Get<sf::Texture>(id);
					m_textures.insert(std::make_pair(id, handle));
				}
			}
		}

		if (so->Layer > so->TmxMap->getLayers().size())
			return;

		// Get the tile layer.
		const auto& layer = so->TmxMap->getLayers().at(so->Layer);
		if (!layer || layer->getType() != tmx::Layer::Type::Tile) return;
		const auto& tilelayer = layer->getLayerAs<tmx::TileLayer>();

		// Get the chunk.
		if (so->Chunk > tilelayer.getChunks().size()) return;
		const auto& chunk = tilelayer.getChunks().at(so->Chunk);

		// Create vertex array.
		{
			const auto& tile_size = so->TmxMap->getTileSize();
			const auto& chunk_size = chunk.size;
			auto tile_count = chunk_size.x * chunk_size.y;

			m_chunk_vertices.setPrimitiveType(sf::PrimitiveType::Triangles);
			m_chunk_vertices.resize(chunk_size.x * chunk_size.y * 6);

			for (int i = 0; i < tile_count; ++i)
			{
				Vector2di pos{ getTilePosition(chunk_size, i) };

				auto* quad = &m_chunk_vertices[i * 6];
				// 0 quad[0].position = { static_cast<float>(pos.x * tile_size.x), static_cast<float>(pos.y * tile_size.y) };
				// 1 quad[1].position = { static_cast<float>((pos.x + 1) * tile_size.x), static_cast<float>(pos.y * tile_size.y) };
				// 4 quad[2].position = { static_cast<float>((pos.x + 1) * tile_size.x), static_cast<float>((pos.y + 1) * tile_size.y) };
				// 2 quad[3].position = { static_cast<float>(pos.x * tile_size.x), static_cast<float>((pos.y + 1) * tile_size.y) };
				// 0 1 2 2 4 1

				quad[0].position = { static_cast<float>(pos.x * tile_size.x), static_cast<float>(pos.y * tile_size.y) };
				quad[1].position = { static_cast<float>((pos.x + 1) * tile_size.x), static_cast<float>(pos.y * tile_size.y) };
				quad[2].position = { static_cast<float>(pos.x * tile_size.x), static_cast<float>((pos.y + 1) * tile_size.y) };
				quad[3].position = { static_cast<float>(pos.x * tile_size.x), static_cast<float>((pos.y + 1) * tile_size.y) };
				quad[4].position = { static_cast<float>((pos.x + 1) * tile_size.x), static_cast<float>((pos.y + 1) * tile_size.y) };
				quad[5].position = { static_cast<float>((pos.x + 1) * tile_size.x), static_cast<float>(pos.y * tile_size.y) };
			}
		}

		renderToTexture();
	}
}

void TMXRenderComponent::OnDetached(ComponentEntity& owner)
{

}

/////////////////////////////

Rectf TMXRenderComponent::GetBoundingBox() const
{
	if (auto so = m_owner.lock())
	{
		auto pos = so->GetPosition();
		const auto& chunksize = so->ChunkSize;

		return Rectf{ pos.x , pos.y, chunksize.x, chunksize.y };
	}

	return Rectf{};
}

void TMXRenderComponent::Render(sf::RenderTarget& window, std::chrono::milliseconds elapsed)
{
	if (auto so = m_owner.lock())
	{
		if (m_textures.empty())
			return;

		auto pos = so->GetPosition();
		auto scale = so->GetScale();
		auto rotate = so->GetAngle();

		sf::RenderStates state;
		state.transform
			.translate({ pos.x, pos.y })
			.rotate(sf::degrees(rotate))
			.scale({ scale.x, scale.y });

		auto game = BabyDI::Get<tdrp::Game>();
		window.draw(m_sprite, state);

		if (Settings->GetAs<bool>("Debug.drawtmxbbox"))
		{
			Window::RenderPhysics(window, so);
		}
	}
}

inline Vector2di TMXRenderComponent::getTilePosition(const tmx::Vector2i& chunk_size, uint32_t index) const
{
	Vector2di result;

	switch (m_tmx->getRenderOrder())
	{
		case tmx::RenderOrder::RightDown:
			result.x = (chunk_size.x - 1) - (index % chunk_size.x);
			result.y = index / chunk_size.y;
			break;
		case tmx::RenderOrder::RightUp:
			result.x = (chunk_size.x - 1) - (index % chunk_size.x);
			result.y = (chunk_size.y - 1) - (index / chunk_size.y);
			break;
		case tmx::RenderOrder::LeftDown:
			result.x = index % chunk_size.x;
			result.y = index / chunk_size.y;
			break;
		case tmx::RenderOrder::LeftUp:
			result.x = index % chunk_size.x;
			result.y = (chunk_size.y - 1) - (index / chunk_size.y);
			break;
	}

	return result;
}

void TMXRenderComponent::renderToTexture()
{
	auto resources = BabyDI::Get<tdrp::ResourceManager>();

	if (auto so = m_owner.lock())
	{
		const auto& layer = m_tmx->getLayers().at(so->Layer);
		if (!layer || layer->getType() != tmx::Layer::Type::Tile)
			return;

		const auto& tilelayer = layer->getLayerAs<tmx::TileLayer>();
		if (tilelayer.getChunks().size() <= so->Chunk)
			return;

		const auto& chunk = tilelayer.getChunks().at(so->Chunk);
		if (chunk.size.x * chunk.size.y * 6 != m_chunk_vertices.getVertexCount())
			return;

		const auto tilesize = m_tmx->getTileSize();
		const auto& tilesets = m_tmx->getTilesets();
		Vector2di size_in_pixels{ chunk.size.x * static_cast<int>(tilesize.x), chunk.size.y * static_cast<int>(tilesize.y) };

		// Set our render state.
		sf::RenderStates state;

		// Create our texture.
		auto texture = std::make_shared<sf::RenderTexture>();
		auto success = texture->create(size_in_pixels.x, size_in_pixels.y);
		if (!success)
			return;

		texture->clear(sf::Color::Transparent);

		for (const auto& tileset : tilesets)
		{
			// Configure the texture coordinates for this tileset.
			bool hasTile = false;
			for (size_t i = 0; i < chunk.tiles.size(); ++i)
			{
				auto* quad = &m_chunk_vertices[i * 6];

				// Retrieve the tile.  If no tile found, zero the texture coordinates.
				// It may be on another tileset.
				const auto& t = chunk.tiles.at(i);
				const auto* tile = tileset.getTile(t.ID);
				if (tile == nullptr)
				{
					quad[0].texCoords = { 0.0f, 0.0f };
					quad[1].texCoords = { 0.0f, 0.0f };
					quad[2].texCoords = { 0.0f, 0.0f };
					quad[3].texCoords = { 0.0f, 0.0f };
					quad[4].texCoords = { 0.0f, 0.0f };
					quad[5].texCoords = { 0.0f, 0.0f };
					quad[0].color = sf::Color::Transparent;
					quad[1].color = sf::Color::Transparent;
					quad[2].color = sf::Color::Transparent;
					quad[3].color = sf::Color::Transparent;
					quad[4].color = sf::Color::Transparent;
					quad[5].color = sf::Color::Transparent;
					continue;
				}

				// Record that we have a tile.  If we have no tiles, we skip rendering this tileset later.
				hasTile = true;

				auto tu = tile->imagePosition.x;
				auto tv = tile->imagePosition.y;
				auto sizex = tile->imageSize.x;
				auto sizey = tile->imageSize.y;

				// Construct our texture coordinates.
				// 0 quad[0].texCoords = { static_cast<float>(tu), static_cast<float>(tv) };
				// 1 quad[1].texCoords = { static_cast<float>(tu + sizex), static_cast<float>(tv) };
				// 4 quad[2].texCoords = { static_cast<float>(tu + sizex), static_cast<float>(tv + sizey) };
				// 2 quad[3].texCoords = { static_cast<float>(tu), static_cast<float>(tv + sizey) };
				// 0 1 2 2 4 1
				quad[0].texCoords = { static_cast<float>(tu), static_cast<float>(tv) };
				quad[1].texCoords = { static_cast<float>(tu + sizex), static_cast<float>(tv) };
				quad[2].texCoords = { static_cast<float>(tu), static_cast<float>(tv + sizey) };
				quad[3].texCoords = { static_cast<float>(tu), static_cast<float>(tv + sizey) };
				quad[4].texCoords = { static_cast<float>(tu + sizex), static_cast<float>(tv + sizey) };
				quad[5].texCoords = { static_cast<float>(tu + sizex), static_cast<float>(tv) };

				// Set color.
				quad[0].color = sf::Color::White;
				quad[1].color = sf::Color::White;
				quad[2].color = sf::Color::White;
				quad[3].color = sf::Color::White;
				quad[4].color = sf::Color::White;
				quad[5].color = sf::Color::White;
			}

			// Now render this tileset to our final texture.
			if (hasTile)
			{
				filesystem::path image = tileset.getImagePath();
				auto id = resources->FindId<sf::Texture>(image.filename().string());
				if (id != 0)
				{
					if (auto t = m_textures[id].lock())
					{
						state.texture = t.get();
						state.blendMode = sf::BlendNone;
						texture->draw(m_chunk_vertices, state);
					}
				}
			}
		}

		// Finish the rendering on our texture.
		texture->display();

		// Create the sprite for this chunk and position it.
		sf::Sprite sprite{ texture->getTexture() };

		// Save our sprite and texture via the chunk index.
		m_sprite = std::move(sprite);
		m_render_texture = texture;
	}
}

} // end namespace tdrp::render::component
