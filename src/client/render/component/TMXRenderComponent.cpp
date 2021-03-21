#include <tmxlite/Map.hpp>
#include <tmxlite/TileLayer.hpp>

#include "client/game/Game.h"
#include "client/render/component/TMXRenderComponent.h"
#include "client/render/resource/SFMListream.h"

#include "engine/filesystem/File.h"
#include "engine/scene/SceneObject.h"


namespace tdrp::render::component
{

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

				auto id = resources->FindId<sf::Texture>(image.filename().string());
				if (id == 0)
				{
					auto game = BabyDI::Get<tdrp::Game>();

					auto file = game->Server.FileSystem.GetFile(image);
					if (file && file->Opened())
					{
						auto texture = std::make_shared<sf::Texture>();

						tdrp::render::SFMListream stream(*file);
						texture->loadFromStream(stream);

						id = resources->Add(image.filename().string(), std::move(texture));

						auto handle = resources->Get<sf::Texture>(id);
						m_textures.insert(std::make_pair(id, handle));
					}
				}
				else
				{
					auto handle = resources->Get<sf::Texture>(id);
					m_textures.insert(std::make_pair(id, handle));
				}
			}
		}

		const auto& layer = so->TmxMap->getLayers().at(so->Layer);
		if (!layer || layer->getType() != tmx::Layer::Type::Tile)
			return;

		const auto& tilelayer = layer->getLayerAs<tmx::TileLayer>();

		// Create vertex array.
		{
			auto tile_size = so->TmxMap->getTileSize();
			auto chunk_size = tilelayer.getChunks().at(0).size;
			auto tile_count = chunk_size.x * chunk_size.y;

			m_chunk_vertices.setPrimitiveType(sf::Quads);
			m_chunk_vertices.resize(chunk_size.x * chunk_size.y * 4);

			for (size_t i = 0; i < static_cast<size_t>(tile_count); ++i)
			{
				Vector2di pos{ getTilePosition(chunk_size, i) };

				auto* quad = &m_chunk_vertices[i * 4];
				quad[0].position = { static_cast<float>(pos.x * tile_size.x), static_cast<float>(pos.y * tile_size.y) };
				quad[1].position = { static_cast<float>((pos.x + 1) * tile_size.x), static_cast<float>(pos.y * tile_size.y) };
				quad[2].position = { static_cast<float>((pos.x + 1) * tile_size.x), static_cast<float>((pos.y + 1) * tile_size.y) };
				quad[3].position = { static_cast<float>(pos.x * tile_size.x), static_cast<float>((pos.y + 1) * tile_size.y) };
			}
		}

		// Render initial chunks.
		auto chunks = getVisibleChunks(game->Camera.GetViewRect());
		for (const auto& chunk : chunks)
		{
			renderChunkToTexture(chunk);
		}
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
		auto bounds = m_tmx->getBounds();

		return Rectf{ pos.x - bounds.left, pos.y - bounds.top, bounds.width, bounds.height };
	}

	return Rectf{};
}

void TMXRenderComponent::Render(sf::RenderTarget& window)
{
	if (auto so = m_owner.lock())
	{
		if (m_textures.empty())
			return;

		auto pos = so->GetPosition();
		auto scale = so->GetScale();
		auto rotate = so->GetRotation();

		sf::RenderStates state;
		state.transform
			.translate({ pos.x, pos.y })
			.rotate(rotate)
			.scale({ scale.x, scale.y });

		auto game = BabyDI::Get<tdrp::Game>();
		auto chunks = getVisibleChunks(game->Camera.GetViewRect());

		for (const auto& chunk : chunks)
		{
			if (m_sprites.contains(chunk))
				window.draw(m_sprites.at(chunk));
			else
				renderChunkToTexture(chunk);
		}
	}
}

std::vector<size_t> TMXRenderComponent::getVisibleChunks(const Rectf& view) const
{
	std::vector<size_t> result;

	if (auto so = m_owner.lock())
	{
		const auto& layer = m_tmx->getLayers().at(so->Layer);
		if (!layer || layer->getType() != tmx::Layer::Type::Tile)
			return result;

		const auto& tilesize = so->TmxMap->getTileSize();
		const auto& tilelayer = layer->getLayerAs<tmx::TileLayer>();

		auto pos = so->GetPosition();

		// Iterate through the chunks and test if they are within our view rect.
		for (size_t i = 0; i < tilelayer.getChunks().size(); ++i)
		{
			const auto& chunk = tilelayer.getChunks().at(i);

			Rectf rect{ pos.x + static_cast<float>(chunk.position.x) * tilesize.x, pos.y + chunk.position.y * tilesize.y
				, chunk.size.x * static_cast<float>(tilesize.x), chunk.size.y * static_cast<float>(tilesize.y) };
			if (math::containsOrIntersects(rect, view))
				result.push_back(i);
		}
	}

	return result;
}

