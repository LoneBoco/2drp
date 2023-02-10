#pragma once

#include <deque>
#include <set>
#include <any>

#include <tmxlite/Map.hpp>

#include "engine/common.h"

#include "engine/component/Component.h"
#include "engine/physics/Physics.h"
#include "engine/scene/ObjectClass.h"
#include "engine/scene/ObjectAttributes.h"
#include "engine/scene/ObjectProperties.h"
#include "engine/scene/Tileset.h"
#include "engine/script/Function.h"


namespace tdrp::server
{
	class Player;
}
namespace tdrp::scene
{
	class Scene;
}

namespace tdrp
{

//! The type of a scene object.
enum class SceneObjectType
{
	DEFAULT = 0,
	STATIC,
	ANIMATED,
	TILEMAP,
	TMX,
	TEXT,

	COUNT
};


#define _PROPDEF(name, type) \
virtual type Get##name() const; \
virtual void Set##name(const type&);


constexpr SceneObjectID GlobalSceneObjectIDFlag = 0x80'00'00'00;

class SceneObject : public ComponentEntity
{
	SCRIPT_SETUP;
	SCRIPT_FUNCTION(OnCreated);
	SCRIPT_FUNCTION(OnUpdate);
	SCRIPT_FUNCTION(OnEvent);
	SCRIPT_FUNCTION(OnOwnershipChange);
	SCRIPT_FUNCTION(OnAnimationEnd);
	SCRIPT_FUNCTION(OnAttributeChange);
	SCRIPT_FUNCTION(OnAttached);
	SCRIPT_FUNCTION(OnCreatePhysics);
	SCRIPT_FUNCTION(OnCollision);
	SCRIPT_FUNCTION(OnCollisionEnd);
	SCRIPT_FUNCTION(OnSolveContact);

public:
	SceneObject(const std::shared_ptr<ObjectClass> c, const SceneObjectID id);
	virtual ~SceneObject();

	SceneObject& operator=(const SceneObject& other);

	SceneObject() = delete;
	SceneObject(const SceneObject& other) = delete;
	SceneObject(SceneObject&& other) = delete;
	SceneObject& operator=(SceneObject&& other) = delete;

public:
	std::weak_ptr<const ObjectClass> GetClass() const;
	std::shared_ptr<ObjectClass> GetClass();
	std::weak_ptr<server::Player> GetOwningPlayer() const;
	std::weak_ptr<scene::Scene> GetCurrentScene() const;

	void SetOwningPlayer(std::shared_ptr<server::Player> player);
	bool SetCurrentScene(std::shared_ptr<scene::Scene> scene);

	void AttachTo(std::shared_ptr<SceneObject> other);
	SceneObjectID GetAttachedId();

	std::optional<playrho::BodyID> GetInitializedPhysicsBody();
	void SynchronizePhysics();

public:
	_PROPDEF(Position, Vector2df);
	_PROPDEF(Depth, int64_t);
	_PROPDEF(Angle, float);
	_PROPDEF(Scale, Vector2df);
	_PROPDEF(Velocity, Vector2df);
	_PROPDEF(Acceleration, Vector2df);
	_PROPDEF(VelocityAngle, float);
	_PROPDEF(AccelerationAngle, float);
	_PROPDEF(Direction, int64_t);
	_PROPDEF(Image, std::string);
	_PROPDEF(Entity, std::string);
	_PROPDEF(Animation, std::string);
	_PROPDEF(Text, std::string);

public:
	virtual SceneObjectType GetType() const
	{
		return SceneObjectType::DEFAULT;
	}

	virtual Rectf GetBounds() const;

	bool IsGlobal()
	{
		return (ID & GlobalSceneObjectIDFlag) != 0;
	}

public:
	const SceneObjectID ID;
	std::string Name;
	bool Visible = true;
	bool Replicated = true;

	ObjectAttributes Attributes;
	ObjectProperties Properties;

	std::string ClientScript;
	std::string ServerScript;

