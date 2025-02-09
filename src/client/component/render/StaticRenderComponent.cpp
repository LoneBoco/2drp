#include "client/game/Game.h"
#include "client/component/render/StaticRenderComponent.h"
#include "client/loader/SFMListream.h"
#include "client/loader/ResourceLoaders.h"

#include "engine/filesystem/File.h"
#include "engine/scene/SceneObject.h"

#include <PlayRho/PlayRho.hpp>
#include <SFML/Graphics/Sprite.hpp>


namespace tdrp::component::render
{

void StaticRenderComponent::Initialize(ComponentEntity& owner)
{
	if (auto p_so = dynamic_cast<SceneObject*>(&owner))
	{
		// Re-acquire our scene object from the server so it is properly reference counted by shared_ptr.
		auto game = BabyDI::Get<tdrp::Game>();
		m_owner = game->Server->GetSceneObjectById(p_so->ID);

		// Bind our provider.
		owner.RegisterProvider("Size", std::bind(&StaticRenderComponent::provide_size, this));
		owner.RegisterProvider("BoundingBox", std::bind(&StaticRenderComponent::provide_boundingbox, this));
	}
}

void StaticRenderComponent::OnAttached(ComponentEntity& owner)
{
	auto resources = BabyDI::Get<tdrp::ResourceManager>();

	m_texture.reset();

	if (auto so = m_owner.lock())
	{
		if (so->GetType() == SceneObjectType::STATIC)
		{
			load_image(so->GetImage());
		}

		m_handle_image_change = so->Properties.Get(Property::IMAGE)->UpdateDispatch.Subscribe(std::bind(&StaticRenderComponent::image_property_update, this, std::placeholders::_1));
	}
}

void StaticRenderComponent::OnDetached(ComponentEntity& owner)
{
}

/////////////////////////////

Rectf StaticRenderComponent::GetBoundingBox() const
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
				if (m_texture.expired())
					return Rectf{ pos.x, pos.y, 0.0f, 0.0f };

				auto texture = m_texture.lock();
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

void StaticRenderComponent::Render(sf::RenderTarget& window, const Rectf& viewRect, std::chrono::milliseconds elapsed)
{
	if (!m_sprite.has_value())
		return;

	if (auto so = m_owner.lock())
	{
		if (so->GetType() != SceneObjectType::STATIC)
			return;

		if (m_texture.expired())
			return;

		auto pos = so->GetPosition();
		auto scale = so->GetScale();

		m_sprite.value().setPosition({ pos.x, pos.y });
		m_sprite.value().setScale({ scale.x, scale.y });

		// Draw the bounding box first to avoid a weird SFML bug.
		if (Settings->GetAs<bool>("Debug.drawstaticbbox"))
		{
			sf::RectangleShape shape;
			auto bbox = GetBoundingBox();
			shape.setFillColor(sf::Color::Transparent);
			shape.setOutlineColor(sf::Color::Red);
			shape.setOutlineThickness(1.0f);
			shape.setPosition({ bbox.pos.x, bbox.pos.y });
			shape.setSize({ bbox.size.x, bbox.size.y });
			window.draw(shape);

			tdrp::render::Window::RenderPhysics(window, so);
		}

		window.draw(m_sprite.value());
	}
}

///////////////////////////////////////////////////////////////////////////////

void StaticRenderComponent::load_image(const std::string& image)
{
	auto resources = BabyDI::Get<tdrp::ResourceManager>();
	auto id = loader::LoadTexture(image);
	if (id != 0)
	{
		auto handle = resources->Get<sf::Texture>(id);
		m_texture = handle;

		if (auto texture = m_texture.lock())
		{
			m_sprite = std::make_optional<sf::Sprite>(*texture);
		}
	}
}

void StaticRenderComponent::image_property_update(uint16_t attribute_id)
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

std::any StaticRenderComponent::provide_size() const
{
	auto box = GetBoundingBox();
	return std::make_any<Vector2df>(box.size);
}

std::any StaticRenderComponent::provide_boundingbox() const
{
	auto box = GetBoundingBox();
	return std::make_any<Rectf>(box);
}

} // end namespace tdrp::component::render
