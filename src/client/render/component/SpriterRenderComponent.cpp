#include "client/game/Game.h"
#include "client/render/component/SpriterRenderComponent.h"
#include "client/render/resource/SFMListream.h"

#include "engine/filesystem/File.h"
#include "engine/scene/SceneObject.h"

#include "client/loader/spriter/filefactory.h"
#include "client/loader/spriter/objectfactory.h"


namespace tdrp::render::component
{

void SpriterRenderComponent::Initialize(ComponentEntity& owner)
{
	if (auto p_so = dynamic_cast<SceneObject*>(&owner))
	{
		// Re-acquire our scene object from the server so it is properly reference counted by shared_ptr.
		auto game = BabyDI::Get<tdrp::Game>();
		m_owner = game->Server.GetSceneObjectById(p_so->ID);

		// Bind our provider.
		owner.RegisterProvider("Size", std::bind(&SpriterRenderComponent::provide_size, this));
	}
}

void SpriterRenderComponent::OnAttached(ComponentEntity& owner)
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
		// m_handle_image = so->Properties.Get(Property::IMAGE)->UpdateDispatch.Subscribe(std::bind(&SpriterRenderComponent::property_update_image, this, std::placeholders::_1));
		m_handle_entity = so->Properties.Get(Property::ENTITY)->UpdateDispatch.Subscribe(std::bind(&SpriterRenderComponent::property_update_entity, this, std::placeholders::_1));
		m_handle_animation = so->Properties.Get(Property::ANIMATION)->UpdateDispatch.Subscribe(std::bind(&SpriterRenderComponent::property_update_animation, this, std::placeholders::_1));
		m_handle_attributes = so->Attributes.DirtyUpdateDispatch.Subscribe(std::bind(&SpriterRenderComponent::attributes_update, this, std::placeholders::_1));
	}
}

void SpriterRenderComponent::OnDetached(ComponentEntity& owner)
{

}

/////////////////////////////

Rectf SpriterRenderComponent::GetBoundingBox() const
{
	if (auto so = m_owner.lock())
	{
		auto pos = so->GetPosition();

		if (so->GetType() != SceneObjectType::ANIMATED)
			return Rectf{ pos.x, pos.y, 0.0f, 0.0f };

		auto animated_so = std::dynamic_pointer_cast<AnimatedSceneObject>(so);
		if (!animated_so || !animated_so->Animation)
			return Rectf{ pos.x, pos.y, 0.0f, 0.0f };

		auto scale = so->GetScale();
		auto size = animated_so->Animation->getSize();
		
		return Rectf{ pos.x, pos.y, static_cast<float>(size.x) * scale.x, static_cast<float>(size.y) * scale.y };
	}

	return Rectf{ 0.0f };
}

void SpriterRenderComponent::Render(sf::RenderTarget& window, std::chrono::milliseconds elapsed)
{
	if (auto so = m_owner.lock())
	{
		if (so->GetType() != SceneObjectType::ANIMATED)
			return;

		auto animated_so = std::dynamic_pointer_cast<AnimatedSceneObject>(so);
		if (!animated_so || !animated_so->Animation)
			return;

		animated_so->Animation->setScale(SpriterEngine::point{ animated_so->GetScale().x, animated_so->GetScale().y });
		animated_so->Animation->setPosition(SpriterEngine::point{ animated_so->GetPosition().x, animated_so->GetPosition().y });

		// Time elapsed right before render so our position gets updated properly.
		animated_so->Animation->setTimeElapsed(elapsed.count());
		animated_so->Animation->render();
	}
}

///////////////////////////////////////////////////////////////////////////////

void SpriterRenderComponent::load_animation(const std::string& model, const std::string& entity, const std::string& animation)
{
	// m_textures.clear();

	auto resources = BabyDI::Get<tdrp::ResourceManager>();
	auto game = BabyDI::Get<tdrp::Game>();

	auto id = resources->FindId<SpriterEngine::SpriterModel>(model);
	if (id == 0)
	{
		auto filepath = game->Server.FileSystem.GetFilePath(model);
		if (!filepath.empty())
		{
			auto spritermodel = std::make_shared<SpriterEngine::SpriterModel>(new loader::spriter::FileFactory(game->GetRenderWindow()), new loader::spriter::ObjectFactory(game->GetRenderWindow()));
			spritermodel->loadFile(filepath.string());

			id = resources->Add(model, std::move(spritermodel));
		}
	}
	if (id == 0)
		return;

	if (auto so = m_owner.lock())
	{
		if (so->GetType() != SceneObjectType::ANIMATED)
			return;

		auto animated_so = std::dynamic_pointer_cast<AnimatedSceneObject>(so);
		if (!animated_so)
			return;

		if (auto sm = resources->Get<SpriterEngine::SpriterModel>(id).lock())
		{
			auto e = std::shared_ptr<SpriterEngine::EntityInstance>(sm->getNewEntityInstance(entity));
			if (e)
			{
				animated_so->Animation = e;
				e->setCurrentAnimation(animation);

				// TODO: Set stuff like scale.
			}
		}
	}
}

void SpriterRenderComponent::property_update_entity(const uint16_t attribute_id)
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

		load_animation(animated_so->GetAnimationModel(), animated_so->GetAnimationEntity());
	}
}

void SpriterRenderComponent::property_update_animation(const uint16_t attribute_id)
{
	if (PropertyById(attribute_id) != Property::ANIMATION)
		return;

	if (auto so = m_owner.lock())
	{
		if (so->GetType() != SceneObjectType::ANIMATED)
			return;

		auto animated_so = std::dynamic_pointer_cast<AnimatedSceneObject>(so);
		if (!animated_so || !animated_so->Animation)
			return;

		auto animation = animated_so->GetAnimation();
		if (animated_so->Animation->currentAnimationName() == animation)
		{
			animated_so->Animation->setCurrentTime(0);
			return;
		}

		animated_so->Animation->setCurrentAnimation(animation);
	}
}

void SpriterRenderComponent::attributes_update(const uint16_t attribute_id)
{
	if (auto so = m_owner.lock())
	{
		if (so->GetType() != SceneObjectType::ANIMATED)
			return;

		auto animated_so = std::dynamic_pointer_cast<AnimatedSceneObject>(so);
		if (!animated_so || !animated_so->Animation)
			return;

		auto attribute = so->Attributes.Get(attribute_id);
		auto variable = animated_so->Animation->getVariable(attribute->GetName());
		if (variable)
		{
			switch (attribute->GetType())
			{
				case AttributeType::STRING:
					variable->setStringValue(attribute->GetString());
					break;
				case AttributeType::FLOAT:
				case AttributeType::DOUBLE:
					variable->setRealValue(static_cast<SpriterEngine::real>(attribute->GetDouble()));
					break;
				case AttributeType::SIGNED:
				case AttributeType::UNSIGNED:
					variable->setIntValue(static_cast<int>(attribute->GetSigned()));
					break;
			}
		}
	}
}

std::any SpriterRenderComponent::provide_size()
{
	auto box = GetBoundingBox();
	return std::make_any<Vector2df>(box.size);
}

} // end namespace tdrp::render::component