/*
const tmx::Tileset::Tile* TMXRenderComponent::getTileFromTilesets(uint32_t ID) const
{
	const auto& tilesets = m_tmx->getTilesets();
	for (const auto& tileset : tilesets)
	{
		const auto* tile = tileset.getTile(ID);
		if (tile != nullptr)
			return tile;
	}

	return nullptr;
}
*/

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

void TMXRenderComponent::renderChunkToTexture(size_t index)
{
	auto resources = BabyDI::Get<tdrp::ResourceManager>();

	if (auto so = m_owner.lock())
	{
		const auto& layer = m_tmx->getLayers().at(so->Layer);
		if (!layer || layer->getType() != tmx::Layer::Type::Tile)
			return;

		const auto& tilelayer = layer->getLayerAs<tmx::TileLayer>();
		if (tilelayer.getChunks().size() <= index)
			return;

		const auto& chunk = tilelayer.getChunks().at(index);
		if (chunk.size.x * chunk.size.y * 4 != m_chunk_vertices.getVertexCount())
			return;

		const auto tilesize = m_tmx->getTileSize();
		const auto& tilesets = m_tmx->getTilesets();
		Vector2di size_in_pixels{ chunk.size.x * static_cast<int>(tilesize.x), chunk.size.y * static_cast<int>(tilesize.y) };

		// Check if this chunk pushes our bounds out.
		if (chunk.position.x < so->Bounds.pos.x)
			so->Bounds.pos.x = static_cast<float>(chunk.position.x);
		if (chunk.position.y < so->Bounds.pos.y)
			so->Bounds.pos.y = static_cast<float>(chunk.position.y);
		if (chunk.position.x + size_in_pixels.x > so->Bounds.pos.x + so->Bounds.size.x)
			so->Bounds.size.x = chunk.position.x + size_in_pixels.x - so->Bounds.pos.x;
		if (chunk.position.y + size_in_pixels.y > so->Bounds.pos.y + so->Bounds.size.y)
			so->Bounds.size.y = chunk.position.y + size_in_pixels.y - so->Bounds.pos.y;

		// Set our render state.
		sf::RenderStates state;

		// Create our texture.
		auto texture = std::make_shared<sf::RenderTexture>();
		texture->create(size_in_pixels.x, size_in_pixels.y);
		texture->clear(sf::Color::Transparent);

		for (const auto& tileset : tilesets)
		{
			// Configure the texture coordinates for this tileset.
			bool hasTile = false;
			for (size_t i = 0; i < chunk.tiles.size(); ++i)
			{
				auto* quad = &m_chunk_vertices[i * 4];

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
					quad[0].color = sf::Color::Transparent;
					quad[1].color = sf::Color::Transparent;
					quad[2].color = sf::Color::Transparent;
					quad[3].color = sf::Color::Transparent;
					continue;
				}

				// Record that we have a tile.  If we have no tiles, we skip rendering this tileset later.
				hasTile = true;

				auto tu = tile->imagePosition.x;
				auto tv = tile->imagePosition.y;
				auto sizex = tile->imageSize.x;
				auto sizey = tile->imageSize.y;

				// Construct our texture coordinates.
				quad[0].texCoords = { static_cast<float>(tu), static_cast<float>(tv) };
				quad[1].texCoords = { static_cast<float>(tu + sizex), static_cast<float>(tv) };
				quad[2].texCoords = { static_cast<float>(tu + sizex), static_cast<float>(tv + sizey) };
				quad[3].texCoords = { static_cast<float>(tu), static_cast<float>(tv + sizey) };

				// Set color.
				quad[0].color = sf::Color::White;
				quad[1].color = sf::Color::White;
				quad[2].color = sf::Color::White;
				quad[3].color = sf::Color::White;
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
		sprite.setPosition(static_cast<float>(chunk.position.x), static_cast<float>(chunk.position.y));

		// Save our sprite and texture via the chunk index.
		m_sprites.insert({ index, std::move(sprite) });
		m_render_textures.insert({ index, texture });
	}
}

} // end namespace tdrp::render::component
