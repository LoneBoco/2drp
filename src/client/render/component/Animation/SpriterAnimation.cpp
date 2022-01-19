#include "client/render/component/Animation/SpriterAnimation.h"

#include "client/game/Game.h"

#include "engine/filesystem/File.h"
#include "engine/scene/SceneObject.h"
#include "engine/resources/Resource.h"

#include "spriterengine/spriterengine.h"
#include "client/loader/spriter/filefactory.h"
#include "client/loader/spriter/objectfactory.h"


namespace tdrp::render::component::animation
{

void SpriterAnimation::Load(const filesystem::path& image)
{
	auto resources = BabyDI::Get<tdrp::ResourceManager>();
	auto game = BabyDI::Get<tdrp::Game>();

	if (auto so = m_owner.lock())
	{
		auto filename = image.filename().string();

		auto id = resources->FindId<SpriterEngine::SpriterModel>(filename);
		if (id == 0)
		{
			auto filepath = game->Server.FileSystem.GetFilePath(filename);
			if (!filepath.empty())
			{
				auto spritermodel = std::make_shared<SpriterEngine::SpriterModel>(new loader::spriter::FileFactory(game->GetRenderWindow()), new loader::spriter::ObjectFactory(game->GetRenderWindow()));
				spritermodel->loadFile(filepath.string());

				id = resources->Add(filename, std::move(spritermodel));
			}
		}
		if (id == 0)
			return;

		m_model = resources->Get<SpriterEngine::SpriterModel>(id);
		if (auto spritermodel = m_model.lock())
		{
			m_animation = std::unique_ptr<SpriterEngine::EntityInstance>(spritermodel->getNewEntityInstance(so->GetEntity()));
			m_animation->setCurrentAnimation(so->GetAnimation());
		}
	}
}

void SpriterAnimation::Render(sf::RenderTarget& window, std::chrono::milliseconds elapsed)
{
	if (auto so = m_owner.lock())
	{
		if (so->GetType() != SceneObjectType::ANIMATED)
			return;

		auto animated_so = std::dynamic_pointer_cast<AnimatedSceneObject>(so);
		if (!animated_so || !m_animation)
			return;

		m_animation->setScale(SpriterEngine::point{ animated_so->GetScale().x, animated_so->GetScale().y });
		m_animation->setPosition(SpriterEngine::point{ animated_so->GetPosition().x, animated_so->GetPosition().y });

		// Time elapsed right before render so our position gets updated properly.
		m_animation->setTimeElapsed(elapsed.count());
		m_animation->render();
	}
}

Rectf SpriterAnimation::GetBoundingBox() const
{
	if (auto so = m_owner.lock())
	{
		auto pos = so->GetPosition();

		if (so->GetType() != SceneObjectType::ANIMATED)
			return Rectf{ pos.x, pos.y, 0.0f, 0.0f };

		auto animated_so = std::dynamic_pointer_cast<AnimatedSceneObject>(so);
		if (!animated_so || !m_animation)
			return Rectf{ pos.x, pos.y, 0.0f, 0.0f };

		auto scale = so->GetScale();
		auto size = m_animation->getSize();

		return Rectf{ pos.x, pos.y, static_cast<float>(size.x) * scale.x, static_cast<float>(size.y) * scale.y };
	}

	return Rectf{ 0.0f };
}

std::any SpriterAnimation::GetSize() const
{
	auto box = GetBoundingBox();
	return std::make_any<Vector2df>(box.size);
}

std::any SpriterAnimation::GetAnimation() const
{
	return std::make_any<SpriterEngine::EntityInstance*>(m_animation.get());
}

void SpriterAnimation::SetScale(const Vector2df& scale)
{
	if (auto so = m_owner.lock())
	{
		if (so->GetType() != SceneObjectType::ANIMATED)
			return;

		auto animated_so = std::dynamic_pointer_cast<AnimatedSceneObject>(so);
		if (!animated_so || !m_animation)
			return;

		m_animation->setScale(SpriterEngine::point{ scale.x, scale.y });
	}
}

void SpriterAnimation::SetPosition(const Vector2df& position)
{
	if (auto so = m_owner.lock())
	{
		if (so->GetType() != SceneObjectType::ANIMATED)
			return;

		auto animated_so = std::dynamic_pointer_cast<AnimatedSceneObject>(so);
		if (!animated_so || !m_animation)
			return;

		m_animation->setPosition(SpriterEngine::point{ position.x, position.y });
	}
}

void SpriterAnimation::UpdateImage(const std::string& image)
{
	if (auto so = m_owner.lock())
	{
		if (so->GetType() != SceneObjectType::ANIMATED)
			return;

		auto animated_so = std::dynamic_pointer_cast<AnimatedSceneObject>(so);
		if (!animated_so)
			return;

		Load(animated_so->GetImage());
	}
}

void SpriterAnimation::UpdateEntity(const std::string& entity)
{
	if (auto so = m_owner.lock())
	{
		if (so->GetType() != SceneObjectType::ANIMATED)
			return;

		auto animated_so = std::dynamic_pointer_cast<AnimatedSceneObject>(so);
		if (!animated_so)
			return;

		if (auto sm = m_model.lock())
		{
			// Try to save the current animation time.
			SpriterEngine::real old_anim_time = 0;
			if (m_animation)
			{
				old_anim_time = m_animation->getCurrentTime();
			}

			m_animation = std::unique_ptr<SpriterEngine::EntityInstance>(sm->getNewEntityInstance(so->GetEntity()));
			if (m_animation)
			{
				// Try to replace the animation and time.
				if (!animated_so->GetAnimation().empty())
				{
					m_animation->setCurrentAnimation(animated_so->GetAnimation());
					m_animation->setCurrentTime(old_anim_time);
				}
			}
		}
	}
}

void SpriterAnimation::UpdateAnimation(const std::string& animation)
{
	if (auto so = m_owner.lock())
	{
		if (so->GetType() != SceneObjectType::ANIMATED)
			return;

		auto animated_so = std::dynamic_pointer_cast<AnimatedSceneObject>(so);
		if (!animated_so || !m_animation)
			return;

		if (m_animation->currentAnimationName() == animation)
		{
			m_animation->setCurrentTime(0);
			return;
		}

		m_animation->setCurrentAnimation(animation);
	}
}

void SpriterAnimation::UpdateAttribute(const uint16_t attribute_id)
{
	if (auto so = m_owner.lock())
	{
		if (so->GetType() != SceneObjectType::ANIMATED)
			return;

		auto animated_so = std::dynamic_pointer_cast<AnimatedSceneObject>(so);
		if (!animated_so || !m_animation)
			return;

		auto attribute = so->Attributes.Get(attribute_id);
		auto variable = m_animation->getVariable(attribute->GetName());
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

} // end namespace tdrp::render::component
