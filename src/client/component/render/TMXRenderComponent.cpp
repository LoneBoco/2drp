#include <ranges>
#include <span>

#include <tmxlite/Map.hpp>
#include <tmxlite/TileLayer.hpp>

#include "client/game/Game.h"
#include "client/component/render/TMXRenderComponent.h"
#include "client/loader/SFMListream.h"
#include "client/loader/ResourceLoaders.h"

#include "engine/filesystem/File.h"
#include "engine/scene/SceneObject.h"


namespace tdrp::component::render
{

// Get the tile position based on the render order.
static Vector2du getTilePosition(const Vector2du& chunk_size, uint32_t index, tmx::RenderOrder render_order)
{
	Vector2du result;

	switch (render_order)
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

static std::shared_ptr<sf::RenderTexture> renderToTexture(uint32_t chunk_idx, TMXRenderComponent::Chunk& render_chunk, std::shared_ptr<TMXSceneObject>& so, std::unordered_map<size_t, std::weak_ptr<sf::Texture>>& tileset_textures, tmx::RenderOrder render_order, tdrp::range_of<uint32_t> auto&& tiles)
{
	auto resources = BabyDI::Get<tdrp::ResourceManager>();

	size_t tile_count = std::ranges::size(tiles);
	if (tile_count == 0 || chunk_idx >= so->Chunks.size())
		return nullptr;

	auto& chunk = so->Chunks.at(chunk_idx);

	// Create our texture.
	auto render_texture = std::make_shared<sf::RenderTexture>();
	auto success = render_texture->resize({ static_cast<uint32_t>(render_chunk.Bounds.size.x + 0.4f), static_cast<uint32_t>(render_chunk.Bounds.size.y + 0.4f) });
	if (!success)
		return nullptr;

	// Clear the color.
	render_texture->clear(sf::Color::Transparent);

	sf::VertexArray vertex_array{ sf::PrimitiveType::Triangles, static_cast<size_t>(tile_count) * 6 };
	for (const auto& [tilesetGID, tileset] : so->Tilesets)
	{
		const auto& tile_size = tileset->TileDimensions;
		vertex_array.clear();
		int tile_index = 0;
		for (uint32_t tile : tiles)
		{
			// Check if the tile exists in this tileset.
			if (tile == 0 || tile < tilesetGID.FirstGID || tile > tilesetGID.LastGID)
			{
				++tile_index;
				continue;
			}

			Vector2du pos{ getTilePosition(chunk.Size, tile_index, render_order) };
			std::array<sf::Vertex, 6> vertices;

			// 0 quad[0].position = { static_cast<float>(pos.x * tilesize.x), static_cast<float>(pos.y * tilesize.y) };
			// 1 quad[1].position = { static_cast<float>((pos.x + 1) * tilesize.x), static_cast<float>(pos.y * tilesize.y) };
			// 4 quad[2].position = { static_cast<float>((pos.x + 1) * tilesize.x), static_cast<float>((pos.y + 1) * tilesize.y) };
			// 2 quad[3].position = { static_cast<float>(pos.x * tilesize.x), static_cast<float>((pos.y + 1) * tilesize.y) };
			// 0 1 2 2 4 1
			vertices[0].position = { static_cast<float>(pos.x * tile_size.x), static_cast<float>(pos.y * tile_size.y) };
			vertices[1].position = { static_cast<float>((pos.x + 1) * tile_size.x), static_cast<float>(pos.y * tile_size.y) };
			vertices[2].position = { static_cast<float>(pos.x * tile_size.x), static_cast<float>((pos.y + 1) * tile_size.y) };
			vertices[3].position = { static_cast<float>(pos.x * tile_size.x), static_cast<float>((pos.y + 1) * tile_size.y) };
			vertices[4].position = { static_cast<float>((pos.x + 1) * tile_size.x), static_cast<float>((pos.y + 1) * tile_size.y) };
			vertices[5].position = { static_cast<float>((pos.x + 1) * tile_size.x), static_cast<float>(pos.y * tile_size.y) };

			auto localtile = tile - tilesetGID.FirstGID;
			auto pixel_pos = tileset->GetTilePixelPosition(localtile);

			// Construct our texture coordinates.
			// 0 quad[0].texCoords = { static_cast<float>(tu), static_cast<float>(tv) };
			// 1 quad[1].texCoords = { static_cast<float>(tu + sizex), static_cast<float>(tv) };
			// 4 quad[2].texCoords = { static_cast<float>(tu + sizex), static_cast<float>(tv + sizey) };
			// 2 quad[3].texCoords = { static_cast<float>(tu), static_cast<float>(tv + sizey) };
			// 0 1 2 2 4 1
			vertices[0].texCoords = { static_cast<float>(pixel_pos.x), static_cast<float>(pixel_pos.y) };
			vertices[1].texCoords = { static_cast<float>(pixel_pos.x + tile_size.x), static_cast<float>(pixel_pos.y) };
			vertices[2].texCoords = { static_cast<float>(pixel_pos.x), static_cast<float>(pixel_pos.y + tile_size.y) };
			vertices[3].texCoords = { static_cast<float>(pixel_pos.x), static_cast<float>(pixel_pos.y + tile_size.y) };
			vertices[4].texCoords = { static_cast<float>(pixel_pos.x + tile_size.x), static_cast<float>(pixel_pos.y + tile_size.y) };
			vertices[5].texCoords = { static_cast<float>(pixel_pos.x + tile_size.x), static_cast<float>(pixel_pos.y) };

			// Set color.
			vertices[0].color = sf::Color::White;
			vertices[1].color = sf::Color::White;
			vertices[2].color = sf::Color::White;
			vertices[3].color = sf::Color::White;
			vertices[4].color = sf::Color::White;
			vertices[5].color = sf::Color::White;

			// Append triangles.
			for (auto& v : vertices)
				vertex_array.append(v);

			++tile_index;
		}

		// Set our render state.
		sf::RenderStates state;

		// Load the texture image.
		filesystem::path image = tileset->File;
		auto id = resources->FindId<sf::Texture>(image.filename().string());
		if (id != 0)
		{
			if (auto t = tileset_textures[id].lock(); t != nullptr)
			{
				state.texture = t.get();
				state.blendMode = sf::BlendNone;
				render_texture->draw(vertex_array, state);
			}
		}
	}

	// Register that we loaded the tiles for this chunk.
	chunk.TilesLoaded = true;
	return render_texture;
}

TMXRenderComponent::~TMXRenderComponent()
{
}

void TMXRenderComponent::Initialize(ComponentEntity& owner)
{
	if (auto p_so = dynamic_cast<SceneObject*>(&owner))
	{
		// Re-acquire our scene object from the server so it is properly reference counted by shared_ptr.
		auto game = BabyDI::Get<tdrp::Game>();
		auto so = game->Server->GetSceneObjectById(p_so->ID);
		if (so->GetType() == SceneObjectType::TMX)
		{
			auto so_tmx = std::dynamic_pointer_cast<TMXSceneObject>(so);
			m_owner = so_tmx;
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
		for (const auto& tileset : so->Tilesets)
		{
			if (auto id = loader::LoadTexture(tileset.second->File); id != 0)
			{
				auto handle = resources->Get<sf::Texture>(id);
				m_textures.insert(std::make_pair(id, handle));
			}
		}

		SetMaxChunks(so->Chunks.size());
	}
}

void TMXRenderComponent::OnDetached(ComponentEntity& owner)
{
}

/////////////////////////////

Rectf TMXRenderComponent::GetBoundingBox() const
{
	if (auto so = m_owner.lock())
		return so->GetBounds();

	return Rectf{};
}

void TMXRenderComponent::Render(sf::RenderTarget& window, const Rectf& viewRect, std::chrono::milliseconds elapsed)
{
	if (auto so = m_owner.lock())
	{
		if (m_textures.empty())
			return;

		auto pos = so->GetPosition();
		auto scale = so->GetScale();
		auto rotate = so->GetAngle();

		// Get rendered chunks.
		so->CalculateChunksToRender(viewRect, m_chunks_to_render);

		// Render each chunk.
		for (auto chunk_idx : m_chunks_to_render)
		{
			if (chunk_idx >= m_chunks.size())
				continue;

			// If we are awaiting this chunk data, skip it.
			auto& so_chunk = so->Chunks.at(chunk_idx);
			if (so_chunk.ChunkRequested)
				continue;

			// Try to get the chunk we are going to render.
			auto& chunk = m_chunks[chunk_idx];
			if (!chunk.has_value())
			{
				// If we don't have the chunk, and we know the map, render it.
				if (auto map = so->GetMap(); map != nullptr)
					chunk = RenderChunkToTexture(chunk_idx, map);
			}

			// If we still don't have the chunk, and we are a guest, request the chunk data.
			if (!chunk.has_value())
			{
				auto game = BabyDI::Get<tdrp::Game>();
				if (game->Server->IsGuest())
				{
					so_chunk.ChunkRequested = true;
					game->Server->RequestSceneObjectChunkData(so, chunk_idx);
				}
				continue;
			}

			// Render the chunk.
			auto& cv = chunk.value();
			if (cv.RenderTexture && cv.Sprite.has_value())
			{
				sf::RenderStates state{};
				state.transform
					.translate({ pos.x + cv.Bounds.pos.x, pos.y + cv.Bounds.pos.y })
					.rotate(sf::degrees(rotate))
					.scale({ scale.x, scale.y });

				window.draw(cv.Sprite.value(), state);
			}
		}

		if (Settings->GetAs<bool>("Debug.drawtmxbbox"))
		{
			tdrp::render::Window::RenderPhysics(window, so);
		}
	}
}

/////////////////////////////

void TMXRenderComponent::SetMaxChunks(size_t max)
{
	m_chunks.resize(max);
	m_chunks_to_render.reserve(max);
}

std::optional<TMXRenderComponent::Chunk> TMXRenderComponent::RenderChunkToTexture(uint32_t chunk_idx, std::shared_ptr<tmx::Map> tmx)
{
	auto so = m_owner.lock();
	if (!so || so->Tilesets.empty())
		return std::nullopt;

	const auto& layers = so->GetMap()->getLayers();
	if (so->Layer >= layers.size())
		return std::nullopt;

	const auto& layer = layers.at(so->Layer);
	if (layer->getType() != tmx::Layer::Type::Tile)
		return std::nullopt;

	const auto& tilelayer = layers.at(so->Layer)->getLayerAs<tmx::TileLayer>();
	if (chunk_idx >= so->Chunks.size() || chunk_idx >= tilelayer.getChunks().size())
		return std::nullopt;

	const auto& mapchunk = tilelayer.getChunks().at(chunk_idx);
	const auto& chunks = so->Chunks;
	const auto& chunk = chunks.at(chunk_idx);

	// Get the tile size.
	const auto& first_tileset = so->Tilesets.front().second;
	const auto& tile_size = first_tileset->TileDimensions;

	// Save the bounds of the chunk for positioning.
	Chunk render_chunk{};
	render_chunk.Bounds =
	{
		static_cast<float>(chunk.Position.x * tile_size.x),
		static_cast<float>(chunk.Position.y * tile_size.y),
		static_cast<float>(chunk.Size.x * tile_size.x),
		static_cast<float>(chunk.Size.y * tile_size.y)
	};

	// Calculate sizes.
	auto tile_count = chunk.Size.x * chunk.Size.y;
	auto tiles = mapchunk.tiles | std::ranges::views::transform([](const tmx::TileLayer::Tile& tile) -> const uint32_t { return tile.ID; });

	auto render_texture = renderToTexture(chunk_idx, render_chunk, so, m_textures, tmx->getRenderOrder(), tiles);
	if (!render_texture)
		return std::nullopt;

	// Finish the rendering on our texture.
	render_texture->display();

	// Save our sprite and texture via the chunk index.
	render_chunk.Sprite = std::make_optional<sf::Sprite>(render_texture->getTexture());
	render_chunk.RenderTexture = render_texture;

	// log::PrintLine(log::game, "TMX [{}] Rendered texture for chunk {}.", so->ID, chunk_idx);
	return std::make_optional(std::move(render_chunk));
}

void TMXRenderComponent::RenderChunkToTexture(uint32_t chunk_idx, const std::span<const uint32_t>& tiles)
{
	auto so = m_owner.lock();
	if (!so || so->Tilesets.empty())
		return;

	auto& chunks = so->Chunks;
	if (chunk_idx >= chunks.size())
		return;

	auto& chunk = chunks.at(chunk_idx);
	if (chunk.TilesLoaded)
		return;

	// Get the tile size.
	const auto& first_tileset = so->Tilesets.front().second;
	const auto& tile_size = first_tileset->TileDimensions;

	// Save the bounds of the chunk for positioning.
	Chunk render_chunk{};
	render_chunk.Bounds =
	{
		static_cast<float>(chunk.Position.x * tile_size.x),
		static_cast<float>(chunk.Position.y * tile_size.y),
		static_cast<float>(chunk.Size.x * tile_size.x),
		static_cast<float>(chunk.Size.y * tile_size.y)
	};

	// Calculate sizes.
	auto tile_count = chunk.Size.x * chunk.Size.y;

	auto render_texture = renderToTexture(chunk_idx, render_chunk, so, m_textures, so->RenderOrder, tiles);
	if (!render_texture)
		return;

	// If the chunk was requested, remove that flag.
	chunk.ChunkRequested = false;

	// Finish the rendering on our texture.
	render_texture->display();

	// Save our sprite and texture via the chunk index.
	render_chunk.Sprite = std::make_optional<sf::Sprite>(render_texture->getTexture());
	render_chunk.RenderTexture = render_texture;

	// log::PrintLine(log::game, "TMX [{}] Rendered texture for chunk {}.", so->ID, chunk_idx);

	if (m_chunks.size() <= chunk_idx)
	{
		m_chunks.resize(static_cast<size_t>(chunk_idx) + 1);
		m_chunks_to_render.reserve(static_cast<size_t>(chunk_idx) + 1);
	}
	m_chunks[chunk_idx] = std::make_optional(std::move(render_chunk));
}

} // end namespace tdrp::component::render