	std::optional<playrho::BodyID> PhysicsBody;
	bool PhysicsChanged = false;

protected:
	const std::shared_ptr<ObjectClass> m_object_class;
	std::weak_ptr<server::Player> m_owning_player;
	std::weak_ptr<scene::Scene> m_current_scene;
	std::weak_ptr<SceneObject> m_attached_to;
	std::vector<std::weak_ptr<SceneObject>> m_attached;
};

using SceneObjectPtr = std::shared_ptr<SceneObject>;

inline std::weak_ptr<const ObjectClass> SceneObject::GetClass() const
{
	return m_object_class;
}
inline std::shared_ptr<ObjectClass> SceneObject::GetClass()
{
	return m_object_class;
}
inline std::weak_ptr<server::Player> SceneObject::GetOwningPlayer() const
{
	return m_owning_player;
}
inline std::weak_ptr<scene::Scene> SceneObject::GetCurrentScene() const
{
	return m_current_scene;
}


/////////////////////////////

class StaticSceneObject : public SceneObject
{
public:
	StaticSceneObject(const std::shared_ptr<ObjectClass> c, const SceneObjectID id) : SceneObject(c, id)	{}
	virtual ~StaticSceneObject() {}

	SceneObjectType GetType() const override
	{
		return SceneObjectType::STATIC;
	}
};

/////////////////////////////

class AnimatedSceneObject : public SceneObject
{
public:
	AnimatedSceneObject(const std::shared_ptr<ObjectClass> c, const SceneObjectID id) : SceneObject(c, id) {}
	virtual ~AnimatedSceneObject() {}

	virtual SceneObjectType GetType() const override
	{
		return SceneObjectType::ANIMATED;
	}

public:
	AnimatedSceneObject& operator=(const AnimatedSceneObject& other);

public:
	void SetAnimation(const std::string& image) override;
};

/////////////////////////////

class TiledSceneObject : public SceneObject
{
public:
	TiledSceneObject(const std::shared_ptr<ObjectClass> c, const SceneObjectID id) : SceneObject(c, id) {}
	virtual ~TiledSceneObject() {}

	virtual SceneObjectType GetType() const override
	{
		return SceneObjectType::TILEMAP;
	}

public:
	virtual Rectf GetBounds() const override
	{
		if (Tileset == nullptr)
			return Rectf();
		return Rectf(GetPosition(), math::convert<float>(Dimension) * math::convert<float>(Tileset->TileDimensions));
	}

public:
	Vector2di Dimension;
	std::vector<char> TileData;
	std::shared_ptr<scene::Tileset> Tileset;
};

/////////////////////////////

class TMXSceneObject : public SceneObject
{
public:
	TMXSceneObject(const std::shared_ptr<ObjectClass> c, const SceneObjectID id) : SceneObject(c, id) {}
	virtual ~TMXSceneObject() {}

	TMXSceneObject& operator=(const TMXSceneObject& other);

	virtual SceneObjectType GetType() const override
	{
		return SceneObjectType::TMX;
	}

public:
	virtual Rectf GetBounds() const override;

public:
	std::shared_ptr<tmx::Map> TmxMap;
	size_t Layer = 0;
	size_t Chunk = 0;
	Vector2df ChunkSize;
};

/////////////////////////////

class TextSceneObject : public SceneObject
{
public:
	TextSceneObject(const std::shared_ptr<ObjectClass> c, const SceneObjectID id) : SceneObject(c, id), m_font_size(12) {}
	virtual ~TextSceneObject() {}

	SceneObjectType GetType() const override
	{
		return SceneObjectType::TEXT;
	}

public:
	const std::string& GetFont() const;
	void SetFont(const std::string& font);

	uint32_t GetFontSize() const;
	void SetFontSize(uint32_t size);

	bool GetCentered() const;
	void SetCentered(bool centered);

protected:
	std::string m_font;
	uint32_t m_font_size;
	bool m_centered = false;
};

///////////////////////////////////////////////////////////////////////////////

} // end namespace tdrp
