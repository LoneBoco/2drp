#include "client/game/Game.h"
#include "client/render/component/RenderComponent.h"
#include "client/render/resource/SFMListream.h"

#include "engine/filesystem/File.h"
#include "engine/scene/SceneObject.h"

namespace tdrp::render::component
{

void RenderComponent::Initialize(ComponentEntity& owner)
{
	if (auto p_so = dynamic_cast<SceneObject*>(&owner))
	{
		// Re-acquire our scene object from the server so it is properly reference counted by shared_ptr.
		BabyDI::Injected<tdrp::Game> game;
		m_owner = game->Server.GetSceneObjectById(p_so->ID);
	}
}

void RenderComponent::OnAttached(ComponentEntity& owner)
{
	BabyDI::Injected<tdrp::ResourceManager> resources;

	m_textures.clear();
	m_sounds.clear();

	if (auto so = dynamic_cast<SceneObject*>(&owner))
	{
		// Load the images.
		if (so->GetType() == SceneObjectType::STATIC || so->GetType() == SceneObjectType::TILED)
		{
			std::string image;

			if (so->GetType() == SceneObjectType::STATIC)
				image = so->GetImage();
			else
			{
				auto tiled = dynamic_cast<TiledSceneObject*>(&owner);
				image = tiled->Tileset->File;
			}

			auto id = resources->FindId<sf::Texture>(image);
			if (id == 0)
			{
				BabyDI::Injected<tdrp::Game> game;

				auto file = game->Server.FileSystem.GetFile(image);
				if (file && file->Opened())
				{
					auto texture = std::make_shared<sf::Texture>();

					tdrp::render::SFMListream stream(*file);
					texture->loadFromStream(stream);

					id = resources->Add(image, std::move(texture));

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

		// Create vertex array.
		if (so->GetType() == SceneObjectType::TILED)
		{
			if (auto tiled_so = dynamic_cast<TiledSceneObject*>(so))
			{
				auto tile_size = tiled_so->Tileset->TileDimensions;
				auto tile_count = tiled_so->Dimension;
				auto texture = m_textures.begin()->second.lock();
				if (texture == nullptr)
					return;

				m_vertices.setPrimitiveType(sf::Quads);
				m_vertices.resize(tile_count.x * tile_count.y * 4);

				auto texture_size = texture->getSize();
				for (size_t i = 0; i < tiled_so->TileData.size(); i += 2)
				{
					auto tile_number = i / 2;
					auto x = tile_number % tile_count.x;
					auto y = tile_number / tile_count.x;
					auto tu = tiled_so->TileData[i];
					auto tv = tiled_so->TileData[i + 1];

					auto* quad = &m_vertices[tile_number * 4];
					quad[0].position = { static_cast<float>(x * tile_size.x), static_cast<float>(y * tile_size.y) };
					quad[1].position = { static_cast<float>((x + 1) * tile_size.x), static_cast<float>(y * tile_size.y) };
					quad[2].position = { static_cast<float>((x + 1) * tile_size.x), static_cast<float>((y + 1) * tile_size.y) };
					quad[3].position = { static_cast<float>(x * tile_size.x), static_cast<float>((y + 1) * tile_size.y) };

					quad[0].texCoords = { static_cast<float>(tu * tile_size.x), static_cast<float>(tv * tile_size.y) };
					quad[1].texCoords = { static_cast<float>((tu + 1) * tile_size.x), static_cast<float>(tv * tile_size.y) };
					quad[2].texCoords = { static_cast<float>((tu + 1) * tile_size.x), static_cast<float>((tv + 1) * tile_size.y) };
					quad[3].texCoords = { static_cast<float>(tu * tile_size.x), static_cast<float>((tv + 1) * tile_size.y) };
				}
			}
		}
	}
}

void RenderComponent::OnDetached(ComponentEntity& owner)
{

}

/////////////////////////////

Rectf RenderComponent::GetBoundingBox() const
{
	if (auto so = m_owner.lock())
	{
		auto pos = so->GetPosition();

		switch (so->GetType())
		{
			default:
			case SceneObjectType::DEFAULT:
				return Rectf{ pos.x, pos.y, 0.0f, 0.0f };
			
			case SceneObjectType::STATIC:
			{
				if (m_textures.empty())
					return Rectf{ pos.x, pos.y, 0.0f, 0.0f };

				auto texture = m_textures.begin()->second.lock();
				if (texture == nullptr)
					return Rectf{ pos.x, pos.y, 0.0f, 0.0f };

				auto texture_size = texture->getSize();
				auto scale = so->GetScale();

				return Rectf{ pos.x, pos.y, texture_size.x * scale.x, texture_size.y * scale.y };
			}
			
			case SceneObjectType::TILED:
			{
				if (auto tiled_so = dynamic_cast<TiledSceneObject*>(so.get()))
				{
					auto tile_size = tiled_so->Tileset->TileDimensions;
					auto tile_count = tiled_so->Dimension;
					auto scale = so->GetScale();

					return Rectf{ pos.x, pos.y, tile_count.x * tile_size.x * scale.x, tile_count.x * tile_size.x * scale.y };
					
				}
				return Rectf{ pos.x, pos.y, 0.0f, 0.0f };
			}
			
			case SceneObjectType::ANIMATED:
			{
				return Rectf{ pos.x, pos.y, 0.0f, 0.0f };
			}
		}
	}

	return Rectf{};
}

void RenderComponent::Render(sf::RenderTarget& window)
{
	if (auto so = m_owner.lock())
	{
		switch (so->GetType())
		{
			default:
			case SceneObjectType::DEFAULT:
				return;

			case SceneObjectType::STATIC:
			{
				if (m_textures.empty())
					return;

				auto pos = so->GetPosition();
				auto scale = so->GetScale();

				m_sprite.setPosition({ pos.x, pos.y });
				m_sprite.setScale({ scale.x, scale.y });

				auto texture = m_textures.begin()->second.lock();
				if (texture == nullptr)
					return;

				m_sprite.setTexture(*texture, true);

				window.draw(m_sprite);
				return;
			}

			case SceneObjectType::TILED:
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

				auto texture = m_textures.begin()->second.lock();
				if (texture == nullptr)
					return;

				state.texture = texture.get();

				window.draw(m_vertices, state);
				return;
			}

			case SceneObjectType::ANIMATED:
			{
				return;
			}
		}
	}
}

} // end namespace tdrp::render::component
