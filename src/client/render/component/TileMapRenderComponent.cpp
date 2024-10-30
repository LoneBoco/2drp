#include "client/game/Game.h"
#include "client/render/component/TileMapRenderComponent.h"
#include "client/loader/SFMListream.h"
#include "client/loader/ResourceLoaders.h"

#include "engine/filesystem/File.h"
#include "engine/scene/SceneObject.h"


namespace tdrp::render::component
{

void TileMapRenderComponent::Initialize(ComponentEntity& owner)
{
	if (auto p_so = dynamic_cast<SceneObject*>(&owner))
	{
		// Re-acquire our scene object from the server so it is properly reference counted by shared_ptr.
		auto game = BabyDI::Get<tdrp::Game>();
		m_owner = game->Server.GetSceneObjectById(p_so->ID);
	}
}

void TileMapRenderComponent::OnAttached(ComponentEntity& owner)
{
	auto resources = BabyDI::Get<tdrp::ResourceManager>();

	m_textures.clear();
	m_sounds.clear();

	if (auto so = m_owner.lock())
	{
		auto tiled_so = std::dynamic_pointer_cast<TiledSceneObject>(so);
		if (!tiled_so)
			return;

		// Load the images.
		{
			std::string image = tiled_so->Tileset->File;

			auto id = loader::LoadTexture(image);
			if (id != 0)
			{
				auto handle = resources->Get<sf::Texture>(id);
				m_textures.insert(std::make_pair(id, handle));
			}
		}

		sf::VertexArray vertices;
		auto tileset = m_textures.begin()->second.lock();
		if (tileset == nullptr)
			return;

		// Create vertex array.
		{
			auto tile_size = tiled_so->Tileset->TileDimensions;
			auto tile_count = tiled_so->Dimension;

			vertices.setPrimitiveType(sf::PrimitiveType::Triangles);
			vertices.resize(tile_count.x * tile_count.y * 6);

			for (size_t i = 0; i < tiled_so->TileData.size(); i += 2)
			{
				auto tile_number = i / 2;
				auto x = tile_number % tile_count.x;
				auto y = tile_number / tile_count.x;
				auto tu = tiled_so->TileData[i];
				auto tv = tiled_so->TileData[i + 1];

				/*
				* 0--1   5
				* | /   /|
				* |/   /_|
				* 2   3  4
				*/

				auto* quad = &vertices[tile_number * 6];
				// 0 quad[0].position = { static_cast<float>(x * tile_size.x), static_cast<float>(y * tile_size.y) };
				// 1 quad[1].position = { static_cast<float>((x + 1) * tile_size.x), static_cast<float>(y * tile_size.y) };
				// 4 quad[2].position = { static_cast<float>((x + 1) * tile_size.x), static_cast<float>((y + 1) * tile_size.y) };
				// 2 quad[4].position = { static_cast<float>(x * tile_size.x), static_cast<float>((y + 1) * tile_size.y) };

				quad[0].position = { static_cast<float>(x * tile_size.x), static_cast<float>(y * tile_size.y) };
				quad[1].position = { static_cast<float>((x + 1) * tile_size.x), static_cast<float>(y * tile_size.y) };
				quad[2].position = { static_cast<float>(x * tile_size.x), static_cast<float>((y + 1) * tile_size.y) };

				quad[3].position = { static_cast<float>(x * tile_size.x), static_cast<float>((y + 1) * tile_size.y) };
				quad[4].position = { static_cast<float>((x + 1) * tile_size.x), static_cast<float>((y + 1) * tile_size.y) };
				quad[5].position = { static_cast<float>((x + 1) * tile_size.x), static_cast<float>(y * tile_size.y) };


				// 0 quad[0].texCoords = { static_cast<float>(tu * tile_size.x), static_cast<float>(tv * tile_size.y) };
				// 1 quad[1].texCoords = { static_cast<float>((tu + 1) * tile_size.x), static_cast<float>(tv * tile_size.y) };
				// 4 quad[2].texCoords = { static_cast<float>((tu + 1) * tile_size.x), static_cast<float>((tv + 1) * tile_size.y) };
				// 2 quad[3].texCoords = { static_cast<float>(tu * tile_size.x), static_cast<float>((tv + 1) * tile_size.y) };

				quad[0].texCoords = { static_cast<float>(tu * tile_size.x), static_cast<float>(tv * tile_size.y) };
				quad[1].texCoords = { static_cast<float>((tu + 1) * tile_size.x), static_cast<float>(tv * tile_size.y) };
				quad[2].texCoords = { static_cast<float>(tu * tile_size.x), static_cast<float>((tv + 1) * tile_size.y) };

				quad[3].texCoords = { static_cast<float>(tu * tile_size.x), static_cast<float>((tv + 1) * tile_size.y) };
				quad[4].texCoords = { static_cast<float>((tu + 1) * tile_size.x), static_cast<float>((tv + 1) * tile_size.y) };
				quad[5].texCoords = { static_cast<float>((tu + 1) * tile_size.x), static_cast<float>(tv * tile_size.y) };
			}
		}

		// Render the sprites.
		{
			int sprite_pixels_x = tiled_so->Dimension.x * tiled_so->Tileset->TileDimensions.x;
			int sprite_pixels_y = tiled_so->Dimension.y * tiled_so->Tileset->TileDimensions.y;
			int sprite_count_x = 1 + sprite_pixels_x / sf::Texture::getMaximumSize();
			int sprite_count_y = 1 + sprite_pixels_y / sf::Texture::getMaximumSize();
			int sprites = sprite_count_x * sprite_count_y;

			for (auto i = 0; i < sprites; ++i)
			{
				int x = i % sprite_count_x;
				int y = i / sprite_count_x;
				int width = std::min(sf::Texture::getMaximumSize(), sprite_pixels_x - (sf::Texture::getMaximumSize() * x));
				int height = std::min(sf::Texture::getMaximumSize(), sprite_pixels_y - (sf::Texture::getMaximumSize() * y));
				width = tdrp::math::next_power_of_two(width);
				height = tdrp::math::next_power_of_two(height);

				sf::RenderStates state;
				state.transform.translate({ static_cast<float>(-x * sf::Texture::getMaximumSize()), static_cast<float>(-y * sf::Texture::getMaximumSize()) });
				state.texture = tileset.get();

				auto texture = std::make_shared<sf::RenderTexture>();
				auto success = texture->create(width, height);
				if (!success) continue;

				texture->clear();
				texture->draw(vertices, state);
				texture->display();

				sf::Sprite sprite{ texture->getTexture() };
				sprite.setPosition({ static_cast<float>(x * sf::Texture::getMaximumSize()), static_cast<float>(y * sf::Texture::getMaximumSize()) });
				m_sprites.push_back(std::move(sprite));

				m_render_textures.push_back(texture);
			}
		}
	}
}

void TileMapRenderComponent::OnDetached(ComponentEntity& owner)
{

}

/////////////////////////////

Rectf TileMapRenderComponent::GetBoundingBox() const
{
	if (auto so = m_owner.lock())
	{
		auto pos = so->GetPosition();

		if (auto tiled_so = std::dynamic_pointer_cast<TiledSceneObject>(so))
		{
			auto tile_size = tiled_so->Tileset->TileDimensions;
			auto tile_count = tiled_so->Dimension;
			auto scale = so->GetScale();

			return Rectf{ pos.x, pos.y, tile_count.x * tile_size.x * scale.x, tile_count.x * tile_size.x * scale.y };
		}
		return Rectf{ pos.x, pos.y, 0.0f, 0.0f };
	}

	return Rectf{};
}

void TileMapRenderComponent::Render(sf::RenderTarget& window, const Rectf& viewRect, std::chrono::milliseconds elapsed)
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

		for (auto& sprite : m_sprites)
		{
			window.draw(sprite, state);
		}
	}
}

} // end namespace tdrp::render::component
