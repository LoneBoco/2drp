#pragma once

#include "spriterengine/spriterengine.h"

#include "engine/common.h"

#include "client/render/component/Animation/IAnimationSystem.h"

namespace tdrp::render::component::animation
{

class SpriterAnimation : public IAnimationSystem
{
public:
	SpriterAnimation(std::weak_ptr<SceneObject> owner) : IAnimationSystem(owner) {}
	~SpriterAnimation() override {}

	SpriterAnimation(const SpriterAnimation& other) = delete;
	SpriterAnimation(SpriterAnimation&& other) = delete;
	SpriterAnimation& operator=(const SpriterAnimation& other) = delete;
	SpriterAnimation& operator=(SpriterAnimation&& other) = delete;
	bool operator==(const SpriterAnimation& other) = delete;

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
	std::unique_ptr<SpriterEngine::EntityInstance> m_animation;
	std::weak_ptr<SpriterEngine::SpriterModel> m_model;
};

} // end namespace tdrp::render::component::animation
