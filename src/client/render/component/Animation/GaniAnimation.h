#pragma once

#include "SFML/Graphics.hpp"
#include "SFML/Audio.hpp"

#include "engine/common.h"

#include "client/render/component/Animation/IAnimationSystem.h"
#include "client/loader/gani/GaniLoader.h"

namespace tdrp::render::component::animation
{

class GaniAnimation : public IAnimationSystem
{
public:
	GaniAnimation(std::weak_ptr<SceneObject> owner) : IAnimationSystem(owner) {}
	~GaniAnimation() override {}

	GaniAnimation(const GaniAnimation& other) = delete;
	GaniAnimation(GaniAnimation&& other) = delete;
	GaniAnimation& operator=(const GaniAnimation& other) = delete;
	GaniAnimation& operator=(GaniAnimation&& other) = delete;
	bool operator==(const GaniAnimation& other) = delete;

	void Load(const filesystem::path& image) override;
	void Render(sf::RenderTarget& window, std::chrono::milliseconds elapsed) override;

	Rectf GetBoundingBox() const override;
	std::any GetSize() const override;
	std::any GetAnimation() const override;

	void SetScale(const Vector2df& scale) override;
	void SetPosition(const Vector2df& position) override;

	void UpdateImage(const std::string& image) override;
	void UpdateEntity(const std::string& entity) override;
	void UpdateAnimation(const std::string& animation) override;
	void UpdateAttribute(const uint16_t attribute_id) override;

protected:
	std::string convert_from_attribute(const std::string& attribute) const;
	void recalculate_bounding_box();
	void swap_images(const std::string& attribute, const std::string& image);

protected:
	std::unordered_map<size_t, std::weak_ptr<sf::Texture>> m_textures;
	std::unordered_map<size_t, std::optional<sf::Sprite>> m_sprites;
	std::unordered_map<std::string, std::shared_ptr<sf::Sound>> m_sounds;
	std::weak_ptr<loader::GaniAnimation> m_animation;
	std::size_t m_current_frame = 0;
	std::chrono::milliseconds m_elapsed = 0ms;
	Rectf m_cached_bounding_box;
	
	std::string m_current_animation;
	bool m_animation_stopped = false;
	bool m_count_full_first_frame = true;
};

} // end namespace tdrp::render::component::animation
