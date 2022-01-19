#include "client/game/Game.h"
#include "client/render/component/RenderComponent.h"
#include "client/loader/SFMListream.h"
#include "client/loader/ResourceLoaders.h"

#include "engine/filesystem/File.h"
#include "engine/scene/SceneObject.h"


namespace tdrp::render::component
{

void RenderComponent::Initialize(ComponentEntity& owner)
{
	if (auto p_so = dynamic_cast<SceneObject*>(&owner))
	{
		// Re-acquire our scene object from the server so it is properly reference counted by shared_ptr.
		auto game = BabyDI::Get<tdrp::Game>();
		m_owner = game->Server.GetSceneObjectById(p_so->ID);

		// Bind our provider.
		owner.RegisterProvider("Size", std::bind(&RenderComponent::provide_size, this));
	}
}

void RenderComponent::OnAttached(ComponentEntity& owner)
{
	auto resources = BabyDI::Get<tdrp::ResourceManager>();

	m_textures.clear();
	//m_sounds.clear();

	if (auto so = m_owner.lock())
	{
		if (so->GetType() == SceneObjectType::STATIC)
		{
			load_image(so->GetImage());
		}

		m_handle_image_change = so->Properties.Get(Property::IMAGE)->UpdateDispatch.Subscribe(std::bind(&RenderComponent::image_property_update, this, std::placeholders::_1));
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
		}
	}

	return Rectf{};
}

void RenderComponent::Render(sf::RenderTarget& window, std::chrono::milliseconds elapsed)
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
		}
	}
}

///////////////////////////////////////////////////////////////////////////////

void RenderComponent::load_image(const std::string& image)
{
	m_textures.clear();

	auto resources = BabyDI::Get<tdrp::ResourceManager>();
	auto id = loader::LoadTexture(image);
	if (id != 0)
	{
		auto handle = resources->Get<sf::Texture>(id);
		m_textures.insert(std::make_pair(id, handle));
	}
}

void RenderComponent::image_property_update(uint16_t attribute_id)
{
	if (PropertyById(attribute_id) != Property::IMAGE)
		return;

	if (auto so = m_owner.lock())
	{
		if (so->GetType() == SceneObjectType::STATIC)
		{
			load_image(so->GetImage());
		}
	}
}

std::any RenderComponent::provide_size()
{
	auto box = GetBoundingBox();
	return std::make_any<Vector2df>(box.size);
}

} // end namespace tdrp::render::component
