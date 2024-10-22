#include <ranges>

#include "client/render/component/Animation/GaniAnimation.h"

#include "client/game/Game.h"
#include "client/loader/gani/GaniLoader.h"
#include "client/loader/SFMListream.h"
#include "client/loader/ResourceLoaders.h"

#include "engine/filesystem/File.h"
#include "engine/scene/SceneObject.h"
#include "engine/resources/Resource.h"

namespace tdrp::render::component::animation
{

void GaniAnimation::Load(const filesystem::path& image)
{
	m_textures.clear();
	//m_sprites.clear();
	m_sounds.clear();
	m_animation.reset();
	m_current_frame = 0;
	m_elapsed = 0ms;
	m_cached_bounding_box = Rectf{};
	m_current_animation.clear();
	m_animation_stopped = false;
	m_count_full_first_frame = true;

	auto resources = BabyDI::Get<tdrp::ResourceManager>();
	auto game = BabyDI::Get<tdrp::Game>();

	if (auto so = m_owner.lock())
	{
		auto filename = image.filename().string();

		// Find or load the gani.
		auto id = resources->FindId<loader::GaniAnimation>(filename);
		if (id == 0)
		{
			auto filepath = game->Server.FileSystem.GetFilePath(filename);
			if (!filepath.empty())
			{
				auto gani = loader::GaniLoader::LoadGani(filepath);
				id = resources->Add(filename, std::move(gani));
			}
		}
		if (id == 0)
			return;

		m_current_animation = filename;
		m_animation = resources->Get<loader::GaniAnimation>(id);
		if (auto anim = m_animation.lock())
		{
			// Load sounds.
			for (auto& sound : anim->Sounds)
			{
				auto sid = loader::LoadSound(sound);
				if (sid != 0)
				{
					auto buffer = resources->Get<sf::SoundBuffer>(sid);
					if (auto b = buffer.lock())
					{
						// Load the sound buffer into a sound.
						sf::Sound s;
						s.setBuffer(*b);

						m_sounds[sound] = std::make_shared<sf::Sound>(std::move(s));
					}
				}
			}

			// Load textures and sprites.
			for (const auto& [index, sprite] : anim->Sprites)
			{
				auto tid = loader::LoadTexture(convert_from_attribute(sprite.Image));
				if (tid != 0)
				{
					auto texture = resources->Get<sf::Texture>(tid);
					if (auto t = texture.lock())
					{
						m_textures[index] = texture;
						if (auto existing_sprite = m_sprites.find(index); existing_sprite != std::end(m_sprites))
						{
							existing_sprite->second.setTexture(*t);
							existing_sprite->second.setTextureRect({ { sprite.Source.pos.x, sprite.Source.pos.y }, { sprite.Source.size.x, sprite.Source.size.y } });
						}
						else m_sprites.emplace(std::make_pair(index, sf::Sprite{ *t, { { sprite.Source.pos.x, sprite.Source.pos.y }, { sprite.Source.size.x, sprite.Source.size.y } } }));
					}
				}
				else log::PrintLine("!! Could not load texture {} for gani {}.", sprite.Image, image.filename().string());
			}
		}

		recalculate_bounding_box();
	}
}

void GaniAnimation::Render(sf::RenderTarget& window, std::chrono::milliseconds elapsed)
{
	// Only elapse the time if we are playing an animation and we have gone beyond the first frame.
	// We want to start counting only after the first frame displays and starts playing.
	if (!m_animation_stopped && !m_count_full_first_frame)
		m_elapsed += elapsed;

	if (auto so = m_owner.lock())
	{
		if (auto anim = m_animation.lock())
		{
			auto game = BabyDI::Get<tdrp::Game>();
			m_count_full_first_frame = false;

			bool queued_setbackto = false;

			// Play our frames.
			// Only draw the last frame, but make sure to run any sounds or scripts along the way.
			while (!m_animation_stopped && m_elapsed >= std::chrono::milliseconds{ anim->Frames[m_current_frame].Delay })
			{
				auto cur_delay = std::chrono::milliseconds{ anim->Frames[m_current_frame].Delay };
				m_elapsed -= cur_delay;

				// Play sounds.
				for (const auto& sound : anim->Frames[m_current_frame].Sounds)
				{
					if (auto it = m_sounds.find(sound.Sound); it != std::end(m_sounds))
					{
						auto so_pos = so->GetPosition();
						it->second->setPosition({ so_pos.x + sound.X, 0, so_pos.y + sound.Y });
						it->second->setMinDistance(640.0f);
						it->second->play();

						// Stick it in this list so it survives being deleted.
						game->PlayingSounds.push_back(it->second);
					}
				}

				// TODO: Scripts.

				// Increase our frame counter.
				++m_current_frame;
				if (m_current_frame >= anim->Frames.size())
				{
					so->OnAnimationEnd.RunAll(m_current_animation);
					if (!anim->IsLooped || !anim->Setbackto.empty())
					{
						m_animation_stopped = true;
						m_current_frame = anim->Frames.size() - 1;
					}
					else
					{
						m_current_frame = 0;
					}
				}
			}

			// Get our current direction.
			const auto& cur_frame = anim->Frames[m_current_frame];
			auto direction = so->Properties[Property::DIRECTION].GetAs<uint8_t>();
			if (direction >= cur_frame.Sprites.size())
				direction = 0;
			if (cur_frame.Sprites.empty())
				return;

			// Draw sprites.
			for (const auto& sprite : cur_frame.Sprites[direction])
			{
				auto sprite_iter = m_sprites.find(sprite.Index);
				if (sprite_iter == std::end(m_sprites))
					continue;

				auto& s = sprite_iter->second;

				auto so_pos = so->GetPosition();
				s.setPosition({ so_pos.x + sprite.X, so_pos.y + sprite.Y });
				window.draw(s);
			}

			if (m_animation_stopped && !anim->Setbackto.empty())
				so->SetAnimation(anim->Setbackto);
		}
	}
}

Rectf GaniAnimation::GetBoundingBox() const
{
	return m_cached_bounding_box;
}

std::any GaniAnimation::GetSize() const
{
	auto box = GetBoundingBox();
	return std::make_any<Vector2df>(box.size);
}

std::any GaniAnimation::GetAnimation() const
{
	return std::make_any<std::weak_ptr<loader::GaniAnimation>>(m_animation);
}

void GaniAnimation::SetScale(const Vector2df& scale)
{
}

void GaniAnimation::SetPosition(const Vector2df& position)
{
}

void GaniAnimation::UpdateImage(const std::string& image)
{
}

void GaniAnimation::UpdateEntity(const std::string& entity)
{
}

void GaniAnimation::UpdateAnimation(const std::string& animation)
{
	if (m_current_animation == animation)
	{
		if (auto anim = m_animation.lock())
		{
			if (anim->Frames.empty())
				return;

			if (!anim->IsContinuous)
			{
				bool reset = true;
				if (auto anim = m_animation.lock())
				{
					// Single frame animations usually don't have IsContinuous set.
					// Avoid spamming changes.
					if (anim->Frames.size() == 1)
						reset = false;
				}

				if (reset)
				{
					m_elapsed = 0ms;
					m_current_frame = 0;
					m_count_full_first_frame = true;
				}
				else
				{
					// We are going to leave this alone.  Unset the dirty flag so it doesn't get sent.
					if (auto so = m_owner.lock())
					{
						auto prop = so->Properties.Get(Property::ANIMATION);
						prop->Dirty = false;
					}
				}
			}
		}
	}
	else
	{
		Load(animation);

		// If our animation failed to load, at least call OnAnimationEnd in case we have a way to fix this issue.
		if (m_animation.expired())
		{
			if (auto so = m_owner.lock())
				so->OnAnimationEnd.RunAll("");
		}
	}
}

void GaniAnimation::UpdateAttribute(const uint16_t attribute_id)
{
	if (auto so = m_owner.lock())
	{
		const auto& attr = so->Attributes.Get(attribute_id);

		const auto name = attr->Name;
		if (boost::iequals(name, "SPRITES")
			|| boost::iequals(name, "HEAD")
			|| boost::iequals(name, "BODY")
			|| boost::iequals(name, "SWORD")
			|| boost::iequals(name, "SHIELD")
			|| boost::istarts_with(name, "ATTR")
			)
		{
			swap_images(name, attr->GetAs<std::string>());
		}
	}
}

std::string GaniAnimation::convert_from_attribute(const std::string& attribute) const
{
	if (auto so = m_owner.lock())
	{
		const auto& attr = so->Attributes.Get(attribute);
		if (attr)
			return attr->GetAs<std::string>();

		if (boost::iequals(attribute, "SPRITES"))
			return "sprites.png";
		if (boost::iequals(attribute, "HEAD"))
			return "head0.png";
		if (boost::iequals(attribute, "BODY"))
			return "body0.png";
		if (boost::iequals(attribute, "SWORD"))
			return "sword0.png";
		if (boost::iequals(attribute, "SHIELD"))
			return "shield0.png";

		return {};
	}

	return {};
}

void GaniAnimation::recalculate_bounding_box()
{
	Vector2df topleft = { 0.0f, 0.0f };
	Vector2df bottomright = { 0.0f, 0.0f };

	auto so = m_owner.lock();
	auto anim = m_animation.lock();
	if (!so || !anim)
		return;

	for (const auto& frame : anim->Frames)
	{
		for (auto i = 0; i < frame.Sprites.size(); ++i)
		{
			const auto& dir = frame.Sprites[i];
			for (const auto& sprite : dir)
			{
				auto it = m_sprites.find(sprite.Index);
				if (it == std::end(m_sprites))
					continue;

				const auto& sp = it->second;

				if (sprite.X < topleft.x)
					topleft.x = static_cast<float>(sprite.X);
				if (sprite.Y < topleft.y)
					topleft.y = static_cast<float>(sprite.Y);

				const auto& sp_box = sp.getGlobalBounds();
				if (sprite.X + sp_box.width > bottomright.x)
					bottomright.x = sprite.X + sp_box.width;
				if (sprite.Y + sp_box.height > bottomright.y)
					bottomright.y = sprite.Y + sp_box.height;
			}
		}
	}

	m_cached_bounding_box = Rectf{ topleft.x, topleft.y, bottomright.x - topleft.x, bottomright.y - topleft.y };
	return;
}

void GaniAnimation::swap_images(const std::string& attribute, const std::string& image)
{
	if (auto animation = m_animation.lock())
	{
		for (const auto& [index, sprite] : animation->Sprites)
		{
			if (boost::iequals(sprite.Image, attribute))
			{
				auto resources = BabyDI::Get<tdrp::ResourceManager>();

				// Load new texture.
				{
					auto tid = loader::LoadTexture(image);
					m_textures[index] = resources->Get<sf::Texture>(tid);
				}

				// Load new sprite.
				{
					if (auto texture = m_textures[index].lock())
					{
						if (auto it = m_sprites.find(index); it != std::end(m_sprites))
						{
							auto& s = it->second;
							s.setTexture(*texture);
							s.setTextureRect({ { sprite.Source.pos.x, sprite.Source.pos.y }, { sprite.Source.size.x, sprite.Source.size.y } });
						}
						else
						{
							sf::Sprite s;
							s.setTexture(*texture);
							s.setTextureRect({ { sprite.Source.pos.x, sprite.Source.pos.y }, { sprite.Source.size.x, sprite.Source.size.y } });
							m_sprites[index] = std::move(s);
						}
					}
				}
			}
		}

		recalculate_bounding_box();
	}
}

} // end namespace tdrp::render::component::animation
