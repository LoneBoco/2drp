#pragma once

#include <any>

#include "SFML/Graphics.hpp"

#include "engine/common.h"
#include "engine/filesystem/common.h"

namespace tdrp
{
	class SceneObject;
}

namespace tdrp::render::component::animation
{

class IAnimationSystem
{
public:
	IAnimationSystem(std::weak_ptr<SceneObject> owner) : m_owner(owner) {}
	virtual ~IAnimationSystem() {}

	IAnimationSystem(const IAnimationSystem& other) = delete;
	IAnimationSystem(IAnimationSystem&& other) = delete;
	IAnimationSystem& operator=(const IAnimationSystem& other) = delete;
	IAnimationSystem& operator=(IAnimationSystem&& other) = delete;
	bool operator==(const IAnimationSystem& other) = delete;

	virtual void Load(const filesystem::path& image) = 0;
	virtual void Render(sf::RenderTarget& window, std::chrono::milliseconds elapsed) = 0;

	virtual Rectf GetBoundingBox() const = 0;
	virtual std::any GetSize() const = 0;
	virtual std::any GetAnimation() const = 0;

	virtual void SetScale(const Vector2df& scale) = 0;
	virtual void SetPosition(const Vector2df& position) = 0;

	virtual void UpdateImage(const std::string& image) = 0;
	virtual void UpdateEntity(const std::string& entity) = 0;
	virtual void UpdateAnimation(const std::string& animation) = 0;
	virtual void UpdateAttribute(const uint16_t attribute_id) = 0;

protected:
	std::weak_ptr<SceneObject> m_owner;
};

} // end namespace tdrp::render::component::animation
