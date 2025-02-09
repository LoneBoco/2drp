#include "client/game/Game.h"

#include "client/component/render/AnimationRenderComponent.h"
#include "client/component/render/animation/SpriterAnimation.h"
#include "client/component/render/animation/GaniAnimation.h"

#include "engine/filesystem/File.h"
#include "engine/scene/SceneObject.h"

#include "spriterengine/spriterengine.h"
#include "client/loader/spriter/filefactory.h"
#include "client/loader/spriter/objectfactory.h"


namespace tdrp::component::render
{

void AnimationRenderComponent::Initialize(ComponentEntity& owner)
{
	if (auto p_so = dynamic_cast<SceneObject*>(&owner))
	{
		// Re-acquire our scene object from the server so it is properly reference counted by shared_ptr.
		auto game = BabyDI::Get<tdrp::Game>();
		m_owner = game->Server->GetSceneObjectById(p_so->ID);

		// Bind our provider.
		owner.RegisterProvider("Size", std::bind(&AnimationRenderComponent::provide_size, this));
		owner.RegisterProvider("BoundingBox", std::bind(&AnimationRenderComponent::provide_boundingbox, this));
		owner.RegisterProvider("Animation", std::bind(&AnimationRenderComponent::provide_animation, this));
	}
}

void AnimationRenderComponent::OnAttached(ComponentEntity& owner)
{
	auto resources = BabyDI::Get<tdrp::ResourceManager>();

	// m_textures.clear();
	// m_sounds.clear();

	if (auto so = m_owner.lock())
	{
		auto animated_so = std::dynamic_pointer_cast<AnimatedSceneObject>(so);
		if (!animated_so)
			return;

		// Set up the event watchers.
		m_handle_image = so->Properties.Get(Property::IMAGE)->UpdateDispatch.Subscribe(std::bind(&AnimationRenderComponent::property_update_image, this, std::placeholders::_1));
		m_handle_entity = so->Properties.Get(Property::ENTITY)->UpdateDispatch.Subscribe(std::bind(&AnimationRenderComponent::property_update_entity, this, std::placeholders::_1));
		m_handle_animation = so->Properties.Get(Property::ANIMATION)->UpdateDispatch.Subscribe(std::bind(&AnimationRenderComponent::property_update_animation, this, std::placeholders::_1));
		m_handle_attributes = so->Attributes.DirtyUpdateDispatch.Subscribe(std::bind(&AnimationRenderComponent::attributes_update, this, std::placeholders::_1));

		load_animation();
	}
}

void AnimationRenderComponent::OnDetached(ComponentEntity& owner)
{

}

/////////////////////////////

Rectf AnimationRenderComponent::GetBoundingBox() const
{
	if (auto so = m_owner.lock())
	{
		auto pos = so->GetPosition();

		if (so->GetType() != SceneObjectType::ANIMATED)
			return Rectf{ pos.x, pos.y, 0.0f, 0.0f };

		auto animated_so = std::dynamic_pointer_cast<AnimatedSceneObject>(so);
		if (!animated_so || !m_animation)
			return Rectf{ pos.x, pos.y, 0.0f, 0.0f };

		auto bbox = m_animation->GetBoundingBox();
		bbox.pos += pos;
		return bbox;

		/*
		auto scale = so->GetScale();
		auto size = m_animation->GetSize();
		if (size.has_value())
		{
			if (size.type() == typeid(Vector2df))
			{
				auto s = std::any_cast<Vector2df>(size);
				return Rectf{ pos.x, pos.y, s.x * scale.x, s.y * scale.y };
			}
			else if (size.type() == typeid(Vector2di))
			{
				auto s = std::any_cast<Vector2di>(size);
				return Rectf{ pos.x, pos.y, static_cast<float>(s.x) * scale.x, static_cast<float>(s.y) * scale.y };
			}
		}
		*/
	}

	return Rectf{ 0.0f };
}

void AnimationRenderComponent::Render(sf::RenderTarget& window, const Rectf& viewRect, std::chrono::milliseconds elapsed)
{
	if (auto so = m_owner.lock())
	{
		if (so->GetType() != SceneObjectType::ANIMATED)
			return;

		// Draw the bounding box first to avoid a weird SFML bug.
		if (Settings->GetAs<bool>("Debug.drawanimatedbbox"))
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

		if (m_animation)
			m_animation->Render(window, elapsed);
	}
}

///////////////////////////////////////////////////////////////////////////////

void AnimationRenderComponent::load_animation()
{
	// m_textures.clear();

	auto resources = BabyDI::Get<tdrp::ResourceManager>();
	auto game = BabyDI::Get<tdrp::Game>();

	if (auto so = m_owner.lock())
	{
		if (so->GetType() != SceneObjectType::ANIMATED)
			return;

		auto animated_so = std::dynamic_pointer_cast<AnimatedSceneObject>(so);
		if (!animated_so)
			return;

		filesystem::path image{ animated_so->GetImage() };
		if (image.empty())
			image = animated_so->GetAnimation();
		if (image.empty())
			return;

		if (image.extension() == ".gani")
			m_animation = std::make_unique<animation::GaniAnimation>(m_owner);
		else m_animation = std::make_unique<animation::SpriterAnimation>(m_owner);

		m_animation->Load(image);
	}
}

void AnimationRenderComponent::property_update_image(const uint16_t attribute_id)
{
	if (PropertyById(attribute_id) != Property::IMAGE)
		return;

	if (auto so = m_owner.lock())
	{
		if (so->GetType() != SceneObjectType::ANIMATED)
			return;

		auto animated_so = std::dynamic_pointer_cast<AnimatedSceneObject>(so);
		if (!animated_so)
			return;

		filesystem::path image{ animated_so->GetImage() };

		bool load_new = false;

		if (m_animation == nullptr)
			load_new = true;
		else if (typeid(m_animation.get()) == typeid(animation::SpriterAnimation) && image.extension() == ".gani")
			load_new = true;
		else if (typeid(m_animation.get()) == typeid(animation::GaniAnimation) && image.extension() != ".gani")
			load_new = true;

		if (load_new)
			load_animation();

		m_animation->UpdateImage(animated_so->GetImage());
	}
}

void AnimationRenderComponent::property_update_entity(const uint16_t attribute_id)
{
	if (PropertyById(attribute_id) != Property::ENTITY)
		return;

	if (auto so = m_owner.lock())
	{
		if (so->GetType() != SceneObjectType::ANIMATED)
			return;

		auto animated_so = std::dynamic_pointer_cast<AnimatedSceneObject>(so);
		if (!animated_so)
			return;

		if (m_animation == nullptr)
			return;

		m_animation->UpdateEntity(animated_so->GetEntity());
	}
}

void AnimationRenderComponent::property_update_animation(const uint16_t attribute_id)
{
	if (PropertyById(attribute_id) != Property::ANIMATION)
		return;

	if (auto so = m_owner.lock())
	{
		if (so->GetType() != SceneObjectType::ANIMATED)
			return;

		auto animated_so = std::dynamic_pointer_cast<AnimatedSceneObject>(so);
		if (!animated_so)
			return;

		if (m_animation == nullptr)
			return;

		m_animation->UpdateAnimation(animated_so->GetAnimation());
	}
}

void AnimationRenderComponent::attributes_update(const uint16_t attribute_id)
{
	if (auto so = m_owner.lock())
	{
		if (so->GetType() != SceneObjectType::ANIMATED)
			return;

		auto animated_so = std::dynamic_pointer_cast<AnimatedSceneObject>(so);
		if (!animated_so)
			return;

		if (m_animation == nullptr)
			return;

		m_animation->UpdateAttribute(attribute_id);
	}
}

std::any AnimationRenderComponent::provide_size()
{
	auto box = GetBoundingBox();
	return std::make_any<Vector2df>(box.size);
}

std::any AnimationRenderComponent::provide_boundingbox()
{
	auto box = GetBoundingBox();
	return std::make_any<Rectf>(box);
}

std::any AnimationRenderComponent::provide_animation()
{
	if (m_animation == nullptr)
		return {};
	return m_animation->GetAnimation();
}

} // end namespace tdrp::component::render
